// SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause)
/*
 * Copyright (c) 2020 MediaTek Inc.
 * Author jefferry.yen <jefferry.yen@mediatek.com>
 */
#ifdef pr_fmt
#undef pr_fmt
#endif
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#if defined(CONFIG_SERIAL_8250_CONSOLE) && defined(CONFIG_MAGIC_SYSRQ)
#define SUPPORT_SYSRQ
#endif

#include <linux/clk.h>
#include <linux/clk-provider.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/serial_8250.h>
#include <linux/serial_reg.h>
#include <linux/miscdevice.h>
#include <linux/kfifo.h>
#include <linux/fsnotify.h>

#include "8250.h"

#define UART_DEFAULT_BAUD		(115200)

#define CONSOLE_TTY_FLUSH_RETRY		(10)
#define CONSOLE_TTY_FLUSH_PERIOD_MS	(20)

/* debug port UART switch, priority also follow register define sequence */
#define MTK_UART_DBGPORT_PM0_BASE	(0x1C020200 + (0x7E << 2))
#define DBG_PORT_PMUART0_BIT		BIT_MASK(2)
#define MTK_UART_DBGPORT_PM51_BASE	(0x1C020200 + (0x09 << 2) + 1)
#define DBG_PORT_PM51UART_BIT		BIT_MASK(12 - 8)
#define MTK_UART_DBGPORT_FUART_BASE	(0x1C021200 + (0x76 << 2))
#define DBG_PORT_FUART0_BIT		BIT_MASK(0)
#define MTK_UART_DBGPORT_PIU_BASE_0	(0x1C067200 + (0x30 << 2))
#define DBG_PORT_PMUART1_BIT		BIT_MASK(1)
#define MTK_UART_DBGPORT_PIU_BASE_1	(0x1C067200 + (0x30 << 2) + 1)
#define DBG_PORT_UART0_BIT		BIT_MASK(8 - 8)
#define DBG_PORT_UART1_BIT		BIT_MASK(9 - 8)

/* UART DIG MUX */
#define MTK_UART_DIGMUX_BASE		(0x1c601000 + (0x01 << 2))
#define DIGMUX_0_1_REG_IDX		(0)
#define DIGMUX_2_3_REG_IDX		(1)
#define DIGMUX_4_5_REG_IDX		(4)
#define DIGMUX_REG_SZ			(8)
#define DIGMUX_PORT_IDX_0		(0)
#define DIGMUX_PORT_IDX_1		(1)
#define DIGMUX_PORT_IDX_2		(2)
#define DIGMUX_PORT_IDX_3		(3)
#define DIGMUX_PORT_IDX_4		(4)
#define DIGMUX_PORT_IDX_5		(5)
#define DIGMUX_PORT_IDX_MAX		DIGMUX_PORT_IDX_5
#define DIGMUX_SEL_WIDTH		(4)
#define DIGMUX_SEL_MSK_H		(0xF0)
#define DIGMUX_SEL_MSK_L		(0x0F)
#define DIGMUX_SEL_TO_DISABLE		(15)
#define DIGMUX_SEL_MAX			DIGMUX_SEL_TO_DISABLE

/* UART clock synthesizer register setting for high speed UART */
#define UART_SYNTH_DIV_BOUNDARY		(32)
#define UART_SYNTH_DECIMAL_BITS		(11)

#define UART_SYNTH_REG_SIZE		(8)
#define UART_SYNTH_REG_CTRL		(0)
#define UART_SYNTH_SW_RSTZ_MSK		0x0001
#define UART_SYNTH_NF_EN_MSK		0x0100
#define UART_SYNTH_REG_NF		(4)

/* UART IP register setting */
#define UART_BAUD_DIV_SCALE		(16)
#define MTK_UART_FIFO_SZ		(16)
#define MTK_UART_USR			(7)
#define MTK_UART_RST			(10)
#define MTK_UART_IIR_MSK		(0x3F)
#define UART_REG_SW_RSTZ_MSK		(0x01)
#define UART_REG_FORCE_CTS_ENABLE_MSK	(0x10)

/*
 *  UART private data
 */
struct mtk8250_dtv_data {
	void __iomem *synth_base;
	int line;
	int switched_line;
	bool in_suspended;
	int resume_line;
	struct uart_8250_port *port;
	struct clk_bulk_data *clks;
	int clk_num;
	struct clk *suspend_clk;
	struct clk *resume_clk;
	bool high_speed;
	struct timer_list silent_tmr;
	bool clk_enabled;
};

/*
 *  UART proprietary control
 */
struct mtkuart_ctrl_data {
	void __iomem *pm_0_select_base;
	void __iomem *pm51_select_base;
	void __iomem *fuart0_select_base;
	void __iomem *piu_select_base_0;
	void __iomem *piu_select_base_1;
	void __iomem *digmux_select_base;
	spinlock_t dbg_select_lock;
	spinlock_t digmux_select_lock;
};

/* whether or not disable console UART function */
static bool console_need_disable;
static DEFINE_SPINLOCK(mtk8250_console_disable_lock);
/* whether or not receive console UART input */
bool console_disable_rx;
static DEFINE_SPINLOCK(mtk8250_console_disable_rx_lock);
static struct platform_device *uat_control_dev;

/*
 *  Proprietary console UART output gather
 */
struct con_gather {
	/* device */
	struct miscdevice miscdev;
	/* log buffer */
	struct kfifo buff;
	unsigned char *buff_ptr;
	unsigned int buff_sz;
	/* device status */
	bool opened;
	/* whether or not output UART when gather opened */
	bool preserve_output;
	/* enable/disable gather function tester */
	bool drv_write_test;
	/* spinlock for protect setting value */
	spinlock_t setting_lock;
	/* file pointer for using inotify */
	struct file *filp;
};

static struct con_gather *con_out_gather;
#define CONSOLE_OUTPUT_GATHER_NAME	"log2usb"
#define CONSOLE_OUTPUT_GATHER_DEF_SZ	(0x40000)

/*
 * parameter of proprietary console enable/disable rx control
 */
#ifdef MODULE
module_param_named(con_uart_rx_disable, console_disable_rx, bool, 0644);
MODULE_PARM_DESC(con_uart_rx_disable, "Module parameter to switch uart console rx enable/disable.");
#else
static int __init mtk_console_uart_rx_disable(char *unused)
{
	console_disable_rx = true;
	return 1;
}
__setup("con_uart_rx_disable", mtk_console_uart_rx_disable);
#endif

/*
 * parameter of proprietary console enable/disable control
 */
#ifdef MODULE
module_param_named(log_disable, console_need_disable, bool, 0644);
MODULE_PARM_DESC(log_disable, "Module parameter to switch uart console enable/disable.");
#else
static int __init mtk_console_lock(char *unused)
{
	console_need_disable = true;
	return 1;
}
__setup("log_disable", mtk_console_lock);
#endif

/*
 * Override io function, proprietary console enable/disable control
 */
#define UART_KEY_SIZE		(8)

static unsigned char keyword[] = "00112233";
static unsigned char keypool[UART_KEY_SIZE] = {0};
static u32 mtk_console_verbose = 1;
static u32 keyidx;

static unsigned int mtk8250_dtv_mem_serial_in(struct uart_port *p, int offset)
{
	unsigned int data;
	int i;
	struct mtk8250_dtv_data *mtk_up = dev_get_drvdata(p->dev);

	offset = offset << p->regshift;
	data = readb(p->membase + offset);

	/* check switch uart silent */
	if (unlikely(uart_console(p)) && unlikely(offset == (UART_RX << p->regshift))) {
		keypool[keyidx++] = data;
		keyidx %= UART_KEY_SIZE;

		for (i = 0; i < UART_KEY_SIZE; i++)
			if (keypool[(keyidx + i) % UART_KEY_SIZE] != keyword[i])
				break;

		if (i >= UART_KEY_SIZE) {
			mtk_console_verbose = !mtk_console_verbose;
			if (!mtk_console_verbose)
				// trigger timer in console disable(silent) state
				mod_timer(&mtk_up->silent_tmr, jiffies + uart_poll_timeout(p));
		}
	}

	return data;
}

static void mtk8250_dtv_mem_serial_out(struct uart_port *p, int offset, int value)
{
	if (uart_console(p) && likely(offset == UART_TX)) {
		/* check silent out */
		if (unlikely(!mtk_console_verbose || console_need_disable))
			return;

		/* fill console output gather when opened */
		if (con_out_gather && con_out_gather->opened) {
			char in = value & U8_MAX;

			(void)kfifo_in(&con_out_gather->buff, &in, 1);
			if (in == '\n')
				fsnotify_modify(con_out_gather->filp);
			if (!con_out_gather->preserve_output)
				return;
		}
	}

#ifdef CONFIG_SERIAL_8250_LOOPBACK
	struct uart_8250_port *up =
 		container_of(p, struct uart_8250_port, port);
	if (offset == UART_TX && !up->uart_in_dl_write)
		serial8250_loopback_write(up, value);
#endif
#ifdef CONFIG_ROKU_SERIAL_8250
	if (offset == UART_TX && up->uart_nooutput && !up->uart_in_dl_write)
		value = '6';
#endif
	offset = offset << p->regshift;
	writeb(value, p->membase + offset);
}

/*
 * In console disable, UART neither transmitter empty nor transmit-hold-register empty interrupts
 * will be triggered since UART_TX is not written. Need a timer to flush out uart circular buffer.
 */
static inline void __mtk8250_do_stop_tx(struct uart_8250_port *p)
{
	if (serial8250_clear_THRI(p))
		serial8250_rpm_put_tx(p);
}

static void mtk8250_silent_flush_xmit(struct timer_list *t)
{
	struct mtk8250_dtv_data *mtk_up = from_timer(mtk_up, t, silent_tmr);
	struct uart_8250_port *up = mtk_up->port;
	struct uart_port *port = &up->port;
	struct circ_buf *xmit = &port->state->xmit;
	unsigned long flags;

	spin_lock_irqsave(&up->port.lock, flags);

	if (port->x_char) {
		port->icount.tx++;
		port->x_char = 0;
		goto silent_flush_xmit_end;
	}
	if (uart_tx_stopped(port)) {
		serial8250_rpm_get(up);
		__mtk8250_do_stop_tx(up);
		serial8250_rpm_put(up);
		goto silent_flush_xmit_end;
	}
	if (uart_circ_empty(xmit)) {
		__mtk8250_do_stop_tx(up);
		goto silent_flush_xmit_end;
	}

	while (!uart_circ_empty(xmit)) {
		/* fill console output gather when opened */
		if (con_out_gather && con_out_gather->opened) {
			(void)kfifo_in(&con_out_gather->buff, &xmit->buf[xmit->tail], 1);
			fsnotify_modify(con_out_gather->filp);
		}
		xmit->tail = (xmit->tail + 1) & (UART_XMIT_SIZE - 1);
		port->icount.tx++;
	}

	uart_write_wakeup(port);

	/*
	 * With RPM enabled, we have to wait until the FIFO is empty before the
	 * HW can go idle. So we get here once again with empty FIFO and disable
	 * the interrupt and RPM in __stop_tx()
	 */
	if (uart_circ_empty(xmit))
		__mtk8250_do_stop_tx(up);

silent_flush_xmit_end:

	spin_unlock_irqrestore(&up->port.lock, flags);

	/* only register timer operation for console port, no need check console port status */
	if (unlikely(!mtk_console_verbose || console_need_disable)) {
		/* need trigger timer again in console disable(silent) state */
		mod_timer(&mtk_up->silent_tmr, jiffies + uart_poll_timeout(&up->port));
	}
}

/*
 *  8250 driver framework
 */
static void mtk8250_dtv_set_termios(struct uart_port *port, struct ktermios *termios,
				    struct ktermios *old)
{
	struct mtk8250_dtv_data *mtk_up = dev_get_drvdata(port->dev);
	struct circ_buf *xmit = &port->state->xmit;
	speed_t req_speed = tty_termios_baud_rate(termios);

	dev_info(port->dev, "set termios with remain %ld.\n", uart_circ_chars_pending(xmit));
	dev_info(port->dev, "request baudrate %d at uartclk %d\n", req_speed, port->uartclk);

	if (mtk_up->high_speed) {
		unsigned long clk_speed = clk_get_rate(mtk_up->clks[0].clk);
		unsigned long uartclk_tmp;
		uint16_t synth_div;
		unsigned long synth_div_rem;
		int i;

		/* high speed uart, calculate clock synthesizer according requested baud rate */

		dev_info(port->dev, "set synthesizer for high speed uart port\n");
		dev_info(port->dev, "request baudrate %d at clk %ld\n", req_speed, clk_speed);

		/* Special case: B0 rate. */
		if (req_speed == 0)
			req_speed = UART_DEFAULT_BAUD;

		/* DLM-DLL will be (synth_tmp / UART_BAUD_DIV_SCALE) + 1 */
		uartclk_tmp = clk_speed;
		uartclk_tmp /= (req_speed * UART_BAUD_DIV_SCALE * UART_SYNTH_DIV_BOUNDARY);
		uartclk_tmp++;
		uartclk_tmp *= (req_speed * UART_BAUD_DIV_SCALE);
		if (uartclk_tmp >= U32_MAX) {
			dev_err(port->dev, "report uart clock overflow %ld\n", uartclk_tmp);
			uartclk_tmp &= U32_MAX;
		}
		port->uartclk = uartclk_tmp;
		dev_info(port->dev, "report uart clock %d\n", port->uartclk);

		/* calculate uart synthesizer register setting */
		/* integer part */
		synth_div = clk_speed / port->uartclk;
		if (synth_div >= UART_SYNTH_DIV_BOUNDARY)
			dev_err(port->dev, "synth, divider out of range (%d)\n", synth_div);

		/* decimal part, port->uartclk = uartclk_tmp now */
		synth_div_rem = clk_speed % port->uartclk;
		for (i = 0; i < UART_SYNTH_DECIMAL_BITS; i++) {
			dev_info(port->dev, "synth. divider %d, remain %ld\n", synth_div,
				 synth_div_rem);
			uartclk_tmp >>= 1;
			synth_div <<= 1;
			if (synth_div_rem >= uartclk_tmp) {
				synth_div |= 1;
				synth_div_rem -= uartclk_tmp;
			}
		}
		dev_info(port->dev, "synth. register setting 0x%04X\n", synth_div);

		/* setup synthesizer */
		writew(~(UART_SYNTH_SW_RSTZ_MSK | UART_SYNTH_NF_EN_MSK),
		       mtk_up->synth_base + UART_SYNTH_REG_CTRL);
		writew(synth_div, mtk_up->synth_base + UART_SYNTH_REG_NF);
		writew(UART_SYNTH_SW_RSTZ_MSK | UART_SYNTH_NF_EN_MSK,
		       mtk_up->synth_base + UART_SYNTH_REG_CTRL);
	}

	port->status &= ~UPSTAT_AUTOCTS;
	if (termios->c_cflag & CRTSCTS)
		port->status |= UPSTAT_AUTOCTS;

	if (!uart_circ_empty(xmit)) {
		int retry = CONSOLE_TTY_FLUSH_RETRY;

		while (!uart_circ_empty(xmit) && retry) {
			dev_info(port->dev, "buffer remain %ld\n", uart_circ_chars_pending(xmit));
			msleep(CONSOLE_TTY_FLUSH_PERIOD_MS);
			retry--;
		}

		if (!uart_circ_empty(xmit)) {
			dev_warn(port->dev, "clear buffer remain %ld\n",
				 uart_circ_chars_pending(xmit));
			uart_circ_clear(xmit);
			port->ops->stop_tx(port);
		}
	}

	if (port->serial_in(port, UART_IER) & UART_IER_THRI) {
		int retry = CONSOLE_TTY_FLUSH_RETRY;

		while ((port->serial_in(port, UART_IER) & UART_IER_THRI) && retry) {
			dev_info(port->dev, "wait UART IER THRI\n");
			msleep(CONSOLE_TTY_FLUSH_PERIOD_MS);
			retry--;
		}

		if (port->serial_in(port, UART_IER) & UART_IER_THRI) {
			dev_warn(port->dev, "force clear UART IER THRI at IER 0x%X\n",
				 port->serial_in(port, UART_IER));
			port->ops->stop_tx(port);
		}
	}

	dev_info(port->dev, "UART IER before set termios 0x%X (uport->ier 0x%X)\n",
		 port->serial_in(port, UART_IER), up_to_u8250p(port)->ier);

	serial8250_do_set_termios(port, termios, old);

	dev_info(port->dev, "UART IER after set termios 0x%X (uport->ier 0x%X)\n",
		 port->serial_in(port, UART_IER), up_to_u8250p(port)->ier);
}

static int mtk8250_dtv_startup(struct uart_port *port)
{
	int ret;
	struct mtk8250_dtv_data *mtk_up = dev_get_drvdata(port->dev);

	ret = serial8250_do_startup(port);
	if (uart_console(port)) {
		// timer for flush uart xmit buffer when console disabled
		timer_setup(&mtk_up->silent_tmr, mtk8250_silent_flush_xmit, 0);
		// start timer if console already disabled or console output gather is enabled
		if (unlikely(!mtk_console_verbose || console_need_disable) ||
		    (con_out_gather && con_out_gather->opened && !con_out_gather->preserve_output))
			mod_timer(&mtk_up->silent_tmr, jiffies + uart_poll_timeout(port));
	}

	return ret;
}

static void mtk8250_dtv_shutdown(struct uart_port *port)
{
	struct mtk8250_dtv_data *mtk_up = dev_get_drvdata(port->dev);

	serial8250_do_shutdown(port);
	if (uart_console(port))
		// delete timer for flush uart xmit buffer when console disabled
		del_timer_sync(&mtk_up->silent_tmr);
}

static void mtk8250_dtv_do_pm(struct uart_port *port, unsigned int state, unsigned int old)
{
	int ret;

	if (state == UART_PM_STATE_ON) {
		// might trigger device runtime resume (enable module clock)
		ret = pm_runtime_get_sync(port->dev);
		if (ret != 0)
			dev_notice(port->dev, "pm_runtime_get_sync error(%d)\n", ret);
	}

	serial8250_do_pm(port, state, old);

	if (state == UART_PM_STATE_OFF) {
		// might trigger device runtime suspend (disable module clock)
		ret = pm_runtime_put_sync_suspend(port->dev);
		if (ret != 0)
			dev_notice(port->dev, "pm_runtime_put_sync_suspend error(%d)\n", ret);
	}
}

static int mtk8250_dtv_handle_irq(struct uart_port *p)
{
	unsigned int iir;
	unsigned char status;
	unsigned long flags;
	struct uart_8250_port *up = up_to_u8250p(p);

	iir = p->serial_in(p, UART_IIR);

	/*
	 * There are ways to get Designware-based UARTs into a state where
	 * they are asserting UART_IIR_RX_TIMEOUT but there is no actual
	 * data available.  If we see such a case then we'll do a bogus
	 * read.  If we don't do this then the "RX TIMEOUT" interrupt will
	 * fire forever.
	 *
	 * This problem has only been observed so far when not in DMA mode
	 * so we limit the workaround only to non-DMA mode.
	 */
	if (!up->dma && ((iir & MTK_UART_IIR_MSK) == UART_IIR_RX_TIMEOUT)) {
		status = p->serial_in(p, UART_LSR);

		if (!(status & (UART_LSR_DR | UART_LSR_BI)))
			(void) p->serial_in(p, UART_RX);
	}

	/*
	 * procedure of serial8250_handle_irq(p, iir), intercept console RX data when console is
	 * disabled
	 */
	if (iir & UART_IIR_NO_INT)
		goto mtk8250_dtv_handle_no_int_irq;

	spin_lock_irqsave(&p->lock, flags);

	status = serial_port_in(p, UART_LSR);

	if (uart_console(p)) {
		if (unlikely(!mtk_console_verbose || console_need_disable)) {
			/*
			 * console is in disabled, only handle rx data for checking enable password
			 */
			do {
				if (likely(status & UART_LSR_DR))
					(void)p->serial_in(p, UART_RX);

				if (mtk_console_verbose)
					/*
					 * console re-enabled, remain rest data for next iteration
					 */
					break;

				status = p->serial_in(p, UART_LSR);
			} while (status & UART_LSR_DR);
		} else if (status & (UART_LSR_DR | UART_LSR_BI)) {
			if (unlikely(console_disable_rx)) {
				/* Flush RX when console_disable_rx */
				int max_count = U8_MAX;

				do {
					(void)p->serial_in(p, UART_RX);
					if (--max_count == 0)
						break;
					status = p->serial_in(p, UART_LSR);
				} while (status & (UART_LSR_DR | UART_LSR_BI));
			} else
				/* normal status, original procedure for handle uart (w/o DMA) */
				status = serial8250_rx_chars(up, status);
		}
	} else if (status & (UART_LSR_DR | UART_LSR_BI))
		/* normal status, original procedure for handle uart (without DMA) */
		status = serial8250_rx_chars(up, status);

	serial8250_modem_status(up);
	if ((status & UART_LSR_THRE) && (up->ier & UART_IER_THRI))
		serial8250_tx_chars(up);

	uart_unlock_and_check_sysrq(p, flags);

	return 1;

mtk8250_dtv_handle_no_int_irq:

	if ((iir & UART_IIR_BUSY) == UART_IIR_BUSY) {
		/* Clear the USR */
		(void)p->serial_in(p, MTK_UART_USR);
		return 1;
	}

	return 0;
}

/*
 * switch uart disable sysfs group, in /sys/devices/platform/uart_control
 */
static ssize_t console_disable_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	char *str = buf;
	char *end = buf + PAGE_SIZE;

	str += scnprintf(str, end - str, "%d\n", console_need_disable);
	return (str - buf);
}

static ssize_t console_disable_store(struct device *dev, struct device_attribute *attr,
				  const char *buf, size_t count)
{
	unsigned long flags;
	bool console_disable = false;
	int ret;

	if (!buf)
		return -EINVAL;

	ret = kstrtobool(buf, &console_disable);
	if (ret == 0) {
		spin_lock_irqsave(&mtk8250_console_disable_lock, flags);
		console_need_disable = console_disable;
		spin_unlock_irqrestore(&mtk8250_console_disable_lock, flags);

		if (console_need_disable) {
			int i;
			struct uart_8250_port *up;
			struct mtk8250_dtv_data *mtk_up;

			// get console uart port handle, traverse all uart_8250_port
			for (i = 0 ; i < CONFIG_SERIAL_8250_NR_UARTS ; i++) {
				up = serial8250_get_port(i);
				if (uart_console(&up->port))
					break;
			}

			if (i < CONFIG_SERIAL_8250_NR_UARTS) {
				/*
				 * console uart port found, trigger timer in console disable(silent)
				 * state
				 */
				mtk_up = dev_get_drvdata(up->port.dev);
				mod_timer(&mtk_up->silent_tmr,
					  jiffies + uart_poll_timeout(&up->port));
			}
		}
	}

	return (ret < 0) ? ret : count;
}

static DEVICE_ATTR_RW(console_disable);

/*
 * switch receive console UART input, in /sys/devices/platform/uart_control
 */
static ssize_t console_disable_rx_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	char *str = buf;
	char *end = buf + PAGE_SIZE;

	str += scnprintf(str, end - str, "%d\n", console_disable_rx);
	return (str - buf);
}

static ssize_t console_disable_rx_store(struct device *dev, struct device_attribute *attr,
					const char *buf, size_t count)
{
	unsigned long flags;
	bool disable_input = false;
	int ret;

	if (!buf)
		return -EINVAL;

	ret = kstrtobool(buf, &disable_input);
	if (ret == 0) {
		spin_lock_irqsave(&mtk8250_console_disable_rx_lock, flags);
		console_disable_rx = disable_input;
		spin_unlock_irqrestore(&mtk8250_console_disable_rx_lock, flags);
	}

	return (ret < 0) ? ret : count;
}

static DEVICE_ATTR_RW(console_disable_rx);

/*
 * exchange UART port ttySx and ttySy sysfs group, in /sys/devices/platform/uart_control
 */
static int uart_check_swap_8250_port(int line1, int line2, struct uart_8250_port *up1,
				     struct uart_8250_port *up2)
{
	struct mtk8250_dtv_data *mtk_up1, *mtk_up2;

	// check ports have the same status (both activate or deactivate)
	if (tty_port_active(&up1->port.state->port) != tty_port_active(&up2->port.state->port)) {
		dev_err(up1->port.dev, "can not switch uart port with different status.\n");
		return -EBUSY;
	}

	if (!up1->port.mapbase || !up2->port.mapbase) {
		dev_err(up1->port.dev, "can not switch uart port with invalid uart port.\n");
		return -EINVAL;
	}

	// only allow switch out or switch back for single UART IP
	mtk_up1 = up1->port.private_data;
	if ((mtk_up1->switched_line != mtk_up1->line) &&
	    (mtk_up1->switched_line != up2->port.line)) {
		dev_err(up1->port.dev, "only can switch back to ttyS%d.\n", mtk_up1->line);
		return -EINVAL;
	}
	mtk_up2 = up2->port.private_data;
	if ((mtk_up2->switched_line != mtk_up2->line) &&
	    (mtk_up2->switched_line != up1->port.line)) {
		dev_err(up2->port.dev, "only can switch back to ttyS%d.\n", mtk_up2->line);
		return -EINVAL;
	}

	return 0;
}

static int uart_swap_8250_port(struct device *dev, struct uart_8250_port *up1,
			       struct uart_8250_port *up2, bool port_active)
{
	unsigned long flags1, flags2;
	struct mtk8250_dtv_data *mtk_up1, *mtk_up2;
	struct uart_port *p1, *p2;
	unsigned char __iomem *tmp_membase;
	resource_size_t tmp_mapbase;
	unsigned int tmp_irq;
	bool tmp_high_speed;
	struct clk_bulk_data *tmp_clks;
	int tmp_clk_num;
	struct clk *tmp_clk;
	int res1 = 0, res2 = 0;
	struct ktermios tmp_ktermios;

	p1 = &up1->port;
	p2 = &up2->port;
	mtk_up1 = p1->private_data;
	mtk_up2 = p2->private_data;

	// switch ports, lock tty mutex for switching
	mutex_lock(&(p1->state->port.mutex));
	mutex_lock(&(p2->state->port.mutex));
	spin_lock_irqsave(&p1->lock, flags1);
	spin_lock_irqsave(&p2->lock, flags2);

	// hack swap irq: unlink original irq
	if (port_active) {
		up1->ops->release_irq(up1);
		up2->ops->release_irq(up2);
	}

	// hack swap irq: link new irq
	// hack swap irq for link
	tmp_irq = p1->irq;
	p1->irq = p2->irq;
	p2->irq = tmp_irq;

	if (port_active) {
		res1 = up1->ops->setup_irq(up1);
		res2 = up2->ops->setup_irq(up2);
	}

	// hack swap ktermios
	memcpy(&tmp_ktermios, &p1->state->port.tty->termios, sizeof(tmp_ktermios));
	memcpy(&p1->state->port.tty->termios, &p2->state->port.tty->termios, sizeof(tmp_ktermios));
	memcpy(&p2->state->port.tty->termios, &tmp_ktermios, sizeof(tmp_ktermios));
	memcpy(&tmp_ktermios, &p1->state->port.tty->termios_locked, sizeof(tmp_ktermios));
	memcpy(&p1->state->port.tty->termios_locked, &p2->state->port.tty->termios_locked,
	       sizeof(tmp_ktermios));
	memcpy(&p2->state->port.tty->termios_locked, &tmp_ktermios, sizeof(tmp_ktermios));

	// hack swap uart port membase
	tmp_membase = p1->membase;
	p1->membase = p2->membase;
	p2->membase = tmp_membase;

	// hack swap uart port mapbase
	tmp_mapbase = p1->mapbase;
	p1->mapbase = p2->mapbase;
	p2->mapbase = tmp_mapbase;

	// hack swap uart port uartclk value, use tmp_irq variable
	tmp_irq = p1->uartclk;
	p1->uartclk = p2->uartclk;
	p2->uartclk = tmp_irq;

	// UART IP module clocks
	tmp_clks = mtk_up1->clks;
	mtk_up1->clks = mtk_up2->clks;
	mtk_up2->clks = tmp_clks;

	tmp_clk_num = mtk_up1->clk_num;
	mtk_up1->clk_num = mtk_up2->clk_num;
	mtk_up2->clk_num = tmp_clk_num;

	// switched_line id, use tmp_irq variable
	tmp_irq = mtk_up1->switched_line;
	mtk_up1->resume_line = mtk_up1->switched_line = mtk_up2->switched_line;
	mtk_up2->resume_line = mtk_up2->switched_line = tmp_irq;

	// high speed synthesizer setting
	tmp_membase = mtk_up1->synth_base;
	mtk_up1->synth_base = mtk_up2->synth_base;
	mtk_up2->synth_base = tmp_membase;

	// high speed clock parent
	tmp_clk = mtk_up1->suspend_clk;
	mtk_up1->suspend_clk = mtk_up2->suspend_clk;
	mtk_up2->suspend_clk = tmp_clk;

	tmp_clk = mtk_up1->resume_clk;
	mtk_up1->resume_clk = mtk_up2->resume_clk;
	mtk_up2->resume_clk = tmp_clk;

	tmp_high_speed = mtk_up1->high_speed;
	mtk_up1->high_speed = mtk_up2->high_speed;
	mtk_up2->high_speed = tmp_high_speed;

	// unlock tty mutex after switch port finish
	spin_unlock_irqrestore(&p2->lock, flags2);
	spin_unlock_irqrestore(&p1->lock, flags1);
	mutex_unlock(&(p2->state->port.mutex));
	mutex_unlock(&(p1->state->port.mutex));

	dev_notice(dev, "switch done, res1 %d res2 %d\n", res1, res2);

	return (!res1 && !res2) ? 0 : -1;
}

static ssize_t uart_switch_store(struct device *dev, struct device_attribute *attr,
				 const char *buf, size_t count)
{
	unsigned int line1, line2;
	char *str1, *str2 = (char *)buf;
	struct uart_8250_port *up = NULL, *up1 = NULL, *up2 = NULL;
	struct uart_port *p1 = NULL, *p2 = NULL;
	int i;

	if (!buf)
		return -EINVAL;

	// get input string
	str1 = strsep(&str2, ",");
	if (!str1 || !str2 || kstrtouint(str1, 10, &line1) || kstrtouint(str2, 10, &line2))
		return -EINVAL;

	dev_notice(dev, "prepare swap ttyS%d and ttyS%d UART IP\n", line1, line2);

	if (line1 == line2)
		return count;

	// get uart port handle, traverse all uart_8250_port
	for (i = 0 ; i < CONFIG_SERIAL_8250_NR_UARTS ; i++) {
		up = serial8250_get_port(i);
		if (up->port.line == line1)
			up1 = up;
		if (up->port.line == line2)
			up2 = up;
	}

	if (!up1 || !up2)
		// port not found
		return -EINVAL;

	if (uart_check_swap_8250_port(line1, line2, up1, up2) < 0)
		return -EINVAL;

	p1 = &up1->port;
	p2 = &up2->port;

	if (uart_swap_8250_port(dev, up1, up2, tty_port_active(&p1->state->port)) < 0) {
		// switch failed, try switch back
		(void)uart_swap_8250_port(dev, up1, up2, tty_port_active(&p1->state->port));
		return -EINVAL;
	}

	dev_info(dev, "framework line %d switched to line %d\n", p1->line,
		 ((struct mtk8250_dtv_data *)(p1->private_data))->switched_line);
	dev_info(dev, "framework line %d switched to line %d\n", p2->line,
		 ((struct mtk8250_dtv_data *)(p2->private_data))->switched_line);

	return count;
}

static DEVICE_ATTR_WO(uart_switch);

/*
 * get UART port switch status sysfs group, in /sys/devices/platform/uart_control
 */
static ssize_t uart_switch_status_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	char *str = buf;
	char *end = buf + PAGE_SIZE;
	int i;
	struct uart_8250_port *up;
	struct uart_port *p;
	struct mtk8250_dtv_data *mtk_up;

	for (i = 0 ; i < CONFIG_SERIAL_8250_NR_UARTS ; i++) {
		up = serial8250_get_port(i);
		p = &up->port;
		mtk_up = p->private_data;

		if (!mtk_up)
			dev_info(dev, "ttyS%d is unused\n", p->line);
		else {
			dev_info(dev, "ttyS%d is original ttyS%d use ttyS%d's reg is %sactivate\n",
				 p->line, mtk_up->line, mtk_up->switched_line,
				 tty_port_active(&p->state->port) ? "" : "de");
			str += scnprintf(str, end - str, "%d%d%d", mtk_up->line,
					 mtk_up->switched_line, tty_port_active(&p->state->port));
		}
	}

	return (str - buf);
}

static DEVICE_ATTR_RO(uart_switch_status);

/*
 * debug port UART selection sysfs group, in /sys/devices/platform/uart_control
 */
static ssize_t debug_port_select_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct mtkuart_ctrl_data *uart_control = (struct mtkuart_ctrl_data *)dev_get_drvdata(dev);
	char *str = buf;
	char *end = buf + PAGE_SIZE;
	u8 reg;

	if (!uart_control) {
		dev_err(dev, "no UART control priv data to get information\n");
		return -EOPNOTSUPP;
	}

	if (!uart_control->pm_0_select_base || !uart_control->pm51_select_base ||
	    !uart_control->fuart0_select_base || !uart_control->piu_select_base_0 ||
	    !uart_control->piu_select_base_1) {
		dev_err(dev, "cannot read debug port switch register\n");
		return -EOPNOTSUPP;
	}

	reg = readb(uart_control->pm_0_select_base);
	if (reg & DBG_PORT_PMUART0_BIT) {
		str += scnprintf(str, end - str, "PM_UART0\n");
		return (str - buf);
	}
	reg = readb(uart_control->pm51_select_base);
	if (reg & DBG_PORT_PM51UART_BIT) {
		str += scnprintf(str, end - str, "PM51_UART0\n");
		return (str - buf);
	}
	reg = readb(uart_control->fuart0_select_base);
	if (reg & DBG_PORT_FUART0_BIT) {
		str += scnprintf(str, end - str, "FUART0\n");
		return (str - buf);
	}
	reg = readb(uart_control->piu_select_base_0);
	if (reg & DBG_PORT_PMUART1_BIT) {
		str += scnprintf(str, end - str, "PM_UART1\n");
		return (str - buf);
	}
	reg = readb(uart_control->piu_select_base_1);
	if (reg & DBG_PORT_UART0_BIT) {
		str += scnprintf(str, end - str, "UART0\n");
		return (str - buf);
	}
	if (reg & DBG_PORT_UART1_BIT) {
		str += scnprintf(str, end - str, "UART1\n");
		return (str - buf);
	}

	str += scnprintf(str, end - str, "DIG_MUX0\n");
	return (str - buf);
}

static ssize_t debug_port_select_store(struct device *dev, struct device_attribute *attr,
				       const char *buf, size_t count)
{
	struct mtkuart_ctrl_data *uart_control = (struct mtkuart_ctrl_data *)dev_get_drvdata(dev);
	unsigned long flags;
	ssize_t ret = count;

	if (!buf)
		return -EINVAL;

	if (!uart_control) {
		dev_err(dev, "no UART control priv data to get information\n");
		return -EOPNOTSUPP;
	}

	if (!uart_control->pm_0_select_base || !uart_control->pm51_select_base ||
	    !uart_control->fuart0_select_base || !uart_control->piu_select_base_0 ||
	    !uart_control->piu_select_base_1) {
		dev_err(dev, "cannot read debug port select register\n");
		return -EOPNOTSUPP;
	}

	spin_lock_irqsave(&uart_control->dbg_select_lock, flags);

	if (!strncasecmp(buf, "PM_UART0", strlen("PM_UART0"))) {
		writeb(readb(uart_control->pm_0_select_base) | DBG_PORT_PMUART0_BIT,
		       uart_control->pm_0_select_base);
	} else if (!strncasecmp(buf, "PM51_UART0", strlen("PM51_UART0"))) {
		writeb(readb(uart_control->pm_0_select_base) & ~DBG_PORT_PMUART0_BIT,
		       uart_control->pm_0_select_base);
		writeb(readb(uart_control->pm51_select_base) | DBG_PORT_PM51UART_BIT,
		       uart_control->pm51_select_base);
	} else if (!strncasecmp(buf, "FUART0", strlen("FUART0"))) {
		writeb(readb(uart_control->pm_0_select_base) & ~DBG_PORT_PMUART0_BIT,
		       uart_control->pm_0_select_base);
		writeb(readb(uart_control->pm51_select_base) & ~DBG_PORT_PM51UART_BIT,
		       uart_control->pm51_select_base);
		writeb(readb(uart_control->fuart0_select_base) | DBG_PORT_FUART0_BIT,
		       uart_control->fuart0_select_base);
	} else if (!strncasecmp(buf, "PM_UART1", strlen("PM_UART1"))) {
		writeb(readb(uart_control->pm_0_select_base) & ~DBG_PORT_PMUART0_BIT,
		       uart_control->pm_0_select_base);
		writeb(readb(uart_control->pm51_select_base) & ~DBG_PORT_PM51UART_BIT,
		       uart_control->pm51_select_base);
		writeb(readb(uart_control->fuart0_select_base) & ~DBG_PORT_FUART0_BIT,
		       uart_control->fuart0_select_base);
		writeb(readb(uart_control->piu_select_base_0) | DBG_PORT_PMUART1_BIT,
		       uart_control->piu_select_base_0);
	} else if (!strncasecmp(buf, "UART0", strlen("UART0"))) {
		writeb(readb(uart_control->pm_0_select_base) & ~DBG_PORT_PMUART0_BIT,
		       uart_control->pm_0_select_base);
		writeb(readb(uart_control->pm51_select_base) & ~DBG_PORT_PM51UART_BIT,
		       uart_control->pm51_select_base);
		writeb(readb(uart_control->fuart0_select_base) & ~DBG_PORT_FUART0_BIT,
		       uart_control->fuart0_select_base);
		writeb(readb(uart_control->piu_select_base_0) & ~DBG_PORT_PMUART1_BIT,
		       uart_control->piu_select_base_0);
		writeb(readb(uart_control->piu_select_base_1) | DBG_PORT_UART0_BIT,
		       uart_control->piu_select_base_1);
	} else if (!strncasecmp(buf, "UART1", strlen("UART1"))) {
		writeb(readb(uart_control->pm_0_select_base) & ~DBG_PORT_PMUART0_BIT,
		       uart_control->pm_0_select_base);
		writeb(readb(uart_control->pm51_select_base) & ~DBG_PORT_PM51UART_BIT,
		       uart_control->pm51_select_base);
		writeb(readb(uart_control->fuart0_select_base) & ~DBG_PORT_FUART0_BIT,
		       uart_control->fuart0_select_base);
		writeb(readb(uart_control->piu_select_base_0) & ~DBG_PORT_PMUART1_BIT,
		       uart_control->piu_select_base_0);
		writeb(readb(uart_control->piu_select_base_1) & ~DBG_PORT_UART0_BIT,
		       uart_control->piu_select_base_1);
		writeb(readb(uart_control->piu_select_base_1) | DBG_PORT_UART1_BIT,
		       uart_control->piu_select_base_1);
	} else if (!strncasecmp(buf, "DIG_MUX0", strlen("DIG_MUX0"))) {
		writeb(readb(uart_control->pm_0_select_base) & ~DBG_PORT_PMUART0_BIT,
		       uart_control->pm_0_select_base);
		writeb(readb(uart_control->pm51_select_base) & ~DBG_PORT_PM51UART_BIT,
		       uart_control->pm51_select_base);
		writeb(readb(uart_control->fuart0_select_base) & ~DBG_PORT_FUART0_BIT,
		       uart_control->fuart0_select_base);
		writeb(readb(uart_control->piu_select_base_0) & ~DBG_PORT_PMUART1_BIT,
		       uart_control->piu_select_base_0);
		writeb(readb(uart_control->piu_select_base_1) & ~DBG_PORT_UART0_BIT,
		       uart_control->piu_select_base_1);
		writeb(readb(uart_control->piu_select_base_1) & ~DBG_PORT_UART1_BIT,
		       uart_control->piu_select_base_1);
	} else
		ret = -EINVAL;

	spin_unlock_irqrestore(&uart_control->dbg_select_lock, flags);

	return ret;
}

static DEVICE_ATTR_RW(debug_port_select);

/*
 * UART DIG MUX selection sysfs group, in /sys/devices/platform/uart_control
 */
static ssize_t digmux_select_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct mtkuart_ctrl_data *uart_control = (struct mtkuart_ctrl_data *)dev_get_drvdata(dev);
	char *str = buf;
	char *end = buf + PAGE_SIZE;
	u8 digmux_sts_01, digmux_sts_23, digmux_sts_45;

	if (!uart_control) {
		dev_err(dev, "no UART control priv data to get information\n");
		return -EOPNOTSUPP;
	}

	if (!uart_control->digmux_select_base) {
		dev_err(dev, "cannot read dig mux selection register\n");
		return -EOPNOTSUPP;
	}

	digmux_sts_01 = readb(uart_control->digmux_select_base + DIGMUX_0_1_REG_IDX);
	digmux_sts_23 = readb(uart_control->digmux_select_base + DIGMUX_2_3_REG_IDX);
	digmux_sts_45 = readb(uart_control->digmux_select_base + DIGMUX_4_5_REG_IDX);

	dev_info(dev, "DIG MUX selection: %X:%X:%X:%X:%X:%X\n",
		 digmux_sts_01 & DIGMUX_SEL_MSK_L, digmux_sts_01 >> DIGMUX_SEL_WIDTH,
		 digmux_sts_23 & DIGMUX_SEL_MSK_L, digmux_sts_23 >> DIGMUX_SEL_WIDTH,
		 digmux_sts_45 & DIGMUX_SEL_MSK_L, digmux_sts_45 >> DIGMUX_SEL_WIDTH);

	str += scnprintf(str, end - str, "%X:%X:%X:%X:%X:%X\n",
			 digmux_sts_01 & DIGMUX_SEL_MSK_L, digmux_sts_01 >> DIGMUX_SEL_WIDTH,
			 digmux_sts_23 & DIGMUX_SEL_MSK_L, digmux_sts_23 >> DIGMUX_SEL_WIDTH,
			 digmux_sts_45 & DIGMUX_SEL_MSK_L, digmux_sts_45 >> DIGMUX_SEL_WIDTH);

	return (str - buf);
}

static ssize_t digmux_select_store(struct device *dev, struct device_attribute *attr,
				   const char *buf, size_t count)
{
	struct mtkuart_ctrl_data *uart_control = (struct mtkuart_ctrl_data *)dev_get_drvdata(dev);
	unsigned int port_idx, select_idx;
	char *str1, *str2 = (char *)buf;
	unsigned long flags;
	u8 reg;

	if (!buf)
		return -EINVAL;

	if (!uart_control) {
		dev_err(dev, "no UART control priv data to get information\n");
		return -EOPNOTSUPP;
	}

	if (!uart_control->digmux_select_base) {
		dev_err(dev, "cannot read dig mux switch register\n");
		return -EOPNOTSUPP;
	}

	// get input string
	str1 = strsep(&str2, ",");
	if (!str1 || !str2 || kstrtouint(str1, 0, &port_idx) || kstrtouint(str2, 0, &select_idx))
		return -EINVAL;

	if ((port_idx > DIGMUX_PORT_IDX_MAX) || (select_idx > DIGMUX_SEL_MAX))
		return -EINVAL;

	dev_notice(dev, "prepare set DIG MUX port %d select index %d\n", port_idx, select_idx);

	spin_lock_irqsave(&uart_control->digmux_select_lock, flags);

	if (port_idx < DIGMUX_PORT_IDX_2)
		reg = readb(uart_control->digmux_select_base + DIGMUX_0_1_REG_IDX);
	else if (port_idx < DIGMUX_PORT_IDX_4)
		reg = readb(uart_control->digmux_select_base + DIGMUX_2_3_REG_IDX);
	else
		reg = readb(uart_control->digmux_select_base + DIGMUX_4_5_REG_IDX);

	if (port_idx & 1) {
		reg &= DIGMUX_SEL_MSK_L;
		reg |= select_idx << DIGMUX_SEL_WIDTH;
	} else {
		reg &= DIGMUX_SEL_MSK_H;
		reg |= select_idx;
	}

	if (port_idx < DIGMUX_PORT_IDX_2)
		writeb(reg, uart_control->digmux_select_base + DIGMUX_0_1_REG_IDX);
	else if (port_idx < DIGMUX_PORT_IDX_4)
		writeb(reg, uart_control->digmux_select_base + DIGMUX_2_3_REG_IDX);
	else
		writeb(reg, uart_control->digmux_select_base + DIGMUX_4_5_REG_IDX);

	spin_unlock_irqrestore(&uart_control->digmux_select_lock, flags);

	return count;
}

static DEVICE_ATTR_RW(digmux_select);

static struct attribute *mtk8250_uart_control_attrs[] = {
	&dev_attr_console_disable.attr,
	&dev_attr_console_disable_rx.attr,
	&dev_attr_uart_switch.attr,
	&dev_attr_uart_switch_status.attr,
	&dev_attr_debug_port_select.attr,
	&dev_attr_digmux_select.attr,
	NULL,
};

static const struct attribute_group mtk8250_control_attr_group = {
	.attrs = mtk8250_uart_control_attrs,
};

static int mtk8250_dtv_probe_of(struct platform_device *pdev, struct uart_port *p,
				struct mtk8250_dtv_data *mtk_up)
{
	struct device_node *np = pdev->dev.of_node;
	unsigned int prop_value = 0;
	int ret;
	struct resource res;

	memset(p, 0, sizeof(*p));

	ret = of_address_to_resource(np, 0, &res);
	if (ret) {
		dev_err(&pdev->dev, "no registers defined\n");
		return -EINVAL;
	}
	p->mapbase = res.start;
	p->mapsize = resource_size(&res);
	p->membase = devm_ioremap(&pdev->dev, res.start, resource_size(&res));
	if (!p->membase)
		return -ENOMEM;

	ret = of_property_read_u32(np, "clock-frequency", &prop_value);
	if (ret < 0) {
		dev_err(&pdev->dev, "module clock frequency not defined in dts(%d)\n", ret);
		return ret;
	}
	p->uartclk = prop_value;

	ret = of_property_read_u32(np, "uart-device-id", &prop_value);
	if (ret < 0)
		dev_warn(&pdev->dev, "dynamic assign port number(%d)\n", ret);
	p->line = prop_value;

	ret = of_property_read_u32(np, "reg-shift", &prop_value);
	if (ret < 0) {
		dev_err(&pdev->dev, "module reg-shift not defined in dts(%d)\n", ret);
		return ret;
	}
	p->regshift = prop_value;

	/* whether support high speed UART */
	mtk_up->high_speed =
	    of_property_read_bool(pdev->dev.of_node, "high-speed-uart");
	dev_info(&pdev->dev, "%ssupport high speed uart\n", mtk_up->high_speed ? "" : "not ");

	/* module clock sources */
	mtk_up->clk_num = devm_clk_bulk_get_all(&pdev->dev, &mtk_up->clks);
	if (mtk_up->clk_num < 0) {
		dev_err(&pdev->dev, "fail to get bulk clks(%d)\n", mtk_up->clk_num);
		return mtk_up->clk_num;
	}
	dev_info(&pdev->dev, "got %d clks\n", mtk_up->clk_num);

	if (mtk_up->high_speed) {
		struct clk_hw *clk_hw;
		unsigned int synth_reg_addr;

		/* high speed uart */

		/* need enable UART clock synthesizer */
		ret = of_property_read_u32(np, "uart-synth-reg", &synth_reg_addr);
		if (ret < 0) {
			dev_err(&pdev->dev, "synth_reg_addr not defined in dts(%d)\n", ret);
			mtk_up->high_speed = false;
			return 0;
		}

		/* UART clock synthesizer clock parent index */
		ret = of_property_read_u32(np, "uart-synth-clk-index", &prop_value);
		if (ret < 0) {
			dev_err(&pdev->dev, "synth_reg_addr not defined in dts(%d)\n", ret);
			mtk_up->high_speed = false;
			return 0;
		}

		/* get suspend/resume clock parent */
		clk_hw = __clk_get_hw(mtk_up->clks[0].clk);
		if (!clk_hw) {
			dev_err(&pdev->dev, "Can not get clk_hw of high speed uart clk0\n");
			mtk_up->high_speed = false;
			return 0;
		}

		mtk_up->resume_clk = clk_hw_get_parent_by_index(__clk_get_hw(mtk_up->clks[0].clk),
						 prop_value)->clk;
		mtk_up->suspend_clk = clk_hw_get_parent_by_index(__clk_get_hw(mtk_up->clks[0].clk),
						 0)->clk;
		if (!mtk_up->resume_clk || !mtk_up->suspend_clk) {
			dev_err(&pdev->dev, "Neither suspend nor resume clk parent found\n");
			mtk_up->high_speed = false;
			return 0;
		}

		mtk_up->synth_base = devm_ioremap(&pdev->dev, synth_reg_addr, UART_SYNTH_REG_SIZE);
		if (IS_ERR(mtk_up->synth_base)) {
			dev_err(&pdev->dev, "failed to map synthesizer ioresource (%ld)\n",
				PTR_ERR(mtk_up->synth_base));
			mtk_up->high_speed = false;
		}
	}

	return 0;
}

/*
 * platform driver function
 */
static int __maybe_unused mtk8250_dtv_runtime_suspend(struct device *dev)
{
	struct mtk8250_dtv_data *mtk_up = dev_get_drvdata(dev);

	dev_info(dev, "disable clock/sw_en (%d/%d).\n", mtk_up->line, mtk_up->switched_line);

	/* disable all clks, include sw_en*/
	if (!mtk_up->clk_enabled)
		dev_info(dev, "clock already disabled\n");
	else {
		clk_bulk_disable_unprepare(mtk_up->clk_num, mtk_up->clks);
		mtk_up->clk_enabled = false;

		if (mtk_up->high_speed) {
			int ret;

			/* high speed uart, 1st clk is clk mux, select clk mux to default parent */
			ret = clk_set_parent(mtk_up->clks[0].clk, mtk_up->suspend_clk);
			if (ret < 0)
				dev_err(dev, "set baudrate parent clk to default clk failed(%d)\n",
					ret);

			/* disable synthesizer */
			writew(~(UART_SYNTH_SW_RSTZ_MSK | UART_SYNTH_NF_EN_MSK),
			       mtk_up->synth_base + UART_SYNTH_REG_CTRL);
		}
	}

	return 0;
}

static int __maybe_unused mtk8250_dtv_runtime_resume(struct device *dev)
{
	int ret = 0;
	struct mtk8250_dtv_data *mtk_up = dev_get_drvdata(dev);

	/* enable all clks, include sw_en*/
	if (mtk_up->clk_enabled)
		dev_info(dev, "clock already enabled\n");
	else {
		if (mtk_up->high_speed) {
			/* high speed uart, 1st clk is clk mux, select clk mux to uart clk synth. */
			ret = clk_set_parent(mtk_up->clks[0].clk, mtk_up->resume_clk);
			if (ret < 0) {
				dev_err(dev, "set baudrate parent clk to synth. clk failed(%d)\n",
					ret);
				return ret;
			}

			/* enable synthesizer */
			writew(UART_SYNTH_SW_RSTZ_MSK | UART_SYNTH_NF_EN_MSK,
			       mtk_up->synth_base + UART_SYNTH_REG_CTRL);
		}

		ret = clk_bulk_prepare_enable(mtk_up->clk_num, mtk_up->clks);
		if (ret < 0) {
			dev_err(dev, "bulk clk enable failed(%d)\n", ret);
			return ret;
		}
		mtk_up->clk_enabled = true;
	}

	dev_info(dev, "port clock/sw_en enabled (%d/%d).\n", mtk_up->line, mtk_up->switched_line);

	return ret;
}

static int __maybe_unused mtk8250_dtv_suspend(struct device *dev)
{
	struct mtk8250_dtv_data *mtk_up = dev_get_drvdata(dev);
	struct uart_port *port = &(serial8250_get_port(mtk_up->line)->port);
	struct circ_buf *xmit = &port->state->xmit;

	dev_info(dev, "suspend with remain %ld, real port %d\n", uart_circ_chars_pending(xmit),
		 mtk_up->switched_line);

	if (uart_console(port) && !console_suspend_enabled && !uart_circ_empty(xmit)) {
		int retry = CONSOLE_TTY_FLUSH_RETRY;

		while (!uart_circ_empty(xmit) && retry) {
			dev_info(dev, "buffer remain %ld\n", uart_circ_chars_pending(xmit));
			msleep(CONSOLE_TTY_FLUSH_PERIOD_MS);
			retry--;
		}

		if (!uart_circ_empty(xmit)) {
			dev_warn(dev, "clear buffer remain %ld\n", uart_circ_chars_pending(xmit));
			uart_circ_clear(xmit);
			port->ops->stop_tx(port);
		}
	}

	if (mtk_up->line != mtk_up->switched_line) {
		/*
		 * suspend at UART switched, restore switched status,
		 * swap UART port mtk_up->line and mtk_up->switched_line
		 */
		struct mtk8250_dtv_data *mtk_up2;
		int i, line1, line2;
		struct uart_8250_port *up, *up1 = NULL, *up2 = NULL;
		struct uart_port *p1, *p2;

		line1 = mtk_up->line;
		line2 = mtk_up->switched_line;
		dev_warn(dev, "suspend switch %d and %d\n", line1, line2);

		// get uart port handle, traverse all uart_8250_port
		for (i = 0 ; i < CONFIG_SERIAL_8250_NR_UARTS ; i++) {
			up = serial8250_get_port(i);
			if (up->port.line == line1)
				up1 = up;
			if (up->port.line == line2)
				up2 = up;
		}

		if (!up1 || !up2) {
			// port not found
			dev_crit(dev, "Cannot suspend due to port switch to invalid port id\n");
			return -EBUSY;
		}

		if (uart_check_swap_8250_port(line1, line2, up1, up2) < 0) {
			dev_alert(dev, "Restore UART switch (%d/%d) failed, restore manually.\n",
				  line1, line2);
			return -EBUSY;
		}

		p1 = &up1->port;
		p2 = &up2->port;

		if (uart_swap_8250_port(dev, up1, up2, tty_port_active(&p1->state->port)) < 0) {
			// switch failed, try switch back
			(void)uart_swap_8250_port(dev, up1, up2, tty_port_active(&p1->state->port));
			dev_alert(dev, "Restore UART switch (%d/%d) failed.\n", line1, line2);
			return -EBUSY;
		}

		dev_info(dev, "framework line %d switched to line %d\n", p1->line,
			 ((struct mtk8250_dtv_data *)(p1->private_data))->switched_line);
		dev_info(dev, "framework line %d switched to line %d\n", p2->line,
			 ((struct mtk8250_dtv_data *)(p2->private_data))->switched_line);

		// Mark for resume function restore switch status
		mtk_up->resume_line = line2;
		mtk_up2 = (struct mtk8250_dtv_data *)p2->private_data;
		mtk_up2->resume_line = line1;
	}

	serial8250_suspend_port(mtk_up->line);

	mtk_up->in_suspended = true;

	// always keep original console clock alive because some non-kernel code put out console log
	if (!uart_console(port))
		(void)mtk8250_dtv_runtime_suspend(dev);

	return 0;
}

static int __maybe_unused mtk8250_dtv_resume(struct device *dev)
{
	struct mtk8250_dtv_data *mtk_up = dev_get_drvdata(dev);
	struct uart_port *port = &(serial8250_get_port(mtk_up->line)->port);
	int ret;

	// original console clock always keep alive, no need enable
	if (!uart_console(port)) {
		ret = mtk8250_dtv_runtime_resume(dev);
		if (ret != 0)
			return ret;
	}

	serial8250_resume_port(mtk_up->line);
	mtk_up->in_suspended = false;

	if (mtk_up->switched_line != mtk_up->resume_line) {
		/*
		 * suspend at UART switched, restore to switched status before suspend,
		 * swap UART port mtk_up->line and mtk_up->resume_line
		 */
		struct mtk8250_dtv_data *mtk_up2;
		int i, line1, line2;
		struct uart_8250_port *up, *up1 = NULL, *up2 = NULL;
		struct uart_port *p1, *p2;

		port = &(serial8250_get_port(mtk_up->resume_line)->port);

		/*
		 * need resume_line is also resumed before restore switch status, otherwise restore
		 * switch status in resume_line port resume procedure
		 */
		mtk_up2 = (struct mtk8250_dtv_data *)port->private_data;
		if (mtk_up2->in_suspended) {
			dev_info(dev, "postpone restore switch port to %d\n", mtk_up->resume_line);
			goto resume_finish;
		}

		line1 = mtk_up->switched_line;
		line2 = mtk_up->resume_line;
		dev_warn(dev, "resume switch %d and %d\n", line1, line2);

		// get uart port handle, traverse all uart_8250_port
		for (i = 0 ; i < CONFIG_SERIAL_8250_NR_UARTS ; i++) {
			up = serial8250_get_port(i);
			if (up->port.line == line1)
				up1 = up;
			if (up->port.line == line2)
				up2 = up;
		}

		if (!up1 || !up2) {
			// port not found
			dev_crit(dev, "Cannot resume due to port switch to invalid port id\n");
			return -EBUSY;
		}

		if (uart_check_swap_8250_port(line1, line2, up1, up2) < 0) {
			dev_alert(dev, "Restore UART switch (%d/%d) failed, restore manually.\n",
				  line1, line2);
			return -EBUSY;
		}

		p1 = &up1->port;
		p2 = &up2->port;

		dev_warn(dev, "switch %d and %d activate %d\n", line1, line2,
			 tty_port_active(&p1->state->port));
		if (uart_swap_8250_port(dev, up1, up2, tty_port_active(&p1->state->port)) < 0) {
			// switch failed, try switch back
			(void)uart_swap_8250_port(dev, up1, up2, tty_port_active(&p1->state->port));
			dev_alert(dev, "Restore UART switch (%d/%d) failed.\n", line1, line2);
			return -EBUSY;
		}

		dev_info(dev, "framework line %d switched to line %d\n", p1->line,
			 ((struct mtk8250_dtv_data *)(p1->private_data))->switched_line);
		dev_info(dev, "framework line %d switched to line %d\n", p2->line,
			 ((struct mtk8250_dtv_data *)(p2->private_data))->switched_line);
	}

resume_finish:

	dev_info(dev, "port resume done, real port %d.\n", mtk_up->switched_line);

	return 0;
}

static const struct dev_pm_ops mtk8250_dtv_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(mtk8250_dtv_suspend, mtk8250_dtv_resume)
	SET_RUNTIME_PM_OPS(mtk8250_dtv_runtime_suspend, mtk8250_dtv_runtime_resume, NULL)
};

static int mtk8250_dtv_probe(struct platform_device *pdev)
{
	struct uart_8250_port uart = {};
	struct mtk8250_dtv_data *mtk_up;
	int err;
	int irq_no;
	u32 ori_console_verbose;
	bool ori_console_need_disable;

	/* get the interrupt */
	irq_no = platform_get_irq(pdev, 0);
	if (irq_no < 0) {
		dev_err(&pdev->dev, "no irq defined\n");
		return -EINVAL;
	}
	dev_info(&pdev->dev, "irq %d\n", irq_no);

	mtk_up = devm_kzalloc(&pdev->dev, sizeof(*mtk_up), GFP_KERNEL);
	if (!mtk_up)
		return -ENOMEM;

	err = mtk8250_dtv_probe_of(pdev, &uart.port, mtk_up);
	if (err < 0)
		return err;

	spin_lock_init(&uart.port.lock);
	uart.port.serial_in = mtk8250_dtv_mem_serial_in;
	uart.port.serial_out = mtk8250_dtv_mem_serial_out;
	uart.port.handle_irq = mtk8250_dtv_handle_irq;
	uart.port.set_termios = mtk8250_dtv_set_termios;
	uart.port.startup = mtk8250_dtv_startup;
	uart.port.shutdown = mtk8250_dtv_shutdown;
	uart.port.pm = mtk8250_dtv_do_pm;
	uart.port.irq = (unsigned int)irq_no;
	uart.port.type = PORT_16550A;
	uart.port.flags = UPF_FIXED_PORT | UPF_FIXED_TYPE | UPF_NO_THRE_TEST;
	uart.port.fifosize = MTK_UART_FIFO_SZ;
	uart.tx_loadsz = MTK_UART_FIFO_SZ;
	uart.capabilities = UART_CAP_AFE | UART_CAP_FIFO;
	uart.port.dev = &pdev->dev;
	uart.port.iotype = UPIO_MEM;
	uart.port.private_data = mtk_up;

	platform_set_drvdata(pdev, mtk_up);

	pm_runtime_enable(&pdev->dev);

	if (uart.port.line == 0) {
		// temporary enable console because 8250 initial sequence may write UART TXD/RXD
		ori_console_verbose = mtk_console_verbose;
		ori_console_need_disable = console_need_disable;
		mtk_console_verbose = 1;
		console_need_disable = false;
	}

	mtk_up->clk_enabled = false;

	dev_info(&pdev->dev, "try register port %d\n", uart.port.line);
	mtk_up->line = serial8250_register_8250_port(&uart);
	mtk_up->switched_line = mtk_up->line;
	mtk_up->resume_line = mtk_up->line;
	mtk_up->in_suspended = false;
	if (mtk_up->line < 0) {
		dev_err(&pdev->dev, "register 8250 port failed (%d)\n", mtk_up->line);

		if (uart.port.line == 0) {
			// restore console enable/disable status
			mtk_console_verbose = ori_console_verbose;
			console_need_disable = ori_console_need_disable;
		}

		pm_runtime_disable(&pdev->dev);

		return mtk_up->line;
	}

	dev_info(&pdev->dev, "success register to port %d type %d flags 0x%08X\n", mtk_up->line,
		 uart.port.type, uart.port.flags);

	if (uart.port.line == 0) {
		// restore console enable/disable status
		mtk_console_verbose = ori_console_verbose;
		console_need_disable = ori_console_need_disable;
	}

	mtk_up->port = serial8250_get_port(mtk_up->line);

	return 0;
}

static int mtk8250_dtv_remove(struct platform_device *pdev)
{
	struct mtk8250_dtv_data *mtk_up = platform_get_drvdata(pdev);

	serial8250_unregister_port(mtk_up->line);

	pm_runtime_get_sync(&pdev->dev);
	pm_runtime_disable(&pdev->dev);
	pm_runtime_put_noidle(&pdev->dev);

	if (!pm_runtime_status_suspended(&pdev->dev))
		mtk8250_dtv_runtime_suspend(&pdev->dev);

	return 0;
}

static const struct of_device_id mtk8250_dtv_of_match[] = {
	{ .compatible = "mediatek_tv,ns16550" },
	{ /* Sentinel */ }
};
MODULE_DEVICE_TABLE(of, mtk8250_dtv_of_match);

static struct platform_driver mtk8250_dtv_platform_driver = {
	.driver = {
		.name = "mtk16550",
		.pm = &mtk8250_dtv_pm_ops,
		.of_match_table = mtk8250_dtv_of_match,
	},
	.probe = mtk8250_dtv_probe,
	.remove = mtk8250_dtv_remove,
};

#ifdef CONFIG_SERIAL_8250_CONSOLE
static int __init early_mtk8250_dtv_setup(struct earlycon_device *device, const char *options)
{
	return early_serial8250_setup(device, NULL);
}
OF_EARLYCON_DECLARE(mtk8250_dtv, "mediatek_tv,ns16550", early_mtk8250_dtv_setup);
#endif

/*
 *  Console UART output gather functions
 */

/*
 * Note that with only one concurrent reader and one concurrent
 * writer, you don't need extra locking to use these w/r api.
 */
static ssize_t gather_dev_write(struct file *filp, const char __user *from, size_t cnt,
				loff_t *f_pos)
{
	struct con_gather *gather = container_of(filp->private_data, struct con_gather, miscdev);
	unsigned int from_cnt;
	ssize_t ret = cnt;

	if (gather->drv_write_test) {
		ret = kfifo_from_user(&gather->buff, from, (unsigned int)cnt, &from_cnt);
		if (ret) {
			pr_err("log_dev: copy data from user failed\n");
			return -EINVAL;
		}

		*f_pos += from_cnt;
		ret = from_cnt;
	}

	return ret;
}

/*
 * Note that with only one concurrent reader and one concurrent
 * writer, you don't need extra locking to use these w/r api.
 */
static ssize_t gather_dev_read(struct file *filp, char __user *to, size_t cnt, loff_t *f_pos)
{
	struct con_gather *gather = container_of(filp->private_data, struct con_gather, miscdev);
	unsigned int to_cnt;
	ssize_t ret;

	ret = kfifo_to_user(&gather->buff, to, (unsigned int)cnt, &to_cnt);
	if (ret != 0) {
		pr_err("error: "CONSOLE_OUTPUT_GATHER_NAME": copy data to user failed\n");
	} else {
		*f_pos += to_cnt;
		ret = to_cnt;
	}

	return ret;
}

static int gather_dev_open(struct inode *inode, struct file *filp)
{
	struct con_gather *gather = container_of(filp->private_data, struct con_gather, miscdev);

	gather->opened = true;
	pr_info("console output gather device opened\n");

	kfifo_reset_out(&gather->buff);
	(void)kfifo_in(&gather->buff, "Start gather logging\n", strlen("Start gather logging\n"));
	gather->filp = filp;

	return 0;
}

static int gather_dev_release(struct inode *inode, struct file *filp)
{
	struct con_gather *gather = container_of(filp->private_data, struct con_gather, miscdev);

	gather->opened = false;
	pr_info("console output gather device released\n");
	return 0;
}

static const struct file_operations gather_dev_fops = {
	.owner = THIS_MODULE,
	.read = gather_dev_read,
	.write = gather_dev_write,
	.open = gather_dev_open,
	.release = gather_dev_release,
};

/*
 * switch preserve original output when console output gather enabled, in
 * /sys/devices/virtual/"CONSOLE_OUTPUT_GATHER_NAME"
 */
static ssize_t preserve_output_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct con_gather *gather = container_of(dev_get_drvdata(dev), struct con_gather, miscdev);
	char *str = buf;
	char *end = buf + PAGE_SIZE;

	str += scnprintf(str, end - str, "%d\n", gather->preserve_output);
	return (str - buf);
}

static ssize_t preserve_output_store(struct device *dev, struct device_attribute *attr,
				     const char *buf, size_t count)
{
	struct con_gather *gather = container_of(dev_get_drvdata(dev), struct con_gather, miscdev);
	unsigned long flags;
	bool preserve_output = false;
	int ret;

	if (!buf)
		return -EINVAL;

	ret = kstrtobool(buf, &preserve_output);
	if (ret == 0) {
		spin_lock_irqsave(&gather->setting_lock, flags);
		gather->preserve_output = preserve_output;
		spin_unlock_irqrestore(&gather->setting_lock, flags);

		if (!gather->preserve_output) {
			int i;
			struct uart_8250_port *up;
			struct mtk8250_dtv_data *mtk_up;

			// get console uart port handle, traverse all uart_8250_port
			for (i = 0 ; i < CONFIG_SERIAL_8250_NR_UARTS ; i++) {
				up = serial8250_get_port(i);
				if (uart_console(&up->port))
					break;
			}

			if (i < CONFIG_SERIAL_8250_NR_UARTS) {
				/*
				 * console uart port found, trigger timer in console disable(silent)
				 * state
				 */
				mtk_up = dev_get_drvdata(up->port.dev);
				mod_timer(&mtk_up->silent_tmr,
					  jiffies + uart_poll_timeout(&up->port));
			}
		}
	}

	return (ret < 0) ? ret : count;
}

static DEVICE_ATTR_RW(preserve_output);

/*
 * switch enable console UART output gather test function, in
 * /sys/devices/virtual/"CONSOLE_OUTPUT_GATHER_NAME"
 */
static ssize_t output_test_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct con_gather *gather = container_of(dev_get_drvdata(dev), struct con_gather, miscdev);
	char *str = buf;
	char *end = buf + PAGE_SIZE;

	str += scnprintf(str, end - str, "%d\n", gather->drv_write_test);
	return (str - buf);
}

static ssize_t output_test_store(struct device *dev, struct device_attribute *attr,
				 const char *buf, size_t count)
{
	struct con_gather *gather = container_of(dev_get_drvdata(dev), struct con_gather, miscdev);
	unsigned long flags;
	bool drv_write_test = false;
	int ret;

	if (!buf)
		return -EINVAL;

	ret = kstrtobool(buf, &drv_write_test);
	if (ret == 0) {
		spin_lock_irqsave(&gather->setting_lock, flags);
		gather->drv_write_test = drv_write_test;
		spin_unlock_irqrestore(&gather->setting_lock, flags);
	}

	return (ret < 0) ? ret : count;
}

static DEVICE_ATTR_RW(output_test);

/*
 * clear console UART output gather fifo, in /sys/devices/virtual/"CONSOLE_OUTPUT_GATHER_NAME"
 */
static ssize_t clear_fifo_store(struct device *dev, struct device_attribute *attr,
				const char *buf, size_t count)
{
	struct con_gather *gather = container_of(dev_get_drvdata(dev), struct con_gather, miscdev);

	if (!buf)
		return -EINVAL;

	dev_info(dev, "remain fifo element count %d\n", kfifo_len(&gather->buff));
	kfifo_reset_out(&gather->buff);

	return count;
}

static DEVICE_ATTR_WO(clear_fifo);

static ssize_t gather_status_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct con_gather *gather = container_of(dev_get_drvdata(dev), struct con_gather, miscdev);
	char *str = buf;
	char *end = buf + PAGE_SIZE;

	str += scnprintf(str, end - str,
			 "opened: %c\nfifo size %d\noutput test: %c\npreserve output: %c\n",
			 (gather->opened) ? 'Y' : 'N', kfifo_len(&gather->buff),
			 (gather->drv_write_test) ? 'Y' : 'N',
			 (gather->preserve_output) ? 'Y' : 'N');

	return (str - buf);
}
static DEVICE_ATTR_RO(gather_status);

static struct attribute *gather_dev_attrs[] = {
	&dev_attr_preserve_output.attr,
	&dev_attr_output_test.attr,
	&dev_attr_clear_fifo.attr,
	&dev_attr_gather_status.attr,
	NULL,
};

ATTRIBUTE_GROUPS(gather_dev);

/*
 *  UART module init function, include proprietary control
 */
static int __init mtk8250_dtv_init(void)
{
	int ret = 0;
	struct mtkuart_ctrl_data *uart_control;

	if (console_need_disable)
		// disable uart and set loglevel is 0
		console_silent();

	/*
	 * create sysfs group for proprietary control, if sysfs group creation failed, proprietary
	 * uart control function can not work but UART still works.
	 */
	uat_control_dev = platform_device_alloc("uart_control", -1);
	if (!uat_control_dev) {
		pr_err("Can not create platform device for uart control.\n");
		goto end_ctrl_dev;
	}

	uart_control = devm_kzalloc(&uat_control_dev->dev, sizeof(*uart_control), GFP_KERNEL);
	if (uart_control) {
		uart_control->pm_0_select_base = ioremap(MTK_UART_DBGPORT_PM0_BASE, 1);
		uart_control->pm51_select_base = ioremap(MTK_UART_DBGPORT_PM0_BASE, 1);
		uart_control->fuart0_select_base = ioremap(MTK_UART_DBGPORT_FUART_BASE, 1);
		uart_control->piu_select_base_0 = ioremap(MTK_UART_DBGPORT_PIU_BASE_0, 1);
		uart_control->piu_select_base_1 = ioremap(MTK_UART_DBGPORT_PIU_BASE_1, 1);
		uart_control->digmux_select_base = ioremap(MTK_UART_DIGMUX_BASE, DIGMUX_REG_SZ);

		spin_lock_init(&uart_control->dbg_select_lock);
		spin_lock_init(&uart_control->digmux_select_lock);

		platform_set_drvdata(uat_control_dev, uart_control);
	}

	ret = platform_device_add(uat_control_dev);
	if (ret) {
		pr_err("Can not add platform device for uart control.\n");
		goto put_ctrl_dev;
	}

	ret = sysfs_create_group(&uat_control_dev->dev.kobj, &mtk8250_control_attr_group);
	if (ret < 0) {
		pr_err("failed to create file group for uart control.(%d)\n", ret);
		platform_device_del(uat_control_dev);

put_ctrl_dev:
		platform_device_put(uat_control_dev);
		uat_control_dev = NULL;
	}

end_ctrl_dev:
	/*
	 * create UART console output gather device, UART console output cannot redirect to it if
	 * device creation failed, but UART still works.
	 */
	con_out_gather = kzalloc(sizeof(*con_out_gather), GFP_KERNEL);
	if (!con_out_gather)
		goto end_gather_dev;

	con_out_gather->buff_sz = roundup_pow_of_two(CONSOLE_OUTPUT_GATHER_DEF_SZ);
	con_out_gather->buff_ptr = kmalloc(con_out_gather->buff_sz, GFP_KERNEL);
	if (!con_out_gather->buff_ptr)
		goto free_gather_dev;

	ret = kfifo_init(&con_out_gather->buff, con_out_gather->buff_ptr, con_out_gather->buff_sz);
	if (ret)
		goto free_gather_kfifo;

	con_out_gather->miscdev.minor = MISC_DYNAMIC_MINOR;
	con_out_gather->miscdev.name = CONSOLE_OUTPUT_GATHER_NAME;
	con_out_gather->miscdev.fops = &gather_dev_fops;
	con_out_gather->miscdev.groups = gather_dev_groups;
	ret = misc_register(&con_out_gather->miscdev);
	if (ret) {
		pr_err("failed to register console output gather device(%d)\n", ret);
		goto free_gather_kfifo;
	}

	con_out_gather->opened = false;
	con_out_gather->preserve_output = true;
	con_out_gather->drv_write_test = false;
	spin_lock_init(&con_out_gather->setting_lock);

	goto end_gather_dev;

free_gather_kfifo:
	kfree(con_out_gather->buff_ptr);

free_gather_dev:
	kfree(con_out_gather);
	con_out_gather = NULL;

end_gather_dev:
	ret = platform_driver_register(&mtk8250_dtv_platform_driver);
	if (ret) {
		pr_err("8250 platform driver register failed (%d)\n", ret);
		return ret;
	}

	return ret;
}
module_init(mtk8250_dtv_init);

static void __exit mtk8250_dtv_exit(void)
{
	platform_driver_unregister(&mtk8250_dtv_platform_driver);

	if (con_out_gather) {
		misc_deregister(&con_out_gather->miscdev);
		kfree(con_out_gather->buff_ptr);
		kfree(con_out_gather);
		con_out_gather = NULL;
	}

	if (uat_control_dev) {
		struct mtkuart_ctrl_data *uart_control;

		uart_control = (struct mtkuart_ctrl_data *)platform_get_drvdata(uat_control_dev);
		iounmap(uart_control->pm_0_select_base);
		iounmap(uart_control->pm51_select_base);
		iounmap(uart_control->fuart0_select_base);
		iounmap(uart_control->piu_select_base_0);
		iounmap(uart_control->piu_select_base_1);
		iounmap(uart_control->digmux_select_base);

		sysfs_remove_group(&uat_control_dev->dev.kobj, &mtk8250_control_attr_group);
		platform_device_del(uat_control_dev);
		platform_device_put(uat_control_dev);
		uat_control_dev = NULL;
	}
}
module_exit(mtk8250_dtv_exit);

MODULE_AUTHOR("jefferry.yen@mediatek.com");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Mediatek dtv 8250 serial port driver");

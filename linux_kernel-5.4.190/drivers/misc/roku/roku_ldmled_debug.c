#include <linux/errno.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/timex.h>
#include <linux/sched/clock.h>
#include "roku_ldmled_debug.h"

struct buf_flag_pos {
    uint16_t log_pos;
    uint16_t replay_pos;
};

static const struct buf_flag_pos buf_flag_pos[BUF_LAST] = {
    {LOG_LED_INPUT, REPLAY_LED_INPUT},
    {LOG_PWM_OUTPUT, REPLAY_PWM_OUTPUT},
    {LOG_CURRENT_OUTPUT, REPLAY_CURRENT_OUTPUT},
};

static struct frame_payload *ring_buffer_get_frame(struct ring_buffer *buffer,
                                                   int index) {
    int zones = buffer->row * buffer->column;
    int frame_size = sizeof(struct frame_payload) + sizeof(uint16_t) * zones;
    char *buffer_base = (char *)buffer->frame;
    index = index % buffer->total_frames;
    return (struct frame_payload *)(buffer_base + index * frame_size);
}

static struct ring_buffer *ring_buffer_create(enum buffer_type buf_type,
                                              int log_frames, int row,
                                              int column) {
    int zones = row * column;
    int frame_size = sizeof(struct frame_payload) + sizeof(uint16_t) * zones;
    size_t buf_size = sizeof(struct ring_buffer) + frame_size * log_frames;
    struct ring_buffer *buffer =
            (struct ring_buffer *)kmalloc(buf_size, GFP_KERNEL);
    if (!buffer) return NULL;
    memset(buffer, 0, buf_size);
    buffer->total_frames = log_frames;
    buffer->buf_size = buf_size;
    buffer->buf_type = buf_type;
    buffer->write_index = 0;
    buffer->row = row;
    buffer->column = column;
    printk("Creating ring buffer... type is:%d log_fames:%d, total_zone:%d, "
           "malloc bytes are:%ld\n",
           buf_type, log_frames, zones, buf_size);
    return buffer;
}

static void ring_buffer_reset(struct ring_buffer *buffer) {
    buffer->frame_count = 0;
}

static int debug_create_log_buffer(struct ldmled_debug *debug) {
    int i = 0;
    for (i = 0; i < BUF_LAST; ++i) {
        bool need_buf = test_bit(buf_flag_pos[i].log_pos, &debug->debug_mode)
                || test_bit(buf_flag_pos[i].replay_pos, &debug->debug_mode);
        if (!need_buf) continue;

        if (!debug->buffers[i] && debug->log_frames[i]) {
            debug->buffers[i] = ring_buffer_create((enum buffer_type)i,
                                                   debug->log_frames[i],
                                                   debug->row, debug->column);
            if (!debug->buffers[i]) return -ENOMEM;
        }
        if (debug->buffers[i]) {
            ring_buffer_reset(debug->buffers[i]);
        }
    }
    return 0;
}

void debug_free_buffer(struct ldmled_debug *debug) {
    int i = 0;
    for (i = 0; i < BUF_LAST; ++i) {
        if (debug->buffers[i]) {
            kfree(debug->buffers[i]);
            debug->buffers[i] = NULL;
        }
    }
}

int debug_log_frame(struct ldmled_debug *debug, enum buffer_type buf_type,
                    uint16_t *data, struct sysstateregs* sys_state) {
    mutex_lock(&debug->mutex);
    struct ring_buffer *buffer = debug->buffers[buf_type];
    if (!buffer) {
        mutex_unlock(&debug->mutex);
        return -EIO;
    }
    int zones = buffer->row * buffer->column;
    struct frame_payload *frame =
            ring_buffer_get_frame(buffer, buffer->write_index);
    uint16_t *zone_base = frame->zone_data;
    int i = 0;
    while (i++ < zones) *zone_base++ = *data++;

    if (sys_state) {
        memcpy((void *)&frame->sys_state, (void *)sys_state, sizeof(struct sysstateregs));
    }
    frame->frame_id = debug->frame_tick;
    frame->ts_nsec = local_clock();
    buffer->write_index++;
    buffer->write_index = buffer->write_index % buffer->total_frames;
    buffer->frame_count++;
    if (test_bit(LOG_ONCE, &debug->debug_mode)
        && buffer->frame_count >= buffer->total_frames) {
        clear_bit(buf_flag_pos[buf_type].log_pos, &debug->debug_mode);
    }
    mutex_unlock(&debug->mutex);
    return 0;
}

int debug_replay_frame(struct ldmled_debug *debug, enum buffer_type buf_type,
                       uint16_t *data, struct sysstateregs* sys_state) {
    mutex_lock(&debug->mutex);
    struct ring_buffer *buffer = debug->buffers[buf_type];
    if (!buffer) {
        mutex_unlock(&debug->mutex);
        return -EIO;
    }
    int zones = buffer->row * buffer->column;
    struct frame_payload *frame =
            ring_buffer_get_frame(buffer, buffer->write_index);
    uint16_t *zone_base = frame->zone_data;
    int i = 0;
    while (i++ < zones) *data++ = *zone_base++;

    if (sys_state) {
        memcpy((void *)sys_state, (void *)&frame->sys_state, sizeof(struct sysstateregs));
    }
    buffer->write_index++;
    buffer->write_index = buffer->write_index % buffer->total_frames;
    mutex_unlock(&debug->mutex);
    return 0;
}

int debug_read_buffer(struct ldmled_debug *debug, enum buffer_type buf_type,
                      uint8_t *buf, uint32_t len) {
    mutex_lock(&debug->mutex);
    struct ring_buffer *buffer = debug->buffers[buf_type];
    if (!buffer) {
        mutex_unlock(&debug->mutex);
        return -EIO;
    }
    if (len > buffer->buf_size) {
        pr_warn("Buffer is too large\n");
        len = buffer->buf_size;
    }
    memcpy(buf, buffer, len);
    mutex_unlock(&debug->mutex);
    return len;
}

int debug_write_buffer(struct ldmled_debug *debug, enum buffer_type buf_type,
                      uint8_t *buf, uint32_t len) {
    mutex_lock(&debug->mutex);
    struct ring_buffer *buffer = debug->buffers[buf_type];
    if (!buffer) {
        mutex_unlock(&debug->mutex);
        return -EIO;
    }
    struct ring_buffer *src_buffer = (struct ring_buffer*)(buf);
    if (src_buffer->buf_type != buffer->buf_type ||
             src_buffer->row != buffer->row ||
             src_buffer->column != buffer->column) {
        pr_err("Ring buffer configuration doesn't match\n");
        mutex_unlock(&debug->mutex);
        return -EINVAL;
    }
    if (len > buffer->buf_size) {
        pr_warn("Source buffer is too large\n");
        len = buffer->buf_size;
        src_buffer->buf_size = buffer->buf_size;
        src_buffer->total_frames = buffer->total_frames;
    } else if (len < buffer->buf_size) {
        pr_warn("Source buffer is smaller than target\n");
        //keep current buf_size, not to shrink
        src_buffer->buf_size = buffer->buf_size;
    }
    memcpy(buffer, buf, len);
    mutex_unlock(&debug->mutex);
    return len;
}

int debug_get_debug_mode(struct ldmled_debug *debug) {
    mutex_lock(&debug->mutex);
    int val = 0;
    val = debug->debug_mode;
    mutex_unlock(&debug->mutex);
    return val;
}

int debug_set_debug_mode(struct ldmled_debug *debug, int debug_mode) {
    mutex_lock(&debug->mutex);
    bool prev = test_bit(PROFILING_ON, &debug->debug_mode);
    debug->debug_mode = debug_mode;
    int ret = 0;
    ret = debug_create_log_buffer(debug);
    if (ret) {
        debug_free_buffer(debug);
        debug->debug_mode = 0;
        pr_err("[ldmled] Error. can't create log buffer. ret=%d\n", ret);
    }
    mutex_unlock(&debug->mutex);
    if (test_bit(PROFILING_ON, &debug->debug_mode)) {
        if (!debug->profiling) {
            debug_profiling_create(debug, DEFAULT_PROFILING_FRAMES);
        } else if (!prev) {
            debug_profiling_reset(debug);
        }
    }
    return ret;
}

int debug_get_log_frames(struct ldmled_debug *debug,
                         uint16_t log_frames[BUF_LAST]) {
    mutex_lock(&debug->mutex);
    memcpy(log_frames, debug->log_frames, sizeof(debug->log_frames));
    mutex_unlock(&debug->mutex);
    return 0;
}

int debug_set_log_frames(struct ldmled_debug *debug,
                         uint16_t log_frames[BUF_LAST]) {
    mutex_lock(&debug->mutex);
    debug_free_buffer(debug);
    int i = 0;
    for (i = 0; i < BUF_LAST; ++i) debug->log_frames[i] = log_frames[i];
    int ret = 0;
    ret = debug_create_log_buffer(debug);
    if (ret) {
        pr_err("[ldmled] Error. can't create log buffer. ret=%d\n", ret);
        debug_free_buffer(debug);
        memset(debug->log_frames, 0, sizeof(debug->log_frames));
    }
    mutex_unlock(&debug->mutex);
    return ret;
}

void debug_init(struct ldmled_debug *debug, int row, int column) {
    debug->row = row;
    debug->column = column;
    mutex_init(&debug->mutex);
}

void debug_run_demo_mode(struct ldmled_debug *debug, uint16_t *led_buf) {
    mutex_lock(&debug->mutex);
    int zones = debug->row * debug->column;
    int i = 0;
    for (i = 0; i < zones; i++)
        led_buf[i] = (i == debug->demo_mode.cur_led) ? 0xffff : 0;
    if (!(debug->frame_tick % debug->demo_mode.interval_frames)) {
        debug->demo_mode.cur_led++;
        debug->demo_mode.cur_led = debug->demo_mode.cur_led % zones;
    }
    mutex_unlock(&debug->mutex);
}

void debug_set_test_pattern(struct ldmled_debug* debug, struct ldmled_test_pattern* test_pattern, uint16_t* zone_buf[BUF_LAST]) {
    mutex_lock(&debug->mutex);
    int x = 0, y = 0;
    int i = 0;
    printk("test pattern is set %d %d %d\n", test_pattern->pattern_mode, test_pattern->pwm_win, test_pattern->pwm_bg);
    if (test_pattern->pattern_mode != 0) {
        if (test_pattern->pattern_mode == 1) {
            for (y = 0; y < debug->row; y++) {
                for (x = 0; x < debug->column; x++, i++) {
                    if ( x >= test_pattern->win_x_start && x <= test_pattern->win_x_end
                        && y >= test_pattern->win_y_start && y <= test_pattern->win_y_end) {
                        zone_buf[BUF_PWM_OUTPUT][i] = test_pattern->pwm_win;
                        zone_buf[BUF_CURRENT_OUTPUT][i] = test_pattern->current_win;
                    } else {
                        zone_buf[BUF_PWM_OUTPUT][i] = test_pattern->pwm_bg;
                        zone_buf[BUF_CURRENT_OUTPUT][i] = test_pattern->current_bg;
                    }
                }
            }
        } else if (test_pattern->pattern_mode == 2) {
            for (y = 0; y < debug->row; y++) {
                for (x = 0; x < debug->column; x++, i++) {
                    if ( x >= test_pattern->win_x_start && x <= test_pattern->win_x_end
                        && y >= test_pattern->win_y_start && y <= test_pattern->win_y_end) {  // inside window 1
                        zone_buf[BUF_PWM_OUTPUT][i] = test_pattern->pwm_win;
                        zone_buf[BUF_CURRENT_OUTPUT][i] = test_pattern->current_win;
                    } else if ( x >= test_pattern->win2_x_start && x <= test_pattern->win2_x_end
                        && y >= test_pattern->win2_y_start && y <= test_pattern->win2_y_end) { // inside window 2
                        zone_buf[BUF_PWM_OUTPUT][i] = test_pattern->pwm_win2;
                        zone_buf[BUF_CURRENT_OUTPUT][i] = test_pattern->current_win2;
                    } else {
                        zone_buf[BUF_PWM_OUTPUT][i] = test_pattern->pwm_bg;
                        zone_buf[BUF_CURRENT_OUTPUT][i] = test_pattern->current_bg;
                    }
                }
            }
        } else if (test_pattern->pattern_mode == 3) {
            for (y = 0; y < debug->row; y++) {
                for (x = 0; x < debug->column; x++, i++) {
                    if ( i == test_pattern->pwm_win2) {
                        zone_buf[BUF_PWM_OUTPUT][i] = test_pattern->pwm_win;
                        zone_buf[BUF_CURRENT_OUTPUT][i] = test_pattern->current_win;
                    } else {
                        zone_buf[BUF_PWM_OUTPUT][i] = test_pattern->pwm_bg;
                        zone_buf[BUF_CURRENT_OUTPUT][i] = test_pattern->current_bg;
                    }
                }
            }
        } else if (test_pattern->pattern_mode == 4) {
            for (y = 0; y < debug->row; y++) {
                for (x = 0; x < debug->column; x++, i++) {
                    bool match = x % 2 ? true : false;
                    zone_buf[BUF_PWM_OUTPUT][i] = match ? test_pattern->pwm_win
                                                        : test_pattern->pwm_bg;
                    zone_buf[BUF_CURRENT_OUTPUT][i] = match ? test_pattern->current_win
                                                            : test_pattern->current_bg;
                }
            }
        } else if (test_pattern->pattern_mode == 5) {
            for (y = 0; y < debug->row; y++) {
                for (x = 0; x < debug->column; x++, i++) {
                    bool match = x % 2 ? false : true;
                    zone_buf[BUF_PWM_OUTPUT][i] = match ? test_pattern->pwm_win
                                                        : test_pattern->pwm_bg;
                    zone_buf[BUF_CURRENT_OUTPUT][i] = match ? test_pattern->current_win
                                                            : test_pattern->current_bg;
                }
            }
        }
        debug->debug_mode = (1 << PATTERN_MODE);
    } else {
        debug->debug_mode = 0;
    }
    mutex_unlock(&debug->mutex);
}

int debug_profiling_free(struct ldmled_debug* debug) {
    if (debug->profiling) {
        kfree(debug->profiling);
        debug->profiling = NULL;
    }
    return 0;
}

int debug_profiling_create(struct ldmled_debug* debug, int total_frames) {
    if (total_frames <= 0) return -EINVAL;
    mutex_lock(&debug->mutex);
    debug_profiling_free(debug);

    int frame_size = sizeof(struct frame_timestamp);
    size_t buf_size = sizeof(struct profiling_buffer) + frame_size * total_frames;
    struct profiling_buffer *buffer =
            (struct profiling_buffer *)kmalloc(buf_size, GFP_KERNEL);
    if (!buffer) {
        mutex_unlock(&debug->mutex);
        return -ENOMEM;
    }
    memset(buffer, 0, buf_size);
    buffer->total_frames = total_frames;
    buffer->buf_size = buf_size;
    buffer->write_index = 0;
    printk("Creating profiling buffer... total_frames:%d\n", total_frames);
    debug->profiling = buffer;
    mutex_unlock(&debug->mutex);
    return 0;
}

int debug_profiling_log(struct ldmled_debug* debug, int index, bool last) {
    if (index > MAX_TS) return -EINVAL;
    mutex_lock(&debug->mutex);
    struct profiling_buffer *buffer = debug->profiling;
    if (!buffer) {
        mutex_unlock(&debug->mutex);
        return -EIO;
    }
    buffer->ts[buffer->write_index].nsec[index] = local_clock();
    if (last) {
        buffer->ts[buffer->write_index].frame_id = debug->frame_tick;
        buffer->write_index++;
        if (buffer->write_index >= buffer->total_frames) {
            clear_bit(PROFILING_ON, &debug->debug_mode);
        }
    }
    mutex_unlock(&debug->mutex);
    return 0;
}

int debug_profiling_read(struct ldmled_debug* debug, uint8_t *buf, uint32_t len) {
    mutex_lock(&debug->mutex);
    struct profiling_buffer *buffer = debug->profiling;
    if (!buffer) {
        mutex_unlock(&debug->mutex);
        return -EIO;
    }
    if (len > buffer->buf_size) len = buffer->buf_size;
    memcpy(buf, buffer, len);
    mutex_unlock(&debug->mutex);
    return len;
}

int debug_profiling_reset(struct ldmled_debug* debug) {
    mutex_lock(&debug->mutex);
    struct profiling_buffer *buffer = debug->profiling;
    if (!buffer) {
        mutex_unlock(&debug->mutex);
        return -EIO;
    }
    buffer->write_index = 0;
    mutex_unlock(&debug->mutex);
    return 0;
}

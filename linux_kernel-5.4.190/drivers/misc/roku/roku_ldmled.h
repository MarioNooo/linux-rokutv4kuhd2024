#ifndef ldmled_h_9d1e4b11756ddbca
#define ldmled_h_9d1e4b11756ddbca
#include "roku_ldm_boost.h"
#include "roku_ldm_fault.h"

enum buffer_type {
    BUF_LED_INPUT,
    BUF_PWM_OUTPUT,
    BUF_CURRENT_OUTPUT,
    BUF_LAST
};

enum {
    LOG_LED_INPUT = 0,
    LOG_PWM_OUTPUT = 1,
    LOG_CURRENT_OUTPUT = 2,
    LOG_ONCE = 3,
    REPLAY_LED_INPUT = 4,
    REPLAY_PWM_OUTPUT = 5,
    REPLAY_CURRENT_OUTPUT = 6,

    PROFILING_ON = 8,
    BYPASS_ALGO = 9,
    DEMO_MODE = 10,
    BYPASS_CONTROL = 11,
    PATTERN_MODE = 12,
    BOOST_BACK_DISABLE = 13,
};

struct frame_payload {
    uint32_t frame_id;
    uint64_t ts_nsec;
    struct sysstateregs sys_state;
    uint16_t zone_data[0];
};

struct ring_buffer {
    enum buffer_type buf_type;
    uint32_t total_frames;
    uint32_t buf_size;
    uint16_t row;
    uint16_t column;
    uint32_t write_index;
    uint32_t frame_count;
    struct frame_payload frame[0];
};

struct bfi_control {
    uint8_t bfi_on;
    uint16_t blank_time;  //value is based on : 65536 means total VSYNC
    uint32_t start_delay; // delay value is based on : 2^24 means total VSYNC, 
    uint32_t row_delay;
    uint32_t column_delay;
};

struct ldmled_test_pattern {
    uint8_t  pattern_mode; // 0: off, 1: window pattern, 2: 2 windows pattern, other value, for future usage
    uint8_t  win_x_start;
    uint8_t  win_y_start;
    uint8_t  win_x_end;
    uint8_t  win_y_end;
    uint8_t  win2_x_start;
    uint8_t  win2_y_start;
    uint8_t  win2_x_end;
    uint8_t  win2_y_end;
    uint16_t pwm_bg;
    uint16_t current_bg;
    uint16_t pwm_win;
    uint16_t current_win;
    uint16_t pwm_win2;
    uint16_t current_win2;
};

struct ldmled_power_config {
    uint16_t current_init;
    uint16_t current_max;
    uint32_t power_max;
};

struct spi_device_reg {
    int dev_id;
    int dev_addr;
    int payload_size;
    uint32_t payload[0];
};

#define DEFAULT_PROFILING_FRAMES  500
#define   MAX_TS     3
struct frame_timestamp {
    uint32_t frame_id;
    uint64_t nsec[MAX_TS];
};

struct profiling_buffer {
    uint32_t total_frames;
    uint32_t buf_size;
    uint32_t write_index;
    struct frame_timestamp ts[0];
};
struct spi_device_io {
    int buf_size;
    struct spi_device_reg dev[0];
};

#define LDMLED_BASE   'P'
#define LDMLED_GET_LOG_BUFFER     _IOWR(LDMLED_BASE, 1, int)
#define LDMLED_SET_LOG_BUFFER     _IOWR(LDMLED_BASE, 2, int)
#define LDMLED_GET_SPI_DEVICE     _IOWR(LDMLED_BASE, 3, int)
#define LDMLED_SET_SPI_DEVICE     _IOWR(LDMLED_BASE, 4, int)
#define LDMLED_INIT_MAPPING       _IOW(LDMLED_BASE,  5, int)
#define LDMLED_SET_BFI_CONTROL    _IOWR(LDMLED_BASE, 6, int)
#define LDMLED_GET_DEDBUG_MODE    _IOR(LDMLED_BASE,  7, int)
#define LDMLED_SET_DEDBUG_MODE    _IOW(LDMLED_BASE,  8, int)
#define LDMLED_GET_LOG_FRAMES     _IOR(LDMLED_BASE,  9, int)
#define LDMLED_SET_LOG_FRAMES     _IOW(LDMLED_BASE,  10, int)
#define LDMLED_SET_SYSSTATE       _IOWR(LDMLED_BASE, 11, int)
#define LDMLED_SET_BOOST_ALGREGS  _IOWR(LDMLED_BASE, 12, int)
#define LDMLED_GET_PROFILE        _IOWR(LDMLED_BASE, 13, int)
#define LDMLED_SET_PROFILE        _IOWR(LDMLED_BASE, 14, int)
#define LDMLED_SET_POWER_CONFIG   _IOWR(LDMLED_BASE, 15, int)
#define LDMLED_SET_TESTPATTERN    _IOWR(LDMLED_BASE, 16, int)
#define LDMLED_SET_BOARD_ON       _IOWR(LDMLED_BASE, 17, int)
#define LDMLED_SET_OUTPUT_GPIO    _IOWR(LDMLED_BASE, 18, int)
#define LDMLED_GET_INPUT_GPIO     _IOWR(LDMLED_BASE, 19, int)
#define LDMLED_CLEAR_FAULT        _IOR(LDMLED_BASE,  20, int)
#define LDMLED_PROCESS_FAULT      _IOR(LDMLED_BASE,  21, int)

int roku_ldmled_board_on(bool on);

#endif // ldmled_h_9d1e4b11756ddbca

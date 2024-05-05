#ifndef ldmled_debug_19ff917b4c15b325
#define ldmled_debug_19ff917b4c15b325

#include "roku_ldmled.h"

struct demo_mode {
    uint16_t cur_led;
    uint16_t interval_frames;
};

struct ldmled_debug {
    uint16_t row;
    uint16_t column;
    uint32_t frame_tick;
    struct mutex mutex;
    unsigned long debug_mode;
    uint16_t log_frames[BUF_LAST];
    struct demo_mode demo_mode;
    struct ring_buffer* buffers[BUF_LAST];
    struct profiling_buffer *profiling;
};

int debug_set_debug_mode(struct ldmled_debug* debug, int debug_mode);
int debug_get_debug_mode(struct ldmled_debug* debug);
int debug_set_log_frames(struct ldmled_debug* debug, uint16_t log_frames[BUF_LAST]);
int debug_get_log_frames(struct ldmled_debug* debug, uint16_t log_frames[BUF_LAST]);

int debug_read_buffer(struct ldmled_debug* debug, enum buffer_type buf_type,
                      uint8_t* buf, uint32_t len);

int debug_write_buffer(struct ldmled_debug *debug, enum buffer_type buf_type,
                      uint8_t *buf, uint32_t len);

int debug_log_frame(struct ldmled_debug* debug, enum buffer_type buf_type,
                    uint16_t* data, struct sysstateregs* sys_state);

int debug_replay_frame(struct ldmled_debug* debug, enum buffer_type buf_type,
                    uint16_t* data, struct sysstateregs* sys_state);

void debug_free_buffer(struct ldmled_debug* debug);
void debug_init(struct ldmled_debug* debug, int row, int column);
void debug_run_demo_mode(struct ldmled_debug* debug, uint16_t* led_buf);

int debug_profiling_read(struct ldmled_debug* debug, uint8_t* buf, uint32_t len);
int debug_profiling_log(struct ldmled_debug* debug, int index, bool last);
int debug_profiling_create(struct ldmled_debug* debug, int total_frames);
int debug_profiling_reset(struct ldmled_debug* debug);
int debug_profiling_free(struct ldmled_debug* debug);
void debug_set_test_pattern(struct ldmled_debug* debug, struct ldmled_test_pattern* test_pattern, uint16_t* zone_buf[BUF_LAST]);

#endif // ldmled_debug_19ff917b4c15b325

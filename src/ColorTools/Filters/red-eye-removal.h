
typedef struct {
    gint threshold;
} red_eye_removal_config;

void operation_red_eye_removal(void * in_buf, void * out_buf, long width, long height, red_eye_removal_config * config);

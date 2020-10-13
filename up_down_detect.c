#include <assert.h>
#include <capture.h>
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>

static float blue_threshold_bin_norm_sum_cbcr(uint8_t* data, size_t num_pairs) {
    assert(UINT_MAX > num_pairs);
    unsigned int sum = 0;

    for (size_t i = 0; i < num_pairs; i++) {
        uint8_t cb = data[2 * i];
        uint8_t cr = data[2 * i + 1];
        if (cb > cr)
            ++sum;
    }

    return (float)sum / num_pairs;
}

static bool up_is_up_nv12(uint8_t* frame, size_t width, size_t height) {
    uint8_t* color_data = frame + width * height;
    size_t channel_size = width * height / 4;

    float upper_blueness =
        blue_threshold_bin_norm_sum_cbcr(color_data, channel_size / 2);

    float lower_blueness = blue_threshold_bin_norm_sum_cbcr(
        color_data + channel_size, channel_size / 2);

    printf("Upper: %.2f, Lower: %.2f\n", upper_blueness, lower_blueness);

    return upper_blueness > lower_blueness;
}

int main() {
    size_t width = 640;
    size_t height = 480;

    media_native* cam = capture_open_native(width, height);
    capture_start_native(cam);

    uint8_t* frame;

    for (int i = 0;; ++i) {
        frame = capture_get_image_native(cam);
        printf(up_is_up_nv12(frame, width, height) ? "Up\n" : "Down\n");
    }
    return 0;
}

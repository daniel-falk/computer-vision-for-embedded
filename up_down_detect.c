#include <assert.h>
#include <capture.h>
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>

static float blue_threshold_bin_norm_sum_cbcr(uint8_t* data, size_t num_pairs) {
    /* Make sure that we wont risk an integer roll-over. */
    assert(UINT_MAX > num_pairs);

    /* Count the number of pixels where the blue chroma is higher than the
     * red chroma. */
    unsigned int sum = 0;
    for (size_t i = 0; i < num_pairs; i++) {
        uint8_t cb = data[2 * i];
        uint8_t cr = data[2 * i + 1];
        if (cb > cr)
            ++sum;
    }

    /* Return the normalized sum, i.e. a value from 0 to 1. */
    return (float)sum / num_pairs;
}

static bool up_is_up_nv12(uint8_t* frame, size_t width, size_t height) {
    /* In NV12 format we first has the full resolution gray channel, we're
     * not going to use this. Then we have the color data alternating
     * between blue chroma and red chroma subsampled 2x2 pixels.
     */
    uint8_t* color_data = frame + width * height;
    size_t channel_size = width * height / 4;

    /* Calculate a "blueness-measure" for the top half of the image. */
    float upper_blueness =
        blue_threshold_bin_norm_sum_cbcr(color_data, channel_size / 2);

    /* Calculate a "blueness-measure" for the bottom half of the image. */
    float lower_blueness = blue_threshold_bin_norm_sum_cbcr(
        color_data + channel_size, channel_size / 2);

    /* Print measures for transparency and debuging */
    printf("Upper: %.2f, Lower: %.2f\n", upper_blueness, lower_blueness);

    /* If top half of the frame has higher "blueness-measure" than the
     * bottom, then we guess that the image is in the correct orientation
     * and thus returns true. This should work quite well when the sky
     * is infact blue, less ok when the bottom part of the image is a
     * blue ocean.
     */
    return upper_blueness > lower_blueness;
}

int main() {
    /* Select the image width and height to use. 640x480 is pretty low in todays
     * standard, but for most analytics it is infact enought and most definitely
     * it is so for this simple example.
     */
    size_t width = 640;
    size_t height = 480;

    /* Use the SDKs libcapture API function to start an image stream */
    media_native* cam = capture_open_native(width, height);
    capture_start_native(cam);

    /* The frame pointer is only a pointer to a banch of 8-bit unsigned
     * integers, we must read the SDK documentation to know what they represent.
     * This is partly due to the fact that we use the native capture API, the
     * most low-level image API in the Axis SDK.
     */
    uint8_t* frame;

    /* Loop forever, let the SDK/system sleep until a frame is ready. Call the
     * analytics routine with the frame and print the result.
     */
    for (int i = 0;; ++i) {
        frame = capture_get_image_native(cam);
        printf(up_is_up_nv12(frame, width, height) ? "Up\n" : "Down\n");
    }
    return 0;
}

#include <assert.h>
#include <capture.h>
#include <inttypes.h>
#include <stdio.h>
#include <unistd.h>

static void write_pgm_gray(uint8_t* frame, size_t width, size_t height) {
    FILE* fp;

    fp = fopen("/tmp/gray.pgm", "w");
    fprintf(fp, "P5\n");
    fprintf(fp, "# CREATOR: daniel-falk\n");
    fprintf(fp, "%d %d\n", width, height);
    fprintf(fp, "%d\n", 255);

    for (size_t row = 0; row < height; row++) {
        for (size_t col = 0; col < width; col++)
            fputc(frame[row * width + col], fp);
    }

    fclose(fp);
}

static uint8_t* blue_threshold_bin_cbcr(uint8_t* data, size_t num_pairs) {
    uint8_t* img = calloc(num_pairs, sizeof img);
    assert(img);

    unsigned int sum = 0;

    for (size_t i = 0; i < num_pairs; i++) {
        uint8_t cb = data[2 * i];
        uint8_t cr = data[2 * i + 1];
        if (cb > cr) {
            img[i] = 255;
            sum++;
        }
    }

    printf("Norm sum: %.2f\n", (float)sum / num_pairs);

    return img;
}

static void save_threshold_img(uint8_t* frame, size_t width, size_t height) {
    uint8_t* color_data = frame + width * height;
    size_t channel_width = width / 2;
    size_t channel_height = height / 2;
    size_t channel_size = channel_width * channel_height;
    uint8_t* th_img = blue_threshold_bin_cbcr(color_data, channel_size);
    write_pgm_gray(th_img, channel_width, channel_height);
    free(th_img);
}

int main() {
    size_t width = 640;
    size_t height = 480;

    media_native* cam = capture_open_native(width, height);
    capture_start_native(cam);

    uint8_t* frame;

    for (int i = 0;; ++i) {
        frame = capture_get_image_native(cam);
        save_threshold_img(frame, width, height);
        sleep(1);
    }
    return 0;
}

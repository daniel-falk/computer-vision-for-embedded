#include <capture.h>
#include <inttypes.h>
#include <stdio.h>
#include <unistd.h>

static void write_pgm_nv12(uint8_t* frame, size_t width, size_t height) {
    size_t stacked_height = height * 3 / 2;
    size_t offset = width * height;

    FILE* fp;
    fp = fopen("/tmp/channels.pgm", "w");
    fprintf(fp, "P5\n");
    fprintf(fp, "# CREATOR: daniel-falk\n");
    fprintf(fp, "%d %d\n", width, stacked_height);
    fprintf(fp, "%d\n", 255);

    for (size_t row = 0; row < height; row++) {
        for (size_t col = 0; col < width; col++)
            fputc(frame[row * width + col], fp);
    }

    for (size_t row = 0; row < stacked_height - height; row++) {
        for (size_t col = 0; col < width; col += 2)
            fputc(frame[offset + row * width + col], fp);
        for (size_t col = 0; col < width; col += 2)
            fputc(frame[offset + 1 + row * width + col], fp);
    }

    fclose(fp);
}

int main() {
    size_t width = 640;
    size_t height = 480;

    media_native* cam = capture_open_native(width, height);
    capture_start_native(cam);

    uint8_t* frame;

    for (int i = 0;; ++i) {
        frame = capture_get_image_native(cam);
        write_pgm_nv12(frame, width, height);
        sleep(1);
    }
    return 0;
}

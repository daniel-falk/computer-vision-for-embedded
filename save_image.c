#include <capture.h>
#include <inttypes.h>
#include <stdio.h>
#include <unistd.h>

static void write_pgm_nv12(uint8_t* frame, size_t width, size_t height) {
    /* When showing the different components in the NV12 image we can put
     * the chroma components next to each other under the luminance plane.
     * The luminance plane is the same height as the full image since it
     * is not subsampled. The chroma planes are half the height of the
     * full image since they are subsampled 2x2. The total height of the
     * stacked NV12 image is thus 50% more than the full image height.
     */
    size_t stacked_height = height * 3 / 2;

    /* Offset to the chroma channels is the size of the luminance plane,
     * since it is not subsampled it is one byte per pixel in the
     * full image.
     */
    size_t offset = width * height;

    /* Save as a pgm image which can be displayed with most image viewers.
     * First comes the pgm-header...
     */
    FILE* fp;
    fp = fopen("/tmp/channels.pgm", "w");
    fprintf(fp, "P5\n");
    fprintf(fp, "# CREATOR: daniel-falk\n");
    fprintf(fp, "%d %d\n", width, stacked_height);
    fprintf(fp, "%d\n", 255);

    /* Then the luminance (gray) plane... */
    for (size_t row = 0; row < height; row++) {
        for (size_t col = 0; col < width; col++)
            fputc(frame[row * width + col], fp);
    }

    /* And finally the chroma channels next to each other... */
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
     * image save function with the frame. Repeat every second.
     */
    for (int i = 0;; ++i) {
        frame = capture_get_image_native(cam);
        write_pgm_nv12(frame, width, height);
        sleep(1);
    }
    return 0;
}

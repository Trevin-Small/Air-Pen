#include "esp_camera.h"
#include "image_processing.h"

/*
 * Given the greyscale image in fb->buf, binarize the image and
 * write it into the bin_fb (binary frame buffer) parameter.
 */
void binarize_image(camera_fb_t *fb, uint8_t *bin_fb) {
  const uint8_t THRESHOLD = 100;
  const uint8_t * buf = fb->buf;
  const size_t len = fb->len;

  for (int i = 0; i < len; i++) {

    if (buf[i] < THRESHOLD) {
      bin_fb[i] = 0;
    } else {
      bin_fb[i] = 255;
    }

  }

}

/*
 * Find all blobs within the given image frame, and calculate their size and brightness
 * Add the 4 highest intensity blobs (avg brightness per pixel) to the blob array
 */
void find_blobs(camera_fb_t *fb, uint8_t *bin_fb, blob_arr_t *blob_arr) {

}

/*
 * Add the latest blob_arr to the blob buffer
 */
void add_blobs(blob_buf_t *blob_buf, blob_arr_t *blob_arr) {

  blob_buf->buf[++blob_buf->back] = *blob_arr;

}

/*
 * Get the most recent blob_arr from the blob buffer
 */
void get_latest_blobs(blob_buf_t *blob_buf, blob_arr_t *blob_arr) {

  *blob_arr = blob_buf->buf[blob_buf->front++];

}
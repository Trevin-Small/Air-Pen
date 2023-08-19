#pragma once
#include <stdint.h>

#define BLOB_THRESHOLD  100
#define BLOB_ARR_SIZE     4
#define BLOB_BUF_SIZE     8


/*
 * A blob is defined as a group of bright pixels within a greyscale image.
 */
typedef struct blob {
  size_t size;             // Number of pixels in the blob
  uint8_t x;               // Center x of the blob
  uint8_t y;               // Center y of the blob
  uint8_t avg_brightness;  // Average pixel brightness of the blob
} blob_t;

/*
 * Array of <BLOB_ARR_SIZE> blobs with the greatest average brightness found in
 * an image frame.
 */
typedef struct blob_arr {
  blob_t blobs[BLOB_ARR_SIZE];
} blob_arr_t;

/*
 * Circular buffer of blob arrays allows processing of multiple samples of blob
 * positions to find blobs which are near one another and similar in size and
 * brightness.
 *
 * This enables picking the blob that corresponds to the position of the Air Pen
 * infrared light reference source, even with infrared noise in the environment.
 */
typedef struct blob_buf {
  blob_arr_t buf[BLOB_BUF_SIZE];
  uint8_t front;
  uint8_t back;
} blob_buf_t;

void binarize_image(camera_fb_t *fb, uint8_t *bin_fb);
void find_blobs(camera_fb_t *fb, uint8_t *bin_fb, blob_arr_t *blob_arr);
void add_blobs(blob_buf_t *blob_buf, blob_arr_t * blob_arr);
int get_latest_blobs(blob_buf_t *blob_buf, blob_arr_t * blob_arr);
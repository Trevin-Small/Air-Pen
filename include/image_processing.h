#pragma once
#include "esp_camera.h"
#include <stdint.h>
#include <vector>
#include <stack>

#define MAX_NUM_BLOBS     8
#define BLOB_THRESHOLD  100
#define BLOB_ARR_SIZE     4
#define BLOB_BUF_SIZE     8


/*
 * A blob is defined as a group of bright pixels within a greyscale image.
 */
typedef struct blob {
  uint32_t size;           // Number of pixels in the blob
  uint32_t pos;            // Position of the blob in the buffer
  uint8_t min_x;           // Minimum x coordinate of the blob
  uint8_t max_x;           // Maximum x coordinate of the blob
  uint8_t min_y;           // Minimum y coordinate of the blob
  uint8_t max_y;           // Maximum y coordinate of the blob
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
long long floodfill(camera_fb_t *fb, uint8_t *flood_buf, uint32_t pos, uint8_t flood_num, blob_t *blob);
void find_blobs(camera_fb_t *fb, blob_arr_t *blob_arr);
void add_blobs(blob_buf_t *blob_buf, blob_arr_t *blob_arr);
void get_latest_blobs(blob_buf_t *blob_buf, blob_arr_t *blob_arr);
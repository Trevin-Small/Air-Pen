#ifndef IMG_PROCESSING_H
#define IMG_PROCESSING_H

#include "esp_camera.h"
#include "circular_buffer.h"
#include <stdint.h>
#include <vector>
#include <stack>

#define MAX_BLOBS_PER_FRAME   8   // Maximum number of blobs to find in an image.
#define BLOB_THRESHOLD      100   // Threshold to categorize a pixel as white/black
#define BLOB_BUF_SIZE         8   // Number of blob arrays to keep in the buffer

#define PEN_BLOB_MIN_SIZE     2   // Min size the reference IR blob wil show up as
#define PEN_BLOB_MAX_SIZE    10   // Max size the reference IR blob wil show up as


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
 * Circular buffer of vectors of blobs. Looking through blobs in the buffer allows
 * for picking out blobs that are close to one another, moving in the same trend,
 * and/or flashing at certain frequencies.
 *
 * Helpful for finding specific light sources.
 */
circular_buffer<std::vector<blob_t>> blob_buf;


void binarize_image(camera_fb_t *fb, uint8_t *bin_fb);
long long floodfill(camera_fb_t *fb, std::vector<uint8_t> &flood_buf, uint32_t pos, uint8_t flood_num, blob_t &blob);
void find_blobs(camera_fb_t *fb, std::vector<blob_t> &blob_arr);

#endif
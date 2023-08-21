#ifndef IMG_PROCESSING_H
#define IMG_PROCESSING_H

#include "esp_camera.h"
#include "circular_buffer.h"
#include <stdint.h>
#include <vector>
#include <stack>

#define MAX_BLOBS_PER_FRAME   8   // Maximum number of blobs to find in an image.
#define BLOB_BUF_SIZE         8   // Number of blob arrays to keep in the buffer

#define REF_BLOB_THRESHOLD  180   // Threshold to categorize a pixel as white/black
#define REF_BLOB_MIN_SIZE     2   // Min size the reference IR blob wil show up as
#define REF_BLOB_MAX_SIZE    10   // Max size the reference IR blob wil show up as
#define REF_BLOB_MAX_SPEED   10   // Max speed the ref IR blob can move in pixels/millisec


/*
 * A blob is defined as a group of bright pixels within a greyscale image.
 */
typedef struct blob {
  uint32_t left;            // Left x coordinate of the blob
  uint32_t right;           // Right x coordinate of the blob
  uint32_t top;             // Top y coordinate of the blob
  uint32_t bottom;          // Bottom y coordinate of the blob
  uint32_t size;            // Number of pixels in the blob
  uint32_t total_brightness;   // total pixel brightness of the blob
} blob_t;

typedef struct component {
  int parent;
  int size;
  blob_t *blob;
} component_t;


/*
 * Circular buffer of vectors of blobs. Looking through blobs in the buffer allows
 * for picking out blobs that are close to one another, moving in the same trend,
 * and/or flashing at certain frequencies.
 *
 * Helpful for finding specific light sources.
 */
extern circular_buffer<std::vector<blob_t>> blob_buf;


void binarize_image(camera_fb_t *fb, uint8_t *bin_fb);
//long long floodfill(camera_fb_t *fb, std::vector<uint8_t> &flood_buf, uint32_t pos, uint8_t flood_num, blob_t &blob);
void find_blobs(camera_fb_t *fb, std::vector<blob_t> &blob_arr, uint8_t blob_threshold);

#endif
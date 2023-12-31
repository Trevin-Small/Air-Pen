#ifndef IMG_PROCESSING_H
#define IMG_PROCESSING_H

#include "esp_camera.h"
#include <vector>

#define MAX_BLOBS_PER_FRAME   8   // Maximum number of blobs to find in an image.
#define BLOB_BUF_SIZE         8   // Number of blob arrays to keep in the buffer

#define REF_BLOB_THRESHOLD  220   // Threshold to categorize a pixel as white/black
#define REF_BLOB_MIN_SIZE    10   // Min size the reference IR blob wil show up as
#define REF_BLOB_MAX_SIZE   150   // Max size the reference IR blob wil show up as
#define REF_BLOB_MAX_SPEED   10   // Max speed the ref IR blob can move in pixels/millisec


using namespace std;

/*
 * A blob is defined as a group of bright pixels within a greyscale image.
 */

typedef struct blob {
  unsigned int left;               // Left x coordinate of the blob
  unsigned int right;              // Right x coordinate of the blob
  unsigned int top;                // Top y coordinate of the blob
  unsigned int bottom;             // Bottom y coordinate of the blob
  unsigned int size;               // Number of pixels in the blob
  unsigned int total_brightness;   // total pixel brightness of the blob
  unsigned int avg_brightness;     // Average pixel brightness of the blob
} blob_t;


void binarize_image(camera_fb_t *fb, uint8_t *bin_fb);
int find(vector<int> &parent, int p);
void my_union(vector<int> &parent, int p, int q);
vector<blob_t> find_blobs(camera_fb_t *fb, uint8_t blob_threshold);

#endif
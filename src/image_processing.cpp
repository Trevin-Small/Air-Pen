#include "esp_camera.h"
#include "image_processing.h"
#include <stdint.h>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <Arduino.h>

using namespace std;

/*
 * Given the greyscale image in fb->buf, binarize the image and
 * write it into the bin_fb (binary frame buffer) parameter.
 */
void binarize_image(camera_fb_t *fb, uint8_t *bin_fb) {
  const uint8_t * buf = fb->buf;
  const size_t len = fb->len;

  for (int i = 0; i < len; i++) {

    if (buf[i] < REF_BLOB_THRESHOLD) {
      bin_fb[i] = 0;
    } else {
      bin_fb[i] = 255;
    }

  }

}


/*
 * Find the root of a component in a disjoint-set data structure
 */
int find(vector<int> &parent, int p) {
  if (p == parent[p]) return p;
  parent[p] = find(parent, parent[p]); // path compression
  return parent[p];
}

/*
 * Union distjoint sets by root (smaller root becomes parent of larger root)
 */
void my_union(vector<int> &parent, int p, int q) {
  int a = find(parent, p), b = find(parent, q);// a = rootP, b = rootQ
  if (a == b) return;
  if  (a < b) { parent[b]=a; }
  else  { parent[a] = b; }
}

/*
 * Two-pass connected component labeling algorithm. Uses two passes over the image
 * and union-find to quickly label and connect components.
 *
 * Based on: https://en.wikipedia.org/wiki/Connected-component_labeling
 */

vector<blob_t> find_blobs(camera_fb_t *fb, uint8_t blob_threshold) {
  const uint8_t * buf = fb->buf;
  int len = fb->len;
  int width = fb->width;
  int height = fb->height;
  int label_val = 1;

  vector<int> labels(len, 0);
  vector<int> parent(len, 0);
  unordered_map<int, blob_t> blob_map;

  for (int i = 0; i < len; i++) {
    parent[i] = i;

    // If the pixel is not part of the background
    if (buf[i] > blob_threshold) {
      int left = i - 1;
      int up = i - width;

      int left_pixel = -1;
      int up_pixel = -1;

      if (0 <= left) {
        left_pixel = buf[left];
      }

      if (0 <= up) {
        up_pixel = buf[up];
      }

      bool left_neighbor = left_pixel > blob_threshold;
      bool up_neighbor = up_pixel > blob_threshold;

      if (left_neighbor && up_neighbor) {
        labels[i] = labels[left] < labels[up] ? labels[left] : labels[up];
        my_union(parent, labels[left], labels[up]);
      } else if (left_neighbor) {
        labels[i] = labels[left];
      } else if (up_neighbor) {
        labels[i] = labels[up];
      } else { // This pixel is part of a new component

        unsigned int x = i % width;
        unsigned int y = i / width;

        labels[i] = label_val;
        label_val++;
      }

    }

  }

  for (unsigned int y = 0; y < height; y++) {

    for (unsigned int x = 0; x < width; x++) {

      int pos = x + y * width;

      int val = find(parent, labels[pos]);

      if (blob_map.count(val) == 0) {

        blob_map.insert({
          val,
          blob_t {
            .left = x,
            .right = x,
            .top = y,
            .bottom = y,
            .size = 1,
            .total_brightness = buf[pos],
            .avg_brightness = 0
          }
        });

      } else {

        blob_t *b = &blob_map.at(val);
        if (x < b->left) b->left = x;
        if (x > b->right) b->right = x;
        if (y < b->top) b->top = y;
        if (y > b->bottom) b->bottom = y;
        b->size++;
        b->total_brightness += buf[pos];

      }

    }

  }

  blob_map.erase(0);

  vector<blob_t> frame_blobs;

  for (auto & i : blob_map) {
    if (i.second.size < REF_BLOB_MIN_SIZE || i.second.size > REF_BLOB_MAX_SIZE) continue;
    i.second.avg_brightness = i.second.total_brightness / i.second.size;
    frame_blobs.push_back(i.second);
  }

  struct {
    bool operator()(const blob_t &a, const blob_t &b) const { return a.avg_brightness < b.avg_brightness; }
  } blob_cmp;

  std::sort(frame_blobs.begin(), frame_blobs.end(), blob_cmp);

  return frame_blobs;

}


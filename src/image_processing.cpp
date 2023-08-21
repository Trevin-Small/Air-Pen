#include "esp_camera.h"
#include "image_processing.h"
#include <stdint.h>
#include <Arduino.h>
#include <vector>
#include <stack>
#include <algorithm>

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
long long floodfill(camera_fb_t *fb, std::vector<uint8_t> &flood_buf, uint32_t pos, uint8_t flood_num, blob_t &blob) {
  int len = fb->len;
  int width = fb->width;
  std::stack<uint32_t> stack = std::stack<uint32_t>();
  stack.push(pos);

  long long total_brightness = 0;

  while (stack.size() > 0) {

      uint32_t p = stack.top();
      stack.pop();

      flood_buf[p] = flood_num;
      total_brightness += fb->buf[p];
      blob.size++;

      uint32_t pos_x = blob.pos % fb->width;
      uint32_t pos_y = blob.pos / fb->width;

      if (pos_x < blob.left) {
        blob.left = pos_x;
      } else if (pos > blob.right) {
        blob.right = pos_x;
      }

      if (pos_y < blob.top) {
        blob.left = pos_y;
      } else if (pos > blob.bottom) {
        blob.bottom = pos_y;
      }

      int right = p + 1;
      int left = p - 1;
      int down = p + width;
      int up = p - width;

      if (right < len && flood_buf[right] == 0 && fb->buf[right] > REF_BLOB_THRESHOLD) {
        stack.push(right);
      }

      if (0 < left && flood_buf[left] == 0 && fb->buf[left] > REF_BLOB_THRESHOLD) {
        stack.push(left);
      }

      if (0 < up && flood_buf[up] == 0 && fb->buf[up] > REF_BLOB_THRESHOLD) {
        stack.push(up);
      }

      if (down < len && flood_buf[down] == 0 && fb->buf[down] > REF_BLOB_THRESHOLD) {
        stack.push(down);
      }

  }

  return total_brightness;

}
*/

/*
 * Find all blobs within the given image frame, and calculate their size and brightness
 * Add the 4 highest intensity blobs (avg brightness per pixel) to the blob array
 */

/*
void find_blobs(camera_fb_t *fb, std::vector<blob_t> &blob_arr) {

  const uint8_t * buf = fb->buf;
  const size_t len = fb->len;
  uint8_t flood_num = 1;
  std::vector<blob_t> blob_list(MAX_BLOBS_PER_FRAME);
  std::vector<uint8_t> flood_buf(len, 0);

  for (uint32_t i = 0; i < len; i++) {

    // If the pixel is above the threshold and hasn't been added to a blob yet
    if (flood_buf[i] == 0 && buf[i] > REF_BLOB_THRESHOLD) {
      blob_t blob = {
        .left = UINT8_MAX,
        .right = 0,
        .top = UINT8_MAX,
        .bottom = 0,
        .size = 0,
        .total_brightness = 0
      };

      long long total_brightness = floodfill(fb, flood_buf, i, flood_num, blob);

      blob.x = blob.left + (blob.right - blob.left) / 2;
      blob.y = blob.top + (blob.bottom - blob.top) / 2;
      blob.avg_brightness = total_brightness / blob.size;

      blob_list.push_back(blob);
      flood_num++;

    }

  }

  struct {
    bool operator()(blob_t a, blob_t b) const { return a.avg_brightness > b.avg_brightness; }
  } blob_cmp;

  std::sort(blob_list.begin(), blob_list.end(), blob_cmp);

  // Print out the flood fill buffer


  for (int i = 0; i < fb->height; i++) {
    Serial.print("[");
    for (int j = 0; j < fb->width; j++) {
      Serial.print(flood_buf[i * fb->width + j]);
    }
    Serial.println("]");
  }

    for (int i = 0; i < MAX_BLOBS_PER_FRAME; i++) {

    Serial.print("Blob ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(blob_list[i].left);
    Serial.print(" <-> ");
    Serial.print(blob_list[i].right);
    Serial.print(", ");
    Serial.print(blob_list[i].top);
    Serial.print(" <-> ");
    Serial.print(blob_list[i].bottom);
    Serial.print(", ");
    Serial.println(blob_list[i].avg_brightness);

    blob_arr[i] = blob_list[i];
  }

}
*/

/*
 * Add pixel2 to the blob that pixe
 */
void add_pixel_to_blob(camera_fb_t *fb, std::vector<component_t> &components,
  std::vector<int> &labels, int ref_pixel, int pixel_to_add) {

  labels[pixel_to_add] = labels[ref_pixel];
  blob_t * blob = components[labels[ref_pixel]].blob;

  int pixel_x = pixel_to_add % fb->width;
  int pixel_y = pixel_to_add / fb->width;

  if (pixel_x > blob->right) {
    blob->right = pixel_x;
  }

  if (pixel_y > blob->bottom) {
    blob->bottom = pixel_y;
  }

  blob->size++;
  components[labels[ref_pixel]].size++;
  blob->total_brightness += fb->buf[pixel_to_add];
}

component_t find(std::vector<component_t> &parent, int p) {// returns the root
  if (p == parent[p].parent) return parent[p];
  parent[p].parent = find(parent, parent[p].parent).parent; // path compression
  return parent[p];
}

// union by size (smaller root -> larger root)
void my_union(std::vector<component_t> &parent, int p, int q) {
  int a = find(parent, p).parent, b = find(parent, q).parent;// a = rootP, b = rootQ
  if (a == b) return;

  component_t parent_c;
  component_t child_c;

  if  (parent[a].size <= parent[b].size) {
    parent_c = parent[b];
    child_c = parent[a];

  } else  {
    parent_c = parent[a];
    child_c = parent[b];
  }

  blob_t *parent_blob = parent_c.blob;
  blob_t *child_blob = child_c.blob;

  child_c.parent = parent_c.parent;
  child_c.blob = parent_c.blob;
  parent_c.size += child_c.size;

  parent_blob->left = parent_blob->left < child_blob->left ? parent_blob->left : child_blob->left;
  parent_blob->right = parent_blob->right > child_blob->right ? parent_blob->right : child_blob->right;
  parent_blob->top = parent_blob->top < child_blob->top ? parent_blob->top : child_blob->top;
  parent_blob->bottom = parent_blob->bottom > child_blob->bottom ? parent_blob->bottom : child_blob->bottom;

  child_blob->size = 0;

}

/*
 * Two-pass connected component labeling algorithm. Uses two passes over the image
 * and union-find to quickly label and connect components.
 *
 * Based on: https://en.wikipedia.org/wiki/Connected-component_labeling
 */
void find_blobs(camera_fb_t *fb, std::vector<blob_t> &blob_arr, uint8_t blob_threshold) {
  const uint8_t * buf = fb->buf;
  int len = fb->len;
  int width = fb->width;
  int height = fb->height;
  int label_val = 1;

  std::vector<blob_t> temp_blobs;
  std::vector<int> labels(len, 0);
  std::vector<component_t> components(len, component_t { .parent = 0, .size = 0, .blob = 0});

  for (int i = 0; i < len; i++) {

    if (buf[i] > blob_threshold) {
      bool left_neighbor = false;
      bool up_neighbor = false;

      int left = i - 1;
      int up = i - width;
      uint8_t left_val = 0;
      uint8_t up_val = 0;

      if (0 <= left && labels[left] > 0) {
        left_neighbor = true;
        left_val = labels[left];
      }

      if (0 <= up && labels[up] > 0) {
        up_neighbor = true;
        up_val = labels[up];
      }

      if (left_neighbor && up_neighbor) {
        labels[i] = left_val < up_val ? left_val : up_val;
        // union this component
        add_pixel_to_blob(fb, components, labels, left, i);
        my_union(components, labels[up], labels[left]);

      } else if (left_neighbor) {

        add_pixel_to_blob(fb, components, labels, left, i);

      } else if (up_neighbor) {

        add_pixel_to_blob(fb, components, labels, up, i);

      } else {
        labels[i] = label_val;

        uint32_t x_pos = i % width;
        uint32_t y_pos = i / width;

        blob_t blob = {
          .left = x_pos,
          .right = x_pos,
          .top = y_pos,
          .bottom = y_pos,
          .size = 1,
          .total_brightness = buf[i]
        };

        temp_blobs.push_back(blob);

        // add new component here
        components.push_back(component_t { .parent = label_val, .size = 1, .blob = &temp_blobs.back()});

        label_val++;

      }

    }

  }

  for (int i = 0; i < temp_blobs.size(); i++) {
    if (temp_blobs[i].size > 0) {
      blob_arr.push_back(temp_blobs[i]);
    }
  }

  Serial.print("Num blobs: ");
  Serial.println(blob_arr.size());
  Serial.println("Frame:");

  for (int i = 0; i < height; i++) {
    Serial.print("[ ");
    for (int j = 0; j < width; j++) {
      int val = components[labels[i * width + j]].parent;
      Serial.print(char (val + 64));
    }
    Serial.println(" ]");
  }

  Serial.println("\n");

}
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

long long floodfill(camera_fb_t *fb, std::vector<uint8_t> &flood_buf, uint32_t pos, uint8_t flood_num, blob_t &blob) {

  std::stack<uint32_t> stack = std::stack<uint32_t>();
  stack.push(pos);

  long long total_brightness = 0;

  while (stack.size() > 0) {
      uint32_t p = stack.top();
      stack.pop();
      if (p < 0 || p >= fb->len) continue;

      if (flood_buf[p] == 0 && fb->buf[p] > REF_BLOB_THRESHOLD) {
          flood_buf[p] = flood_num;
          total_brightness += fb->buf[p];
          blob.size++;

          uint32_t pos_x = blob.pos % fb->width;
          uint32_t pos_y = blob.pos / fb->width;

          if (pos_x < blob.min_x) {
            blob.min_x = pos_x;
          } else if (pos > blob.max_x) {
            blob.max_x = pos_x;
          }

          if (pos_y < blob.min_y) {
            blob.min_x = pos_y;
          } else if (pos > blob.max_y) {
            blob.max_y = pos_y;
          }

          stack.push(p + 1);
          stack.push(p - 1);
          stack.push(p - fb->width);
          stack.push(p + fb->width);
      }
  }

  flood_buf[fb->len - 2] = 9;

  return total_brightness;

}

/*
 * Find all blobs within the given image frame, and calculate their size and brightness
 * Add the 4 highest intensity blobs (avg brightness per pixel) to the blob array
 */


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
        .size = 0,
        .pos = i,
        .min_x = UINT8_MAX,
        .max_x = 0,
        .min_y = UINT8_MAX,
        .max_y = 0,
        .x = 0,
        .y = 0,
        .avg_brightness = 0
      };

      long long total_brightness = floodfill(fb, flood_buf, i, flood_num, blob);

      blob.x = blob.min_x + (blob.max_x - blob.min_x) / 2;
      blob.y = blob.min_y + (blob.max_y - blob.min_y) / 2;
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

  /*

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
    Serial.print(blob_list[i].min_x);
    Serial.print(" <-> ");
    Serial.print(blob_list[i].max_x);
    Serial.print(", ");
    Serial.print(blob_list[i].min_y);
    Serial.print(" <-> ");
    Serial.print(blob_list[i].max_y);
    Serial.print(", ");
    Serial.println(blob_list[i].avg_brightness);

    blob_arr[i] = blob_list[i];
  }

  */

}

void find_blobs2(camera_fb_t *fb, std::vector<blob_t> &blob_arr) {
  uint8_t *buf = fb->buf;
  uint32_t len = fb->len;
  uint32_t height = fb->height;
  uint32_t width = fb->width;
  uint32_t flood_num = 1;
  std::vector<blob_t> blob_list(MAX_BLOBS_PER_FRAME);
  std::vector<uint8_t> flood_buf(len, 0);

  for (int i = 0; i < len; i += 4) {

    uint32_t chunk_val = *((uint32_t *) &buf[i]);

    if (chunk_val > REF_BLOB_THRESHOLD) {

      int j;

      if (chunk_val < 256) {
        j = 3;
      } else if (chunk_val < 65536) {
        j = 2;
      } else if (chunk_val < 16777216) {
        j = 1;
      } else {
        j = 0;
      }

      uint8_t * pixel = (&buf[i]) + j;
      uint32_t pos = pixel - buf;

      blob_t blob = {
        .size = 1,
        .pos = pos,
        .min_x = UINT8_MAX,
        .max_x = 0,
        .min_y = UINT8_MAX,
        .max_y = 0,
        .x = 0,
        .y = 0,
        .avg_brightness = 0
      };

      long long total_brightness = floodfill(fb, flood_buf, pos, flood_num, blob);

      blob.x = blob.min_x + (blob.max_x - blob.min_x) / 2;
      blob.y = blob.min_y + (blob.max_y - blob.min_y) / 2;
      blob.avg_brightness = total_brightness / blob.size;


      blob_list.push_back(blob);
      flood_num++;

    }

  }

  struct {
    bool operator()(blob_t a, blob_t b) const { return a.avg_brightness > b.avg_brightness; }
  } blob_cmp;

  std::sort(blob_list.begin(), blob_list.end(), blob_cmp);

}
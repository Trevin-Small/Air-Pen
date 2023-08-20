#include "esp_camera.h"
#include "image_processing.h"
#include <stdint.h>
#include <Arduino.h>
#include <vector>
#include <stack>

/*
 * Given the greyscale image in fb->buf, binarize the image and
 * write it into the bin_fb (binary frame buffer) parameter.
 */
void binarize_image(camera_fb_t *fb, uint8_t *bin_fb) {
  const uint8_t * buf = fb->buf;
  const size_t len = fb->len;

  for (int i = 0; i < len; i++) {

    if (buf[i] < BLOB_THRESHOLD) {
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

      if (flood_buf[p] == 0 && fb->buf[p] > BLOB_THRESHOLD) {
          flood_buf[p] = flood_num;
          total_brightness += fb->buf[p];
          blob.size++;
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
  uint8_t blob_num = 0;
  std::vector<blob_t> blob_list(255, blob_t { 0 });
  std::vector<uint8_t> flood_buf(len, 0);

  for (uint32_t i = 0; i < len; i++) {

    // If the pixel is above the threshold and hasn't been added to a blob yet
    if (flood_buf[i] == 0 && buf[i] > BLOB_THRESHOLD) {
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

      long long total_brightness = floodfill(fb, flood_buf, i, blob_num + 1, blob);

      blob.x = (blob.max_x - blob.min_x) / 2;
      blob.y = (blob.max_y - blob.min_y) / 2;
      blob.avg_brightness = total_brightness / blob.size;

      blob_list[blob_num++] = blob;

      if (blob_num == MAX_NUM_BLOBS) {
        break;
      }

    }

  }

  //sort(blob_list);
  //blob_arr = blob_list[0:7];

  // Print out the flood fill buffer

  for (int i = 0; i < fb->height; i++) {
    Serial.print("[");
    for (int j = 0; j < fb->width; j++) {
      Serial.print(flood_buf[i * fb->width + j]);
    }
    Serial.println("]");
  }

}

/*
 * Add the latest blob_arr to the blob buffer
 */
void add_blobs(blob_buf_t &blob_buf, std::vector<blob_t> &blob_arr) {

  blob_buf.buf[++blob_buf.write_pos] = blob_arr;

}

/*
 * Get the most recent blob_arr from the blob buffer
 */
void get_latest_blobs(blob_buf_t &blob_buf, std::vector<blob_t> &blob_arr) {

  blob_arr = blob_buf.buf[blob_buf.read_pos++];

}
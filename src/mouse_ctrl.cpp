#include "mouse_ctrl.h"
#include "image_processing.h"
#include "BlePeripheral.h"
#include <Arduino.h>

pair<int, int> get_raw_pos(blob_t b) {
  return {b.left + (b.right - b.left) / 2, b.top + (b.bottom - b.top) / 2};
}

pair<int, int> get_screen_pos(blob_t b, camera_fb_t *fb) {
  pair<int, int> raw_pos = get_raw_pos(b);
  return {(SCREEN_WIDTH / fb->width) * raw_pos.first, (SCREEN_HEIGHT / fb->height) * raw_pos.second};
}

void send_mouse_coords(BlePeripheral ble, circular_buffer<vector<blob_t>> &blob_buf, camera_fb_t *fb) {

  // If the blob buffer is full and connected via bluetooth to computer
  if(blob_buf.full()) {

    // Get the oldest blobs
    vector<blob_t> frame_blobs = blob_buf.pop();

    if (frame_blobs.size() == 0) {
      return;
    }

    pair<int, int> coords = get_screen_pos(frame_blobs[0], fb);

    ble.send_mouse_coords(coords);
  }

}
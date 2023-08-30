#ifndef MOUSE_CTRL_H
#define MOUSE_CTRL_H

#include "image_processing.h"
#include "circular_buffer.h"
#include "BlePeripheral.h"
#include <utility>

pair<int, int> get_raw_pos(blob_t b);
pair<int, int> get_screen_pos(blob_t b, camera_fb_t *fb);
void send_mouse_coords(BlePeripheral ble, circular_buffer<vector<blob_t>> &blob_buf, camera_fb_t *fb);

#endif
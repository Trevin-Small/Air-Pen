#include "esp_camera.h"
#include "camera_config.h"
#include "image_processing.h"
#include <stdint.h>
#include <Arduino.h>
//#include <BlePeripheral.h>
#include <BleCombo.h>
#include <vector>

using namespace std;

//BlePeripheral AirPen;
BleComboKeyboard Keyboard;
BleComboMouse Mouse(&Keyboard);
vector<blob_t> blobs;

const int SCREEN_WIDTH = 1500;
const int SCREEN_HEIGHT = 1000;

void setup() {

  Serial.begin(115200);

  //AirPen = BlePeripheral(true, true);

  Mouse.begin();
  Keyboard.begin();

  esp_err_t err = camera_init();

  sensor_t * cam = esp_camera_sensor_get();
  cam->set_brightness(cam, -1);     // -2 to 2
  cam->set_contrast(cam, 2);       // -2 to 2
  cam->set_saturation(cam, 0);     // -2 to 2
  cam->set_awb_gain(cam, 0);
  cam->set_aec2(cam, 0);
  cam->set_ae_level(cam, 0);
  cam->set_agc_gain(cam, 0);
  cam->set_bpc(cam, 0);
  cam->set_wpc(cam, 0);
  cam->set_raw_gma(cam, 0);
  cam->set_lenc(cam, 0);

  Serial.println("Starting Air Pen...");

  delay(3000);
}

int get_x(blob_t b) {
  return b.left + (b.right - b.left) / 2;
}

int get_y(blob_t b) {
  return b.top + (b.bottom - b.top) / 2;
}

int map_x_to_screen(int x, int frame_width) {
  return (SCREEN_WIDTH / frame_width) * x;
}

int map_y_to_screen(int y, int frame_height) {
  return (SCREEN_HEIGHT / frame_height) * y;
}

int currX = SCREEN_WIDTH / 2;
int currY = SCREEN_HEIGHT / 2;

void loop() {

  //acquire a fb
  camera_fb_t * fb = esp_camera_fb_get();

  find_blobs(fb, blobs, REF_BLOB_THRESHOLD);

  if(blobs.size() > 0 && Keyboard.isConnected()) {

    int x = map_x_to_screen(get_x(blobs.back()), 96);
    int y = map_y_to_screen(get_y(blobs.back()), 96);
    blobs.clear();

    Mouse.move(x - currX, y - currY);
    currX = x;
    currY = y;
  }


  //return the fb buffer back to the driver for reuse
  esp_camera_fb_return(fb);

  delay(10);

}
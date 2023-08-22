#include "camera_pins.h"
#include "esp_camera.h"
#include "image_processing.h"
#include <stdint.h>
#include <Arduino.h>
#include <vector>

std::vector<blob_t> blobs;

static camera_config_t config = {
  .pin_pwdn = PWDN_GPIO_NUM,
  .pin_reset = RESET_GPIO_NUM,
  .pin_xclk = XCLK_GPIO_NUM,
  .pin_sscb_sda = SIOD_GPIO_NUM,
  .pin_sscb_scl = SIOC_GPIO_NUM,

  .pin_d7 = Y9_GPIO_NUM,
  .pin_d6 = Y8_GPIO_NUM,
  .pin_d5 = Y7_GPIO_NUM,
  .pin_d4 = Y6_GPIO_NUM,
  .pin_d3 = Y5_GPIO_NUM,
  .pin_d2 = Y4_GPIO_NUM,
  .pin_d1 = Y3_GPIO_NUM,
  .pin_d0 = Y2_GPIO_NUM,

  .pin_vsync = VSYNC_GPIO_NUM,
  .pin_href = HREF_GPIO_NUM,
  .pin_pclk = PCLK_GPIO_NUM,

  .xclk_freq_hz = 20000000,
  .ledc_timer = LEDC_TIMER_0,
  .ledc_channel = LEDC_CHANNEL_0,

  .pixel_format = PIXFORMAT_GRAYSCALE,
  .frame_size = FRAMESIZE_96X96,
  .jpeg_quality = 63,
  .fb_count = 3,
  .fb_location = CAMERA_FB_IN_PSRAM,
  .grab_mode = CAMERA_GRAB_LATEST,
};

// camera init
esp_err_t camera_init() {
  //initialize the camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
      Serial.println("Camera Init Failed");
      return err;
  }

  return ESP_OK;
}

void setup() {

  Serial.begin(115200);

  esp_err_t err = camera_init();

  sensor_t * cam = esp_camera_sensor_get();
  cam->set_hmirror(cam, 1);
  cam->set_brightness(cam, 0);     // -2 to 2
  cam->set_contrast(cam, 1);       // -2 to 2
  cam->set_saturation(cam, 2);     // -2 to 2

  delay(3000);

  /*
  for (int i = 0; i < 10; i++) {
    esp_camera_fb_get();
    delay(10);
  }

  cam->set_whitebal(cam, 0);
  cam->set_awb_gain(cam, 0);
  cam->set_gain_ctrl(cam, 0);
  cam->set_exposure_ctrl(cam, 0);
  cam->set_aec2(cam, 0);
  */

  Serial.println("hi");

  delay(3000);
}


void loop() {

  //acquire a fb
  camera_fb_t * fb = esp_camera_fb_get();

  unsigned long ms = millis();

  find_blobs(fb, blobs, REF_BLOB_THRESHOLD);

  unsigned long ms2 = millis();

  Serial.print("Blob Detection Runtime: ");
  Serial.print(ms2 - ms);
  Serial.println("ms");

  for (const auto b : blobs) {
    Serial.print("Blob size: ");
    Serial.println(b.size);
  }

  blobs.clear();

  //return the fb buffer back to the driver for reuse
  esp_camera_fb_return(fb);

  delay(5000);

}
#include "camera_pins.h"
#include "esp_camera.h"
#include "image_processing.h"
#include <stdint.h>
#include <Arduino.h>
#include <vector>

std::vector<blob_t> blobs(MAX_BLOBS_PER_FRAME, blob_t { 0 });

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
  .jpeg_quality = 12,
  .fb_count = 1,
  .fb_location = CAMERA_FB_IN_PSRAM,
  //.pixel_format = PIXFORMAT_RGB565, // for face detection/recognition
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

  delay(3000);

  Serial.println("hi");

  delay(3000);
}

void loop() {

  //acquire a fb
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
      Serial.println("Camera Capture Failed");
      delay(1000);
  }

  find_blobs(fb, blobs);
  Serial.println("\n\n");

  //return the fb buffer back to the driver for reuse
  esp_camera_fb_return(fb);

  delay(1000);
}

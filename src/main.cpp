#include "esp_camera.h"
#include "camera_config.h"
#include "image_processing.h"
#include "circular_buffer.h"
#include "BlePeripheral.h"
#include "mouse_ctrl.h"
#include <Arduino.h>
#include <stdint.h>
#include <string>
#include <vector>

using namespace std;

// Initialize the BLE device as a BLE peripheral
BlePeripheral BlePeriph("Scribe", "TKS");

/*
 * Circular buffer of blobs in image frames. Connecting blobs in sequential
 * images frames allows for identifying blobs that are similar in
 * size, position, and intensity, moving in the same trend, and/or
 * flashing at certain frequencies.
 *
 * Used for recognizing the IR lighthouse amongst ambient IR noise.
 */
circular_buffer<vector<blob_t>> blob_buf(BLOB_BUF_SIZE);


void setup() {

  Serial.begin(115200);

  // Initialize camera and its settings
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

  delay(1000);

  // Enable bluetooth
  BlePeriph.begin();
}

void loop() {

  //acquire a fb
  camera_fb_t * fb = esp_camera_fb_get();

  // Find the blobs in the image frame and add them to the blob buffer
  blob_buf.push(find_blobs(fb, REF_BLOB_THRESHOLD));

  // If the device is connected to a computer via Bluetooth
  if (BlePeriph.is_connected()) {

    // Calculate the mouse coords with blob buf and send them via BlePeriph
    send_mouse_coords(BlePeriph, blob_buf, fb);

  }

  //return the fb buffer back to the driver for reuse
  esp_camera_fb_return(fb);

  delay(10);

}
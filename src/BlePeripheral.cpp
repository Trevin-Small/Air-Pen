#include "BlePeripheral.h"
#include <stdint.h>
#include <utility>
#include <string>
#include <Arduino.h>

using namespace std;

int filter_motion(int m) {
  if (m < MIN_MOTION_THRESHOLD || m > MAX_MOTION_THRESHOLD) {
    return 0;
  }

  return m;
}

BlePeripheral::BlePeripheral(std::string name, std::string manufacturer_name) : Mouse(&Keyboard) {
  Keyboard.deviceName = name;
  Keyboard.deviceManufacturer = manufacturer_name;
  Keyboard.batteryLevel = (uint8_t) 100;
};


void BlePeripheral::begin() {
  Keyboard.begin();
  Mouse.begin();
}

bool BlePeripheral::is_connected() {
  return Keyboard.isConnected();
}

pair<int, int> BlePeripheral::get_mouse_coords() {
  return mouse_coords;
}

void BlePeripheral::send_mouse_coords(pair<int, int> coords) {
  int dx = coords.first - mouse_coords.first;
  int dy = coords.second - mouse_coords.second;

  //dx = filter_motion(dx);
  //dy = filter_motion(dy);

  mouse_coords.first += dx;
  mouse_coords.second += dy;

  Mouse.move(dx, dy);
}

void BlePeripheral::send_click(int button = 0) {
  Mouse.click(button);
}

void BlePeripheral::send_keystroke(int key) {
  Keyboard.press(key);
}

void BlePeripheral::update_battery_level(uint8_t bat_level) {
  Keyboard.setBatteryLevel(bat_level);
}
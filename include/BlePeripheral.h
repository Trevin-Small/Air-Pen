#ifndef BLE_H
#define BLE_H

#include <BleComboKeyboard.h>
#include <BleComboMouse.h>
#include <utility>
#include <string>


#define SCREEN_WIDTH    1500
#define SCREEN_HEIGHT   1000

#define MIN_MOTION_THRESHOLD     0
#define MAX_MOTION_THRESHOLD   400

using namespace std;

class BlePeripheral {

private:
  BleComboKeyboard Keyboard;
  BleComboMouse Mouse;
  pair<int, int> mouse_coords;

public:
  BlePeripheral(std::string name, std::string manufacturer_name);
  void begin();
  bool is_connected();
  pair<int, int> get_mouse_coords();
  void send_mouse_coords(pair<int, int> coords);
  void send_click(int button);
  void send_keystroke(int key);
  void update_battery_level(uint8_t bat_level);

};

#endif
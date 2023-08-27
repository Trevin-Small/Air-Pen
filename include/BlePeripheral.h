#ifndef MOUSE_H
#define MOUSE_H

#include <BleCombo.h>
#include <utility>

using namespace std;

class BlePeripheral {

public:
  BlePeripheral(bool keyboard_enabled, bool mouse_enabled);
  pair<int, int> get_mouse_coords();
  pair<int, int> update_mouse_coords();
  void send_mouse_coords();
  void send_click(int button);
  void send_keystroke(int key);

private:
  bool keyboard_enabled;
  bool mouse_enabled;
  int mouse_x;
  int mouse_y;
};


#endif
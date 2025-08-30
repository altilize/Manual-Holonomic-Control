/*
  Authored by
  Moses Jaguar
  2025
  
  Controller using ESP32 Dev Module V4 - Use bluepad32 board manager
  using UART0 because V4 version only has 1 UART, so dont forget to disconnect the rx tx wiring to Receiver when uploading the code
*/

#include "Global.h"

int x = 0, y = 0, z = 0;  // untuk di transmit ke STM

// ----------------- Main -------------------
void processGamepad(ControllerPtr ctl) {
  bool L1pressed = ctl->buttons() == 0x0010;
  bool R1pressed = ctl->buttons() == 0x0020;
  bool L2pressed = ctl->buttons() == 0x0040;
  bool R2pressed = ctl->buttons() == 0x0080;
  bool Xpressed = ctl->buttons() == 0x0001;
  bool TrianglePressed = ctl->buttons() == 0x0008;
  bool SquarePressed = ctl->buttons() == 0x0004;
  bool CirclePressed = ctl->buttons() == 0x0002;
  int joystickY_value = ctl->axisY();
  int joystickX_value = ctl->axisX();
  int joystickRY_value = ctl->axisRY();
  int joystickRX_value = ctl->axisRX();


  x = exponentialMapping(joystickX_value, 200.0);
  y = exponentialMapping(joystickY_value, 200.0);
  z = exponentialMapping(joystickRX_value,200.0);

  Serial.printf("M %d %d %d\n", x, -y, z); // kirim ke STM32

  // dumpGamepad(ctl); // for debugging
}

// ---------------- Which controller do u use? ----------------
void processControllers() {
  for (auto myController : myControllers) {
    if (myController && myController->isConnected() && myController->hasData()) {
      if (myController->isGamepad()) {
        processGamepad(myController);
      } else {
        Serial.println("Unsupported controller");
      }
    }
  }
}


void setup() {
  Serial.begin(115200);
  Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
  const uint8_t* addr = BP32.localBdAddress();
  Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

  BP32.setup(&onConnectedController, &onDisconnectedController);
  BP32.forgetBluetoothKeys();
  BP32.enableVirtualDevice(false);
}

void loop() {
  bool dataUpdated = BP32.update();
  if (dataUpdated) {
    processControllers();
  }

  // delayMicroseconds(20);
}

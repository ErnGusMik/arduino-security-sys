# Arduino Security System
A security system using an Arduino UNO, ultrasonic sensor, microphone, LDR sensor, buzzer, RGB light and OLED display.

---
### Required components
* Arduino main board (I used an Arduino UNO)
* Active buzzer module
* Sound sensor (conected to Analog 0)
* RGB light
* LDR (light) sensor
* HC-SR04 Ultrasonic sensor
* 0.96 inch OLED display (connected to A4 and A5)
* Phone with Bluetooth

---
### Schematics
![Schematics Image](https://github.com/ErnGusMik/arduino-security-sys/blob/main/security_sys.png?raw=true)
This image does not include a 0.96 inch OLED display (connected to A4 and A5)

---
### Code
The code is available in the `security_system` folder.

---
### Operation
To operate the security system, download an Arduino bluetooth controller to your phone (I used 'Arduino bluetooth controller' from the Play Store).
Connect to the Arduino bluetooth module ('HC-05' for me).

To enable the system, send the number 1 through bluetooth to the Arduino. To disable -- 0. (I entered 'Switch Mode' on the controller and configured it)

---
### How it works
When the system is on, the system checks for changes in the sound, light, and distance measurements. If a change is detected, the sound module emits a beep for half a second and shows 'DETECTED' on the OLED display.

Enjoy ;)

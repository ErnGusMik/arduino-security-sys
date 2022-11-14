bool active = false;
bool alert = false;
const int sampleWindow = 50;                              // Sample window width in mS (50 mS = 20Hz)
unsigned int prevSound;
unsigned int sample;
unsigned int lightStatus;
unsigned int prevLightStatus;
long duration;
int distanceCm;
int prevDistance;

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// CENTER: 64, 32

void setup() {
  pinMode(3, OUTPUT); // BUZZER
  pinMode(A0, INPUT); // MICROPHONE
  pinMode(6, OUTPUT); // R (RGB LIGHT)
  pinMode(7, OUTPUT); // G (RGB LIGHT)
  pinMode(5, OUTPUT); // B (RGB LIGHT)
  pinMode(A1, INPUT); // LIGHT SENSOR
  pinMode(8, OUTPUT); // TRIG (ULTRASONIC)
  pinMode(9, INPUT); // ECHO (ULTRASONIC)
  pinMode(LED_BUILTIN, OUTPUT); // BUILT IN LED (FOR TESTING)
  Serial.begin(9600);
  digitalWrite(3, HIGH);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  delay(200);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 5);
  // Display static text
  display.println("Alarm system");
  display.setCursor(0, 15);
  display.println("By Erny");
  display.display();
  delay(4000);
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 5);
  display.println("Alarm off");
  display.display();
  digitalWrite(7, 255);
}

void test() {
  // LIGHT
  lightStatus = analogRead(A1);
  if (prevLightStatus == 0) {
    prevLightStatus = lightStatus;
  } else if (prevLightStatus+50 > lightStatus && prevLightStatus-50 < lightStatus) {
    alert = false;
  } else {
    Serial.println("ALERT LIGHT");
    alert = true;
  }
  prevLightStatus = lightStatus;


  // DISTANCE
  digitalWrite(8, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(8, HIGH);
  delayMicroseconds(10);
  digitalWrite(8, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(9, HIGH);
  // Calculating the distance
  distanceCm = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  // Displays the distance on the Serial Monitor
  if (prevDistance == 0) {
    prevDistance == distanceCm;
  } else if (prevDistance+2 > distanceCm && prevDistance-2 < distanceCm) {
    alert = false;
  } else {
    Serial.println("ALERT DISTANCE");
    alert = true;
  }
  prevDistance = distanceCm;


  // SOUND
  unsigned long startMillis= millis();                   // Start of sample window
  float peakToPeak = 0;                                  // peak-to-peak level

  unsigned int signalMax = 0;                            //minimum value
  unsigned int signalMin = 1024;                         //maximum value

                                                        // collect data for 50 mS
  while (millis() - startMillis < sampleWindow)
  {
    sample = analogRead(0);                             //get reading from microphone
    if (sample < 1024)                                  // toss out spurious readings
    {
        if (sample > signalMax)
        {
          signalMax = sample;                           // save just the max levels
        }
        else if (sample < signalMin)
        {
          signalMin = sample;                           // save just the min levels
        }
    }
  }
  peakToPeak = signalMax - signalMin;                    // max - min = peak-peak amplitude
  if (prevSound == 0) {
    prevSound = peakToPeak;
  } else if (prevSound != peakToPeak && prevSound+1 != peakToPeak && prevSound-1 != peakToPeak && prevSound-2 != peakToPeak && prevSound+2 != peakToPeak) {
    Serial.println("ALERT SOUND");
    alert =  true;
  }
  prevSound = peakToPeak;
}

void drawCentreString(const char *buf, int x, int y)
{
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(buf, x, y, &x1, &y1, &w, &h); //calc width of new string
    display.setCursor(x - w / 2, y);
    display.println(buf);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 5);
    display.println("Bluetooth message received");
    display.display();
    delay(1000);
    char incoming_val = Serial.read();
    Serial.println(incoming_val);
    digitalWrite(LED_BUILTIN, HIGH);
    if (incoming_val == '1') {
      display.clearDisplay();
      display.setCursor(0, 5);
      display.setTextSize(2);
      display.println("Please exit");
      display.setCursor(0, 25);
      display.println("5s");
      display.display();
      digitalWrite(6, 255);
      digitalWrite(7, 255);
      digitalWrite(5, 0);
      delay(5000);
      digitalWrite(7, 0);
      delay(400);
      digitalWrite(6, 0);
      active = true;
    } else if (incoming_val == '0') {
      display.clearDisplay();
      display.setCursor(0, 5);
      display.setTextSize(2);
      display.println("Alarm off");
      display.display();
      digitalWrite(7, 255);
      digitalWrite(5, 0);
      digitalWrite(6, 0);
      digitalWrite(3, LOW);
      delay(100);
      digitalWrite(3, HIGH);
      delay(100);
      digitalWrite(3, LOW);
      delay(100);
      digitalWrite(3, HIGH);
      active = false;
      prevLightStatus = 0;
      prevDistance = 0;
      prevSound = 0;
    } else {
      display.clearDisplay();
      display.setCursor(0, 5);
      display.setTextSize(1);
      display.println("Command unrecognised");
      display.display();
      if (active == true) {
        display.clearDisplay();
        display.setCursor(0, 5);
        display.setTextSize(2);
        display.println("Alarm on");
        display.display();
      } else {
        display.clearDisplay();
        display.setCursor(0, 5);
        display.setTextSize(2);
        display.println("Alarm off");
        display.display();
      }
    }
    digitalWrite(LED_BUILTIN, LOW);
  } else if (Serial.available() == 0) {
    // OLED DISPLAY: Connect to Bluetooth
  }
  if (active) {
    test();
    delay(50);
  }
  if (alert == true || alert == 1) {
    display.setTextSize(1);
    display.setCursor(0, 25);
    display.println("DETECTED");
    display.display();
    digitalWrite(3, LOW);
    digitalWrite(6, 255);
    digitalWrite(7, 0);
    digitalWrite(5, 0);
    delay(500);
    digitalWrite(3, HIGH);
    digitalWrite(6, 0);
    digitalWrite(7, 0);
    digitalWrite(5, 0);
    alert = false;
    delay(1000);
  }
}

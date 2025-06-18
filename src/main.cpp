#include <Arduino.h>
#include <BleKeyboard.h>
#include <BLEDevice.h>
#include <BLESecurity.h>

// Define GPIO pins for the five buttons (active low when pressed)
const int BUTTON1_PIN = 0;
const int BUTTON2_PIN = 1;
const int BUTTON3_PIN = 2;
const int BUTTON4_PIN = 3;
const int BUTTON5_PIN = 4;
const int BUTTON6_PIN = 5;
const int BUTTON7_PIN = 6;
const int BUTTON8_PIN = 7;
const int LED_PIN    = 8;

// Create a BleKeyboard instance with 6 buttons
BleKeyboard bleKeyboard("ESP32 Keyboard", "My Company", 100);

void setup() {
  Serial.begin(115200);
  // Initialize button pins as inputs with pull-up resistors
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  pinMode(BUTTON3_PIN, INPUT_PULLUP);
  pinMode(BUTTON4_PIN, INPUT_PULLUP);
  pinMode(BUTTON5_PIN, INPUT_PULLUP);
  pinMode(BUTTON6_PIN, INPUT_PULLUP);
  pinMode(BUTTON7_PIN, INPUT_PULLUP);
  pinMode(BUTTON8_PIN, INPUT_PULLUP);

  pinMode(LED_PIN, OUTPUT);
  // Initialize BLE with security for HID pairing
  BLEDevice::init("ESP32 Keyboard");
  BLESecurity *pSecurity = new BLESecurity();
  // Require bonding and Just Works pairing (no IO capabilities)
  pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_BOND);
  pSecurity->setCapability(ESP_IO_CAP_NONE);
  // Enforce 16-byte encryption and key distribution for bonding
  pSecurity->setKeySize(16);
  pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
  pSecurity->setRespEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
  // Start the BLE Keyboard service
  bleKeyboard.begin();
  Serial.println("BLE Keyboard started. Waiting for connection...");
}

void loop() {
  if (bleKeyboard.isConnected()) {
    digitalWrite(LED_PIN, HIGH);
    static bool lastState[8] = { HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH };
    const int pins[8] = {
      BUTTON5_PIN,
      BUTTON6_PIN, 
      BUTTON7_PIN, 
      BUTTON8_PIN,
      BUTTON2_PIN, 
      BUTTON3_PIN, 
      BUTTON4_PIN,
      BUTTON1_PIN 
    };
    const char keyMap[8] = { 'p', 'b', 'w', 0, ',', '.', ']', '\\' };
    // index 3 ('SHIFT') will be handled separately as a modifier key
    for (int i = 0; i < 8; i++) {
      bool current = digitalRead(pins[i]);
      if (current == LOW && lastState[i] == HIGH) {
        if (i == 3) {
          bleKeyboard.press(KEY_LEFT_SHIFT);
        } else {
          bleKeyboard.write(keyMap[i]);
        }
        Serial.print("Sent key: ");
        Serial.println(keyMap[i]);
        if (i == 3) {
          bleKeyboard.release(KEY_LEFT_SHIFT);
        }
      }
      lastState[i] = current;
    }
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  delay(10); // Small delay to debounce and reduce CPU load
}
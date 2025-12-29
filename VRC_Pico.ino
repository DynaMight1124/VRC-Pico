/* * Adapted for Raspberry Pi Pico (RP2040)
 * Original code: kekse23.de RCUSB4 v1.1
 * Requires: Raspberry Pi Pico/RP2040 by Earle F. Philhower & Adafruit TinyUSB library 
 */

#include <Adafruit_TinyUSB.h>

// ==========================================
// CONFIGURATION: GPIO PINS
// ==========================================
// You can set pins here
#define PIN_CHAN1 28  // Steering
#define PIN_CHAN2 3   // Throttle
#define PIN_CHAN3 4   // Channel 3
#define PIN_CHAN4 5   // Channel 4

// ==========================================
// HID REPORT DESCRIPTOR
// ==========================================
// This defines the Pico as a Gamepad with 4 Axes
uint8_t const desc_hid_report[] = {
  TUD_HID_REPORT_DESC_GAMEPAD(HID_REPORT_ID(1))
};

Adafruit_USBD_HID usb_hid;

// Report payload: 4 Axes (8-bit signed values: -127 to 127) + Buttons (not used but required structure)
struct __attribute__((packed)) GamepadReport {
  int8_t  x;       // X Axis
  int8_t  y;       // Y Axis
  int8_t  z;       // Z (Not used)
  int8_t  rz;      // Rz (Not used)
  int8_t  rx;      // Rx Axis
  int8_t  ry;      // Ry Axis
  uint8_t hat;     // Hat switch
  uint32_t buttons;// Buttons
};

GamepadReport gpReport;

// Global Variables
volatile unsigned long Timer[4];
volatile unsigned int Value[4];
volatile bool ValChanged[4] = {false, false, false, false};
unsigned int NewValue[4];

// ==========================================
// INTERRUPT SERVICE ROUTINES (ISRs)
// ==========================================

void isr1() {
  unsigned long now = micros();
  if (digitalRead(PIN_CHAN1) == HIGH) {
    Timer[0] = now;
  } else {
    // Falling edge: calculate pulse width
    if (now > Timer[0]) {
      Value[0] = (Value[0] + (now - Timer[0])) / 2; // Simple averaging
      ValChanged[0] = true;
    }
  }
}

void isr2() {
  unsigned long now = micros();
  if (digitalRead(PIN_CHAN2) == HIGH) {
    Timer[1] = now;
  } else {
    if (now > Timer[1]) {
      Value[1] = (Value[1] + (now - Timer[1])) / 2;
      ValChanged[1] = true;
    }
  }
}

void isr3() {
  unsigned long now = micros();
  if (digitalRead(PIN_CHAN3) == HIGH) {
    Timer[2] = now;
  } else {
    if (now > Timer[2]) {
      Value[2] = (Value[2] + (now - Timer[2])) / 2;
      ValChanged[2] = true;
    }
  }
}

void isr4() {
  unsigned long now = micros();
  if (digitalRead(PIN_CHAN4) == HIGH) {
    Timer[3] = now;
  } else {
    if (now > Timer[3]) {
      Value[3] = (Value[3] + (now - Timer[3])) / 2;
      ValChanged[3] = true;
    }
  }
}

// ==========================================
// SETUP
// ==========================================
void setup() {
  // 1. SET USB NAMES
  USBDevice.setManufacturerDescriptor("Pico"); // Optional: Change the manufacturer name
  USBDevice.setProductDescriptor("VRC-USB");        // This is the device name you will see

  // 2. Setup USB HID
  usb_hid.setPollInterval(2);
  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  usb_hid.begin();
  
  // 3. Setup Pins
  pinMode(PIN_CHAN1, INPUT);
  pinMode(PIN_CHAN2, INPUT);
  pinMode(PIN_CHAN3, INPUT);
  pinMode(PIN_CHAN4, INPUT);

  // 4. Attach Interrupts
  attachInterrupt(digitalPinToInterrupt(PIN_CHAN1), isr1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_CHAN2), isr2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_CHAN3), isr3, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_CHAN4), isr4, CHANGE);

  // Initialize report to center/zero
  gpReport.x = 0;
  gpReport.y = 0;
  gpReport.rx = 0;
  gpReport.ry = 0;
  gpReport.z = 0;
  gpReport.rz = 0;
  gpReport.hat = 0;
  gpReport.buttons = 0;
}

// ==========================================
// LOOP
// ==========================================
void loop() {
  if ( !usb_hid.ready() ) return;

  bool sendUpdate = false;

  // --- CHANNEL 1 (Steering) ---
  if (ValChanged[0]) {
    NewValue[0] = (NewValue[0] + Value[0]) / 2;
    
    long mapped = map(NewValue[0], 750, 2250, -127, 127); 
    
    gpReport.x = (int8_t)constrain(mapped, -127, 127);
    ValChanged[0] = false;
    sendUpdate = true;
  }

  // --- CHANNEL 2 (Throttle) ---
  if (ValChanged[1]) {
    NewValue[1] = (NewValue[1] + Value[1]) / 2;
    
    long mapped = map(NewValue[1], 750, 2250, -127, 127);
    
    gpReport.y = (int8_t)constrain(mapped, -127, 127);
    ValChanged[1] = false;
    sendUpdate = true;
  }

  // --- CHANNEL 3 ---
  if (ValChanged[2]) {
    NewValue[2] = (NewValue[2] + Value[2]) / 2;
    long mapped = map(NewValue[2], 750, 2250, -127, 127);
    gpReport.rx = (int8_t)constrain(mapped, -127, 127);
    ValChanged[2] = false;
    sendUpdate = true;
  }

  // --- CHANNEL 4 ---
  if (ValChanged[3]) {
    NewValue[3] = (NewValue[3] + Value[3]) / 2;
    long mapped = map(NewValue[3], 750, 2250, -127, 127);
    gpReport.ry = (int8_t)constrain(mapped, -127, 127);
    ValChanged[3] = false;
    sendUpdate = true;
  }

  // Only send USB data if something changed
  if (sendUpdate) {
    usb_hid.sendReport(1, &gpReport, sizeof(gpReport));
  }
  
  // Minimal delay to prevent USB flooding
  delay(1);
}
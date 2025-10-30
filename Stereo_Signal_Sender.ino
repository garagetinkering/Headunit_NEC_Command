/* Slow NEC frame 
   Mark = transistor ON (line LOW)
   Space = transistor OFF (line HIGH via HU pull-up)
*/

#include <Arduino.h>
#define BUTTON_PIN 13
#define NEC_OUT_PIN 32

const int BASE = 562;          // us (NEC base)
const int NEC_TIME = BASE;     // us (approx)
const int START_HI = 9000;     // leader mark
const int START_LO = 4500;     // leader space
const int ONE_SPACE = 1687;
const int ZERO_SPACE = 562;
const int BIT_MARK = 562;
const int END_MARK = 562;

const uint8_t KENWOOD_ADDR = 0xB9; // Kenwood specific address
const uint8_t KEY_TEST     = 0x0e; // Kenwood command for play/pause

void nec_set_pin(bool on) {
  if (on) {
    pinMode(NEC_OUT_PIN, OUTPUT);
    digitalWrite(NEC_OUT_PIN, HIGH);
  } else {
    pinMode(NEC_OUT_PIN, INPUT);
  }
}

void mark_us(unsigned long usec) {
  nec_set_pin(true);
  delayMicroseconds(usec);
}

void space_us(unsigned long usec) {
  nec_set_pin(false);
  if (usec) delayMicroseconds(usec);
}

void nec_one() {
  mark_us(BIT_MARK);
  space_us(ONE_SPACE);
}
void nec_zero() {
  mark_us(BIT_MARK);
  space_us(ZERO_SPACE);
}

void sendNECByte(uint8_t b) {
  for (uint8_t i=0;i<8;i++) {
    if (b & 1) nec_one(); else nec_zero();
    b >>= 1;
  }
}

void sendNEC(uint8_t addr, uint8_t data) {
  uint8_t naddr = ~addr;
  uint8_t ndata = ~data;
  // leader
  mark_us(START_HI);
  space_us(START_LO);
  // addr, ~addr, data, ~data
  sendNECByte(addr);
  sendNECByte(naddr);
  sendNECByte(data);
  sendNECByte(ndata);
  // final mark
  mark_us(END_MARK);
  space_us(0);
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(NEC_OUT_PIN, INPUT);
  delay(100);
  Serial.println("Slow NEC debug ready");
}

void loop() {
  static bool prev = false;
  bool pressed = digitalRead(BUTTON_PIN) == LOW;
  if (pressed && !prev) {
    Serial.println("Sending slow NEC test frame");
    sendNEC(KENWOOD_ADDR, KEY_TEST);
    delay(300);
  }
  prev = pressed;
}

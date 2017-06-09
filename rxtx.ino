
#include "rxtx.h"

void rxtxSetup() {
  // RX/TX Relay (HIGH == RX)
  pinMode(RX_TX_RELAY, OUTPUT);
  // 144.800 TX module (LOW = ENABLE)
  pinMode(TX_EN, OUTPUT);

  rxtxRX();
}
void rxtxRX() {
  digitalWrite(TX_EN, HIGH);
  delay(100);
  digitalWrite(RX_TX_RELAY, LOW);
}
void rxtxTX() {
  digitalWrite(RX_TX_RELAY, HIGH);
  delay(100);
  digitalWrite(TX_EN, LOW);
  delay(500);
}

void printWait(int milis, int ledInterval){
  int interval = 100;
  int led = 0;
  while (milis > 0){
    milis -= interval;
    delay(interval);
    if (milis % 1000 == 0) Serial.println(milis/1000);
    led += interval;
    if (led > 2 * ledInterval){
      led = 0;
      digitalWrite(LED_PIN, LOW);
    }else if (led > ledInterval){
      digitalWrite(LED_PIN, HIGH);
    }  
  }
}

double lastBlink = 0;
boolean lastLed = LOW;
void blinkLed(int interval) {
  double curBlink = millis();
  if ((curBlink - interval) > lastBlink) {
    lastBlink = curBlink;
    lastLed = !lastLed;
    digitalWrite(LED_PIN, lastLed);
  }
}

void yo(String msg, int val, boolean nl){
  Serial.print(msg);
  Serial.print(": ");
  Serial.print(val);
  Serial.print("\t");
  if (nl) Serial.println();
}


void printWait(int milis, int ledInterval){
  int interval = 100;
  int led = 0;
  while (milis > 0){
    if (milis % 1000 == 0) Serial.println(milis);
    milis -= interval;
    delay(interval);
    led += interval;
    if (led > 2 * ledInterval){
      led = 0;
      digitalWrite(LED_PIN, LOW);
    }else if (led > ledInterval){
      digitalWrite(LED_PIN, HIGH);
    }  
  }
}

void yo(String msg, int val, boolean nl){
  Serial.print(msg);
  Serial.print(": ");
  Serial.print(val);
  Serial.print("\t");
  if (nl) Serial.println();
}


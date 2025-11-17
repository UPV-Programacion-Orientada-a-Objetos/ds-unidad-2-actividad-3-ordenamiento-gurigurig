void setup() {
  Serial.begin(9600);
  
  int readings[] = {105, 5, 210, 99, 1, 500, 20, 15};
  int numReadings = 8;
  
  for (int i = 0; i < numReadings; i++) {
    Serial.println(readings[i]);
    delay(100);
  }
}

void loop() {
}
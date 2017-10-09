#define LM35_PIN    A1

void setup() {
  Serial.begin(9600);

  pinMode(A1, INPUT);
}

void loop() {

  float tempC;
  int reading;

  reading = analogRead(A1);
  tempC = (float) reading*500/1024;

  Serial.print(millis());
  Serial.print(" - ");
  Serial.print(tempC);
  Serial.println(" C");
  // put your main code here, to run repeatedly:

}

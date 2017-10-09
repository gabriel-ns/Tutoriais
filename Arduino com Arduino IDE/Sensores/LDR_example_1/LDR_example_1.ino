#define LDR_PIN           A0

void setup() {

  Serial.begin(9600);

}

void loop() {

  int leitura;

  leitura = analogRead(LDR_PIN);

  Serial.print(millis());
  Serial.print(" - ");
  Serial.println(leitura);

  delay(250);

}

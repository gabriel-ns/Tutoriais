#define HC_SR04_ECHO     D4
#define HC_SR04_TRIGGER  D5

void setup() {
  Serial.begin(9600);

  pinMode(HC_SR04_ECHO, INPUT);
  pinMode(HC_SR04_TRIGGER, OUTPUT);

  digitalWrite(HC_SR04_TRIGGER, LOW);

}

void loop() {
  
  long duration;
  float distance;
  
  digitalWrite(HC_SR04_TRIGGER, LOW);
  delayMicroseconds(2);
  digitalWrite(HC_SR04_TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(HC_SR04_TRIGGER, LOW);
  
  duration = pulseIn(HC_SR04_ECHO, HIGH);

  /**
   * Velocidade do som = 340,29 m/s
   * 
   * duration é o tempo de ida e volta em microsegundos. 
   * A precisão do sensor é dada na ordem de centímetros, então calcula-se
   * a velocidade do som em cm/us
   * 
   * 1m -> 100 cm
   * 1s -> 100000 us
   * 
   * Vsom = 340,29 * 100 / 1000000 cm/us
   * 
   * Vsom = 0,034029
   * 
   * Dada a equação da velocidade V = S/T, temos que
   * S = V*T
   * S = 0,030429 * T
   * 0,030429 = 1/29,3867
   * 
   * S = T / 29,3867
   * 
   * Como duration = distâcida de ida e volta:
   * 
   * S = (duration/2) / 29,3867
   */
  distance = (duration/2) / 29.3867;

  Serial.print(millis());
  Serial.print(" - ");
  Serial.print(distance);
  Serial.println(" cm");

  

}

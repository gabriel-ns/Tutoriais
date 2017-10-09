#define PWM_PIN    D0

/** 
 *  Cria uma variável que representará o duty cycle 
 *  Esta variável possui 8 bits e é um inteiro com sinal
 *  podendo ir de -126 até 127. Como o duty irá de 0 a 100
 *  estes valores estão compatíveis
 */
int8_t duty = 0;

int i = 0;

typedef struct
{
  uint32_t timestamp[500];
  int8_t   duty[500];
}data_t;

data_t data;


void setup()
{
  Serial.begin(230400);
  Serial.println("Starting");
  pinMode(PWM_PIN, OUTPUT);
}

void loop()
{
  static uint32_t startTime = micros();
  /** Inicia ciclo acionando o pino */
  digitalWrite(PWM_PIN, HIGH);

    /** Mantém o pino ativo durante o duty cycle */
  delayMicroseconds(duty);

    /** Desliga a saída após o periodo ativo */
  digitalWrite(PWM_PIN, LOW);

  /** Mantém desligado pelo resto do período do PWM */
  delayMicroseconds(100 - duty);

  /** Calcula um novo valor para o duty */
  duty = duty + 1;

  /** Quando o duty cycle chega em 100, ele é zerado formando uma onda dente de serra */
  if(duty >= 100) duty = 0;

  if(i < 500) data.timestamp[i] = micros(), data.duty[i++] = duty;
  if(i == 500) 
  {
    for(i = 0; i < 500; i++)
    {
      Serial.println(String(data.timestamp[i] - startTime) + ";" + String(data.duty[i]));
    }
    i++;
  }
  

}

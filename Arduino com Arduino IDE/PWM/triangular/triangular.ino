#include <math.h>

#define PWM_PIN    D0
#define DATASIZE   1000

/** 
 *  Cria uma variável que representará o duty cycle 
 *  Esta variável possui 8 bits e é um inteiro com sinal
 *  podendo ir de -126 até 127. Como o duty irá de 0 a 100
 *  estes valores estão compatíveis
 */
int8_t duty = 0;

int i = 0;

bool rising = true;

typedef struct
{
  uint32_t timestamp[DATASIZE];
  int8_t   duty[DATASIZE];
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

  /** Calcula um novo valor para o duty. Se a etapa for de subida, incrementa o duty. Senão, decrementa */
  if(rising) duty = duty + 1;
  else duty = duty - 1;

  /** Quando o duty cycle chega em 100 ou em 0, inverte-se a etapa de subida/descida */
  if(duty >= 100 || duty <= 0) rising = !rising;

  if(i < DATASIZE) data.timestamp[i] = micros(), data.duty[i++] = duty;
  if(i == DATASIZE) 
  {
    for(i = 0; i < DATASIZE; i++)
    {
      Serial.println(String(data.timestamp[i] - startTime) + ";" + String(data.duty[i]));
    }
    i++;
  }
  

}

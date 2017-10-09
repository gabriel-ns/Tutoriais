#include <math.h>
#define PWM_PIN    D0
#define DATASIZE   500

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
  uint32_t timestamp[DATASIZE];
  int8_t   duty[DATASIZE];
}data_t;

data_t data;

const float pi = 3.14159267;



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
  float degangle = ((int) ((micros() / 250) * 5.625) % 360 );
  float angle = rad((degangle));
  float value = 50.0 + (50.0 * sin(angle));
  duty = (int8_t) value;

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

float deg(float rad) {
  return rad / 2 / pi * 360;
}

float rad(float deg) {
  return deg / 360 * 2 * pi;
}

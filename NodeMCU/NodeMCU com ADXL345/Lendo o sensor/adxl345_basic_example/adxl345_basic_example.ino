/*
 * SGN - Robótica Educacional
 * DEZ-2017
 *
 * Projeto: Exemplo de utilização do sensor ADXL345 com NodeMCU
 *
 * Resumo: Inicialização e leitura do sensor ADXL345 com evio de dados através da porta serial.
 *
 * Configuração da IDE: Placa - NodeMCU 1.0 (ESP-12E Module)
 * 
 * CONEXÕES:
 *
 * ADXL345    NodeMCU
 * VCC    - 3V
 * GND    - GND
 * CS   - Não Conectado
 * INT1   - Não Conectado
 * INT2   - Não Conectado
 * SDO    - Não Conectado
 * SDA    - D2
 * SCL    - D1
 *
 */

/* ADXL345 Library available at https://github.com/jenschr/Arduino-libraries */
#include "ADXL345.h"

ADXL345 adxl;

double samplingRate = 100;

void setup()
{
  Serial.begin(115200);

  /* Start the sensor */
  adxl.powerOn();

  /* Set the sampling rate to 100Hz.
   * The available values are: 3200, 1600, 800, 400, 200, 50, 25, 12.5 and 6.25
   */
  samplingRate = 6.25;
  adxl.setRate(samplingRate);

    /* Set the sensor full scale range to 2G */
  adxl.setRangeSetting(2);
}

void loop()
{
  int x,y,z;
  adxl.readAccel(&x, &y, &z);

  Serial.print("X = ");
  Serial.print((int16_t) x);
  Serial.print(" | Y = ");
  Serial.print((int16_t) y);
  Serial.print(" | Z = ");
  Serial.println((int16_t) z);

  delayMicroseconds(1000000/samplingRate);
}


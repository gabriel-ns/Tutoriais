/**
 * SGN Robótica Educacional - 2017
 * 
 * Sketch I2C_DAC_MCP4725
 * 
 * Descrição: 
 * Este sketch tem como objetivo exemplificar o uso dos comandos da biblioteca
 * Wire na utilização do DAC MCP4725 para a geração de formas de ondas desejadas.
 * 
 * Datasheet do componente: https://www.sparkfun.com/datasheets/BreakoutBoards/MCP4725.pdf
 */

#define SINE_WAVE     0
#define SAWTOOTH_WAVE 1
#define TRIANG_WAVE   2
#define SQUARE_WAVE   3

#define WAVE_SELECTION 0

#if (WAVE_SELECTION < 0 || WAVE_SELECTION > 3)
#error  "INVALID WAVE TYPE"
#endif 

#include <Wire.h>
#include <math.h>

#define MCP4725_DEVICE_ADDRESS    0x62    /** Endereço I2C de 7 bits do DAC. O endereço pode ser obtido no datasheet. */
#define COMMAND_WRITE_DAC         0x40    /** Comando para escrever dados na memória do DAC. */

/** 
 *  Cria uma variável para armazenar a tensão de saída do DAC em milivolts.
 *  Esta variável é unsigned pois não utilizaremos valores negativos.
 *  Para evitar a escrita de valores aleatórios, esta variável será inicializada zerada.
 */
unsigned int voltage_mv = 0;

float angle = 0;
void setup() {

  /** Inicialização da comunicação I2C. Não é necessário atribuir um endereço */
  Wire.begin();
  TWBR = ((F_CPU / 400000L) - 16) / 2; // Set I2C frequency to 400kHz

}

void loop() {

  /**
   * De acordo com o datasheet, a tensão de saída depende da tensão de alimentação do DAC.
   * No caso de alimentação com o VCC do Arduino, esta tensão vale 5V, ou 5000mV.
   * Como o DAC possui uma resolução de 12 bits, temos 4096 intervalos, onde cada incremento
   * equivale a 5000/4096 mV.
   * 
   * Para transformar a tensão desejada no código binário para o DAC dividi-se a tensão
   * desejada pelo valor de um incrementom resultando em:
   * 
   * código = (tensão) * (4096) / (5000)
   * 
   * OBS: Uma variável uint16_t significa unsigned int de 16 bits.
   * 
   * Nota-se o seguinte: ((uint32_t) (voltage_mv*(4096)))
   * 
   * uint32_t significa que unsigned int de 32 bits. Na posição em que foi escrito, isso irá
   * converter o resultado da operação (voltage_mv*(4096)) num número de 32 bits. Mas por quê?
   * Pela seguinte razão: A variável code é de 16 bits, logo essa operação será feita com números de 
   * 16 bits. O problema é que isso significa que o valor máximo que esta variável pode assumir é 
   * de 2^16, ou seja, 65535.
   * Como estamos multiplicando um valor por 4096, isso estouraria o valor da variável(o chamado overflow) em qualquer multiplicação
   * com voltage_mv valendo mais do que 15, resultando num erro de cálculo.
   * Convertendo esta operação para um número de 32 bits, garantimos que não haverá overflow,
   * e assim o cálculo estará correto!
   * 
   */
   uint16_t code = ((uint32_t)voltage_mv*4096)/5000;

   Wire.beginTransmission(MCP4725_DEVICE_ADDRESS);    /** Inicia a comunicação com o DAC */
   Wire.write(COMMAND_WRITE_DAC);                     /** Envia o comando sinalizando que iremos enviar um comando para o DAC */
   
   Wire.write(code / 16);                               /** Esta operação irá filtrar os 16 bits da variável code,
                                                        * sendo que o resultado será apenas os 8 bits mais significativos
                                                        * formando um byte completo. Esta divisão é feita de acordo com o datsheet
                                                        * do dipositivo.
                                                        * O resultado da operação é enviado ao DAC
                                                        */
  Wire.write((code % 16) << 4);                          /** Já esta operação irá selecionar apenas os bits
                                                          * menos significativos do código.
                                                          * Em seguida, este valor é deslocado em 4 posições
                                                          * de forma que o byte possua a informação em seus bits mais
                                                          * significativos, conforme datasheet
                                                           */
 Wire.endTransmission();                                /** Encerra a comunicação */


 /** Seleção da forma de onda:
  *   A forma de onda será dada pela forma como a variável voltage_mv
  *   for modificada.
  */
  if(WAVE_SELECTION == SINE_WAVE)
  {
      angle = angle + 0.05;   /** Incremento do ângulo em radianos */
      voltage_mv = abs((((float)500 * sin(angle))));   /** Como não temos valores negativos para o DAC, utilizaremos valores absolutos, resultando numa onda somente positiva. */
      if(angle > 2*PI) angle = 0;
  }
  else if(WAVE_SELECTION == SAWTOOTH_WAVE)
  {
    voltage_mv = voltage_mv + 50;        /** Incremento do valor da tensão */
    if(voltage_mv > 4000) 
    {
      voltage_mv = 0; /** Limitador para o valor da tensão */
    }
  }
  else if(WAVE_SELECTION == TRIANG_WAVE)
  {
    static bool sinal = true;
    if(sinal) voltage_mv = voltage_mv + 100;
    else voltage_mv = voltage_mv - 100;
    if(voltage_mv <= 0 || voltage_mv >= 4000) sinal = !sinal;
  }
  else if(WAVE_SELECTION == SQUARE_WAVE)
  {
    if(voltage_mv == 0) voltage_mv = 1500;
    else voltage_mv = 0;
    delayMicroseconds(7500);
    
  }

}

/**
   SGN Robótica Educacional - Agosto/2017

   Datalogger utilizando RTC e Cartão SD.

   Bibliotecas utilizadas:
   SPI, Wire e SD: Incluídas na IDE do Arduino

   RTCLib -> https://github.com/adafruit/RTClib
   IRremote -> https://github.com/z3t0/Arduino-IRremote

   Conexões:

   Arduino UNO | Módulo MicroSD
   VCC -> VCC
   GND -> GND
   4   -> CS
   11  -> MOSI
   12  -> MISO
   13  -> SCK

   ARDUINO UNO | RTC1307
   VCC  -> VCC
   GND  -> GND
   A4   -> SDA
   A5   -> SCL
*/

#include <SPI.h>
#include <Wire.h>
#include <SD.h>
#include <RTClib.h>
#include <IRremote.h>

int irPin = 4;
int sdCardCSpin = 10;
int ldrPin = A0;
int ntcPin = A1;
int distEchoPin = 6;
int distTrigPin = 7;

// Cria o objeto RTC
RTC_DS1307 RTC;

// Cria o receptor de infravermelho
IRrecv IRSensor(irPin);
decode_results results;

File file;

String temperatureFilename = "temp.csv";
String luminosityFilename  = "lum.csv";
String distanceFilename    = "dist.csv";
String IRCommandsFilename  = "IR_cmd.csv";

unsigned long lastReadTime = 0;
unsigned long lastIRCheckTime = 0;

// Protótipos de funções
int   getDistance();
int   getLuminosity();
int   getTemperature();
void  checkIRSensor();

void storeDistance();
void storeLuminosity();
void storeTemperature();
void storeIRCommand(unsigned long cmd);

void  createDataFiles();
void  writeTimestampToFile();
bool createFile(String filename);
bool openFile(String filename);

void setup() {
  Serial.begin(9600);

  pinMode(sdCardCSpin, OUTPUT);
  pinMode(ldrPin, INPUT);
  pinMode(ntcPin, INPUT);
  pinMode(distEchoPin, INPUT);
  pinMode(distTrigPin, OUTPUT);

  digitalWrite(distTrigPin, LOW);

  SD.begin();

  createDataFiles();

  IRSensor.enableIRIn(); // Inicializa o sensor Infravermelho

  RTC.begin();

  // Este comando irá ajustar o RTC para o momento em que o código for compilado.
  // É provável que exista uma diferença de segundos entre o horario real e o horário marcado.
  // Remova o comentário da linha para fazer o ajuste
  // O ajuste é necessário sempre que a bateria for removida

  // RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));

  // Para especificar uma data para o RTC, use este comando
  // Exemplo de data: 17/08/2017 20:48:30:

  // RTC.adjust(DateTime(2017, 8, 17, 20, 48, 30));

}

void loop() {

  if (millis() - lastReadTime >= 5000)
  {
    lastReadTime = millis();
    
    storeDistance();
    storeLuminosity();
    storeTemperature();
  }

  if (millis() - lastIRCheckTime >= 150)
  {
    lastIRCheckTime = millis();
    checkIRSensor();
  }


}

int   getDistance()
{
  digitalWrite(distTrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(distTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(distTrigPin, LOW);

  long duration = pulseIn(distEchoPin, HIGH, 300000);

  /**
     Velocidade do som = 340,29 m/s

     duration é o tempo de ida e volta em microsegundos.
     A precisão do sensor é dada na ordem de centímetros, então calcula-se
     a velocidade do som em cm/us

     1m -> 100 cm
     1s -> 100000 us

     Vsom = 340,29 * 100 / 1000000 cm/us

     Vsom = 0,034029

     Dada a equação da velocidade V = S/T, temos que
     S = V*T
     S = 0,030429 * T
     0,030429 = 1/29,3867

     S = T / 29,3867

     Como duration = distâcida de ida e volta:

     S = (duration/2) / 29,3867
  */
  float distance = (duration / 2) / 29.3867;

  int dist = distance;

  if (dist > 200) return 0;
  return dist;
}

int   getLuminosity()
{
  return analogRead(ldrPin);
}

int   getTemperature()
{
  return analogRead(ntcPin);
}

void  checkIRSensor()
{

  if (IRSensor.decode(&results)) {
    storeIRCommand(results.value);
    IRSensor.resume(); // Receive the next value
  }
}

void  createDataFiles()
{
  // Verifica a existência do arquivo de registro de temperatura no cartão de memória
  if (!SD.exists(temperatureFilename))
  {
    delay(100);
    // Se o arquivo não existir, tenta criar o arquivo
    if (createFile(temperatureFilename) == true)
    {
      // Escreve os títulos das colunas no arquivo
      file.println(F("Data;Hora;Temperatura"));
      file.close();
    }
  }
  // Verifica a existência do arquivo de registro de luminosidade no cartão de memória
  if (!SD.exists(luminosityFilename))
  {
    delay(100);
    // Se o arquivo não existir, tenta criar o arquivo
    if (createFile(luminosityFilename) == true)
    {
      // Escreve os títulos das colunas no arquivo
      file.println(F("Data;Hora;Luminosidade"));
      file.close();
    }
  }

  // Verifica a existência do arquivo de registro de distância no cartão de memória
  if (!SD.exists(distanceFilename))
  {
    delay(100);
    // Se o arquivo não existir, tenta criar o arquivo
    if (createFile(distanceFilename) == true)
    {
      // Escreve os títulos das colunas no arquivo
      file.println(F("Data;Hora;Distancia"));
      file.close();
    }
  }

  // Verifica a existência do arquivo de registro de comandos de controle remoto no cartão de memória
  if (!SD.exists(IRCommandsFilename))
  {
    delay(100);
    // Se o arquivo não existir, tenta criar o arquivo
    if (createFile(IRCommandsFilename) == true)
    {
      // Escreve os títulos das colunas no arquivo
      file.println(F("Data;Hora;Comando Recebido"));
      file.close();
    }
  }
}

bool createFile(String filename)
{
  file = SD.open(filename, FILE_WRITE);

  if (file)
  {
    Serial.println("Arquivo '" + filename + "' criado com sucesso!");
    return true;
  } else
  {
    Serial.println("Erro ao criar o arquivo '" + filename + "'");
    return false;
  }
}

void  writeTimestampToFile()
{
  DateTime now = RTC.now();

  file.print(now.day(), DEC);
  file.print('/');
  file.print(now.month(), DEC);
  file.print('/');
  file.print(now.year(), DEC);
  file.print(';');
  file.print(now.hour(), DEC);
  file.print(':');
  file.print(now.minute(), DEC);
  file.print(':');
  file.print(now.second(), DEC);
  file.print(";");
}

void storeDistance()
{
  if (openFile(distanceFilename))
  {
    int dist = getDistance();
    writeTimestampToFile();
    file.println(dist);
    file.close();
  }
}

void storeLuminosity()
{
  if (openFile(luminosityFilename))
  {
    int lum = getLuminosity();
    writeTimestampToFile();
    file.println(lum);
    file.close();
  }
}
void storeTemperature()
{
  if (openFile(temperatureFilename))
  {
    int temp = getTemperature();
    writeTimestampToFile();
    file.println(temp);
    file.close();
  }
}
void storeIRCommand(unsigned long cmd)
{
  if (openFile(IRCommandsFilename))
  {
    writeTimestampToFile();
    file.println(cmd, HEX);
    file.close();
  }
}

bool openFile(String filename)
{
  file = SD.open(filename, FILE_WRITE);

  if (file)
  {
    Serial.println("Arquivo '" + filename + "' aberto com sucesso!");
    return true;
  } else
  {
    Serial.println("Erro ao abrir o arquivo '" + filename + "'");
    return false;
  }
}



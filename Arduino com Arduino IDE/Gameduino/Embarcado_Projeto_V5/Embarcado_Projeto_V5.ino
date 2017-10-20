#include <SPI.h>
#include <GD.h>
#include <Keypad.h>
#include "bgm.h"
#include <avr/pgmspace.h>

#define SINE 0

#include "spritesheet.h"
#include "bkg.h"

#define LEFT      1
#define DOWN      2
#define UP        4
#define RIGHT     8

#define ANALOGENABLE 0

#define P1_LEFT   bitMap[0]&1
#define P1_DOWN   bitMap[0]&4
#define P1_UP     bitMap[0]&2
#define P1_RIGHT  bitMap[0]&8

#define P2_LEFT   bitMap[1]&1
#define P2_DOWN   bitMap[1]&2
#define P2_UP     bitMap[1]&4
#define P2_RIGHT  bitMap[1]&8

#define P1_B4     bitMap[2]&1
#define P1_B1     bitMap[2]&2
#define P1_B3     bitMap[2]&4
#define P1_B2     bitMap[2]&8

#define P2_B4     bitMap[3]&1
#define P2_B1     bitMap[3]&2
#define P2_B3     bitMap[3]&4
#define P2_B2     bitMap[3]&8

//PINOS NO ARDUINO
#define ROW0 0 //FIO VERDE
#define ROW1 1 //FIO VERMELHO
#define ROW2 2 //FIO ROXO
#define ROW3 3 //FIO OU MARRINZA

#define COL0 4 //FIO VERDE - JOYSTICK LEFT
#define COL1 5 //FIO AMARELO - JOYSTICK UP
#define COL2 6 //FIO PRETO - JOYSTICK DOWN
#define COL3 7 // FIO LARANJA - JOYSTICK RIGHTs


#define BG_ZERO         0
#define BG_A            11
#define BG_DOT          10
#define BG_BLACK        38
//#define BG_WHITE        41
#define BG_RED          45
#define BG_GREEN        46
#define BG_YELLOW       47


const byte ROWS = 4 ; // Four rows
const byte COLS = 4; // Three columns
char keys[ROWS][COLS] = {
  {'0', '1', '2', '3'},
  {'4', '5', '6', '7'},
  {'8', '9', 'A', 'B'},
  {'C', 'D', 'E', 'F'},
};
byte rowPins[ROWS] = {ROW0, ROW1, ROW2, ROW3};
byte colPins[COLS] = {COL0, COL1, COL2, COL3};
Keypad player = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS );

/*VARIÁVEIS DOS SONS*/

long soundLastTime1 = 0;
long soundNextTime1 = 0;
int soundSize1 = sizeof(voice1) / sizeof(byte);
int soundPointer1 = 0;
bool soundPlay1 = 0;


long soundLastTime2 = 0;
long soundNextTime2 = 0;
int soundSize2 = sizeof(voice2) / sizeof(byte);
int soundPointer2 = 0;
bool soundPlay2 = 0;
bool enableSound = 0;

bool SFXenable = 0;
byte SFXcounter = 0;
byte SFXtimer=150;
long SFXlastTime = 0;



/* Variáveis de tempo das Threads */
#define printTime 35
#define rotateTime 250
long lastPrintTime = 0;
long lastRotateTime = 0;

/******************************/

/*Parametros do jogo */
#define MAXBOOKNUMBER 30
#define MINBOOKNUMBER 15
#define speedLimit 3
byte booknumber = MINBOOKNUMBER;
byte lastBookNumber = MINBOOKNUMBER;

/*
   GAMESTATE
   1 - INTRODUÇÃO: MENU COM OS NÍVEIS DE DIFICULDADE
   2 - EM JOGO:    JOGO RODANDO
   3 - JUBILADO:   TELA DE ENDGAME - JUBILADO + MENU: REINGRESSAR?S/N
   4 - PARABENS VOCE SE FORMOU !
*/
byte gameState = 1;
int actual_record = 0;

/*
    Limites da tela
*/
#define leftEdge 16
#define rightEdge 368
#define topEdge 48
#define bottomEdge 272


#define ANIMATION_COUNTER 2

static void clear_line(byte y) {
  uint16_t dst = RAM_PIC + 64 * y;
  for (int i = 0; i < 50; i++)GD.wr(dst + i, BG_BLACK); //PONTO
}


static void print_text(byte x, byte y, String text) {
  uint16_t dst = RAM_PIC + 64 * y + x;
  for (int i = 0; i < text.length(); i++) {
    if (text.charAt(i) == ' ') GD.wr(dst + i, BG_BLACK);
    else if (text.charAt(i) == '-') GD.wr(dst + i, 37);
    else if (text.charAt(i) < 'A')GD.wr(dst + i, text.charAt(i) - '0' + BG_ZERO);
    else GD.wr(dst + i, text.charAt(i) - 'A' + BG_A);

  }

}

struct Player {
  byte counter = ANIMATION_COUNTER;
  int x, y;
  //PD - PLAYER DATA
  //SPRITE + ROTACAO
  byte pD;
  byte lastCollision = 0xff;
  bool collision;
  bool immunity = 0;
  unsigned long CP = 0;
  byte CR = 4;
};

static struct Player P1;

void readn(byte *dst, unsigned int addr, int c) {
  GD.__start(addr);
  while (c--)
    *dst++ = SPI.transfer(0);
  GD.__end();
}

unsigned long lastCollisionTime = 0;
static byte coll[256];
static void load_coll() {
  while (GD.rd(VBLANK) == 0)  // Wait until vblank
    ;
  while (GD.rd(VBLANK) == 1)  // Wait until display
    ;
  while (GD.rd(VBLANK) == 0)  // Wait until vblank
    ;
  readn(coll, COLLISION, sizeof(coll));

  P1.collision = 0;

  for (byte i = 0; i <= MAXBOOKNUMBER; i++) {
    if (coll[i] == 0 && i != P1.lastCollision) {
      P1.collision = 1;
      P1.lastCollision = i;
      lastCollisionTime = millis();
      Serial.println(i);
      break;
    }

  }
  if (  P1.collision == 0 && millis() - lastCollisionTime >= 500 )P1.lastCollision = 0;


}


/*
   Estrutura de um livro
*/
struct bookSprites {
  int x, y;
  /*bookSpeed
      Os 4 primeiros bits referem-se a velocidade em X.
      Os 4 ultimos, a Y.
      Os bits 0 e 4 são de sinal.
      sXXXsYYY
  */
  byte bSpd;
  /*bookData
     Bits 0-3 Determina a cor (img) do livro.
     Bits 4-7 Determina rotação

     CORES:
     10 - VERDE
     11 - VERMELHO
     12 - AZUL
     13 - AMARELO
     14 - CINZA

     SEQUENCIA DE ROTAÇÃO:
     0 - 5  -  6  -  3 -  0
  */
  byte bD;
};

static struct  bookSprites book[MAXBOOKNUMBER];

int getSpriteImg(byte bD) {
  int img = bD >> 4;
  switch (img) {
    case 0:
    case 1:
      img = 0;
      break;

    case 2:
    case 3:
      img = 1;
      break;

    case 4:
    case 5:
      img = 2;
      break;

    case 6:
    case 7:
      img = 3;
      break;

    case 8:
    case 9:
      img = 4;
      break;

    case 10:
    case 11:
      img = 5;
      break;

    case 12:
    case 13:
      img = 6;
      break;

    case 14:
      img = 7;
      break;

    default:
      img = 0;
      break;
  }
  return img;

}

/* Determina qual será a paleta de cores do sprite */
int getSpritePal(byte bd) {
  int pal = bd >> 4;
  if (pal % 2 == 0) pal = 4;
  else pal = 6;
  return pal;

}

//mySprite - Decodifica o sprData, que contem a imagem e a rotação, e envia ao gameduino.
void mySprite(int spr, int x, int y, byte bD) {
  //GD.sprite(sprite number, x, y, img, pal, rot, jk)
  int img, pal;
  byte rot;
  //rot = bD;
  rot = (bD << 4);
  rot >>= 4;
  img = getSpriteImg(bD);
  pal = getSpritePal(bD);
  // Serial.println("TESTE");
  GD.sprite(spr, x, y, img, pal, rot, 0);
}

//moveSprite - Calcula a nova posição dos livros/*

void moveSprites() {
  //Livros
  char vx;
  char vy;
  for (int i = 1; i <= booknumber; i++) {
    //Cálculo de vx
    vx = book[i - 1].bSpd >> 4;
    if (vx > speedLimit) vx = (vx - speedLimit) * (-1);
    book[i - 1].x = book[i - 1].x + vx;

    //calculo de vy
    vy =  book[i - 1].bSpd << 4;
    vy = vy >> 4;
    if (vy > speedLimit) vy = (vy - speedLimit) * (-1);
    book[i - 1].y = book[i - 1].y + vy;

    //colisão com a borda
    if ( (book[i - 1].y > bottomEdge && vy > 0)  || (book[i - 1].y < topEdge && vy < 0)) {
      if (vy > 0) vy = vy + speedLimit;
      else vy *= (-1);
      book[i - 1].bSpd >>= 4;
      book[i - 1].bSpd <<= 4;
      book[i - 1].bSpd += vy;
    }

    if ((book[i - 1].x > rightEdge && vx > 0)  || (book[i - 1].x < leftEdge && vx < 0)) {
      if (vx > 0) vx = vx + speedLimit;
      else vx *= (-1);
      book[i - 1].bSpd <<= 4;
      book[i - 1].bSpd >>= 4;
      book[i - 1].bSpd += (vx << 4);
    }



  }
}

//Desenhar todos os livros na tela
void printSprites() {
  int i = 0;
  for (i = 1; i <= MAXBOOKNUMBER; i++) {
    mySprite(i, book[i - 1].x, book[i - 1].y, book[i - 1].bD);
  }
}


byte nextRotation(int x) {
  byte rot;
  switch (x) {
    case 0:
      rot = 5;
      break;
    case 5:
      rot = 6;
      break;
    case 6:
      rot = 3;
      break;
    default:
      rot = 0;
  }
  return rot;
}

void rotateSprites() {
  byte rot;
  for (int i = 1; i <= MAXBOOKNUMBER; i++) {
    rot =  book[i - 1].bD << 4;
    rot >>= 4;
    //Serial.println("in Data = " + String(book[i-1].bD) + " || in Rot = " + String(rot, DEC));
    rot = nextRotation(rot);
    book[i - 1].bD >>= 4;
    /*book[i - 1].bD++;
      if (book[i - 1].bD > 14) book[i - 1].bD = 10;*/
    book[i - 1].bD <<= 4;
    book[i - 1].bD += rot;
    // Serial.println("out Data = " + String(book[i-1].bD) + " || out Rot = " + String(rot, DEC));
  }

}

void startGame() {

  int bn = 0; //book number
  booknumber = MINBOOKNUMBER;
  lastBookNumber = 5;
  for (int i = 1; i <= MAXBOOKNUMBER; i++) {
    bn = i - 1;
    if (bn < booknumber) {
      do {

        book[bn].x = random(16, 350);
        book[bn].y = random(16, 284);


      } while ( book[bn].x > ((leftEdge + rightEdge) * 0.35) &&
                book[bn].x < ((leftEdge + rightEdge) * 0.65) &&
                book[bn].y > ((topEdge + bottomEdge) * 0.35) &&
                book[bn].y < ((topEdge + bottomEdge) * 0.65));
    }

    else {
      book[bn].x = 450;
      book[bn].y = 350;
    }
    book[bn].bSpd = (random(1, (1 + (2 * speedLimit))) << 4) + random(1, (1 + (2 * speedLimit)));
    //book[bn].bSpd = 172;
    book[bn].bD = (random(10, 15) << 4) + random(0, 7);
    //mySprite(bn, book[bn].x, book[bn].y, book[bn].bD);
  }

  P1.x = (leftEdge + rightEdge) / 2;
  P1.y = (topEdge + bottomEdge) / 2;
  P1.pD = 0;
  mySprite(0, P1.x, P1.y, P1.pD);
  load_coll();
  P1.collision = 0;
  //gameStart = 1;
  P1.CP = 0;
  P1.CR = 4;

}

void load_arena() {
  for (byte y = 0; y < 38; y++)  GD.copy(RAM_PIC + y * 64, bkg_pic + y * 50, 50);
  GD.copy(RAM_CHR, bkg_chr, sizeof(bkg_chr));
  GD.copy(RAM_PAL, bkg_pal, sizeof(bkg_pal));
  //GD.sprite(sprite number, x, y, img, pal, rot, jk)
  GD.sprite(255, 200 - 8, 32, 4, 4, 0, 0);
  clear_line(0);
  print_text(0, 0, "      PROGRAMACAO DE SOFTWARE EMBARCADO 2Q2016");
  print_text(5, 1, "CP");
  print_text(15, 1, "CR");
  print_text(30, 1, "NUMERO DE FORMADOS");
  print_text(30, 2, "MAIS DE 6000 ALUNOS");



}


void setup() {
  //Inicia o Gameduino
  GD.begin();
  Serial.begin(115200);
  pinMode(4, INPUT_PULLUP);
  //Carregar as paletas
  GD.copy(PALETTE16A, spritesheet_sprpal, sizeof(spritesheet_sprpal));
  GD.copy(RAM_SPRIMG, spritesheet_sprimg, sizeof(spritesheet_sprimg));
  load_arena();
  gameState = 1;
  //startGame();

}


void switchAnim(byte dir) {
  byte img = P1.pD >> 4;
  byte rot =  P1.pD << 4;
  rot >>= 4;




  if (dir == 0) {
    if (img == 1) img = 0;
    if (img == 3) img = 2;
    if (img == 5) img = 4;
  }

  else if (dir == LEFT) {
    if (img == 2) img = 3;
    else img = 2;
    rot = 0;

  }

  else if (dir == RIGHT) {
    if (img == 2) img = 3;
    else img = 2;
    rot = 2;
  }

  else if (dir == UP) {
    if (rot == 0) rot = 2;
    else rot = 0;
    img = 5;

  }

  else if (dir == DOWN) {
    if (rot == 0) rot = 2;
    else rot = 0;
    img = 1;
  }

  // if(P1.collision) img = 8;

  P1.pD = (img << 4) + rot;

}

void readPlayerInput() {
  float tmp;
  byte dir = 0;
  byte v = 1;
  int read_x = 500;
  int read_y = 500;
  if (player.P1_B3)v = 3;
  else if (player.P1_B4)v = 3;

  if (ANALOGENABLE) {
    read_x = analogRead(A0);
    read_y = analogRead(A1);
  }




  if (read_x > 900 || read_x < 100 || read_y > 900 || read_y < 100) v = 2;

  // if( (book[i-1].y > bottomEdge && vy > 0)  || (book[i-1].y < topEdge && vy < 0)){
  // if( (book[i-1].x > rightEdge && vx > 0)  || (book[i-1].x < leftEdge && vx < 0)){


  if (player.P1_LEFT ||  read_x > 600) {
    dir = LEFT;
    if (P1.x > leftEdge) P1.x -= v;
  }

  else if (player.P1_RIGHT ||  read_x < 450) {
    dir = RIGHT;
    if (P1.x < rightEdge)P1.x += v;
  }

  if (player.P1_DOWN || read_y < 450) {
    dir = DOWN;
    if (P1.y < bottomEdge)P1.y += v;
  }
  else if (player.P1_UP || read_y > 600) {
    dir = UP;
    if (P1.y > topEdge)P1.y -= v;
  }


  if (player.bitMap[0] != 0 || read_x > 600 || read_x < 450 || read_y > 600 || read_y < 450) {
    P1.counter--;
    if (P1.counter <= 0) {
      switchAnim(dir);
      P1.counter = ANIMATION_COUNTER;
    }
  }

  else {
    switchAnim(0);
    P1.counter = ANIMATION_COUNTER;
  }

}


static void draw_score(byte x, byte y, long n) {
  uint16_t dst = RAM_PIC + 64 * y + x;
  GD.wr(dst + 0, BG_ZERO + (n / 100) % 10);
  GD.wr(dst + 1, BG_ZERO + 10);    //PONTO
  GD.wr(dst + 2, BG_ZERO + (n / 10) % 10);
  GD.wr(dst + 3, BG_ZERO + n  % 10);
}


static void startBook(byte bn) {
  book[bn].x = (leftEdge + rightEdge) / 2;
  if (bn % 2 == 0) book[bn].y = topEdge + 1;
  else book[bn].y = bottomEdge - 1;
}

static void clear_screen() {
  for (int i = 0; i < 38; i++) clear_line(i);
  mySprite(0, 450, 350, P1.pD);
}

static void clear_books() {
  for (int i = 0; i < MAXBOOKNUMBER; i++) {
    book[i].x = 450;
    book[i].y = 350;
  }
  printSprites();

}

void startSound() {
  soundPointer1 = 0;
  soundPlay1 = 0;
  soundPointer2 = 0;
  soundPlay2 = 0;
  enableSound = 1;

 soundLastTime1 = 0;
 soundNextTime1 = 0;
 soundLastTime2 = 0;
 soundNextTime2 = 0;

}

void stopSound() {
  stopTone(6);
  stopTone(0);
  soundPointer1 = 0;
  soundPlay1 = 0;
  soundPointer2 = 0;
  soundPlay2 = 0;
  enableSound = 0;
}
void playTone(int freq, int voice, int amp) {
  GD.voice(voice, SINE, freq, amp, amp);
  GD.voice(voice + 1, SINE, freq * 1.5, amp * 0.75, amp * 0.75);
  GD.voice(voice + 2, SINE, freq * 2.0, amp * 0.5, amp * 0.5);
  GD.voice(voice + 3, SINE, freq * 2.5, amp * 0.16, amp * 0.16);
  GD.voice(voice + 4, SINE, freq * 3.0, amp * 0.01, amp * 0.01);
  GD.voice(voice + 5, SINE, freq * 3.5, amp * 0.01, amp * 0.01);
}

void stopTone(int voice) {

  GD.voice(voice, SINE, 0, 0, 0);
  GD.voice(voice + 1, SINE, 0, 0, 0);
  GD.voice(voice + 2, SINE, 0, 0, 0);
  GD.voice(voice + 3, SINE, 0, 0, 0);
  GD.voice(voice + 4, SINE, 0, 0, 0);
  GD.voice(voice + 5, SINE, 0, 0, 0);

}

void loop() {

  String s;

  byte dir = player.getKey();
  if (gameState == 1) {

    clear_screen();
    //byte dir = player.getKey();
    s =  F("PARABENS");
    print_text(25 - (s.length() / 2), 8, s);
    s =  F("VOCE PASSOU NO ENEM");
    print_text(25 - (s.length() / 2), 10, s);
    s =  F("AGORA VOCE DEVE SUPERAR OS DESAFIOS");
    print_text(25 - (s.length() / 2), 12, s);
    s =  F("PRESENTES NA GRADUACAO DA UFABC");
    print_text(25 - (s.length() / 2), 14, s)    ;
    s =  F("DESEJAMOS BOA SORTE A VOCE");
    print_text(25 - (s.length() / 2), 16, s);
    s =  F("VOCE VAI PRECISAR");
    print_text(25 - (s.length() / 2), 18, s);
    //print_text(25-(s.length()/2), 9, s);
    s =  F("PRESSIONE O BOTAO AZUL PARA INICIAR");
    print_text(25 - (s.length() / 2), 24, s);
    s =  F("PRESSIONE O BOTAO VERMELHO PARA OBTER INSTRUCOES");
    print_text(25 - (s.length() / 2), 28, s);
    //free(s);
    do {
      dir = player.getKey();
      if (player.P1_B1) gameState = 2;
      else if (player.P1_B2) gameState = 5;
      delay(100);
    } while (gameState == 1);
    while (player.P1_B1 || player.P1_B2)dir = player.getKey();



  }

  if (gameState == 2) {
    load_arena();
    startGame();
    startSound();
    do {
      byte dir = player.getKey();

      //Movimento
      if (millis() - lastPrintTime >= printTime) {

        lastPrintTime = millis();
        readPlayerInput();
        mySprite(0, P1.x, P1.y, P1.pD);
       //  mySprite(0, 450, P1.y, P1.pD);


        booknumber = MINBOOKNUMBER + (P1.CP / (100 / (MAXBOOKNUMBER - MINBOOKNUMBER)));


        if (booknumber != lastBookNumber) {
          startBook(booknumber);
          lastBookNumber = booknumber;
        }


        moveSprites();
        printSprites();

        load_coll();
        if (P1.collision) {
          P1.CR--;
          SFXenable=1;
        }


      }
/*
 *    bool SFXenable = 0;
      byte SFXcounter = 0;
      byte SFXtimer=150;
      long SFXlastTime = 0;

 */
      if((millis() - SFXlastTime > SFXtimer) && SFXenable){
            SFXlastTime = millis();
            
            if(SFXcounter == 0){
                  SFXcounter++;
                  playTone((16000/P1.CR), 20, 75);
                  SFXtimer = 200;                                                   
            }
            else if (SFXcounter == 1){
                  SFXcounter++;
                  playTone((12000/P1.CR), 20, 75);
                  SFXtimer = 600;
            }
            else{
                  SFXcounter=0;
                  SFXenable=0;
                  stopTone(20);
                  SFXtimer = 200;
                  
                  
            }
            
      }

      if (millis() - lastRotateTime >= rotateTime) {
        lastRotateTime = millis();
        rotateSprites();
        P1.CP++;
        draw_score(4, 2, P1.CP);
        if (P1.CP > 100)  P1.CP = 0;
        print_text(15, 2, String(P1.CR));
      }

      int freq = 0;

      if ((millis() - soundLastTime1 >= soundNextTime1) && enableSound) {
        soundLastTime1 = millis();
        if (soundPointer1 < (soundSize1 / 2)) {
          freq = pgm_read_word_near(scaleFrequency + pgm_read_byte_near(voice1 + soundPointer1)) * 4;
          if (!soundPlay1) {
            playTone(freq, 0, 50);
            soundPlay1 = 1;
            soundNextTime1 = pgm_read_byte_near(tempo1 + soundPointer1) *(32-(P1.CP/10));

          }
          else {
            stopTone(0);
            soundNextTime1 =  pgm_read_byte_near(tempo1 + soundPointer1) * 2;
            soundPlay1 = 0;
            soundPointer1++;

          }
        }

        else if (soundPointer1 < soundSize1) {
          if (soundPointer2 >= soundSize2 / 2) {
            freq = pgm_read_word_near(scaleFrequency + pgm_read_byte_near(voice1 + soundPointer1)) * 4;
            if (!soundPlay1) {
              playTone(freq, 0, 50);
              soundPlay1 = 1;
              soundNextTime1 = pgm_read_byte_near(tempo1 + soundPointer1) * (32-(P1.CP/10));

            }
            else {
              stopTone(0);


              soundNextTime1 =  pgm_read_byte_near(tempo1 + soundPointer1) * 2;
              soundPlay1 = 0;
              soundPointer1++;

            }

          }
        }
        else {


          startSound();



        }
      }

      if ((millis() - soundLastTime2 >= soundNextTime2) && enableSound) {
        soundLastTime2 = millis();
        if (soundPointer2 < (soundSize2 / 2)) {
          freq = pgm_read_word_near( scaleFrequency + pgm_read_byte_near(voice2 + soundPointer2)) * 4;
          if (!soundPlay2) {
            playTone(freq, 6, 40);
            soundPlay2 = 1;
            soundNextTime2 = pgm_read_byte_near(tempo2 + soundPointer2) * (32-(P1.CP/10));

          }
          else {
            stopTone(6);


            soundNextTime2 =  pgm_read_byte_near(tempo2 + soundPointer2) * 2;
            soundPlay2 = 0;
            soundPointer2++;


          }
        }

        else if (soundPointer2 < soundSize2) {
          if (soundPointer1 >= (soundSize1 / 2)) {
            freq = pgm_read_word_near( scaleFrequency + pgm_read_byte_near(voice2 + soundPointer2)) * 4;
            if (!soundPlay2) {
              playTone(freq, 6, 40);
              soundPlay2 = 1;
              soundNextTime2 = pgm_read_byte_near(tempo2 + soundPointer2) * (32-(P1.CP/10));

            }
            else {
              stopTone(6);


              soundNextTime2 =  pgm_read_byte_near(tempo2 + soundPointer2) * 2;
              soundPlay2 = 0;
              soundPointer2++;


            }
          }
        }

        else {
          startSound();

        }
      }



      if (P1.CR <= 0) gameState = 3;
      if (P1.CP >= 100) gameState = 4;
    } while (gameState == 2);
    stopSound();
  }

  if (gameState == 3) {
    // String s;
    clear_screen();
     stopTone(20);
    clear_books();
    s =  F("JUBILADO ");
    print_text(25 - (s.length() / 2), 15, s);
    s =  F("NAO SE PREOCUPE");
    print_text(25 - (s.length() / 2), 17, s);
    s =  F("VOCE NAO FOI O PRIMEIRO NEM SERA O ULTIMO");
    print_text(25 - (s.length() / 2), 18, s);
    s =  F("SUAS OPCOES SAO DESISTIR OU REINGRESSAR");
    print_text(25 - (s.length() / 2), 20, s);
    s =  F("REINGRESSAR - BOTAO AZUL  ");
    print_text(25 - (s.length() / 2), 23, s);
    s =  F("TER UMA VIDA - BOTAO VERMELHO");
    print_text(25 - (s.length() / 2), 25, s);

    do {
      dir = player.getKey();
      if (player.P1_B1) gameState = 2;
      else if (player.P1_B2) gameState = 1;
      delay(100);
    } while (gameState == 3);

    while (player.P1_B1 || player.P1_B2)dir = player.getKey();


  }

  if (gameState == 4) {

    clear_screen();
     stopTone(20);
    clear_books();
    s =  F("PARABENS");
    print_text(25 - (s.length() / 2), 8, s);
    s =  F("ACREDITE OU NAO");
    print_text(25 - (s.length() / 2), 10, s);
    s =  F("VOCE SE FORMOU NA UFABC");
    print_text(25 - (s.length() / 2), 11, s);
    s = F("SEU CR NO FINAL FOI ");
    s += String(P1.CR, DEC);
    print_text(25 - (s.length() / 2), 12, s);

    if (P1.CR == 4) s = F("VOCE NAO PODE SER DESSE PLANETA");
    else if (P1.CR == 3) s = F("SUA GRADUACAO FOI HEROICA");
    else if (P1.CR == 2) s = F("NADA MAL DEPOIS DE REPROVAR TANTO");
    else if (P1.CR == 1) s = F("ESSA FOI POR POUCO HEIN");
    print_text(25 - (s.length() / 2), 14, s);

    if (P1.CR == 3) {
      s = F("ME PASSA UM POUCO DESSE CR AI PFVR");
      print_text(25 - (s.length() / 2), 16, s);
    }



    s =  F("COMEMORE MUITO COM SEUS AMIGOS E FAMILIA");
    print_text(25 - (s.length() / 2), 19, s);
    s =  F("SO NAO VA BEBER MUITO ");
    print_text(25 - (s.length() / 2), 20, s);
    s =  F("MENTIRA PODE BEBER SIM");
    print_text(25 - (s.length() / 2), 22, s);

    s =  F("PRESSIONE O BOTAO AZUL PARA SOFRER NOVAMENTE");
    print_text(25 - (s.length() / 2), 25, s);
    s =  F("PRESSIONE O BOTAO VERMELHO PARA SER FELIZ");
    print_text(25 - (s.length() / 2), 26, s);

    //  print_text(10,10,"VOCE SE FORMOU");
    do {
      dir = player.getKey();
      if (player.P1_B1) gameState = 2;
      else if (player.P1_B2) gameState = 1;
      delay(100);
    } while (gameState == 4);
    while (player.P1_B1 || player.P1_B2)dir = player.getKey();
  }

  if (gameState == 5) {
    //String s;
    clear_screen();

    s =  F("INSTRUCOES");
    print_text(25 - (s.length() / 2), 10, s);
    s =  F("PARA SE FORMAR NA UFABC VOCE");
    print_text(25 - (s.length() / 2), 12, s);
    s =  F("PRECISA DE JOGO DE CINTURA");
    print_text(25 - (s.length() / 2), 13, s);
    s =  F("DESVIE DAS MATERIAS -LIVROS- ");
    print_text(25 - (s.length() / 2), 15, s);
    s =  F("O MAXIMO QUE CONSEGUIR");
    print_text(25 - (s.length() / 2), 16, s);
    s =  F("CADA VEZ QUE UM LIVRO TE ATINGIR ");
    print_text(25 - (s.length() / 2), 17, s);
    s =  F("SIGNIFICA QUE VOCE FOI REPROVADO NA MATERIA");
    print_text(25 - (s.length() / 2), 18, s);
    s =  F("PORTANTO SEU CR IRA CAIR ");
    print_text(25 - (s.length() / 2), 19, s);
    s =  F("QUANDO SEU CR CHEGAR EM ZERO");
    print_text(25 - (s.length() / 2), 21, s);
    s =  F("VOCE SERA JUBILADO");
    print_text(25 - (s.length() / 2), 22, s);
    s =  F("TENTE SE FORMAR ANTES DISSO ");
    print_text(25 - (s.length() / 2), 24, s);
    s =  F("USE OS DIRECIONAIS PARA DESVIAR DOS LIVROS");
    print_text(25 - (s.length() / 2), 25, s);
    s =  F("SEU CP IRA SUBIR ENQUANTO VOCE AGUENTAR");
    print_text(25 - (s.length() / 2), 26, s);
    s =  F("ALEM DISSO O BOTAO VERDE ATIVA O TURBO");
    print_text(25 - (s.length() / 2), 28, s);
    s =  F("QUE TE AJUDA NA HORA DO APERTO");
    print_text(25 - (s.length() / 2), 29, s);
    s =  F("PRESSIONE O BOTAO AZUL PARA COMECAR");
    print_text(25 - (s.length() / 2), 31, s);
    // print_text(10,10,"VOCE SE FORMOU");
    do {
      dir = player.getKey();
      if (player.P1_B1) gameState = 2;
      // else if (player.P1_B2) gameState = 1;
      delay(100);
    } while (gameState == 5);
    while (player.P1_B1)dir = player.getKey();
  }
}

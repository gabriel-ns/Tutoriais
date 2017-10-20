#define C3   131
#define CH3  139
#define D3   147
#define DH3  155
#define E3   165
#define F3   174
#define FH3  185
#define G3   196
#define GH3  208
#define A3   220
#define AH3  233
#define B3   246
#define C4   262
#define CH4  277
#define D4   294
#define DH4  311
#define E4   330
#define F4   350
#define FH4  370
#define G4   392
#define GH4  415
#define A4   440
#define AH4  466
#define B4   494
#define C5   523
#define CH5  554
#define D5   587
#define DH5  622
#define E5   659
#define F5   698
#define FH5  740
#define G5   784
#define GH5  831
#define A5   880
#define AH5  932
#define B5   988

const PROGMEM uint16_t scaleFrequency[] = {
                                    C3 ,  
                                    CH3,  
                                    D3 ,  
                                    DH3,  
                                    E3 ,  
                                    F3 ,  
                                    FH3,  
                                    G3 ,  
                                    GH3,  
                                    A3 ,  
                                    AH3,  
                                    B3 ,  
                                    C4 ,  
                                    CH4,  
                                    D4 ,  
                                    DH4,  
                                    E4 ,  
                                    F4 ,  
                                    FH4,  
                                    G4 ,  
                                    GH4,  
                                    A4 ,  
                                    AH4,  
                                    B4 ,  
                                    C5 ,  
                                    CH5,  
                                    D5 ,  
                                    DH5,  
                                    E5 ,  
                                    F5 ,  
                                    FH5,  
                                    G5 ,  
                                    GH5,  
                                    A5 ,  
                                    AH5,  
                                    B5  

};
/*
*10
seminima.         = 96
seminima          = 64
colcheia.         = 48
colcheia          = 32
semicolcheia.     = 24
semicolcheia      = 16


seminima.         = 24
seminima          = 16
colcheia.         = 12
colcheia          = 8
semicolcheia.     = 6
semicolcheia      = 4
*/

const PROGMEM uint8_t  voice1[]  = {0 ,0 ,0 ,0 ,3,5,0 ,0 ,0 ,0 ,10,11,0 ,0 ,0 ,0 ,3,5,0 ,0 ,0 ,0 ,10,11,5 ,5 ,5 ,5 ,8,10,5 ,5 ,5 ,5 ,15,16,5 ,5 ,5 ,5 ,8,10,5 ,5 ,5 ,5 ,15,16};
const PROGMEM uint8_t  tempo1[]  = {12,12,12,12,8,8,12,12,12,12,8 ,8 ,12,12,12,12,8,8,12,12,12,12,8 ,8 ,12,12,12,12,8,8 ,12,12,12,12,8 ,8 ,12,12,12,12,8,8 ,12,12,12,12,8 ,8 };

const PROGMEM uint8_t voice2[] = {17,15,24,17,15,23,17,15,22,22,21,32,29,24,32,29,23,32,29,22,20,22};
const PROGMEM uint8_t tempo2[] = { 4, 4,56, 4, 4,56, 4, 4,56, 4,60, 4, 4,56, 4, 4,56, 4, 4,56, 4,60};







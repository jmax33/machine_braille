
#define COL_REG_INPUT 13
#define COL_REG_CLK  12
#define COL_REG_OUTPUT 11

#define COL_REG_MAX 12


#define LIN_REG_INPUT 10
#define LIN_REG_CLK 8
#define LIN_REG_OUTPUT  9

#define LIN_REG_MAX 6


#define SECONDE 1000


#define PITCH_TIME 1

#define DELAY_BETWEEN_WORD 5*SECONDE //entre deux séries de 12 lettres
#define DELAY_BETWEEN_PHRASE SECONDE//5*60*SECONDE
#define DELAY_BETWEEN_LETTER 5*SECONDE

#define PRINT_STRING "hello  world"
#include "typeReg.h"

/*
table de correspondances :
 
 a = 100000
 b = 110000
 c = 100100
 d = 100110
 e = 100010
 f = 110100
 g = 110110
 h = 110010
 i = 010100
 j = 010110
 
 k = 101000
 l = 111000
 m = 101100
 n = 101110
 o = 101010
 p = 111100
 q = 111110
 r = 111010
 s = 011100
 t = 011110
 
 u = 101001
 v = 111001
 x = 101101
 y = 101111
 z = 101011
 
 w = 010111
 
 */
#define NB_LETTER_BRAILLE 12
#define NB_CARAC ('z'-'a') + 1 //add espace
#define LEN_BRAILLE 6




char SpaceBraille[LEN_BRAILLE +1] = "000000";
char tabAlphaBraille[NB_CARAC][LEN_BRAILLE+1] = 
{ 
  "100000", /*a*/ 
  "110000", //b
  "100100", //c
  "100110", //d
  "100010", //e
  "110100", //f
  "110110", //g
  "110010", //h
  "010100", //i
  "010110", //j
  "101000", //k
  "111000", //l
  "101100", //m
  "101110", //n
  "101010", //o
  "111100", //p
  "111110", //q
  "111010", //r
  "011100", //s
  "011110", //t
  "101001", //u
  "111001", //v
  "010111", //w
  "101101", //x
  "101111", //y
  "101011" //z
};


void SetState(REGISTER_t *pReg , REGISTER_PIN_e pin , int State)
{
  //Serial.println("SetState");
  digitalWrite(pReg->pin[pin],State);
 // delayMicroseconds(pReg->PithTime);
 delay(pReg->PithTime);
}




void SendPitch (REGISTER_t *pReg,REGISTER_PIN_e pin)
{
  //Serial.println("SendPitch");
  SetState(pReg,pin,HIGH);
  SetState(pReg,pin,LOW);
}


void RstReg(REGISTER_t *pReg,int state)
{
  int i;
  SetState(pReg,REG_INPUT,state);
  for (i=0; i<=pReg->RegMax ; i++)
  {
    SendPitch(pReg,REG_CLK);
  }
  SendPitch(pReg,REG_OUTPUT); 
  Serial.println("RstReg"); 
}

void InitReg(REGISTER_t *pReg)
{

  pinMode(pReg->pin[REG_INPUT], OUTPUT);
  pinMode(pReg->pin[REG_CLK], OUTPUT);   
  pinMode(pReg->pin[REG_OUTPUT], OUTPUT); 

  digitalWrite(pReg->pin[REG_INPUT],LOW);
  digitalWrite(pReg->pin[REG_OUTPUT],LOW);
  digitalWrite(pReg->pin[REG_CLK],LOW);

  delay(100);
  RstReg(pReg,LOW);
}


REGISTER_t RegCol;
REGISTER_t RegLin;


void setup()
{
  
  Serial.begin(9600);

  RegCol.pin[REG_CLK] 		= COL_REG_CLK;
  RegCol.pin[REG_INPUT] 	= COL_REG_INPUT;
  RegCol.pin[REG_OUTPUT] 	= COL_REG_OUTPUT;
  RegCol.PithTime		= PITCH_TIME;
  RegCol.RegMax			= COL_REG_MAX;


  RegLin.pin[REG_CLK] 		= LIN_REG_CLK;
  RegLin.pin[REG_INPUT] 	= LIN_REG_INPUT;
  RegLin.pin[REG_OUTPUT] 	= LIN_REG_OUTPUT;
  RegLin.PithTime		= PITCH_TIME;
  RegLin.RegMax			= LIN_REG_MAX;

  InitReg(&RegLin);
  InitReg(&RegCol);

}


const char *MyString = PRINT_STRING;
int state[2] = {LOW,HIGH};
#define GET_LETTER_INDEX(_l) ((_l)-'a')

void IncReg(REGISTER_t *pReg)
{
  SendPitch(pReg,REG_CLK);
  SendPitch(pReg,REG_OUTPUT );
}

//#define TEST

void loop()
{
#ifdef TEST
  int i,j;

  j=1;
  while (1)
  {
    SetState(&RegCol,REG_INPUT,state[j] );  
    Serial.println("debut");  
    Serial.println(state[j],DEC);
    for (i=0;i<= RegCol.RegMax ; i++)
    {
      SendPitch(&RegCol,REG_CLK); 
      SendPitch(&RegCol,REG_OUTPUT); 
      Serial.println(i,DEC); 
      delay(500);
    }
   
    SetState(&RegLin,REG_INPUT,state[j] );  
    for (i=0;i<= RegLin.RegMax ; i++)
    {
      SendPitch(&RegLin,REG_CLK); 
      SendPitch(&RegLin,REG_OUTPUT); 
      Serial.println(i,DEC); 
      delay(500);
    }
    
    Serial.println("Fin");  
    delay(5000);
    
    j++;
    j %= 2;
  }
#else
  unsigned long LenString;


  LenString = strlen(MyString);

  //prepare substring
  RstReg(&RegCol,LOW);
  while (LenString > 0)
  {
    unsigned long SubStringSize;
    unsigned long iLetter,i;
    char CurrentString[NB_LETTER_BRAILLE] = {0};
    char strDebug[200];

    SubStringSize = LenString > NB_LETTER_BRAILLE?NB_LETTER_BRAILLE:LenString;
    LenString -= SubStringSize;

    memcpy(CurrentString,MyString,SubStringSize);
    CurrentString[SubStringSize] = 0;

    Serial.println("debut");  
    Serial.println(CurrentString);  

    sprintf(strDebug,"Len %lu %lu",LenString,SubStringSize);
    Serial.println(strDebug);   

    SetState(&RegCol,REG_INPUT,HIGH);
    IncReg(&RegCol);
    SetState(&RegCol,REG_INPUT,LOW);
    
    IncReg(&RegCol);
    
    Serial.println("Col1");
    delay(1000);
    for (iLetter = 0 ; iLetter < SubStringSize ; iLetter++)
    {
      char *MyLetter = CurrentString[iLetter] == ' ' ? SpaceBraille: tabAlphaBraille[GET_LETTER_INDEX(CurrentString[iLetter])];
      sprintf(strDebug,"L:%c %s",CurrentString[iLetter],MyLetter);
      Serial.println(strDebug); 
      for (i=1 ; i <= LEN_BRAILLE ; i++)
      {
        int iL = LEN_BRAILLE-i;
        SetState(&RegLin,REG_INPUT,state[MyLetter[iL]-'0'] );
        SendPitch(&RegLin,REG_CLK);
        sprintf(strDebug,"L:%c",MyLetter[iL]);
        Serial.println(strDebug); 
      }
      SendPitch(&RegLin,REG_OUTPUT);
      delay(DELAY_BETWEEN_LETTER);
      RstReg(&RegLin,LOW);
      IncReg(&RegCol);
    }
    RstReg(&RegCol,LOW);   
    delay(DELAY_BETWEEN_WORD);
  }
  delay(DELAY_BETWEEN_PHRASE);
  //////////// END ////////////////////////////
#endif
}



#define CPT_RST 8
#define CPT_CLK 9

#define CPT_DELAY 500
#define CPT_MAX 7

#define REG_INPUT 11
#define REG_CLK  13
#define REG_OUTPUT 10
#define REG_MAX 24

#define SECONDE 1000

#define PITCH_TIME 1000 // horloge µs 1 seconde = 1000 millisecondes   ; 1 milliseconde = 1000 µ secondes


#define DELAY_BETWEEN_WORD 5*SECONDE //entre deux série de 12 lettres
#define DELAY_BETWEEN_PHRASE 5 * 60 * SECONDE
#define PRINT_STRING "hello  world"

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
#define LEN_BRAILLE 7
char tabAlphaBraille[NB_CARAC][LEN_BRAILLE] = 
{
  "100000", //a 
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

char SpaceBraille[LEN_BRAILLE] = "000000";

void SetState(int OutPut,int State)
{
  digitalWrite(OutPut,State);
  if (OutPut == REG_CLK)
    delayMicroseconds(PITCH_TIME);
  else
    delay(1000);
}

void SendPitch (int OutPut)
{
  SetState(OutPut,HIGH);
  SetState(OutPut,LOW);
}


void InitCpt()
{
  pinMode(CPT_RST, OUTPUT);   
  pinMode(CPT_CLK, OUTPUT);   
  digitalWrite(CPT_CLK,LOW);
  SetState(CPT_RST,LOW);
  SendPitch(CPT_RST);
}

void RstReg(int state)
{
  int i;
  SetState(REG_INPUT,state);
  for (i=0; i<REG_MAX ; i++)
  {
    SendPitch(REG_CLK);
  }
  SendPitch(REG_OUTPUT); 
}

void InitReg()
{
    
  pinMode(REG_INPUT, OUTPUT);
  pinMode(REG_CLK, OUTPUT);   
  pinMode(REG_OUTPUT, OUTPUT); 
  
  digitalWrite(REG_INPUT,LOW);
  digitalWrite(REG_CLK,LOW);
 
  RstReg(LOW);

  SetState(REG_INPUT,HIGH);
  SendPitch(REG_CLK);
  SendPitch(REG_OUTPUT);
}

void setup()
{

  int i;
  Serial.begin(9600);

  InitCpt();
  InitReg();
  
  for (i=0;i< LEN_BRAILLE -1 ; i++)
  {
     SendPitch(CPT_CLK);
     delay(5000);
  }
}


char *MyString = PRINT_STRING;
int state[2] = {LOW,HIGH};

#define GET_LETTER_INDEX(_l) ((_l)-'a')
//#define TEST

void loop()
{

#ifndef TEST
  unsigned int cptVal;
  unsigned int LenString;
  
  LenString = strlen(MyString);
  
  //prepare substring

  SendPitch(CPT_RST);
  while (LenString > 0)
  {
    unsigned int SubStringSize,CurrentSize,ModuloReg;
    
    char CurrentString[NB_LETTER_BRAILLE] = {0};
    char strDebug[200];
    
    SubStringSize = LenString > NB_LETTER_BRAILLE?NB_LETTER_BRAILLE:LenString;
    LenString -= SubStringSize;
    
    memcpy(CurrentString,MyString,SubStringSize);
    CurrentString[SubStringSize] = 0;
    
    Serial.println("debut");  
    Serial.println(CurrentString);  
    
    sprintf(strDebug,"Len %d %d",LenString,SubStringSize);
    Serial.println(strDebug);   
    
    
    
    for (cptVal = 0 ; cptVal< LEN_BRAILLE -1 ; cptVal++)
    {//move ligne
      CurrentSize = SubStringSize;
      ModuloReg = NB_LETTER_BRAILLE - CurrentSize;
  
      SetState(REG_INPUT,LOW);
      while (ModuloReg > 0 )
      {
        SendPitch(REG_CLK);
  //      Serial.println("MODULO");
        ModuloReg--;
      }  
      
      while( CurrentSize > 0)
      { //preprare col
//	Serial.println("preprare col");
        char CurrentLetter = tolower(CurrentString[CurrentSize -1 ]);
	char *pBrailleCode;

 //       sprintf(strDebug,"L:%c Size %d cpt:%d",CurrentLetter,CurrentSize,cptVal);
//        Serial.println(strDebug);

	if (CurrentLetter >= 'a' && CurrentLetter <= 'z')
	{
	  pBrailleCode = tabAlphaBraille[GET_LETTER_INDEX(CurrentLetter)];
//	  Serial.println(pBrailleCode);
        }
	else
	{//special carac
	  pBrailleCode = SpaceBraille;
	}

	CurrentSize --;
//	Serial.println(pBrailleCode[cptVal]);
        SetState(REG_INPUT,state[pBrailleCode[cptVal]-'0']);
	SendPitch(REG_CLK);	
      }
    
      
      SendPitch(REG_OUTPUT);
      SendPitch(CPT_CLK);
      
    //  delay(1000);
    }
    SendPitch(CPT_RST);
    RstReg(LOW);
//    Serial.println("fin");
    delay(DELAY_BETWEEN_WORD);
  }
  delay(DELAY_BETWEEN_PHRASE);
//////////// END ////////////////////////////


#else
#define TIME_TEST 1000

  int i;
  Serial.println("debut");
 
  Serial.println("REG 1");
  SetState(REG_INPUT,HIGH);
  for (i=0;i<12;i++)
  {
    SendPitch(REG_CLK);
    SendPitch(REG_OUTPUT);
    delay(TIME_TEST);        
  }
 
  Serial.println("CPT");
  for (i=0;i<9;i++)
  {
   SendPitch(CPT_CLK);
   delay(TIME_TEST*10);      
  }
  

  
  SetState(REG_INPUT,LOW);
  Serial.println("REg2");
  for (i=0;i<12;i++)
  {
    SendPitch(REG_CLK);
    SendPitch(REG_OUTPUT);
    delay(TIME_TEST);        
  }
   
#endif
}

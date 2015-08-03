typedef enum 
{
  REG_OUTPUT,
  REG_INPUT,
  REG_CLK,
  NB_REG_PIN
}REGISTER_PIN_e;

typedef struct 
{
  int pin[NB_REG_PIN];
  int PithTime;
  int RegMax;
}REGISTER_t; 


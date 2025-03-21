// Calculator modeled on hp-35 using single precision
// copyright 2025 William R Cooke

// Are we building for a PC(1) or an AVR(0)?
#define FOR_PC     0


//#if (FOR_PC)
#include <stdio.h>
//#endif

#include <stdint.h>
#include <ctype.h>
#include <math.h>

#if (FOR_PC)
#include <termios.h>
#include <unistd.h>
#endif




#define NUM_DIGS   6
#define NUM_MCHARS (NUM_DIGS + 1)          // # mantissa chars = num digits + dp
#define NUM_EDIGS  2
#define NUM_ECHARS (NUM_EDIGS)             // digits plus sign
#define NUM_CHARS (NUM_DIGS + NUM_EDIGS + 3)   // include dp and two signs

#define DEG_TO_RAD (0.01745329)            // PI / 180
#define RAD_TO_DEG (59.29578)              // 180 / PI


void clear_display(void);
void init(void);
void un_init(void);
int get_key(void);
int parse_key(void);
float get_num(void);
int show_num(float n);
void update(char* buff);

float X;
float Y;
float Z;
float T;
float MEM;


char sign;
char digs[NUM_MCHARS]; // NUM_CHARS];
char esign;
char edigs[NUM_EDIGS];
//int getting_exp = 0;
//int char_ptr;
//int got_dp;
//int exp_ptr;
//int got_arc;

#if (FOR_PC)
struct termios terminal;
struct termios old_terminal;
#endif

typedef enum key
{
  KEY_ZERO    = 0,
  KEY_ONE     = 1,
  KEY_TWO,
  KEY_THREE,
  KEY_FOUR,
  KEY_FIVE,
  KEY_SIX,
  KEY_SEVEN,
  KEY_EIGHT,
  KEY_NINE,
  KEY_DP,
  KEY_PI,
  KEY_DIV,
  KEY_MUL,
  KEY_PLUS,
  KEY_MINUS,
  KEY_ENTER,
  KEY_CHS,
  KEY_EEX,
  KEY_CLX,
  KEY_RECIP,
  KEY_XCHG,
  KEY_SCRL,
  KEY_STO,
  KEY_RCL,
  KEY_SQRT,
  KEY_ARC,
  KEY_SIN,
  KEY_COS,
  KEY_TAN,
  KEY_XY,
  KEY_LOG,
  KEY_LN,
  KEY_EX,
  KEY_CLR,
  KEY_MAX

} key_t;

int get_key(void)
{
  int key = -1;
  char ch = 0;

  #if(FOR_PC)
  read(0, &ch, 1);
  write(1, &ch, 1);  // TODO this should get removed
  #endif

  switch(ch)
  {
    case '0':
      key = KEY_ZERO;
      break;
    case '1':
      key = KEY_ONE;
      break;
    case '2':
      key = KEY_TWO;
      break;
    case '3':
      key = KEY_THREE;
      break;
    case '4':
      key = KEY_FOUR;
      break;
    case '5':
      key = KEY_FIVE;
      break;
    case '6':
      key = KEY_SIX;
      break;
    case '7':
      key = KEY_SEVEN;
      break;
    case '8':
      key = KEY_EIGHT;
      break;
    case '9':
      key = KEY_NINE;
      break;
    case '.':
      key = KEY_DP;
      break;
    case ' ':
      key = KEY_PI;
      break;
    case '/':
      key = KEY_DIV;
      break;
    case '*':
      key = KEY_MUL;
      break;
    case '+':
      key = KEY_PLUS;
      break;
    case '-':
      key = KEY_MINUS;
      break;
    case '\n':
      key = KEY_ENTER;
      break;
    case 'h':
    case 'H':
      key = KEY_CHS;
      break;
    case 'x':
    case 'X':
      key = KEY_EEX;
      break;
    case 8:  // Backspace
      key = KEY_CLX;
      break;
    case 'o':
    case 'O':  // "over"
      key = KEY_RECIP;
      break;
    case 'w':
    case 'W':   // sWap  X <-> Y
      key = KEY_XCHG;
      break;
    case 'r':   
    case 'R':  // R ->
      key = KEY_SCRL;
      break;
    case 'z':  // TODO fix this
      key = KEY_STO;
      break;
    case 'm':
    case 'M':  //Memory 
      key = KEY_RCL;
      break;
    case 'q':
    case 'Q':
      key = KEY_SQRT;
      break;
    case 'a':
    case 'A':
      key = KEY_ARC;
      break;
    case 's':
    case 'S':
      key = KEY_SIN;
      break;
    case 'c':
    case 'C':
      key = KEY_COS;
      break;
    case 't':
    case 'T':
      key = KEY_TAN;
      break;
    case 'p':
    case 'P':
      key = KEY_XY;
      break;
    case 'g':
    case 'G':
      key = KEY_LOG;
      break;
    case 'n':
    case 'N':
      key = KEY_LN;
      break;
    case 'e':
    case 'E':
      key = KEY_EX;
      break;
    case 'k':
    case 'K':  // Klear
      key = KEY_CLR;
      break;
    default:
      #if (FOR_PC)
      printf("Invalid keystroke: %02X\n", key);
      #endif

      break;
  }
  return key;
}

int arc = 0;
int dp = NUM_DIGS - 1;
int got_dp = 0;   // TODO replace with dp
int current_dig = NUM_DIGS;  // include dp
int edig_seen = 0;    // number of exp digits so far
int getting_exp = 0;
//int clear_flags = 0;

enum flags
{
  FL_ARC    = (1 << 0),
  FL_DP     = (1 << 1),
  FL_GDP    = (1 << 2),
  FL_CDIG   = (1 << 3),
  FL_EDIG   = (1 << 4),
  FL_GEXP   = (1 << 5),

  FL_ALL    = (FL_ARC | FL_DP | FL_GDP | FL_CDIG | FL_EDIG | FL_GEXP )
};

// //////////////////////////////////////////////////////////////////////////
/// @fn reset_flags
/// @brief reset keystroke flags after key press
/////////////////////////////////////////////////////////////////////////////
void reset_flags(enum flags f)
{
  arc = 0;
  dp = NUM_DIGS - 1;
  got_dp = 0;   // TODO replace with dp
  current_dig = NUM_DIGS;  // include dp
  edig_seen = 0;    // number of exp digits so far
  getting_exp = 0;
  //c//lear_flags = 0;

}
/// /////////////////////////////////////////////////////////////////////////
/// @fn parse_key
/// @breif Main program loop.  Ket a key, parse it, repeat
/// @return status
/////////////////////////////////////////////////////////////////////////////
int parse_key(void)
{
  int rtn = 0;
  float temp;
  reset_flags(FL_ALL);
  char c;

  while(1)
  {
    int key = get_key();
    switch(key)
    {
      case KEY_ZERO:
      case KEY_ONE:
      case KEY_TWO:
      case KEY_THREE:
      case KEY_FOUR:
      case KEY_FIVE:
      case KEY_SIX:
      case KEY_SEVEN:
      case KEY_EIGHT:
      case KEY_NINE:
        c = (char) (key + '0');
        if(getting_exp)
        {
          // if first exp digit, fill with 0's
          if(edig_seen == 0)
          {
            for(int i = 0; i < NUM_EDIGS; i++)
            {
              edigs[i] = '0';
            }
          }
          // now add the new digit
          // shift
          for(int i = 0; i < NUM_EDIGS - 1; i++)
          {
            edigs[i + 1] = edigs[i];
          }
          edigs[0] = c;
          edig_seen = 1;
        }
        else
        {
          // dp has been hit
          if(dp >= current_dig && current_dig >= 0)
          {
            digs[current_dig--] = c;
          }
          // dp has not been hit
          else if(current_dig > 0)
          {
            digs[current_dig--] = c;
            dp--;
          }
        }

        break;
      case KEY_DP:
        if(dp == current_dig && getting_exp == 0) // got_dp == 0 && getting_exp == 0)
        {
          //char_ptr--;  // dp already displayed
        }
        got_dp = 1;
        break;
      case KEY_PI:
        X = M_PI;
        break;
      case KEY_DIV:
        X = Y / X;
        Y = Z;
        Z = T;
        break;
      case KEY_MUL:
        X = Y * X;
        Y = Z;
        Z = T;
        break;
      case KEY_PLUS:
        X = Y + X;
        Y = Z;
        Z = T;
      case KEY_MINUS:
        X = Y - X;
        Y = Z;
        Z = T;
        break;
      case KEY_ENTER:
        // TODO convert display to float into x;
        T = Z;
        Z = Y;
        Y = X;

        break;
      case KEY_CHS:
      case KEY_EEX:
        getting_exp = 1;
        break;
      case KEY_CLX:
        X = 0.0;
        clear_display();
        break;
      case KEY_RECIP:
        X = 1.0 / X;
        break;
      case KEY_XCHG:
      // TODO
      // convert display -> x
        temp = Y;
        Y = X;
        X = temp;
        break;

      case KEY_SCRL:
        temp = X;
        X = Y;
        Y = Z;
        Z = T;
        T = temp;
        break;

      case KEY_STO:
        MEM = X;
        break;
      case KEY_RCL:
        X = MEM;
        break;
      case KEY_SQRT:
        X = sqrtf(X);
        break;

      case KEY_ARC:
        arc = 1;
        break;
      case KEY_SIN:
        if(arc == 0)
        {
          X = sin(X * DEG_TO_RAD);
        }
        else
        {
          X = RAD_TO_DEG * asin(X);
          arc = 0;
        }
        break;
      case KEY_COS:
        X = cos(X);
      case KEY_TAN:
      case KEY_XY:
      case KEY_LOG:
      case KEY_LN:
      case KEY_EX:
      case KEY_CLR:
      
      default:
        break;


    }
  }


  return rtn;
}

/////////////////////////////////////////////////////////////////////////////
/// @fn clear_display
/// @brief Clears calculator in/out character buffer (numeric)
/////////////////////////////////////////////////////////////////////////////
void clear_display(void)
{
  sign = ' ';
  esign = ' ';
  for(int i = 0; i < NUM_MCHARS; i++)
  {
    digs[i] = ' ';

  }
  for(int i = 0; i < NUM_EDIGS; i++)
  {
    edigs[i] = ' ';
  }
  //getting_exp = 0;
  //char_ptr = NUM_CHARS;
  //got_dp = 0;
  //exp_ptr = NUM_EDIGS;
}

/////////////////////////////////////////////////////////////////////////////
/// @fn init
/// @brief Let's get ready to rumble!
/////////////////////////////////////////////////////////////////////////////
void init(void)
{
  clear_display();

  #if (FOR_PC)
  tcgetattr(0, &terminal);
  old_terminal = terminal;
  cfmakeraw(&terminal);
  tcsetattr(0, 0, &terminal);
  #endif

  X = 0.0;
  Y = 0.0;
  Z = 0.0;
  T = 0.0;
  MEM = 0.0;
  //got_arc = 0;

}

/////////////////////////////////////////////////////////////////////////////
/// @fn get_num
/// @brief Converts the current display contents to a float
/// @return The converted number
/////////////////////////////////////////////////////////////////////////////
float get_num(void)
{
  float rtn = 0.0;
  int seen_dp = 0;
  int dp_pos = 0;

  for(int i = NUM_MCHARS - 1; i >= 0; i--)
  {
    if(isdigit(digs[i]))
    {
      rtn = rtn * 10.0 + (digs[i] - '0');
      if(seen_dp != 0)
      {
        dp_pos--;
      }
    }
    else if(digs[i] == '.') // decimal point
    {
      seen_dp = 1;
    }
    // everything else should be empty spaces
  }
  if(sign == '-')
  {
    rtn = -rtn;
  }

  int exp = 0;
  for(int i = NUM_EDIGS - 1; i >= 0; i--)
  {
    if(isdigit(edigs[i]))
    {
      exp *= 10;
      exp += (edigs[i] - '0');
    }

  }
  if(esign == '-')
  {
    exp = -exp;
  }

  exp += dp_pos;
  if(exp > 0)
  {
    for(int i = 0; i < exp; i++)
    {
      rtn *= 10.0;
    }
  }
  else if(exp < 0)
  {
    exp = -exp;
    for(int i = 0; i < exp; i++)
    {
      rtn /= 10.0;
    }
  }
  // else 0 so do nothing
  #if (FOR_PC)
  printf("final converted number: %f\n", rtn);
  #endif

  return rtn;
}

int show_num(float n)
{ 
  #if (FOR_PC)
  printf("n = %f\n\r",n);
  #endif

  int rtn = 0;

  int dp = 0;  // decimal point digit
  int ex = 0;  // exponent

  uint32_t ni;   // int version of n


  clear_display();
  if( n < 0.0)
  { 
    sign = '-';
    n = -n;
  }

  if(n > 999999.0f)  //scientfic notation >= 1,000,000
  {
    while(n > 999999.0f & ex < 34)  // max 39 - 5 on display (1.23456 39)
    {
      n /= 10.0;
      ex++;
    }
    dp = 5;  // always five for sci notation
  } 

  else if (n < 0.01f)  // scientific notation < 0.01
  {
    while(n < 100000.0 & ex > -34) // ???
    {
      n *= 10.0;
      ex--;
    }
    dp = 5;
    ex += dp;

  }

  else if (n >= 1.0f) // 1.0 to 999,999.0
  {
    ///uint32_t ni = (uint32_t) n;
    while(n < 100000.0)
    {
      n *= 10.0;
      // exp will be 0, but dp may change ex--;
      dp++;
    }
    #if (FOR_PC)
    printf ("1 to 999999 %f ex %d\n\r", n, ex);
    #endif
  }
  else // .01 to < 1.0
  {
    n *= 1000000;  // 10,000 to 999,999
    //ex = -6;
    dp = 6;  // always six for these numbers

  }

  // now convert for display
  ni = (uint32_t) n;

  #if (FOR_PC)
  printf("ni: %d ex: %d dp: %d\n\r", ni, ex, dp);
  #endif

  for (int i = 0; i < NUM_DIGS + 1; i++)
  {
    if (i == dp)
    {
       digs[i] = '.';
    }
    else
    {
      digs[i] = ni % 10 + '0';
      ni /= 10;
    }
  }

  if(ex < 0)
  {
    esign = '-';
    ex = -ex;
  }
  if(ex > 0)
  {
    for(int i = 0; i < NUM_EDIGS; i++)
    {
      edigs[i] = ex % 10 + '0';
      ex /= 10;
    }
  }

  // show it
  update(NULL);
  // convert display back to number TODO: temporary?
  get_num();
  return rtn;
}

/////////////////////////////////////////////////////////////////////////////
// @fn un_init
// @brief PC only.  Recover "normal" settings before exiting
/////////////////////////////////////////////////////////////////////////////
void un_init(void)
{
  #if (FOR_PC)
  tcsetattr(0,0,&old_terminal);
  #endif
}

/////////////////////////////////////////////////////////////////////////////
/// @fn update
/// @brief Write characters to display (LCD or screen)  X reg or char string.
/// @param  buff  Pointer to string.  If NULL, show X register.
/////////////////////////////////////////////////////////////////////////////
void update(char * buff)
{
  if(buff == NULL)
  {
    #if (FOR_PC)
    putchar('*');
    putchar(sign);
    for(int i = NUM_MCHARS-1; i >= 0; i--)
    {
      putchar(digs[i]);
    }
    putchar(esign);
    putchar(edigs[1]);
    putchar(edigs[0]);
    putchar('*');
    putchar('\n');
    putchar('\r');
    #endif
  }
  else
  {
    // TODO show string
  }


}
/////////////////////////////////////////////////////////////////////////////
/// @fn main
/// @param argc 
/// @param argv 
/// @return 
/////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
  int rtn = 0;

  init();

  show_num(1.0);
  show_num(-1.0);
  show_num(987654);
  show_num(-987654);
  show_num(123.45678);
  show_num(-123.45);
  show_num(0.1);
  show_num(0.01);
  show_num(-0.1);
  show_num(-0.05);
  show_num(1234567890.25);
  show_num(-78901234);
  show_num(.000012345678901);
  show_num(1234560000000);
  show_num(12345600000000);
  show_num(123456000000000);
  show_num(-0.0000123456);
  show_num(1);
  show_num(56);
  show_num(123);
  show_num(4567);
  show_num(0.000123456);


  #if (FOR_PC)
  int key;
  while((key = get_key() ) != KEY_CLR)
  {
    printf("key: %02X\n", key);
  }
  #endif

  un_init();


  return rtn;
}
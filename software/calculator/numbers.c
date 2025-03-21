/////////////////////////////////////////////////////////////////////////////
/// @file numbers.c
/// @brief handles internal bcd numbers
/// @copyright 2025 William R Cooke
/////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdint.h>

#define NUMDIGITS  8
#define NUMEXPDIGITS 2
#define NUMDISPDIGITS 6

typedef struct number
{
  uint8_t sign;      // sign of number
  uint8_t mantissa[NUMDIGITS];  // Digits of mantissa, full range
  uint8_t esign;                // Sign of exponent
  uint8_t exp[NUMEXPDIGITS];    // Exponent digits.
} number_t;

float number_to_float(number_t* num)
{
  float rtn = 0.0;


  return rtn;
}

int float_to_number(float f, number_t* num)
{
  int rtn = 0; // default to fail
  // init num
  num->sign = ' ';
  for(int i = 0; i < 8; i++)
  {
    num->mantissa[i] = ' ';
  }
  num->esign = ' ';
  num->exp[0] = ' ';
  num->exp[1] = ' ';

  printf("float: %f\n", f);
  int e = 0;
  if(f > 9999999.0)
  {
    // scientific notation
    while(f >= 10.0)
    {
      e++;
      f /= 10.0;
      printf("f: %f e: %d\n",f,e);
    }
  }
  else if(f<0.001f)
  {
    // scientific notation
    int mult = 1;
    do
    {
      /* code */
      mult *= 10;
      e--;
      printf("f: %f mult: %d f*mult: %f\n", f, mult, f*(float) mult);
    } while (f * (float) mult < 1.0 && mult < 1000000000);
    f *= (float) mult;
    e++;
    printf("new f: %f e: %d\n", f,e);
      
    
    // while (f < 1.0f)
    // {
    //   e--;
    //   f *= 10.0;
    //   printf("f: %d e: %d\n", f, e);
    // }
  }
  printf("adjusted float: %f exp: %2d\n", f, e);

  


  return rtn;
}

void show_number(number_t* num)
{
  putchar(num->sign);
  for(int d = 7; d >1; d--)
  {
    putchar(num->mantissa[d]);
  }
  putchar(num->esign);
  putchar(num->exp[1]);
  putchar(num->exp[0]);
  putchar('\n');

}

int main(int argc, char* argv[])
{
  int rtn = 0;
  number_t n;
  float_to_number(12345678.0, &n);
  show_number(&n);
  float_to_number(10000000.0, &n);
  show_number(&n);
  float_to_number(999999.0, &n);
  show_number(&n);
  float_to_number(.000123, &n);
  show_number(&n);
  float_to_number(.00000000001f, &n);
  show_number(&n);



  return rtn;
}
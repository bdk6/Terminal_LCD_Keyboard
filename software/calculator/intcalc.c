/// @file intcalc.c
/// @brief Floating point calculations built on top of int64
/// @copy William R Cooke 2025 (20250307)

#include <stdio.h>
#include <stdint.h>

#define E 2.718281828459045
#define PI 3.14159265


typedef struct flt
{
  int64_t mant;
  int8_t  exp;
} flt_t;


double ln_consts[] = 
{
  1.0,
  2*E*E,
  3*E*E*E,
  4*E*E*E*E
};

flt_t flt_build(int64_t m, int8_t e)
{
  flt_t rtn;
  rtn.mant = m;
  rtn.exp = e;
  return rtn;
}

int flt_normalize(flt_t* f)
{
  int rtn = 1;

  return rtn;
}


void flt_print(flt_t f)
{
  printf("%lld E%d\n", f.mant,f.exp);
  // scientific notation
  char sign = ' ';
  int64_t m = f.mant;
  int8_t e = f.exp;
  if(m < 0)
  {
    sign = '-';
    m = -m;
  }
  //printf("exp: %d\n", e);
  while(m < 10000000000LL && e > -99)
  {
    m *= 10;
    e--;
  }
  while(m > 9999999999LL & e < 99)
  {
    m /= 10;
    e++;
  }
  //printf("exp after %d\n", e);
  // now we have a ten digit int with an exponent
  // adjusted to x.xxx,xxx,xxx
  e += 9;
  char digits[11];

  if(m <= 9999999999 &&  e >= 0 && e <= 9)
  {
    printf("\n >1: Not scientific notation\n");
    int c = 0;
    int dp = 9 - e;
    uint64_t m1 = m;
    for(int i = 0; i < 11; i++)
    {
      if(i == dp)   // dp goes here
      {
        digits[i] = '.';
      }
      else
      {
        digits[i] = m1 % 10 + '0';
        m1 /= 10;
      }
      c++;
    }
    for(int i = 10; i >= 0; i--)
    {
      putchar(digits[i]);
    }
  }
  else if(e >= -2) // <= -2 && e >= -9)
  {
    printf("\nSMALL: Not scientific notation\n");
    int c = 0;
    int dp = 8 - e;
    printf("DP: %d\n", dp);
    uint64_t m1 = m;   // temporary for debug
    printf("m1 = %ld\n", m1);
    // shift and round
    for(int i = e; i < 0; i++)
    {printf("m1 = %ld exp i: %d\n",m1, i);
      if(i == 1)
      {
        m1 += 5;
      }
      m1 /= 10;
    }
    printf("m1 after exp: %ld\n", m1);
    for(int i = 0; i < 11; i++)
    {
      if(i == dp)  // dp goes here
      {
        digits[i] = '.';
      }
      else
      {
        digits[i] = m1 % 10 + '0';
        m1 /= 10;
      }
      c++;
      printf("i: %d digits[i]: %c c:%d\n", i, digits[i], c);
    }
    printf("last m1 = %ld\n", m1);
    for(int i = 10; i >= 0; i--)
    {
      putchar(digits[i]);
    }
  }

  // TODO check overflow of exponent!
  
  for(int d = 0; d < 10; d++)
  {
    digits[d] = m % 10 + '0';
    m /= 10;
  }
  printf("*** THIS IS WRONG *** NEWSCI: %c%c.%c%c%c%c%c%c%c%c%c E%02d  \n", sign, digits[9], digits[8],digits[7], digits[6], digits[5],digits[4],digits[3], digits[2], digits[1], digits[0], e);
  // temp for test
  //printf("SCI: %c%lld E%02d\n", sign, m, e);

}

/////////////////////////////////////////////////////////////////////////////
/// @fn flt_add
/// @brief add two flt together
/// @param a 
/// @param b 
/// @return a+b
/////////////////////////////////////////////////////////////////////////////
flt_t flt_add(flt_t* a, flt_t* b)
{
  // debug
  printf("\n  add \n");
  flt_print(*a);
  printf("\n");
  flt_print(*b);
  printf("\n");
  // end debug

  flt_t rtn;
  // get signs
  int an = 0;   // is a negative?
  int bn = 0;   // is b negative
  if(a->mant < 0)
  {
    an = 1;
    a->mant = -a->mant;
  }
  if(b->mant < 0)
  {
    bn = 1;
    b->mant = -b->mant;
  }
  flt_t *larger, *smaller;
  if(a->exp > b->exp)
  {
    printf("larger is a\n");
    larger = a;
    smaller = b;
  }
  else if(b->exp > a->exp)
  {
    printf("larger is b\n");
    larger = b;
    smaller = a;
  }
  else  // same exponent so go by mantissa
  {
    printf("same exp %d:  ", a->exp);
    if(a->mant > b->mant)
    {
      larger = a;
      smaller = b;
      printf("larger is a\n");
    }
    else
    {
      larger = b;
      smaller = a;
      printf("larger is b\n");
    }
  }
  // make the number with largest exponent as big as we can
  while(larger->mant < 10000000000000000LL && larger->exp > -100)
  {
    larger->mant *= 10;
    larger->exp--;
  }
  // now match the other
  while(smaller->exp < larger->exp)
  {
    smaller->mant /= 10;
    smaller->exp++;
    flt_print(*smaller); printf("\n");
  }
  while(smaller->exp > larger->exp)
  {
    smaller->mant *= 10;
    smaller->exp--;
    flt_print(*smaller); printf("\n");
  }
  // Finally we can add
  printf("L: %ld : %d\n", larger->mant, larger->exp);
  printf("S: %ld : %d\n", smaller->mant, smaller->exp);

  // fix the signs back
  if(an != 0)
  {
    a->mant = -a->mant;
  }
  if(bn != 0)
  {
    b->mant = -b->mant;
  }
  printf("a mant %ld b mant %ld\n", a->mant, b->mant);
  rtn.mant = a->mant + b->mant;
  rtn.exp = a->exp;  // TODO will this work or do we need to check which is "larger?"
  

  return rtn;
}

/////////////////////////////////////////////////////////////////////////////
/// @fn flt_sub
/// @brief subtract one flt from another
/// @param[in] a minuend
/// @param[in] b subtrahend
/// @return a-b
/////////////////////////////////////////////////////////////////////////////

flt_t flt_sub(flt_t* a, flt_t* b)
{
  b->mant = -b->mant;
  return flt_add(a,b);
}


/////////////////////////////////////////////////////////////////////////////
/// @fn flt_mul
/// @brief multiply two flt 
/// @param x 
/// @param y 
/// @return  x*y
/////////////////////////////////////////////////////////////////////////////
flt_t  flt_mul(flt_t* x, flt_t* y)
{
  flt_t res;
  int sx = 1;  // sign of x
  int sy = 1;  // sign of y
  if(x->mant < 0)
  {
    sx = -1;
    x->mant = -x->mant;
  }
  if(y->mant < 0)
  {
    sy = -1;
    y->mant = -y->mant;
  }
  //int sp = sx * sy;  // sign of product

  // shift operands to avoid overflow (max 9 digits each)
  uint64_t mx = x->mant;  // get abs of x
  while(mx > 999999999L)
  {
    mx += 5; // round
    mx /= 10;
    x->exp++;  // TODO check overflow
  }
  uint64_t my = y->mant;  // get abs of y
  while(my > 999999999L)
  {
    my += 5; // round
    my /= 10;
    y->exp++;
  }
  int16_t pe = x->exp + y->exp; // product exponent
  uint64_t prod = mx * my;
  res.mant = prod * sx * sy;
  res.exp = pe;
  printf("mantissa: %llu exponent: %d\n", res.mant, res.exp);

  return res;
}


/////////////////////////////////////////////////////////////////////////////
/// @fn flt_div
/// @brief divide one flt by another
/// @param x dividend
/// @param y divisor
/// @return  x/y
/////////////////////////////////////////////////////////////////////////////
flt_t  flt_div(flt_t* x, flt_t* y)
{
  flt_t res;
  int sx = 1;  // sign of x
  int sy = 1;  // sign of y
  if(x->mant < 0)
  {
    sx = -1;
    x->mant = -x->mant;
  }
  if(y->mant < 0)
  {
    sy = -1;
    y->mant = -y->mant;
  }

  // Maximize dividend to get max resolution
  uint64_t mx = x->mant;  // get abs of x
  while(mx < 999999999999999999LL & x->exp > -99)
  {
    mx *= 10;
    x->exp--;
  }
  
  uint64_t my = y->mant;  // get abs of y

  int16_t pe = x->exp - y->exp; // product exponent
  uint64_t prod = mx / my;
  res.mant = prod * sx * sy;
  res.exp = pe;
  printf("div: mantissa: %llu exponent: %d\n", res.mant, res.exp);



  return res;
}

// a = 1.0;
// ln(x) = ln a + (x-a)/ a - (x-a)^2/2a^2 + (x-a)^3 / 3a^3 - ...
double myln(double x)
{
  double rtn;
  double e = 2.718281828459045;
  double a = e;
  while(x > e)
  {
    x/=e;
    rtn += 1.0;
  }
  while (x < 1.0)
  {
    x *= e;
    rtn -= 1.0;
  }
  double d = x - a;
  rtn = rtn + 1 + d/a - d*d / (2*a*a) + d*d*d / (3*a*a*a) - d*d*d*d / (4*a*a*a*a) + d*d*d*d*d / (5*a*a*a*a*a) - d*d*d*d*d*d / (6*a*a*a*a*a*a) + d*d*d*d*d*d*d / (7*a*a*a*a*a*a*a);
  //rtn = rtn - d*d*d*d*d*d*d*d / 8;

  return rtn;
}








int main(int argc, char* argv[])
{
  int rtn = 0;
  flt_t a = {100000,0};
  flt_t b = {999,-2};
  flt_print(a);
  printf("\n");
  flt_print(b);
  printf("\n");
  flt_t r;
  r = flt_mul(&a, &b);

  a.mant = 999999999llu;
  a.exp = 0;
  b.mant = 999999999llu;
  b.exp = 0;
  r = flt_mul(&a, &b);
  printf("\n");
  r = flt_mul(&a, &r);
  printf("\n");

  flt_t f = flt_build(99, -39);
  flt_print(f);

  flt_t z = flt_add(&f,&b);
  printf("\nZ: ");
  flt_print(z);
  printf("\n");

  flt_t s= flt_build(1234,0);
  flt_t t = flt_build(900,0);
  flt_t v = flt_add(&s, &t);
  printf("\n1234 + 900 = ");
  flt_print(v);
  printf("\n");
  flt_t u = flt_sub(&s, &t);
  printf("\n1234 - 900 = ");
  flt_print(u);
  printf("\n");

  s = flt_build(5678,-5);
  t = flt_build(900, -4);
  v = flt_add(&s, &t);
  printf("\n 5678 E-5 + 900 E-4 = ");
  flt_print(v);printf("\n");

  v = flt_sub(&s, &t);
  printf("\n5678 E-5 - 900 E-4 = ");
  flt_print(v); printf("\n");

  flt_t d1;
  flt_t d2;
  flt_t d3;
  d1 = flt_build(500000,0);
  d2 = flt_build(-25,0);
  d3 = flt_div(&d1,&d2);
  printf("500000 / -25 = ");
  flt_print(d3);
  printf("\n");

  // double log;
  // printf("ln 1 = %f\n", myln(1));
  // printf("ln e = %f\n", myln(2.718281828459045));
  // printf("ln 2 = %f\n", myln(2.0));
  // printf("ln 0.1 = %f\n", myln(0.1));
  // printf("ln 0.000001 = %f\n", myln(0.000001));
  // printf("ln 10 = %f\n", myln(10.0));



  return rtn;
}


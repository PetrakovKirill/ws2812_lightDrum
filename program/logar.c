#define LOG2E 1.44269504088896340736f

typedef union ADVFLOAT {
  float x;
  struct
  {
    unsigned int mant : 23; /* Mantissa without leading one */ 
    unsigned int exp  : 8;  /* Exponential part */
    unsigned int sign : 1;  /* Indicator of the negative number */
  };
}ADVFLOAT;


float FastLog2( float x ) {
  ADVFLOAT ax;
  int exp;

  ax.x = x;
  exp = ax.exp - 127;
  ax.sign = 0;
  ax.exp = 127;

  return (ax.x - 1.0f) * LOG2E + exp;
}

float FastLog(float x, float a) {
	float result;
	result = FastLog2(x);
	return (result / a);
}
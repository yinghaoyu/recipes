#include <stdio.h>

int a;
int b = 0;
int c = 256;
int d[10];
int e[] = {0, 1, 2, 3};

static int f;
static int g = 0;
static int h = 10;

const char* str = "global str!!!";

int main()
{
  static int i = 0;
  printf("hello world!!!\n");
  return 0;
}

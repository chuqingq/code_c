#include <stdio.h>

const int MAX = 10;

void print(int a[][MAX], int n) {
  for(int x=0;x<n;x++)
  { 
    for(int y=0;y<n;y++)
    {
      printf("%3d", a[x][y]);
    }
    printf("\n");
  }
}

int main()
{
  int c=0,x=0, y=0,n, a[MAX][MAX]={0};
  n=3;
  c = n*n;
  a[x][y] = c;

  while(1!=c)
  {
    // 向下
    while(x+1<n  && !a[x+1][y])
      a[++x][y] = --c;
    // 向右
    while(y+1<n && !a[x][y+1] )
      a[x][++y] = --c;
    // 向上
    while(x-1>=0 && !a[x-1][y])
      a[--x][y] = --c;
    // 向左
    while(y-1>=0  && !a[x][y-1])
      a[x][--y] = --c;
  }

    print(a, n);
}

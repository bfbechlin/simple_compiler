#include <stdio.h>

int a;
int b;
int i = -2;
int j = -2;

void main(){
	a = i + j;
	a = i - j;
	a = i * j;
	a = i / j;
	printf("%i\n", a);
}

void test(){
	a = i + j;
	a = i - j;
	a = i * j;
	a = i / j;
	a = i > j;
	a = i < j;
	a = !(i < j);
	a = i <= j;
	a = i >= j;
	a = i == j;
	a = i != j;
	a = (i < j) || (i == 1);
	a = (i < j) && (i == 1);
}

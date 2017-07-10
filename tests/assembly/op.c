#include <stdio.h>

int a;
int i = 1;
int j = 1;
void main(){
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

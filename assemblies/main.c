#include <stdio.h>
double a = 2.0;
int b;
char c = 'a';
void main(){
	b = 1;
	printf("a = %lf ", a);
	a = 10.1 + b;
	printf("a = %lf\n", a);
}

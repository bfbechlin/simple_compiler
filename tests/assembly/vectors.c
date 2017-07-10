#include <stdio.h>

int i = 1;
int v[10];
void main(){
	v[1] = i;
	v[i] = 2;
	v[i+2] = i + 2;
	v[3] = v[1];
	v[4] = v[i+1];
	v[i+4] = v[v[1]];
}

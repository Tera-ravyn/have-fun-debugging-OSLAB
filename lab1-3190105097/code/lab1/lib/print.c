#include "print.h"
#include "sbi.h"

void puts(char *s) {
	int i = 0;
    	while(s[i++]!='\0')
    	{
		sbi_ecall(0x1, 0x0, s[i], 0, 0, 0, 0, 0);
    	}
}

void puti(int x) {
	char s[100];
	int i = 0;
	if(x<0) {
		x = 0-x;
		sbi_ecall(0x1, 0x0, '-', 0, 0, 0, 0, 0);
	}
    	for(; x/10 != 0 ; i++){
		s[i] = x%10 + '0';
		x /= 10;
	}
	s[i] = x + '0';
	for( ; i >= 0; i--){
    		sbi_ecall(0x1, 0x0, s[i], 0, 0, 0, 0, 0);
	}
}

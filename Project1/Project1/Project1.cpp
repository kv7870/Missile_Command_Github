// Project1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <string.h>

void reverse(char str[], int length) {
	char temp = 0;

	for (int i = 0; i < length / 2; i++) {
		temp = str[i];
		str[i] = str[length- i];
		str[length - i] = temp; 
		
	}


}

int main()
{
	char str[6] = "Kevin";
	int length = strlen(str); 
	reverse(str, length);
	puts(str);
	printf("%s", str); 
    return 0;
}


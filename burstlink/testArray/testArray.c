// testArray.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void hex_bin_to_string(const uint8_t *hex_bin, const uint32_t bin_length, uint8_t *str){

	uint32_t str_length = 2 * bin_length+1;
	uint8_t temp_str[10] = {0};
	uint32_t i = 0;
#ifdef _WIN32
	strcpy_s(str, str_length, "");
#else
	strcpy(str, "");
#endif
	for (i = 0; i<bin_length; i++){
#ifdef _WIN32
		memset(temp_str, 0, 10);
		if (hex_bin[i]<16){
			sprintf_s(temp_str, 10, "0%X", hex_bin[i]);
		}
		else{
			sprintf_s(temp_str, 10, "%X", hex_bin[i]);
		}
		strcat_s(str, str_length, temp_str);
#else
		if (hex_bin[i]<16){
			sprintf(temp_str, "0%X", hex_bin[i]);
		}
		else{
			sprintf(temp_str, "%X", hex_bin[i]);
		}
		strcat(str, temp_str);
#endif
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	uint8_t a[] = "AAAAAAAA";
	uint8_t *bin = (uint8_t *)malloc(sizeof(uint8_t)*10);
	memset(bin, 5, 1);
	uint8_t str[3] = {0};
	hex_bin_to_string(bin, 1, str);
	//sprintf_s(str, 10, "0%X", bin[0]);
	printf("%s\n", str);
	int i = 0;
	for (i = 0; i < 7; i++){
		printf("%d\n", bin[i]);
	}
	free(bin);
	return 0;
}


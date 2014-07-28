// testArray.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <rpc.h>

#pragma comment(lib,"rpcrt4.lib")

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
	uint8_t *uuid_str = (uint8_t *)malloc(sizeof(uint8_t)*40);
	uuid_t muuid;
	UuidCreate(&muuid);
	UuidToStringA(&muuid, (RPC_CSTR *)&uuid_str);
	printf("UUID:%s\n", uuid_str);
}


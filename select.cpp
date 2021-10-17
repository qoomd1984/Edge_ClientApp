//
//File : 	select.cpp
//Function: select the device uart input data, and output the key data;
//author: 	menden
//date:		16OCT2021
//version:	V0.1
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "select.h"



const char* str_a = "+DATA:0064,6,OPEN!!";
const char* str_b = "OPEN";
const char* str_c = "Sat Oct 16 11:07:48 2021";
int string_select(char* str_in)
{
	int i, j, match_flag = 0;
	char str_tmp[20];
	
	i = 0;
	do{
		for(j=0; j<19; j++){
			str_tmp[j] = str_in[i+j];
		}
		str_tmp[19] = '\0';
		if(strcmp(str_tmp, str_a) == 0) {
			match_flag = 1;
			break;
		}
		
		i++;
	}
	while(str_in[i] != '\0');
	
	if(match_flag == 1){
		return 0x00;
	}
	else {
		return 0xff;
	}
}



void string_gen(char* str_in)
{
	int i, j, str_len, array_pos, match_flag = 0;
	char str_tmp[5];
	
	char* str_time;
	time_t rawtime;
	struct tm* timeinfo;
	
	str_len = strlen(str_in);
	
	printf("%d\n", str_len);
	
	i = 0;
	do{
		for(j=0; j<4; j++){
			str_tmp[j] = str_in[i+j];
		}
		str_tmp[4] = '\0';
		if(strcmp(str_tmp, str_b) == 0) {
			match_flag = 1;
			array_pos = i;
		}
		
		i++;
	}
	while(i < str_len);
	
	printf("%d\n", array_pos);
	
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	str_time = asctime(timeinfo);
	
	printf("%s\n", str_time);
	printf("%c\n", str_in[array_pos+9]);
	for(j=0; j<24; j++) {
		str_in[array_pos+9+j] = str_time[j];
	}
}

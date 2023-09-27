/*
 * tagging.h
 *
 *  Created on: Jun 30, 2023
 *      Author: jvlsq
 */

#ifndef INC_TAGGING_H_
#define INC_TAGGING_H_
#include "define.h"
#include "string.h"

void get_recipe_from_classifier(char* classifier, char* recipe){
//	char first_part[] = "RPT,\"Recipe_";
//	int first_part_length = (sizeof first_part) - 1;
//	char last_part[] =  "\"\r\0";
//	int last_part_length = (sizeof last_part) - 1;
//	memcpy(recipe, first_part, first_part_length); //taking out \0
//	memcpy(recipe + first_part_length, classifier, length_classifier - 1);
//	memcpy(recipe + first_part_length + length_classifier - 1, last_part, last_part_length);
	sprintf (recipe, "RPT,\"Recipe_%s", classifier);
}

void get_computer_from_orientation(char* classifier, char* orientation, char* computer){
//	char first_part[] = "RPT,\"Computer";
//	int first_part_length = (sizeof first_part) - 1;
//	char last_part[] =  "\"\r\0";
//	int last_part_length = (sizeof last_part) - 1;
//	memcpy(computer, first_part, first_part_length); //taking out \0
//	memcpy(computer + first_part_length, orientation, length_classifier - 1);
//	memcpy(computer + first_part_length + length_classifier - 1, last_part, last_part_length);
	char * token = strtok(classifier, "-");
	sprintf (computer, "%s ,%s", token, orientation);
}

int get_length_message(char* recipe){ // number characters before /r
	int n = 0 ;
	while((recipe[n] != '\r' && recipe[n] != '\0' ) && (n < MAX_MESSSAGE_SIZE )){
		n++;
	}
	if(n<MAX_MESSSAGE_SIZE) n++; //adding \r

	return n;
}
#endif /* INC_TAGGING_H_ */

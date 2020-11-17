/*******************************************************************/
/***			IZP PROJEKT 2 - prace s datovymi strukturami	 ***/
/***															 ***/
/***projekt plni funkce pokrocilejsiho tabluvkoveho procesoru	 ***/
/*** 															 ***/
/***						Jakub Kuznik        				 ***/
/*** 						listopad\prosinec 2020			  	 ***/
/*******************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define MAX_LINUX_FILE_SIZE 256

int separe(int argc, char *argv[], int *separator); //find cell separator 
int input_error(int argc); //Check if program execution have good syntax.
int save_file(char **argv, char **sheet, int is_there_separator);

int main(int argc, char **argv)
{
	int errors = 0; 
	int is_there_separator = 0; // if there is separator in argv it ll be changed to true 
	int d_separator;	
	char **sheet;
	
	errors = input_error(argc);
	if(errors != 0)
		return -1;
    is_there_separator = separe(argc, argv, &d_separator);
	errors = save_file(argv, sheet, is_there_separator);
	if(errors != 0)
		return -1;

	return 0; 
}
int save_file(char **argv, char **sheet, int is_there_separator)
{
	FILE *sheet_file;
	
	if(is_there_separator == 1)
		sheet_file = fopen(argv[5], "r+"); 
	else
	{
		printf("%s",argv[3]);
		sheet_file = fopen(argv[3], "r+");
	}	
	if(sheet_file == NULL)
	{
		fprintf(stderr, "Can not open file\n");
		return -1;
	}
	return 0;
}

//Check if program execution have good syntax.
//It should be /sps [-d DELIM] CMD_SEQUENCE FILE
//So file name has to be on argv[5] or argv[3]
int input_error(int argc)
{
	if((argc != 5) && (argc != 3))
	{
		fprintf(stderr, "BAD ARGUMENT FORMAT \n./sps [-d DELIM] CMD_SEQUENCE FILE\n");
		return -1;
	}
	return 0;
}
//In table we alwazs have some separator character this function ll get it.
//-d 
//separator is always next argument after -d 
//it there is not any default is ' ' space 
int separe(int argc, char *argv[], int *separator)
{	
	*separator = ' ';
	for(int i = 0; i < argc; i++)  
	{
		if(strcmp(argv[i], "-d")==0)
			if(i+1 < argc)
			{
				*separator = argv[i+1][0];
				return 1;
			}
	}
	return 0; 
}
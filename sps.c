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

typedef struct 
{
	char *one_row;  //one row is made of multiple cell 
	int row_size;
	
}row;


//THIS ARE BEING DONE ON BEGING TO CHECK ARGUMENTS FORMAT AND STORE SEPARATOR 
int separe(int argc, char *argv[], int *separator); //find cell separator 
int input_error(int argc); //Check if program execution have good syntax.

//INPUT ANALYZE FUNCTIONS  
int count_rows(char **argv, int is_there_separator, int *rows_counter);
int count_rows_lenght(char **argv, int row_counter, int is_there_separator, int *rows_lenght);

//FUNCTIONS FOR SHEET MEMORY ALOCATION 
void initialize_sheet(row *sheet, int row_counter);
int alocate_sheet(row *sheet, int row_counter, int *rows_lenght);
int store_sheet(row *sheet, char **argv, int row_counter, int is_there_separator, int *rows_lenght);
void free_sheet(row *sheet, int rows);
void print_sheet(row *sheet, int row_counter);


//FUNCTIONS FOR SIMPLIFICATION
void array_int_init(int size, int *array);


int main(int argc, char **argv)
{
	int errors = 0; 
	int is_there_separator = 0; // if there is separator in argv it ll be changed to true 
	int d_separator;		
	int row_counter = 0;
	int *rows_lenght;

	errors = input_error(argc);
	if(errors != 0) return -1;
    is_there_separator = separe(argc, argv, &d_separator);
	
	errors = count_rows(argv, is_there_separator, &row_counter);
	if(errors != 0) return -1;

	row sheet[row_counter -1];
	
	rows_lenght = malloc((row_counter)* sizeof(rows_lenght));
	array_int_init(row_counter, rows_lenght);

	errors = count_rows_lenght(argv, row_counter, is_there_separator, rows_lenght);
	if(errors != 0) return -1;

	initialize_sheet(sheet, row_counter);
	
	errors = alocate_sheet(sheet, row_counter, rows_lenght);
	if(errors != 0) return -1;

	errors = store_sheet(sheet, argv, row_counter, is_there_separator, rows_lenght);
	if(errors != 0) return -1;

	print_sheet(sheet, row_counter);
	free(rows_lenght);
	free_sheet(sheet, row_counter);
	return 0; 
}
void initialize_sheet(row *sheet, int row_counter)
{
	for(int i=0; i<row_counter;i++)
	{
		sheet[i].row_size = 0;
		sheet[i].one_row = NULL;
	}
}
void print_sheet(row *sheet, int row_counter)
{
	for(int i = 0; i < row_counter;i++)
	{
		for (int j = 0; j < sheet[i].row_size-1 ; j++)
		{
			putchar(sheet[i].one_row[j]);
			if(sheet[i].one_row[j] == '\n')
				break;
		}	
	}
}
//it ll free alocated mememory for sheet 
//destructoror 
void free_sheet(row *sheet, int rows)
{
	for(int i=0; i <rows; i++)
	{
		free(sheet[i].one_row);
		sheet[i].row_size = 0;
		sheet[i].one_row = NULL;
	}	
}
int alocate_sheet(row *sheet, int row_counter, int *rows_lenght)
{
	for(int i=0; i<row_counter; i++)
	{
		sheet[i].one_row = malloc((rows_lenght[i] + 10) * sizeof(char));
		if(sheet[i].one_row == NULL)
		{
			fprintf(stderr, "Maloc error \n");
			return -1;
		}
	}
	return 0;
}

/*This function ll not only alocate memmory but also safe input file to data structure */
int store_sheet(row *sheet, char **argv, int row_counter, int is_there_separator, int *rows_lenght)
{
	int j = 0, k = 0;
	int c;
	FILE *sheet_file;
	int errors = 0;

	if(is_there_separator == 1)sheet_file = fopen(argv[4], "r+"); else sheet_file = fopen(argv[2], "r+");

	sheet[j].row_size = rows_lenght[j];
	while((c=fgetc(sheet_file)) != EOF)
	{
		sheet[j].one_row[k] = c;
		k++;
		if(c == '\n')
		{
			j++;
			sheet[j].row_size = rows_lenght[j];
			k = 0;
		}	
	}
	fclose(sheet_file);
	return 0;
}
int count_rows(char **argv, int is_there_separator, int *rows_counter)
{
	int c = 0;
	FILE *sheet_file;
	if(is_there_separator == 1)	sheet_file = fopen(argv[4], "r+"); else sheet_file = fopen(argv[2], "r+");
	if(sheet_file == NULL)
	{
		fprintf(stderr, "Can not open file\n");
		return -1;
	}
	while((c=fgetc(sheet_file)) != EOF)
		if(c == '\n')
			*rows_counter = *rows_counter + 1;
	fclose(sheet_file);
	return 0;
}
void array_int_init(int size, int *array)
{
	for(int i = 0; i<size;i++)
		array[i] = 0;
}

//This function ll count lenght of every line 
int count_rows_lenght(char **argv, int row_counter, int is_there_separator, int *rows_lenght)
{
	FILE *sheet_file;
	int c, error = 0, i = 0;
	
	if(is_there_separator == 1)	sheet_file = fopen(argv[4], "r+"); else sheet_file = fopen(argv[2], "r+");
	if(sheet_file == NULL)
	{
		fprintf(stderr, "Can not open file\n");
		return -1;
	}
	while(((c=fgetc(sheet_file)) != EOF) && i != row_counter)
	{
		rows_lenght[i]++;
		if(c == '\n')
		{
			rows_lenght[i] += 2;
			i++;
		}
	}
	fclose(sheet_file);
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
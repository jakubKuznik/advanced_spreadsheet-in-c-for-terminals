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

typedef struct rows 
{
	char *row_data;
	int row_size;
	
}row;


int separe(int argc, char *argv[], int *separator); //find cell separator 
int input_error(int argc); //Check if program execution have good syntax.

int save_file(char **argv, row *sheet, int is_there_separator, int row_counter, int biggest_row);

int get_input_sheet_size(row **sheet, char **argv, int *row_counter, int is_there_separator);
int store_to_struct(row ** sheet, char **argv, int row_counter, int is_there_separator, int *rows_lenght);
void reference(row **sheet, int row_counter);
void free_sheet(row **sheet, int rows);
int get_number_of_rows(char **argv, int is_there_separator, int rows_counter);
void print_sheet(row **sheet, int row_counter);
void initialize_sheet(row *sheet, int row_counter);

int main(int argc, char **argv)
{
	int errors = 0; 
	int is_there_separator = 0; // if there is separator in argv it ll be changed to true 
	int d_separator;		
	int row_counter = 0;
	row **sheet;

	errors = input_error(argc);
	if(errors != 0)
		return -1;
    is_there_separator = separe(argc, argv, &d_separator);

	errors = get_input_sheet_size(sheet, argv, &row_counter, is_there_separator);
	if(errors != 0)
		return -1;
	
	print_sheet(sheet, row_counter);
	free_sheet(sheet, row_counter);
	return 0; 
}
void print_sheet(row **sheet, int row_counter)
{
	for(int i = 0; i < row_counter;i++)
		for (int j = 0; j < sheet[i]->row_size-1 ; j++)
			printf("%c",sheet[i]->row_data[j]);
}

//it ll free alocated mememory for sheet 
//destructoror 
void free_sheet(row **sheet, int rows)
{
	for(int i = 0; i < rows; i++)
	{
		free(sheet[i]->row_data);
		sheet[i]->row_data = NULL;
		free(sheet[i]);
		sheet[i] = NULL;
	}
}
//work like an inicializator
void initialize_sheet(row *sheet, int row_counter)
{
	for (int i = 0;i<row_counter;i++)
	{
		sheet->row_data = NULL;
		sheet->row_size = 0;
	}
	
}
int alocate_sheet(row **sheet, int row_counter, int *rows_lenght)
{
	for(int i=0; i<row_counter; i++)
	{
		sheet[i] = malloc(sizeof(row)); 
		if(sheet[i] == NULL)
		{
			fprintf(stderr, "Maloc error \n");
			return -1;
		}
		initialize_sheet(sheet[i], row_counter);
		sheet[i]->row_data = malloc(sizeof(char) * (rows_lenght[i] + 2));
		if(sheet[i]->row_data == NULL)
		{
			fprintf(stderr, "Maloc error \n");
			return -1;
		}
	}
	return 0;
}

/*This function ll not only alocate memmory but also safe input file to data structure */
int store_to_struct(row ** sheet, char **argv, int row_counter, int is_there_separator, int *rows_lenght)
{
	int j = 0, k = 0;
	int c;
	FILE *sheet_file;
	int errors = 0;


	errors = alocate_sheet(sheet, row_counter, rows_lenght);
	if(errors != 0)
		return -1;
	
	if(is_there_separator == 1)
		sheet_file = fopen(argv[4], "r+"); 
	else
		sheet_file = fopen(argv[2], "r+");
	
	sheet[j]->row_size = rows_lenght[j];
	while((c=fgetc(sheet_file)) != EOF)
	{
		sheet[j]->row_data[k] = c;
		k++;
		if(c == '\n')
		{
			j++;
			sheet[j]->row_size = rows_lenght[j];
			k = 0;
		}	
	}
	fclose(sheet_file);
	return 0;
}
int get_number_of_rows(char **argv, int is_there_separator, int rows_counter)
{
	int c = 0;
	rows_counter = 0;
	FILE *sheet_file;
	if(is_there_separator == 1)
		sheet_file = fopen(argv[4], "r+"); 
	else
		sheet_file = fopen(argv[2], "r+");
	if(sheet_file == NULL)
	{
		fprintf(stderr, "Can not open file\n");
		return -1;
	}
	while((c=fgetc(sheet_file)) != EOF)
		if(c == '\n')
			rows_counter++;
	return rows_counter;
	fclose(sheet_file);
}
int get_input_sheet_size(row **sheet, char **argv, int *row_counter, int is_there_separator)
{
	FILE *sheet_file;
	int *rows_lenght;
	int c;
	int error = 0;
	
	*row_counter = get_number_of_rows(argv, is_there_separator, *row_counter);

	rows_lenght = malloc(*row_counter * sizeof(int));
	if(rows_lenght == NULL)
	{
		fprintf(stderr, "Maloc error \n");
		return -1;
	}
	if(is_there_separator == 1)
		sheet_file = fopen(argv[4], "r+"); 
	else
		sheet_file = fopen(argv[2], "r+");
	if(sheet_file == NULL)
	{
		fprintf(stderr, "Can not open file\n");
		return -1;
	}
	rows_lenght[0] = 0;
	int i = 0;
	while((c=fgetc(sheet_file)) != EOF)
	{
		rows_lenght[i]++;
		if(c == '\n')
		{
			rows_lenght[i] += 2;
			i++;
			rows_lenght[i] = 0;
		}
	}
	fclose(sheet_file);
	error = store_to_struct(sheet, argv, *row_counter, is_there_separator, rows_lenght);
	if(error != 0)
	{
		fprintf(stderr, "maloc error \n");
		return -1;
	}
	free(rows_lenght);
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
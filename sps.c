/*******************************************************************/
/***			IZP PROJEKT 2 - prace s datovymi strukturami	 ***/
/***															 ***/
/***projekt plni funkce pokrocilejsiho tabluvkoveho procesoru	 ***/
/*** 															 ***/
/***						Jakub Kuznik        				 ***/
/*** 						listopad\prosinec 2020			  	 ***/
/*******************************************************************/
/*
EXECUTION
./sps [-d DELIM] CMD_SEQUENCE FILE
SELECTIONS  COMMANDS 
[R,C] [R,_][_,C][R1,C1,R2,C2][_,_][min][max][find STR][_]
SHEET EDITING COMMANDS 
irow arow drow icol acol dcol 
SHEET CONTENT EDITING COMMANDS 
set STR clear swap [R,C] sum [R,C] avg [R,C] count [R,C] len [R,C] 
TEMPORARILY VARIABLES COMMANDS 
def _X use _X inc _X [set]
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_LINUX_FILE_SIZE 256
#define COMANDS_MAXIMUM 1000 // bigest command is 11 chars. and i add some reserve 
#define MAX_COMMAND_SIZE 1000
#define FIND_COM_STRING_START 6//[find 
#define COMMNDS_POSITION_WITH_SEPARATOR 3
#define COMMNDS_POSITION_WITHOUT_SEPARATOR 1
#define TEMPO_VARS_MAX 10
#define TEMPO_VARS_LENGHT 1000
#define COMMAND_SEPARATOR ';'
#define NUMBER_IS_LOCATED 5   // def _X use _X inc _X should have number on position 5 
#define BRACKET_POSITION 5   // swap [R,C]
#define ARGUMENT_SUM_POSIBILITY_ONE 5
#define ARGUMENT_SUM_POSIBILITY_TWO 3
#define STRING_START_POSITION 4 //set STR
#define NEXT_NUM_POSITION 3
#define IROW 1
#define AROW 2
#define RESERVE 100 //if i want to transfer \separator to ""
#define ICOL 1
#define ACOL 2
#define NOT_ICOL_ACOL 0
#define ERROR_VALUE_ONE -1 
#define ERROR_VALUE_ZERO 0
#define MIN 1
#define MAX 2
#define AVG 1
#define SUM 2
#define POSITION_FIX 2
#define SPACE_FOR_NEWLINE 2
#define LONG_FORMAT 2
#define SHORT_FORMAT 1
#define LONG_FOMRAT_COMMAS 3
#define SHORT_FORMAT_COMMAS 1
#define LEN 0
#define COUNT 1

typedef struct 
{
	char *one_row;  //one row is made of multiple cell 
	int row_size;
	int cels_in_row;
	
}row;
//FUNCTIONS THAT ARE USED FOR WORK WITH INPUT COMMANDS 
//This function check if there is less them 1000 commands and if there is less tham 1000 commands
int commands_error_check(char **argv, int is_there_separator, int *count_chars, int *count_commands);
void store_commands(char **argv, int is_there_separator, char *commands, int command_char_sum);
/*This function ll find out what command to do and call the functions */
int command_execution(int commands_sum, char *commands, int commands_char_sum, int separator, row *sheet, int *row_counter);
//This function just story one command that ll be executed 
int store_one_command(char *single_command, char *commands, int *last_command, int commands_char_sum);
/*this ll call command and them it ll be executed */
int call_command(row *sheet, char *single_command, int *row_from, int *row_to, int *cell_from, int *cell_to, char tempo_vars[TEMPO_VARS_MAX][TEMPO_VARS_LENGHT], char separator, int *row_counter, int *temp_10);

/*These commands are for sheet table edit.*/
int sheet_edit(row *sheet, char *single_command,int *row_from, int *row_to, int *cell_to, int *cell_from, char separator, int *row_counter);
//It ll put rows up the choosen rows OR It ll put rows under the choosen rows.
int s_e_irow_arow(row *sheet, int row_from, int row_to, int cell_from, int cell_to, char separator, int *row_counter, int irow_arow);
//It delete choosen rows. 
int s_e_drow(row *sheet, int row_from, int row_to, int *row_counter);
//It ll add column right from every chosen cell OR It ll add column left from every chosen cell. 
int s_e_icol_acol(row *sheet,int cell, char separator, int *row_counter,int icol_acol);
//It ll delete every chosen column.
int s_e_dcol(row *sheet, int cell, char separator, int *row_counter);

/*these command are for temporarily variables */
int temp_edit(row *sheet, char *single_command, int *row_to, int *cell_to, char tempo_vars[TEMPO_VARS_MAX][TEMPO_VARS_LENGHT],int *temp_10, char separator );
/*Actual cell ll be stored to X_10 temporarily var that is reserved for selections */
int temp_set(int row_to, int cell_to, int *temp_10);
int temp_def(row *sheet, char *single_command, int row, int cell, char temp_vars[TEMPO_VARS_MAX][TEMPO_VARS_LENGHT], char separator);
int temp_use(row *sheet, char *single_command, int row, int cell, char tempo_vars[TEMPO_VARS_MAX][TEMPO_VARS_LENGHT], char separator);
int temp_inc(char *single_command, char tempo_vars[TEMPO_VARS_MAX][TEMPO_VARS_LENGHT]);

/*these commands are for selection change */
int select_change(row *sheet, char *single_command, int *row_from, int *row_to, int *cell_from, int *cell_to, char last_character, char separator, int *row_counter, int *temp_10);
//This funciton is called if i know that selection has format [R,C] or [R1,C1,R2,C2]
int select_change_simplify(row *sheet, char *single_command, int *row_from, int *row_to, int *cell_from, int *cell_to, char separator, int *row_counter, int *temp_10);
//this command is called if selection is in [R1,C1] or [R1,C1,R2,C2] format
int selec_changer_usual(char *single_command, int *h_rf, int *h_rt, int *h_cf, int *h_ct, int type, row *sheet, int row_counter);
//it ll find smallest num or bigest num in selection 
int select_min_max(row *sheet, int *row_from, int *row_to, int *cell_from, int *cell_to, char separator, int operation);
int select_find(row *sheet, char *single_command, int *row_from, int *row_to, int *cell_from, int *cell_to, char separator );

/*These are for editing sheet content */
int content_edit(row *sheet, char *single_command, int *row_from, int *row_to, int *cell_from, int *cell_to, char last_character, char separator, int row_counter);
//it replace chosen cell selection with STR 
int c_e_set(row *sheet, char *single_command, int r_f, int r_t, int c_f, int c_t, char separator);
//it just delete cells content 
int c_e_clear(row *sheet, int r_f, int r_t, int c_f, int c_t, char separator);
//swap select cell and chosen 
int c_e_swap(row *sheet, char *single_command, char separator, int row, int cell, int row_counter);
//It store sum of not empty cells or some cell lenght to difrent cell
int c_e_len_count(row *sheet, char *single_command, int r_f, int r_t, int c_f, int c_t, char separator, int row_counter, int len_count);
//avg [R,C]  It store sum or avg to chosen cell from selection 
int c_e_sum_avg(row *sheet, char *single_command ,int r_f, int r_t, int c_f, int c_t, char separator, int row_counter, int avg_sum);
int get_sa_cell(char *single_command, int *row, int *cell, bool count);

//THIS ARE BEING DONE ON BEGING TO CHECK ARGUMENTS FORMAT AND STORE SEPARATOR 
int separe(int argc, char *argv[], int *separator); //find cell separator 
int input_error(int argc); //Check if program execution have good syntax.

//INPUT ANALYZE FUNCTIONS  
int count_rows(char **argv, int is_there_separator, int *rows_counter);
int count_rows_lenght(char **argv, int row_counter, int is_there_separator, int *rows_lenght);
int count_cells_in_row(int i, row *sheet, char separator);

//FUNCTIONS FOR SHEET MEMORY ALOCATION 
void initialize_sheet(row *sheet, int row_counter);
void constructor(row *sheet, int row_counter, int *count_rows_lenght);
int alocate_sheet(row *sheet, int row_counter, int *rows_lenght);
int store_sheet(row *sheet, char **argv, int is_there_separator, int *rows_lenght, int row_c);
void free_sheet(row *sheet, int rows);
void end_print_sheet(row *sheet, int row_counter);
int sheet_row_realoc(row *sheet, int row, int space);
int error_maloc(int return_value);

//FUNCTIONS FOR WOKING WITH CELLS 
int delete_cell_value(row *sheet, int row, int cell, char separator);
int change_cell_value(row *sheet, int row, int cell, char separator, char *value, int size);
int get_cell_size(row *sheet, char separator, int cell, int row);
int get_cell_position(row *sheet, int row, int cell, char separator);
int store_row(row *sheet, char *store_to, int row);

//FUNCTIONS FOR EDITING SHEET
int row_move_right(row *sheet, int row, int cell, int space, char separator, int icol_not);
void rewrite_file(row *sheet, int row_counter, char **argv, int is_there_separator, char separator);
int sheet_reformat(row *sheet, int row_counter);
int move_rows_down(row *sheet, int row, int *row_counter, char separator);

//FUNCTIONS FOR SIMPLIFICATION 
void array_int_init(int size, int *array); //This ll initalize int array;
char get_last_char(char *aray);
void array_char_init(char *array, int size);
int get_array_size(char *array, int max_size);

int main(int argc, char **argv)
{
	int errors = 0; 
	int is_there_separator = 0; 		// if there is separator in argv it ll be changed to true 
	int d_separator;		
	int row_counter = 0;
	int *rows_lenght = NULL;
	int commands_char_sum = 0;	
	int commands_sum = 0;				//how many commands are on input 

	errors = input_error(argc);
	if(errors != 0) return 0;
    is_there_separator = separe(argc, argv, &d_separator); 	 //first i ll check input for erors find separator and story commands 
	errors = commands_error_check(argv,is_there_separator, &commands_char_sum, &commands_sum); 
	if(errors != 0) return -1;

	errors = count_rows(argv, is_there_separator, &row_counter); //Count input file rows and size so i can maloc
	if(errors != 0) return -1;

	row sheet[row_counter -1]; //HERE I CREATE ROW SHEET IT IS ARRAY OF ROW THAT CONTAIN CHAR ROW AND ROW SIZE 

	rows_lenght = malloc((row_counter)* sizeof(rows_lenght));
	array_int_init(row_counter, rows_lenght);
	errors = count_rows_lenght(argv, row_counter, is_there_separator, rows_lenght);
	if(errors != 0)
	{			
		free(rows_lenght);		
		return -1;
	}

	initialize_sheet(sheet, row_counter);
	errors = alocate_sheet(sheet, row_counter, rows_lenght);//alocate space in heap for my sheet that can have infinity size 
	if(errors != 0)
	{
		free(rows_lenght);
		return -1;
	}
	errors = store_sheet(sheet, argv, is_there_separator, rows_lenght, row_counter);
	if(errors != 0)
	{
		free(rows_lenght);
		free_sheet(sheet, row_counter);
		return -1;
	}
	
	for(int i = 0; i<row_counter; i++)
		count_cells_in_row(i, sheet, d_separator);

	char commands[commands_char_sum+2]; //here i ll store all the commands that ll be executed on sheet 
	store_commands(argv, is_there_separator, commands, commands_char_sum);
	errors = command_execution(commands_sum, commands, commands_char_sum, d_separator, sheet, &row_counter);
	if(errors != 0)
	{
		free(rows_lenght);
		free_sheet(sheet, row_counter);
		return -1;
	}
	sheet_reformat(sheet, row_counter);
	rewrite_file(sheet, row_counter, argv, is_there_separator, d_separator);
	
	free(rows_lenght);
	free_sheet(sheet, row_counter);
	return 0; 
}
int error_syntax(int return_value)
{
	fprintf(stderr, "Syntax error in commands ");
	return return_value;
}
int error_maloc(int return_value)
{
	fprintf(stderr, "Maloc Error");
	return return_value;
}
/*
If i have some nonsense in sheet i ll rework it.
*/
int sheet_reformat(row *sheet, int row_counter)
{
	int new_size = 0;
	char *help = NULL;
	char *help_two = NULL;
	for(int j = 0; j < row_counter; j++)
	{
		help = malloc(sheet[j].row_size * sizeof(char));
		help_two = malloc(sheet[j].row_size * sizeof(char));
		if(help == NULL && help_two == NULL)
			return error_maloc(ERROR_VALUE_ONE);
		array_char_init(help, sheet[j].row_size);
		array_char_init(help_two, sheet[j].row_size);
		for(int i = 0; i < sheet[j].row_size; i++)
		{
			help[i] = sheet[j].one_row[i];
		}
		new_size = 0;
		for(int i = 0;i < sheet[j].row_size; i++)
		{
			help_two[new_size++] = help[i];
		}
		free(sheet[j].one_row);
		sheet[j].row_size = new_size;
		sheet[j].one_row = malloc(new_size * sizeof(char));
		if(sheet[j].one_row == NULL)
			return error_maloc(ERROR_VALUE_ONE);
		array_char_init(sheet[j].one_row, new_size);
		for(int i = 0; i < new_size; i++)
		{
			sheet[j].one_row[i] = help_two[i];
		}
		free(help_two);
		free(help);
		help = NULL;
	}
	return 0;	
}
//TODO
/*
It move row content from one cell to right.
Also cell and row is altomaticly -1
//BEFORE U CALL THIS FUNCTION U HAVE TO DO SHEET_ROW_REALOC!!!
*/
int row_move_right(row *sheet, int row, int cell, int space, char separator, int icol_not)
{
	int position =0;
	position = get_cell_position(sheet, row, cell, separator);
	if(cell > 1)
	{
		if(icol_not == NOT_ICOL_ACOL)
			position = position +POSITION_FIX ;
	}		
	sheet[row-1].one_row = realloc(sheet[row-1].one_row, (sheet[row-1].row_size + space) * sizeof(char));
	if(sheet[row-1].one_row == NULL)
		return error_maloc(ERROR_VALUE_ONE);
	sheet[row-1].row_size = sheet[row-1].row_size + space;
	//inicialize realoc 
	for(int i = sheet[row-1].row_size - space; i < sheet[row-1].row_size; i++)
		sheet[row-1].one_row[i] = '\0';

	for(int i = sheet[row-1].row_size-1; i > position ;i--)
		sheet[row-1].one_row[i] = sheet[row-1].one_row[i-1];
	return 0;
}
/*
If i want put some new content to cell. I can resize it
Function automaticly do row -1;
*/
int sheet_row_realoc(row *sheet, int row, int space)
{
	space = space;
	row = row -1;
	sheet[row].one_row = realloc(sheet[row].one_row, (sheet[row].row_size + space) * sizeof(char));
	if(sheet[row].one_row == NULL)
		return error_maloc(ERROR_VALUE_ONE);
	sheet[row].row_size = (sheet[row].row_size + space);
	return 0;
}
char get_last_char(char *aray)
{
	char last_char = '\0';
	for(int i = 0; aray[i] != '\0'; i++)
		last_char = aray[i];
	return last_char;
}
/*
//i is number of row 
it just count cels in row.data 
y = change values       1
n = just count them 	0
*/
int count_cells_in_row(int i, row *sheet, char separator)
{
	int cells = 0;
	bool q_mark = false;
	for(int j = 0; j < sheet[i].row_size ; j++)
	{
		if(sheet[i].one_row[j] == '\"')
		{
			if(q_mark == true)
			{
				if(sheet[i].one_row[j-1] != '\\')
					q_mark = false;
			}
			else
			{
				if(j > 0)
					if(sheet[i].one_row[j-1] != '\\')
						q_mark = true;
			}
		}

		if(sheet[i].one_row[j] == separator)
		{
			if(q_mark == false)
			{
				cells = cells + 1;
				if(j > 0)
				{
					if(sheet[i].one_row[j-1] == '\\')
					{
						cells--;
					}
					else
						continue;
				}
			}
		}
	}
	sheet[i].cels_in_row = cells;
	return cells;
}
int get_array_size(char *array, int max_size)
{
	int size = 0;
	for(; size < max_size; size++)
		if(array[size] == '\0')
			break;
	return size;
}
int move_rows_up(row *sheet, int row, int *row_counter)
{
	for(int i = row; i < *row_counter;i++)
	{
		sheet[i-1].cels_in_row = sheet[i].cels_in_row;
		sheet[i-1].one_row = sheet[i].one_row;
		sheet[i-1].row_size = sheet[i].row_size;
	}
	*row_counter = *row_counter -1;
	return 0;
}
int move_rows_down(row *sheet, int row, int *row_counter, char separator)
{
	int c = sheet[0].cels_in_row;
	sheet[*row_counter].one_row = NULL;
	sheet[*row_counter].cels_in_row = 0;
	sheet[*row_counter].row_size = 0;
	int i = *row_counter;
	for(; i >= row; i--)
	{
		sheet[i].one_row = sheet[i-1].one_row;
		sheet[i].row_size = sheet[i-1].row_size;
		sheet[i].cels_in_row = sheet[i-1].cels_in_row;
	}
	sheet[i].one_row = malloc(c+SPACE_FOR_NEWLINE * sizeof(char));
	if(sheet[i].one_row == NULL)
		return error_maloc(ERROR_VALUE_ONE);
	sheet[i].cels_in_row = c;
	sheet[i].row_size = c+SPACE_FOR_NEWLINE;
	for(int j = 0; j < c; j++)
		sheet[i].one_row[j] = separator;
    sheet[i].one_row[c] = '\n';
	sheet[i].one_row[c+1] = '\0';

	*row_counter = *row_counter + 1;
	return 0;
}
//This fun can be used if u want to move all rows in indexation right 
/*
It ll put row up the choosen rows OR It ll put row under the choosen row.
irow 1			up
arow 2 			under
*/
int s_e_irow_arow(row *sheet, int row_from, int row_to, int cell_from, int cell_to, char separator, int *row_counter, int irow_arow)
{
	int error = 0;
	for(int j = row_from; j <= row_to; j++)
	{
		for(int i = cell_from; i <= cell_to; i++)	
		{
			if(irow_arow == AROW)
			{					
				error = move_rows_down(sheet, j+1, row_counter, separator);
				if(error == -1)
					return -1;
			}
			else
			{
				error = move_rows_down(sheet, j, row_counter, separator);
				if(error == -1)
					return -1;
			}
		}
	}
	return 1;
}
/*
It delete choosen rows. 
*/
int s_e_drow(row *sheet, int row_from,  int row_to, int *row_counter)
{
	for(int i = row_to -1; i >= row_from-1; i--)
	{
		free(sheet[i].one_row);
		sheet[i].row_size = 0;
		sheet[i].one_row = NULL;
		sheet[i].cels_in_row = 0;
		move_rows_up(sheet, i+1, row_counter);
	}
	return 1;
}
/*
It ll add column right from every chosen cell. 
It ll add column left from every chosen cell. 
icol = 1
acol = 2
*/
int s_e_icol_acol(row *sheet,int cell, char separator, int *row_counter,int icol_acol)
{
	if(icol_acol == ICOL)
		cell--;
	int position = 0;
	for(int i = 0; i < *row_counter;i++)
	{
		if(cell != sheet[i].cels_in_row)
			row_move_right(sheet,i+1, cell+1, 1, separator, ICOL);
		
		position = get_cell_position(sheet, i+1, cell+1, separator);		
		sheet[i].one_row[position] = separator;
		sheet[i].cels_in_row = sheet[i].cels_in_row + 1;
	}
	return 1;
}
int store_row(row *sheet, char *store_to, int row)
{
	for(int i = 0; i < sheet[row-1].row_size; i++)
		store_to[i] = sheet[row-1].one_row[i];	
	return 0;
}
/*
It ll delete every chosen column.
*/
int s_e_dcol(row *sheet, int cell, char separator, int *row_counter)
{
	int position = 0, x = 0;
	char *help = NULL;
	for(int i = 0; i < *row_counter;i++)
	{
		help = malloc(sheet[i].row_size * sizeof(char));
		if(help == NULL)
			return error_maloc(ERROR_VALUE_ONE);
		array_char_init(help, sheet[i].row_size);
		store_row(sheet, help, i+1);
		position = get_cell_position(sheet, i+1, cell, separator);
		if(cell > 1)
		{				
			position++;
			if(cell == sheet[i].cels_in_row+1)
				help[position] = '\0';
		}
		for(int j = position; j < sheet[i].row_size;j++)
		{
			if(help[j] == separator)	
			{
				if(j > 0)
				{
					if(help[j-1] != '\\')
					{
						help[j] = '\0';	
						break;
					}
				}
				help[j] = '\0';	
				break;
			}
			if(help[j] == '\n')
				break;
			help[j] = '\0';	
		}
		sheet[i].cels_in_row = sheet[i].cels_in_row -1;
		for(int k = 0; k < sheet[i].row_size; k++)
		{
			if(help[k] == '\0')
				continue;
			else
				sheet[i].one_row[x++] = help[k];
		}
		sheet[i].row_size = x;
		x = 0;
		free(help);
	}
	return 1;
}
/*if it found data sturctu command it call it ll be executed*/
//error = -1 bad syntax; 0 = command not found; 1 = command_executed 
int sheet_edit(row *sheet, char *single_command,int *row_from, int *row_to, int *cell_to, int *cell_from, char separator, int *row_counter)
{
	if(strcmp(single_command, "irow")==0) 
		return s_e_irow_arow(sheet, *row_from, *row_to, *cell_from, *cell_to, separator, row_counter, IROW);
	else if(strcmp(single_command, "arow")==0) 
		return s_e_irow_arow(sheet, *row_from, *row_to, *cell_from, *cell_to, separator, row_counter, AROW);
	else if(strcmp(single_command, "drow")==0) 
		return s_e_drow(sheet, *row_from, *row_to,  row_counter);
	else if(strcmp(single_command, "icol")==0) 
		return s_e_icol_acol(sheet, *cell_to, separator, row_counter, ICOL);
	else if(strcmp(single_command, "acol")==0) 
		return s_e_icol_acol(sheet, *cell_to, separator, row_counter, ACOL);
	else if(strcmp(single_command, "dcol")==0) 
		return s_e_dcol(sheet, *cell_to, separator, row_counter);
	return  0;
}
/*
i call this function when i know that selection ll have this format 	
TYPE1		TYPE2
[R,C] || [R1,R2,C1,C2]
*/
int selec_changer_usual(char *single_command, int *h_rf, int *h_rt, int *h_cf, int *h_ct, int type, row *sheet, int row_counter)
{
	int j = 1, k=0;
	char num1[MAX_COMMAND_SIZE], num2[MAX_COMMAND_SIZE], num3[MAX_COMMAND_SIZE], num4[MAX_COMMAND_SIZE];
	array_char_init(num1,MAX_COMMAND_SIZE);
	array_char_init(num2,MAX_COMMAND_SIZE);
	array_char_init(num3,MAX_COMMAND_SIZE);
	array_char_init(num4,MAX_COMMAND_SIZE);
	if(type == SHORT_FORMAT) //[R,C]
	{			
		if(single_command[j] == '_')   // [_,C]
		{
			*h_rf = 1;
			*h_rt = row_counter; 
			for(j = j+2; isdigit(single_command[j]) != 0;j++)
				num1[k++] = single_command[j];
			k++;
			num1[k] = '\0';
			*h_ct = *h_cf = atoi(num1);
		}
		else  // [R,_]  or [R,C]
		{
			for(; isdigit(single_command[j]) != 0;j++)
				num1[k++] = single_command[j];				
			k++;
			num1[k] = '\0';
			
			*h_rt = *h_rf = atoi(num1);
			k = 0;
			if(single_command[j+1] == '_') 
			{					
				*h_cf = 1;
				*h_ct = (sheet[0].cels_in_row + 1);
			}
			else
			{
				for(j=j+1; isdigit(single_command[j]) != 0; j++)
					num2[k++] = single_command[j];
				k++;
				num1[k] = '\0';
				*h_cf = *h_ct = atoi(num2);
			}
		}
		return 1;
	}
	else  //[R1,C1,R2,C2]
	{
		for(; isdigit(single_command[j]) != 0; j++)   			//R1
			num1[k++] = single_command[j];
		k = 0;
		*h_rf = atoi(num1);
	
		for(j = j+1; isdigit(single_command[j]) != 0; j++)  	//C1
			num2[k++] = single_command[j];
		k = 0;
		*h_cf = atoi(num2);

		if(single_command[j+1] == '-')							//R2
		{
			*h_rt = row_counter;
			j = j + NEXT_NUM_POSITION;
		}
		else
		{
			for(j = j+1; isdigit(single_command[j]) != 0; j++)				
				num3[k++] = single_command[j];
			k = 0;
			*h_rt = atoi(num3);
			j++;
		}

		if(single_command[j] == '-')							//R2
			*h_ct = sheet[0].cels_in_row + 1 ;
		else
		{
			for(j = j; isdigit(single_command[j]) != 0; j++)
              	  num4[k++] = single_command[j];
	
			*h_ct = atoi(num4);
		}
		return 1;
	}
	fprintf(stderr, "ERROR cant parse comand type [R,C] or [R1,R2,R3,R4]");
	return 0;
}
void array_char_init(char *array, int size)
{
	for(int i = 0; i < size; i++)
		array[i] = 0;
}
/*
It ll get position where does cell start. 
just right after separator
if u want row 1 teel function row 1 it ll automaticly find row[0]
*/
int get_cell_position(row *sheet, int row, int cell, char separator)
{
	int counter_separator = 0;
	bool q_mark = false;	
	if(cell == 1)
		return 0;
	//if u want to know position before \n
	if(cell > sheet[row-1].cels_in_row +1)
	{
		int i = 0;
		while(sheet[row-1].one_row[i] != '\n')
			i++;
		return i-1;
	}
	int i =0;
	for(; i < sheet[row-1].row_size; i++)
	{
		if(sheet[row-1].one_row[i] == '\"')
		{
			if(q_mark == true)
			{
				if(sheet[row-1].one_row[i-1] != '\\')
					q_mark = false;
			}
			else
			{
				if(i > 0)
					if(sheet[row-1].one_row[i-1] != '\\')
						q_mark = true;
			}
		}
		if(sheet[row-1].one_row[i] == separator)
		{	
			if(q_mark == false)
			{
				if(i > 0)
				{
					if(sheet[row-1].one_row[i-1] != '\\')
					{
						counter_separator++;
						if(counter_separator == cell-1)
							break;
					}
				}
				else
				{
					counter_separator++;
					if(counter_separator == cell-1)
						break;
				}
			}			
		}
	}
	return i-1;
}
/*
just store one cell to *char and end with \0
Warining it store it without separators and \n
*/
int store_one_cell(char *store, row *sheet, int row, int cell, char separator)
{
	int k = 0;
	int position = 0;
	position = get_cell_position(sheet, row, cell, separator);
	if(cell > 1)
	{
		if(cell == sheet[row-1].cels_in_row+1)
			position = position + POSITION_FIX;
		else
			position = position + POSITION_FIX;
	}
	for(;position < sheet[row-1].row_size ;position++)
	{
		store[k++] = sheet[row-1].one_row[position];
		if(sheet[row-1].one_row[position] == '\n')
		{
			k--;
            break;
		}
		if(sheet[row-1].one_row[position] == separator)
		{
			if (position > 0)
			{
				if(sheet[row-1].one_row[position-1] != '\\')
					break;
			}
			else
				break;
		}
	}
	store[k+1] = '\0';
	return 0;
}
/*
it ll find cell where is a biggest or smalest number in selection and them it ll change selection to that cell
operation 1 = min
operation 2 = max
*/
int select_min_max(row *sheet, int *row_from, int *row_to, int *cell_from, int *cell_to, char separator, int operation)
{
	int help = 0, result = 0;
	char *hellper = NULL;
	bool firs_time = false;
	int h_cf = *cell_from, h_rf = *row_from;
	
	for(int i = *row_from-1; i < *row_to; i++)
	{
		for(int j = *cell_from -1; j <= *cell_to+1;j++)
		{
			hellper = malloc(sheet[i].row_size * sizeof(char));
			if(hellper == NULL)
				return error_maloc(ERROR_VALUE_ONE);
			array_char_init(hellper, sheet[i].row_size);
			store_one_cell(hellper, sheet, i+1, j+1, separator);
			if(isdigit(hellper[0]) != 0)
			{
				help = atoi(hellper);
				if(firs_time == false)
				{
					result = help;
					firs_time = true;
					h_rf = i + 1;
					h_cf = j + 1;
				}
				if(operation == MIN)//min
				{
					if(help < result)
					{
						result = help;
						h_rf = i + 1;
						h_cf = j + 1;
					}
				}
				if(operation == MAX)
				{
					if(help > result)
					{
						result = help;
						h_rf = i + 1;
						h_cf = j + 1;
					}
				}
			}
			free(hellper);	
		}
	}
	if(firs_time == false)
	{
		*row_to = *row_from;
		*cell_to = *cell_from;
	}
	else
	{
		*row_to = *row_from = h_rf;
		*cell_to = *cell_from = h_cf;
	}
	
	return result;
}
/*
[find STR]
it ll find string in sheet in alredy existing selection and set that cell as new selection 
*/
int select_find(row *sheet, char *single_command, int *row_from, int *row_to, int *cell_from, int *cell_to, char separator )
{
	char help[MAX_COMMAND_SIZE];
	int k = 0;
	char one_cell[MAX_COMMAND_SIZE];
	for(int i = FIND_COM_STRING_START;single_command[i] != ']' && single_command[i] != '\0';i++)
		help[k++] = single_command[i];
	help[k] = '\0';
	//in selection find first cell where it is stored 
	for(int i = *row_from-1; i < *row_to; i++)
	{
		for(int j = *cell_from -1; j < *cell_to;j++)
		{
			store_one_cell(one_cell, sheet, i+1, j+1, separator);
			if((strstr(one_cell, help)) != NULL)
			{
				*row_to = *row_from = i + 1;
				*cell_from = *cell_to = j +1;
				return 1;
			}
			array_char_init(one_cell, MAX_COMMAND_SIZE);
		}
	}
	return 0;
}
/*
it ll change table select 
If the select is bigger them table it ll make it bigerr 
[R,C], [R1,R2,C1,C2],[_,C], [_,_], [min], [max], [find STR], [_]
*/
int select_change_simplify(row *sheet, char *single_command, int *row_from, int *row_to, int *cell_from, int *cell_to, char separator, int *row_counter, int *temp_10)
{
	int commas = 0;
	// i ll store numbers to this arrays and them parse it to numbers 
	bool command_executed = false;
	int h_rf = *row_from, h_rt = *row_to, h_cf = *cell_from, h_ct = *cell_to;   //h mean help 
	if(strcmp(single_command, "[_,_]")==0)  //[_,_] //all table select 
	{
		h_rf = 1;
		h_rt = *row_counter;
		h_cf = 1;
		h_ct = sheet[0].cels_in_row + 1;
		command_executed = true;
	}
	else if(strcmp(single_command, "[min]")==0)  //[min]
	{
		select_min_max(sheet, &h_rf, &h_rt, &h_cf, &h_ct, separator, MIN);
		command_executed = true;
	}
	else if(strcmp(single_command, "[max]")== 0) //[max]
	{
		select_min_max(sheet, &h_rf, &h_rt, &h_cf, &h_ct, separator, MAX);
		command_executed = true;
	}
	else if(strcmp(single_command, "[_]")== 0)  //[_]
	{
		h_rf = h_rt = temp_10[0];
		h_cf = h_ct = temp_10[1];
		command_executed = true;
	}
	else if(strstr(single_command, "[find") != NULL) //
	{
		select_find(sheet, single_command, &h_rf, &h_rt, &h_cf, &h_ct, separator);
		command_executed = true;
	}
	else
	{
		for(int i = 0; single_command[i] != '\0';i++)
			if(single_command[i] == ',')
				commas++;
		
		if(commas == SHORT_FORMAT_COMMAS) //[R,C]
		{
			if(selec_changer_usual(single_command, &h_rf, &h_rt, &h_cf, &h_ct, SHORT_FORMAT, sheet, *row_counter ) == 1) 
				command_executed = true;
		}
		else if(commas == LONG_FOMRAT_COMMAS)   //[R1,R2,C1,C2]
		{
			if(selec_changer_usual(single_command, &h_rf, &h_rt, &h_cf, &h_ct, LONG_FORMAT, sheet, *row_counter) == 1)
				command_executed = true;
		}
		else
		{
			fprintf(stderr, "ERROR you have to bad format if you want to select it has to be [R,C] or [R1,R2,C1,C2]\n");
			return -1;
		}
	}
	if(command_executed == 1)
	{

		if((h_rt < 1) || (h_ct < 1) || (h_rf < 1) || (h_ct < 1))
		{
			fprintf(stderr, "Error chosen row_or column can`t be < 1 \n");
			return -1;
		}
		else if ((h_rt < h_rf) || (h_ct < h_cf))
		{
			fprintf(stderr, "Error  R1 < R2  &&   C1 <  C2 \n");
			return -1;
		}
		if(h_rt > *row_counter)
		{
			while (h_rf > *row_counter)
				s_e_irow_arow(sheet, *row_counter, *row_counter, *cell_from, *cell_from, separator, row_counter, AROW);	
		
		}
		if(h_ct > sheet[0].cels_in_row+1)
		{
			while (h_ct > sheet[0].cels_in_row+1)
				s_e_icol_acol(sheet, sheet[0].cels_in_row+1, separator, row_counter, ACOL);
		}
		*row_from = h_rf;
		*row_to = h_rt;
		*cell_from = h_cf;
		*cell_to = h_ct;
		return 1;	
	}
	return 0;
}
/*if it found selection command it call it ll be executed*/
int select_change(row *sheet, char *single_command, int *row_from, int *row_to, int *cell_from, int *cell_to, char last_character, char separator, int *row_counter, int *temp_10)
{
	if(strcmp(single_command, "[set]") == 0)  //[set]
		return 0;
	if(single_command[0] == '[')
	{
		if(last_character == ']')
			return select_change_simplify(sheet, single_command, row_from, row_to, cell_from, cell_to, separator,row_counter, temp_10);
		else
			fprintf(stderr, "Error if command start with [ it has to ned with ]");
	}
	return 0;
}
/*
It ll store string to cell. 
*/
int change_cell_value(row *sheet, int row, int cell, char separator, char *value,int size)
{
	int position = get_cell_position(sheet ,row, cell , separator);
	if(cell > 1)
	{
		if(cell == sheet[row-1].cels_in_row+1)
			position = position + POSITION_FIX;
		else
			position = position + POSITION_FIX;
	}
	delete_cell_value(sheet, row, cell, separator);
	row_move_right(sheet, row, cell, size, separator, NOT_ICOL_ACOL);
	for(int i = 0; i < size ;i++)
	{
		if(value[i] == '\0')
			break;
		sheet[row-1].one_row[position] = value[i];
		position = position +1 ;
	}
	return 0;
}
int delete_cell_value(row *sheet, int row, int cell, char separator)
{
	int position = get_cell_position(sheet, row, cell, separator);
	if(cell > 1)
		position = position + POSITION_FIX;

	// if cell is empty
	if(sheet[row-1].one_row[position] == separator)
		return 0;
	if(sheet[row-1].one_row[position] == '\n')
		return 0;
	int size = get_cell_size(sheet, separator, cell, row);
	int i = position;
	char help[sheet[row-1].row_size];
	if(size == 1)
		return 0;
	array_char_init(help, sheet[row-1].row_size);
	for(; sheet[row-1].one_row[i] != '\n';i++)
	{
		if(sheet[row-1].one_row[i] == separator)
			if(sheet[row-1].one_row[i-1] != '\\')
				break;
		sheet[row-1].one_row[i] = '\0';
	}
	int x = 0, j = 0;
	for(; j < sheet[row-1].row_size;)
	{
		if(sheet[row-1].one_row[x] == '\0')
			x = x + 1;
		else if(sheet[row-1].one_row[x] == '\n')
		{
			help[j++] = sheet[row-1].one_row[x++];
			break;
		}
		else
			help[j++] = sheet[row-1].one_row[x++];
	}
	j++;
	sheet[row-1].row_size = j+1;
	sheet[row-1].one_row = realloc(sheet[row-1].one_row ,j+1 * sizeof(char));
	if(sheet[row-1].one_row == NULL)
		return error_maloc(ERROR_VALUE_ONE);
	for(int i =0; i < j+1; i++)
		sheet[row-1].one_row[i] = help[i];	
	return 0;
}
/*
set STR 
just set choosen cells content to STR 
*/
int c_e_clear(row *sheet, int r_f, int r_t, int c_f, int c_t, char separator)
{
	for(int j = r_f; j < r_t+1; j++)
	{
		for(int i = c_f; i < c_t+1; i++)
		{
			delete_cell_value(sheet, j, i, separator);
		}
	}

	return 1;
}
/*
swap [R,C]
*/
int c_e_swap(row *sheet, char *single_command, char separator, int row, int cell, int row_counter)
{
	char num_one[MAX_COMMAND_SIZE], num_two[MAX_COMMAND_SIZE];
    array_char_init(num_one, MAX_COMMAND_SIZE);
	array_char_init(num_two, MAX_COMMAND_SIZE);
	int k =0, i = NUMBER_IS_LOCATED +1, swap_row = 0, swap_cell = 0;

	for(; single_command[i] != ',' ;i++)
	{
		num_one[k++] = single_command[i];
		if(isdigit(single_command[i]) == 0)		
			return error_maloc(ERROR_VALUE_ONE);
	}
	i++;
	k = 0;
	for(;single_command[i] != ']';i++)
	{
		num_two[k++] = single_command[i];
		if(isdigit(single_command[i]) == 0)		
		{
			fprintf(stderr, "ERROR swap [R,C] C is not an number ");
			return -1;
		}
	}
	swap_row = atoi(num_one);
	swap_cell = atoi(num_two);
	if(swap_row > row_counter || swap_cell > sheet[row-1].cels_in_row+1)
	{
		fprintf(stderr, "Error u cant swap ");
		return -1;
	}
	char *phelp = NULL, *phelp_two = NULL;
	phelp = malloc((sheet[swap_row-1].row_size) * sizeof(char));
	if(phelp == NULL)
		return error_maloc(ERROR_VALUE_ONE);
	phelp_two = malloc((sheet[row-1].row_size) * sizeof(char));
	if(phelp_two == NULL)
		return error_maloc(ERROR_VALUE_ONE);

	int cs = 0, cs_two = 0;

	cs = get_cell_size(sheet, separator, swap_cell, swap_row);
	cs_two = get_cell_size(sheet, separator, cell, row);

	array_char_init(phelp, sheet[swap_row-1].row_size);
	array_char_init(phelp_two, sheet[row-1].row_size);
	
	store_one_cell(phelp, sheet, swap_row, swap_cell, separator);
	store_one_cell(phelp_two, sheet, row, cell, separator);
	
	change_cell_value(sheet, swap_row, swap_cell, separator, phelp_two, cs);
	change_cell_value(sheet, row, cell, separator, phelp, cs_two);

	free(phelp_two);
	free(phelp);
	return 1;
}
//WARNING IT LL RETURN SIZE WITHOUT SEPARATOR AND \n
int get_cell_size(row *sheet, char separator, int cell, int row)
{
	int i =0;
	int position = 0;
	position = get_cell_position(sheet, row, cell, separator);

	if(cell > 1)
	{
		if(cell ==  sheet[row-1].cels_in_row +1)
			position = position + POSITION_FIX -1;
		else
			position = position + POSITION_FIX;
	}

	bool first_cell = false;
	if(position == 0)
	{
		first_cell = true;
		position++;
	}
	if(sheet[row-1].one_row[position] == '\n')
		return 0;
	if(sheet[row-1].one_row[position] == separator)
		return 0;
	for(i = position; sheet[row-1].one_row[i] != separator && sheet[row-1].one_row[i-1] != '\\';i++)
	{
		if(sheet[row-1].one_row[i] == '\n')
			break;
	}
	if(first_cell == false)
		return i - position  ;
	else
		return i - position + 1 ;
}
int c_e_set(row *sheet, char *single_command, int r_f, int r_t, int c_f, int c_t, char separator)
{
	char help[MAX_COMMAND_SIZE];
	int position = 0, str_size = 0, x = 0;

	array_char_init(help, MAX_COMMAND_SIZE);
	for(int i = STRING_START_POSITION; single_command[i] != '\0';i++)
	{
		help[x++] = single_command[i];
		str_size++;
	}
	x++;
	help[x] = '\0';
	for(int j = r_f; j < r_t+1; j++)
	{
		for(int i = c_f; i < c_t+1; i++)
		{
			x = 0;
			position = get_cell_position(sheet, j, i, separator);
			if(i > 1)
				position = position + POSITION_FIX;
			
			delete_cell_value(sheet, j, i, separator);
			row_move_right(sheet,j, i, str_size, separator, NOT_ICOL_ACOL);	
			for(int k = position; k < (position + str_size) ;k++)
				sheet[j-1].one_row[k] = help[x++];
		}
	}
	return 1;
}
/*
IF command has something like swap [R,C]
This function ll find that cell 
*/
int get_sa_cell(char *single_command, int *row, int *cell, bool count)
{
	int position = NUMBER_IS_LOCATED;
	if(count == true)
		position = position +2;

	char num_one[MAX_COMMAND_SIZE], num_two[MAX_COMMAND_SIZE];
	array_char_init(num_one, MAX_COMMAND_SIZE);
	array_char_init(num_two, MAX_COMMAND_SIZE);
	int i = position;
	int k = 0;

	for(; single_command[i] != ',' ;i++)
	{
		num_one[k++] = single_command[i];
		if(isdigit(single_command[i]) == 0)		
			return error_syntax(ERROR_VALUE_ONE);
	}
	i++;
	k = 0;
	for(;single_command[i] != ']';i++)
	{
		num_two[k++] = single_command[i];
		if(isdigit(single_command[i]) == 0)		
			return error_syntax(ERROR_VALUE_ONE);
	}
	*row = atoi(num_one);
	*cell = atoi(num_two);
	return 0;
}
/*
It count sum or avreage of choosen cell 
*/
int c_e_sum_avg(row *sheet, char *single_command ,int r_f, int r_t, int c_f, int c_t, char separator, int row_counter, int avg_sum)
{
	char *help = NULL, *help_two = NULL;
	float count = 0.00;
	int num = 0;
	int sum = 0;
	float f_sum = 0.00;
	bool is_num = true;
	int swap_row = 0, swap_cell = 0;
	int error = 0;

	error = get_sa_cell(single_command, &swap_row, &swap_cell, false);
	if(error == -1)
		return -1;
	
	if(swap_row > row_counter || swap_cell > sheet[0].cels_in_row +1)
		return error_syntax(ERROR_VALUE_ONE);

	for(int k = r_f; k < r_t +1; k++)
	{
		help = malloc(sheet[k-1].row_size * sizeof(char));
		if(help == NULL)
			error_maloc(ERROR_VALUE_ONE);
		array_char_init(help, sheet[k-1].row_size);

		for(int j = c_f; j < c_t + 1 ; j++ )
		{
			array_char_init(help, sheet[k-1].row_size);
			store_one_cell(help, sheet, k, j, separator);
			if(isdigit(help[0]) == 0)
				is_num = false;
			if(is_num == true)
			{
				count = count + 1;
				num = atoi(help);
			}
			sum = sum + num;	
		}
		free(help);
		is_num = true;
	}
	f_sum = (float)sum;
	if(avg_sum == SUM)
		f_sum = f_sum;
	else
		f_sum = (f_sum / count);
	
	help = malloc(sheet[0].row_size + sheet[1].row_size * sizeof(char));
	if(help == NULL)
		return error_maloc(ERROR_VALUE_ONE);
	help_two = malloc(sheet[0].row_size + sheet[1].row_size * sizeof(char));
	if(help_two == NULL)
		return error_maloc(ERROR_VALUE_ONE);
	array_char_init(help, sheet[0].row_size+ sheet[1].row_size);
	array_char_init(help_two, sheet[0].row_size+ sheet[1].row_size);
	sprintf(help, " %.1f ", f_sum);
	int size = 0, k = 0;
	for(;help[size] != '\0';)
		size++;
	for(int i =0; i < sheet[1].row_size; i++)
	{
		if(help[i] == ' ')
			continue;
		help_two[k++] = help[i];
	}
	for(int i =0; i < sheet[1].row_size;i++)
		if(help_two[i] == '.')
			if(help_two[i+1] == '0')
			{
				help_two[i] = '\0';
				break;
			}
	change_cell_value(sheet, swap_row, swap_cell, separator, help_two, size );
	free(help);
	free(help_two);
	return 1;
}
/*
To chosen cell store selection cell lenght. Ot it can count no empty values. 
*/
int c_e_len_count(row *sheet, char *single_command, int r_f, int r_t, int c_f, int c_t, char separator, int row_counter, int len_count)
{
	char *help = NULL, *help_two = NULL;
	int num = 0;
	int sum = 0;
	float f_sum = 0.00;
	int swap_row = 0, swap_cell = 0;
	int error = 0;

	if(len_count == COUNT)
		error = get_sa_cell(single_command, &swap_row, &swap_cell, true);
	else
		error = get_sa_cell(single_command, &swap_row, &swap_cell, false);
	if(error == -1)
		return -1;
	if(swap_row > row_counter || swap_cell > sheet[0].cels_in_row +1)
		return error_syntax(ERROR_VALUE_ONE);
	for(int k = r_f; k < r_t +1; k++)
	{
		help = malloc(sheet[k-1].row_size * sizeof(char));
		if(help == NULL)
			error_maloc(ERROR_VALUE_ONE);
		array_char_init(help, sheet[k-1].row_size);

		for(int j = c_f; j < c_t + 1 ; j++ )
		{
			array_char_init(help, sheet[k-1].row_size);
			if(len_count == LEN)
			{
				num = get_cell_position(sheet, k, j, separator);
				if(sheet[k-1].one_row[num] == '\n' || sheet[k-1].one_row[num] == separator)
					num = 0;
				else
					num = 1;	
			}
			else
			{
				num = get_cell_position(sheet, k, j, separator);
				for(int p = num; p < sheet[k-1].row_size; p++ )
				{
					if(sheet[k-1].one_row[p] == '\n')
						num = p - num;
					if(sheet[k-1].one_row[p] == separator)
						num = p - num;
				}
			}
		
			sum = sum + num;
		}
		free(help);
	}

	f_sum = (float)sum;
	help = malloc(sheet[0].row_size + sheet[1].row_size * sizeof(char));
	if(help == NULL)
		return error_maloc(ERROR_VALUE_ONE);
	help_two = malloc(sheet[0].row_size + sheet[1].row_size * sizeof(char));
	if(help_two == NULL)
		return error_maloc(ERROR_VALUE_ONE);
	array_char_init(help, sheet[0].row_size+ sheet[1].row_size);
	array_char_init(help_two, sheet[0].row_size+ sheet[1].row_size);
	sprintf(help, " %.1f ", f_sum);

	int size = 0, k = 0;
	for(;help[size] != '\0';)
		size++;
	for(int i =0; i < sheet[1].row_size; i++)
	{
		if(help[i] == ' ')
			continue;
		help_two[k++] = help[i];
	}
	for(int i =0; i < sheet[1].row_size;i++)
		if(help_two[i] == '.')
			if(help_two[i+1] == '0')
			{
				help_two[i] = '\0';
				break;
			}

	change_cell_value(sheet, swap_row, swap_cell, separator, help_two, size );
	free(help);
	free(help_two);
	return 1;
}
/*if it found command for editing sheet content it call it ll be executed*/
int content_edit(row *sheet, char *single_command, int *row_from, int *row_to, int *cell_from, int *cell_to, char last_character, char separator, int row_counter)
{
	if(strstr(single_command, "set") != NULL)  //set STR 
	{
		if(single_command[0] == '[')
			return 0;
		else
		return c_e_set(sheet, single_command, *row_from, *row_to, *cell_from, *cell_to, separator);
	}
	if(strstr(single_command, "clear") != NULL) //clear
		return c_e_clear(sheet, *row_from, *row_to, *cell_from, *cell_to, separator);
	if(strstr(single_command, "swap") != NULL) //swap [R,C] 
	{
		if(single_command[BRACKET_POSITION] == '[')
			if(last_character == ']')
				return c_e_swap(sheet, single_command, separator, *row_to, *cell_to, row_counter);
			else
				return error_syntax(ERROR_VALUE_ONE);
		else
			return error_syntax(ERROR_VALUE_ONE);
	}
	if(strstr(single_command, "sum") != NULL) //sum [R,C]
	{
		if(single_command[4] == '[')
		{
			if(last_character == ']')
				return c_e_sum_avg(sheet, single_command , *row_from, *row_to, *cell_from, *cell_to, separator, row_counter, SUM);
			else
				return error_syntax(ERROR_VALUE_ONE);
		}
		else
			return error_syntax(ERROR_VALUE_ONE);
	}
	if(strstr(single_command, "avg") != NULL)  //avg [R,C]
	{
		if(single_command[4] == '[')
		{
			if(last_character == ']')
				return c_e_sum_avg(sheet, single_command , *row_from, *row_to, *cell_from, *cell_to, separator, row_counter, AVG);
			else
				return error_syntax(ERROR_VALUE_ONE);
		}
		else
			return error_syntax(ERROR_VALUE_ONE);
	}
	if(strstr(single_command, "count") != NULL) //count [R,C]
	{
		if(single_command[6] == '[')
		{
			if(last_character == ']')
				return c_e_len_count(sheet, single_command, *row_from, *row_to, *cell_from, *cell_to, separator, row_counter, COUNT);
			else
				return 0;
		}
		else
			return error_syntax(ERROR_VALUE_ONE);
	}
	if(strstr(single_command, "len") != NULL) //len [R,C]
	{
		if(single_command[4] == '[')
		{
			if(last_character == ']')
				return c_e_len_count(sheet, single_command, *row_from, *row_to, *cell_from, *cell_to, separator, row_counter, LEN);
			else
				return 0;
		}
		else
			return error_syntax(ERROR_VALUE_ONE);
	}
	return 0;
}
/*Value of cell ll be stored to temp var _X. X can be num 0 9
def _X 
*/
int temp_def(row *sheet, char *single_command, int row, int cell, char temp_vars[TEMPO_VARS_MAX][TEMPO_VARS_LENGHT], char separator)
{
	char help[TEMPO_VARS_LENGHT];
	array_char_init(help, TEMPO_VARS_LENGHT);
	char help_num[2];
	help_num[0] = '0', help_num[1] = '\0';
	int tempo_var_num = 0;
	store_one_cell(help, sheet, row, cell, separator);
	if(isdigit(single_command[NUMBER_IS_LOCATED]) != 0)
	{
		help_num[0] = single_command[NUMBER_IS_LOCATED];
		tempo_var_num = atoi(help_num);
	}
	else
		return error_syntax(ERROR_VALUE_ONE);
	for(int i = 0; i < TEMPO_VARS_LENGHT ; i++)
	{
		if(help[i] == '\0')
		{				
			temp_vars[tempo_var_num][i] = help[i];
			break;
		}
		temp_vars[tempo_var_num][i] = help[i];
	}
	for(int i = 0; i < TEMPO_VARS_LENGHT ; i++)
	{
		if(temp_vars[tempo_var_num][i] == '\0')
			break;
	}
	return 1;
}
/*Actual cell ll be stored to X_10 temporarily var that is reserved for selections */
int temp_set(int row_to, int cell_to, int *temp_10)
{
	temp_10[0] = row_to;
	temp_10[1] = cell_to;
	return 1;
}
/*
Tempo variable ll be used and set to some cell 
use _X
*/
int temp_use(row *sheet, char *single_command, int row, int cell, char tempo_vars[TEMPO_VARS_MAX][TEMPO_VARS_LENGHT], char separator)
{
	int tempo_var_num = 0, size = 0;
	char help_num[2];
	help_num[0] = '0', help_num[1] = '\0';
	char help[TEMPO_VARS_LENGHT];
	if(isdigit(single_command[NUMBER_IS_LOCATED]) != 0)
	{
		help_num[0] = single_command[NUMBER_IS_LOCATED];
		tempo_var_num = atoi(help_num);
	}
	else
		return error_syntax(ERROR_VALUE_ONE);
	//For empty variable 
	if(tempo_vars[tempo_var_num][0] == '\0')
		return 1;
	for(int i = 0; i < TEMPO_VARS_LENGHT;i++)
	{
		size++;
		help[i] = tempo_vars[tempo_var_num][i];
		if(tempo_vars[tempo_var_num][i] == '\0')
			break;
	}
	change_cell_value(sheet,row,cell,separator, help, size);
	return 1;
}
/*
Num value that is stored in temp def ll be increased by one 
*/
int temp_inc(char *single_command, char tempo_vars[TEMPO_VARS_MAX][TEMPO_VARS_LENGHT])
{
	bool is_num = true;
	int temp_var_num = 0, num = 0;
	char help_string[TEMPO_VARS_LENGHT];
	array_char_init(help_string, TEMPO_VARS_LENGHT);
	char help_num[2];
	help_num[0] = '0', help_num[1] = '\0';
	
	if(isdigit(single_command[NUMBER_IS_LOCATED]) != 0)
	{
		help_num[0] = single_command[NUMBER_IS_LOCATED];
		temp_var_num = atoi(help_num);
	}
	else	
		return error_syntax(ERROR_VALUE_ONE);
	
	for(int i = 0; tempo_vars[temp_var_num][i] != '\0' ; i++)
		if(isdigit(tempo_vars[temp_var_num][i] == 0 ))
			is_num = false;
	
	if(is_num == true)
	{
		num = atoi(tempo_vars[temp_var_num]);
		num++;
		array_char_init(tempo_vars[temp_var_num], TEMPO_VARS_LENGHT);
		sprintf(help_string, "%d", num);
		int i = 0;
		for(; i < TEMPO_VARS_LENGHT; i++)
			tempo_vars[temp_var_num][i] = help_string[i];
		tempo_vars[temp_var_num][i+1] = '\0';

	}
	else
	{
		tempo_vars[temp_var_num][0] = '1';
		tempo_vars[temp_var_num][1] = '\0';
	}
	return 1;
}
/*if it found command for temporarily variables it call it ll be executed*/
int temp_edit(row *sheet, char *single_command, int *row_to, int *cell_to, char tempo_vars[TEMPO_VARS_MAX][TEMPO_VARS_LENGHT],int *temp_10, char separator)
{
	if(strstr(single_command, "[set]") != NULL)  //[set]
		return temp_set(*row_to, *cell_to, temp_10);
	if(strstr(single_command, "def _") != NULL)  // def _X
		return temp_def(sheet, single_command, *row_to, *cell_to, tempo_vars, separator);
	if(strstr(single_command, "use _") != NULL)  // use _X
		return temp_use(sheet,single_command, *row_to, *cell_to, tempo_vars, separator);
	if(strstr(single_command, "inc _") != NULL)  // inx _X
		return temp_inc(single_command, tempo_vars);
	return 0;
}
/*
This function ll get one command and execute it 
*/
int call_command(row *sheet, char *single_command, int *row_from, int *row_to, int *cell_from, int *cell_to, char tempo_vars[TEMPO_VARS_MAX][TEMPO_VARS_LENGHT], char separator, int *row_counter, int *temp_10)
{
	//TODO [11_51]  = char1 == 11 
	char last_char = get_last_char(single_command);
	int error = 0;

	//error = -1 bad syntax; 0 = command not found; 1 = command_executed 
	error = sheet_edit(sheet, single_command, row_from, row_to, cell_to, cell_from, separator, row_counter);
	if(error == -1)
		return -1;
	else if(error == 1)
		return 0;

	error = select_change(sheet, single_command, row_from, row_to, cell_from, cell_to, last_char, separator, row_counter, temp_10);
	if(error == -1)
		return -1;
	else if(error == 1)
		return 0;

	error = content_edit(sheet, single_command, row_from, row_to, cell_from, cell_to, last_char, separator, *row_counter);
	if(error == -1)
		return -1;
	else if(error == 1)
		return 0;	
	
	error = temp_edit(sheet, single_command, row_to, cell_to, tempo_vars, temp_10, separator);
	if(error == -1)
		return -1;
	else if(error == 1)
		return 0;	
	
	return 0 ;
}
/*This function ll find out what command to do and call the functions*/ 
int command_execution(int commands_sum, char *commands, int commands_char_sum, int separator, row *sheet, int *row_counter)
{
	char single_command[1000]; //max str that u can write to table is 1000 becouse max command size is 1000
	int last_command = 0;
	int error = 0;
	int row_from = 1, row_to = 1;
	int cell_from = 1, cell_to = 1;
	int temp_10[2]; //row cell
	temp_10[0] = temp_10[1] = 0;
	char tempo_vars[TEMPO_VARS_MAX][MAX_COMMAND_SIZE];

	for(int i = 0; i < TEMPO_VARS_MAX; i++)
		array_char_init(tempo_vars[i], MAX_COMMAND_SIZE);

	for(;last_command <= commands_sum;)
	{
		store_one_command(single_command, commands, &last_command, commands_char_sum);
		error = call_command(sheet, single_command, &row_from, &row_to, &cell_from, &cell_to, tempo_vars, separator, row_counter, temp_10);
		if(error != 0) return -1;
	}
	return 0;
}
/*
This function just story one command that ll be executed 
*/
int store_one_command(char *single_command, char *commands, int *last_command, int commands_char_sum)
{
	int i = 0, help = 0, j = 0;
	if(help != *last_command)
	{
		for(;help<*last_command;i++)
		{
			if(commands[i] == COMMAND_SEPARATOR)	
					if(commands[i-1] != '\\')
                        help++;
		}	
	}
	while (i < commands_char_sum)
	{
		if(commands[i] == COMMAND_SEPARATOR)
			if(commands[i-1] != '\\')
				break;
		single_command[j] = commands[i];
		j++;
		i++;
	}
	single_command[j] = '\0';
	*last_command = *last_command +1;
	return 0;
}
/*Function ll store command to one array */
void store_commands(char **argv, int is_there_separator, char *commands, int command_char_sum)
{
	int check = 0;
	if(is_there_separator == 1) check = COMMNDS_POSITION_WITH_SEPARATOR;
	else check = COMMNDS_POSITION_WITHOUT_SEPARATOR;
	for(int i = 0; i<command_char_sum; i++)
		commands[i] = argv[check][i];
}
/* maximum number of commands is 1000 and max command size is 1000.
Also this function calculate number of chars becouse later i ll store commands to array
*/
int commands_error_check(char **argv, int is_there_separator, int *count_chars, int *count_commands)
{
	int check = 0, command_lenght = 0;
	if(is_there_separator == 1) check = COMMNDS_POSITION_WITH_SEPARATOR;
	else check = COMMNDS_POSITION_WITHOUT_SEPARATOR;
	
	for(int i = 0; argv[check][i] != '\0';i++)
	{
		*count_chars = *count_chars + 1 ;
		command_lenght++;
		if(argv[check][i] == COMMAND_SEPARATOR)
		{
			if(argv[check][i-1] != '\\')
			{
				*count_commands = *count_commands + 1;
				command_lenght = 0;
			}
			continue;
		}
		if(command_lenght > MAX_COMMAND_SIZE )
		{
			fprintf(stderr, "one command has more than 1000 chars");
			return -1;
		}
	}
	if(*count_chars == 0)
	{
		fprintf(stderr, "There are no commands ");
		return -1;	
	}
	if(*count_commands > COMANDS_MAXIMUM )
	{
		fprintf(stderr, "one command has more than 1000 chars");
		return -1;	
	}
	return 0;
}
/*It works like constructor */
void initialize_sheet(row *sheet, int row_counter)
{
	for(int i=0; i<row_counter;i++)
	{
		sheet[i].row_size = 0;
		sheet[i].cels_in_row = 0;
		sheet[i].one_row = NULL;
	}
}
void rewrite_file(row *sheet, int row_counter, char **argv, int is_there_separator, char separator)
{
	FILE *sheet_file;
	if(is_there_separator == 1)sheet_file = fopen(argv[4], "w+"); else sheet_file = fopen(argv[2], "w+");
	char *help = NULL;
	int k = 0;
	for(int i = 0; i < row_counter; i++)
	{
		help = malloc((sheet[i].row_size + RESERVE) * sizeof(char));	 	
		array_char_init(help, sheet[i].row_size + RESERVE);
		for(int j = 0; j < sheet[i].row_size; j++)
		{
			if(sheet[i].one_row[j] == '\\')
			{					
				if(j > 0)
				{
					if(sheet[i].one_row[j-1] == '\\')
					{
						help[k++] = sheet[i].one_row[j];
						continue;
					}
					else
						continue;
				}
			}
			if(sheet[i].one_row[j] == '\"')
			{
				if(j > 0)
				{
					if(sheet[i].one_row[j-1] != '\\')
						continue;
				}
				else
					continue;
			}
			if(sheet[i].one_row[j] == separator)
			{
				if(j > 0)
				{
					if(sheet[i].one_row[j-1] == '\\')
					{
						help[k+1] = separator;
						help[k+2] = '"';
						int i = k-1;
						for(; help[i] != separator && i >= 0;i--)
						{
							help[i+1] = help[i];
						}
						help[i+1] = '"';
						k = k + 3;
						continue;
					}
				}
			}
			help[k++] = sheet[i].one_row[j];
		}
		help[k] = '\0';
		for(int x = 0; help[x] != '\0';x++)
		{
			fputc(help[x], sheet_file);
		}
		k = 0;
		free(help);
		help = NULL;
	}
	fclose(sheet_file);
}
void end_print_sheet(row *sheet, int row_counter)
{
	printf("\n");
	for(int i = 0; i < row_counter;i++)
	{
		for(int j = 0; j < sheet[i].row_size ; j++)
			putchar(sheet[i].one_row[j]);
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
		sheet[i].cels_in_row = 0;
		sheet[i].one_row = NULL;
	}	
}
int alocate_sheet(row *sheet, int row_counter, int *rows_lenght)
{
	for(int i=0; i<row_counter; i++)
	{
		sheet[i].one_row = malloc((rows_lenght[i]) * sizeof(char));
		if(sheet[i].one_row == NULL)
		{
			for(int j = i; i >= 0; i--)
			{
				free(sheet[j].one_row);
				sheet[j].one_row = NULL;
			}
			return error_maloc(ERROR_VALUE_ONE);
		}
	}
	return 0;
}

/*This function ll not only alocate memmory but also safe input file to data structure */
int store_sheet(row *sheet, char **argv, int is_there_separator, int *rows_lenght, int rows_c)
{
	int j = 0, c;
	FILE *sheet_file;
	if(is_there_separator == 1)sheet_file = fopen(argv[4], "r+"); else sheet_file = fopen(argv[2], "r+");
	for(int i = 0; i < rows_c; i++)
		for(int j = 0; j < rows_lenght[i] ;j++)
			sheet[i].one_row[j] = '\0';

	int i = 0;
	while(j < rows_c)
	{
		sheet[j].row_size = rows_lenght[j];
		c = fgetc(sheet_file);
		if(c == EOF)
			break;
		sheet[j].one_row[i] = c;
		i++;
		if(c == '\n')
		{
			j++;
			i = 0;
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
	int c, i = 0;
	array_int_init(row_counter, rows_lenght);
	if(is_there_separator == 1)	sheet_file = fopen(argv[4], "r+"); else sheet_file = fopen(argv[2], "r+");
	if(sheet_file == NULL)
	{
		fprintf(stderr, "Can not open file\n");
		return -1;
	}
	while(((c=fgetc(sheet_file)) != EOF) && i != row_counter)
	{
		rows_lenght[i] = rows_lenght[i] + 1;
		if(c == '\n')
			i++;
	}
	fclose(sheet_file);
	return 0;
}
//Check if program execution have good syntax.
//It should be /sps [-d DELIM] CMD_SEQUENCE FILE
//So file name has to be on argv[5] or argv[3]
int input_error(int argc)
{
	if((argc != ARGUMENT_SUM_POSIBILITY_ONE) && (argc != ARGUMENT_SUM_POSIBILITY_TWO))
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
		{
			if(i+1 < argc)
			{
				*separator = argv[i+1][0];
				return 1;
			}
		}
	}
	return 0; 
}
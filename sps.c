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
[R,C] - výběr buňky na řádku R a sloupci C.
[R,_] - výběr celého řádku R.
[_,C] - výběr celého sloupce C.
[R1,C1,R2,C2] - výběr okna, tj. všech buněk na řádku R a sloupci C, pro které platí R1 <= R <= R2, C1 <= C <= C2. Pokud namísto čísla R2 resp. C2 bude pomlčka, nahrazuje tak maximální řádek resp. sloupec v tabulce.
[_,_] - výběr celé tabulky.
[min] - v již existujícím výběru buněk najde buňku s minimální numerickou hodnotou a výběr nastaví na ni.
[max] - obdobně jako předchozí příkaz, ale najde buňku s maximální hodnotou.
[find STR] - v již existujícím výběru buněk vybere první buňku, jejíž hodnota obsahuje podřetězec STR.
[_] - obnoví výběr z dočasné proměnné (viz níže).

SHEET EDITING COMMANDS 
irow - vloží jeden prázdný řádek nalevo od vybraných buněk.
arow - přidá jeden prázdný řádek napravo od vybraných buněk.
drow - odstraní vybrané řádky.
icol - vloží jeden prázdný sloupec nalevo od vybraných buněk.
acol - přidá jeden prázdný sloupec napravo od vybraných buněk.
dcol - odstraní vybrané sloupce.

SHEET CONTENT EDITING COMMANDS 
set STR - nastaví hodnotu buňky na řetězec STR. Řetězec STR může být ohraničen uvozovkami a může obsahovat speciální znaky uvozené lomítkem (viz formát tabulky)
clear - obsah vybraných buněk bude odstraněn (buňky budou mít prázdný obsah)
swap [R,C] - vymění obsah vybrané buňky s buňkou na řádku R a sloupci C
sum [R,C] - do buňky na řádku R a sloupci C uloží součet hodnot vybraných buněk (odpovídající formátu %g u printf). Vybrané buňky neobsahující číslo budou ignorovány (jako by vybrány nebyly).
avg [R,C] - stejné jako sum, ale ukládá se aritmetický průměr z vybraných buněk
count [R,C] - stejné jako sum, ale ukládá se počet neprázdných buněk z vybraných buněk
len [R,C] - do buňky na řádku R a sloupci C uloží délku řetězce aktuálně vybrané buňky

TEMPORARILY VARIABLES COMMANDS 
def _X - hodnota aktuální buňky bude nastavena do dočasné proměnné X (kde X může být hodnota 0 až 9)
use _X - aktuální buňka bude nastavena na hodnotu z dočasné proměnné X (kde X identifikuje dočasnou proměnnou _0 až _9)
inc _X - numerická hodnota v dočasné proměnné bude zvětšena o 1. Pokud dočasná proměnná neobsahuje číslo, bude výsledná hodnota proměnné nastavená na 1.
[set] - nastaví aktuální výběr buněk do dočasné proměnné _ (ukládá se pouze, které buňky jsou vybrány, nikoliv jejich obsah)

*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define MAX_LINUX_FILE_SIZE 256
#define COMANDS_MAXIMUM 1000 // bigest command is 11 chars. and i add some reserve 
#define MAX_COMMAND_SIZE 1000

#define COMMNDS_POSITION_WITH_SEPARATOR 3
#define COMMNDS_POSITION_WITHOUT_SEPARATOR 1


#define TEMPO_VARS_MAX 10
#define TEMPO_VARS_LENGHT 1000

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
int command_execution(int commands_sum, char *commands, int commands_char_sum, int separator, row *sheet);
//This function just story one command that ll be executed 
int story_one_command(char command_separator, char *single_command, char *commands, int *last_command, int commands_char_sum);
/*this ll call command and them it ll be executed */
int call_command(row *sheet, char *single_command, int *row_from, int *row_to, int *cell_from, int *cell_to, char tempo_vars[TEMPO_VARS_MAX][TEMPO_VARS_LENGHT], char separator);
int call_data_struct_com(row *sheet, char *single_command, int *row_from, int *row_to, int *cell_from, int *cell_to);
int call_temporarily(row *sheet, char *single_command, int *row_from, int *row_to, int *cell_from, int *cell_to, char tempo_vars[TEMPO_VARS_MAX][TEMPO_VARS_LENGHT]);
int call_selection_com(row *sheet, char *single_command, int *row_from, int *row_to, int *cell_from, int *cell_to, char last_character, char separator);


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
void end_print_sheet(row *sheet, int row_counter);
int count_cells_in_row(int i, row *sheet, char separator, int y_n);

//FUNCTIONS FOR SIMPLIFICATION
void array_int_init(int size, int *array); //This ll initalize int array;
char get_last_char(char *aray);

int main(int argc, char **argv)
{
	int errors = 0; 
	int is_there_separator = 0; // if there is separator in argv it ll be changed to true 
	int d_separator;		
	int row_counter = 0;
	int *rows_lenght;

	int commands_char_sum = 0;	
	int commands_sum = 0;  //how many commands are on input 

	//first i ll check input for erors find separator and story commands 
	errors = input_error(argc);
	if(errors != 0) return -1;
    is_there_separator = separe(argc, argv, &d_separator);
	errors = commands_error_check(argv,is_there_separator, &commands_char_sum, &commands_sum); 
	if(errors != 0) return -1;

	//Count input file rows and size so i can maloc 
	errors = count_rows(argv, is_there_separator, &row_counter);
	if(errors != 0) return -1;

	//HERE I CREATE ROW SHEET IT IS ARRAY OF ROW THAT CONTAIN CHAR ROW AND ROW SIZE 
	row sheet[row_counter -1];

	rows_lenght = malloc((row_counter)* sizeof(rows_lenght));
	array_int_init(row_counter, rows_lenght);
	errors = count_rows_lenght(argv, row_counter, is_there_separator, rows_lenght);
	if(errors != 0) return -1;

	//alocate space in heap for my sheet that can have infinity size 
	initialize_sheet(sheet, row_counter);
	errors = alocate_sheet(sheet, row_counter, rows_lenght);
	if(errors != 0) return -1;

	errors = store_sheet(sheet, argv, row_counter, is_there_separator, rows_lenght);
	if(errors != 0) return -1;
	
	for(int i = 0; i<row_counter; i++)
		count_cells_in_row(i, sheet, d_separator, 1);

	char commands[commands_char_sum+2]; //here i ll store all the commands that ll be executed on sheet 
	store_commands(argv, is_there_separator, commands, commands_char_sum);
	errors = command_execution(commands_sum, commands, commands_char_sum, d_separator, sheet);
	if(errors != 0) return -1;

	//printing editet sheet and free memory 
	end_print_sheet(sheet, row_counter);
	free(rows_lenght);
	free_sheet(sheet, row_counter);
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
int count_cells_in_row(int i, row *sheet, char separator, int y_n)
{
	int cells = 0;
	for(int j = 0; j < sheet[i].row_size; j++)
		if(sheet[i].one_row[j] == separator)
			cells++;
	if(y_n == 1)
		sheet[i].cels_in_row = cells;
	return cells;
}

/*if it found data sturctu command it call it ll be executed*/
//error = -1 bad syntax; 0 = command not found; 1 = command_executed 
int call_data_struct_com(row *sheet, char *single_command, int *row_from, int *row_to, int *cell_from, int *cell_to)
{
	if(strcmp(single_command, "irow")==0) 
	{
		return 1;
	}
	else if(strcmp(single_command, "arow")==0) 
	{
		return 1;
	}
	else if(strcmp(single_command, "drow")==0) 
	{
		return 1;
	}
	else if(strcmp(single_command, "icol")==0) 
	{
		return 1;
	}
	else if(strcmp(single_command, "acol")==0) 
	{
		return 1;
	}
	else if(strcmp(single_command, "dcol")==0) 
	{
		return 1;
	}
	return  0;
}
/*
it ll change table select 
If the select is bigger them table it ll make it bigerr 
[R,C], [R1,R2,C1,C2],[_,C], [_,_], [min], [max], [find STR], [_]
*/
int selection_changer(row *sheet, char *single_command, int *row_from, int *row_to, int *cell_from, int *cell_to, char last_character, char separator)
{
	int commas = 0;

	if(strstr(single_command, "[_,_]") != NULL)  //[_,_]
	{			
		return 1;
	}
	else if(strstr(single_command, "[min]") != NULL)  //[min]
	{
		return 1;
	}
	else if(strstr(single_command, "[max]") != NULL) //[max]
	{
		return 1;
	}
	else if(strstr(single_command, "[_]") != NULL)  //[_]
	{
		return 1;
	}
	else if(strstr(single_command, "[find") != NULL)
	{
		return 1;
	}
	else
	{
		for(int i = 0; single_command[i] != '\0';i++)
			if(single_command[i] == ',');
				commas++;
		if(commas == 1) //[R,C]
		{				
			return 1;
		} 
		else if(commas == 3)   //[R1,R1,C1,C2]
		{					
			return 1;
		}
		else
		{
			//TODO error 
			return -1;
		}
	}
	return 0;
}

/*if it found selection command it call it ll be executed*/
int call_selection_com(row *sheet, char *single_command, int *row_from, int *row_to, int *cell_from, int *cell_to, char last_character, char separator)
{
	if(strstr(single_command, "[set]") != NULL)  //[set]
		return 0;

	if(single_command[0] == '[')
	{
		if(last_character == ']')
		{
			selection_changer(sheet, single_command, row_from, row_to, cell_from, cell_to, last_character, separator);
			return 1;		
		}	
		else
			fprintf(stderr, "Error if command start with [ it has to ned with ]");
	}
	return 0;
}

/*if it found command for editing sheet content it call it ll be executed*/
int call_content_edit(row *sheet, char *single_command, int *row_from, int *row_to, int *cell_from, int *cell_to, char last_character)
{
	if(strstr(single_command, "set") != NULL)  //set STR 
	{
		if(single_command[0] != '[')
			return 1;
	}
	if(strstr(single_command, "clear") != NULL) 
		return 1;
	if(strstr(single_command, "swap") != NULL) //swap [R,C] 
	{
		if(single_command[5] == '[')
			if(last_character == ']')
				//CALL SWAP 
				return 1;
			else
			{
				fprintf(stderr, "ERROR bad syntax of swap command ");
				return -1;
			}	
		else
		{
			fprintf(stderr, "ERROR bad syntax of swap command ");
			return -1;
		}
	}
	if(strstr(single_command, "sum") != NULL) //sum [R,C]
	{
		if(single_command[4] == '[')
			if(last_character == ']')
				return 1;
			else
			{
				fprintf(stderr, "ERROR bad syntax of sum command ");
				return -1;
			}
		else
		{
			fprintf(stderr, "ERROR bad syntax of sum command ");
			return -1;
		}
	}
	if(strstr(single_command, "avg") != NULL)  //avg [R,C]
	{
		if(single_command[4] == '[')
			if(last_character == ']')
				return 1;
			else
			{
				fprintf(stderr, "ERROR bad syntax of avg command ");
				return -1;
			}
		else
			{
				fprintf(stderr, "ERROR bad syntax of avg command ");
				return -1;
			}
	}
	if(strstr(single_command, "count") != NULL) //count [R,C]
	{
		if(single_command[6] == '[')
			if(last_character == ']')
				return 1;
			else
			{
				fprintf(stderr, "ERROR bad syntax of avg command ");
				return -1;
			}
		else
		{
			fprintf(stderr, "ERROR bad syntax of avg command ");
			return -1;
		}
	}
	if(strstr(single_command, "len") != NULL) //len [R,C]
	{
		if(single_command[4] == '[')
			if(last_character == ']')
				return 1;
			else
			{
				fprintf(stderr, "ERROR bad syntax of avg command ");
				return -1;
			}
		else
		{
			fprintf(stderr, "ERROR bad syntax of avg command ");
			return -1;
		}		
	}
	return 0;
}


/*if it found command for temporarily variables it call it ll be executed*/
int call_temporarily(row *sheet, char *single_command, int *row_from, int *row_to, int *cell_from, int *cell_to, char tempo_vars[TEMPO_VARS_MAX][TEMPO_VARS_LENGHT])
{

	if(strstr(single_command, "[set]") != NULL)  //[set]
	{
			return 1;
	}
	if(strstr(single_command, "def _") != NULL)  // def _X
	{
			return 1;
	}
	if(strstr(single_command, "use _") != NULL)  // use _X
	{
			return 1;
	}
	if(strstr(single_command, "int _") != NULL)  // inx _X
	{
			return 1;
	}


	return 0;
}
/*
This function ll get one command and execute it 
*/
int call_command(row *sheet, char *single_command, int *row_from, int *row_to, int *cell_from, int *cell_to, char tempo_vars[TEMPO_VARS_MAX][TEMPO_VARS_LENGHT], char separator)
{
	//TODO [11_51]  = char1 == 11 
	char hellper = NULL;
	int commas_in_command = 0;
	char last_char = get_last_char(single_command);
	int error = 0;

	//error = -1 bad syntax; 0 = command not found; 1 = command_executed 
	error = call_data_struct_com(sheet, single_command, row_from, row_to, cell_from, cell_to);
	if(error == -1)
		return -1;
	else if(error == 1)
		return 0;

	error = call_selection_com(sheet, single_command, row_from, row_to, cell_from, cell_to, last_char, separator);
	if(error == -1)
		return -1;
	else if(error == 1)
		return 0;

	error = call_content_edit(sheet, single_command, row_from, row_to, cell_from, cell_to, last_char);
	if(error == -1)
		return -1;
	else if(error == 1)
		return 0;	
	
	error = call_temporarily(sheet, single_command, row_from, row_to, cell_from, cell_to, tempo_vars);
	if(error == -1)
		return -1;
	else if(error == 1)
		return 0;	

	fprintf(stderr, "Error command not found ");
	return 0 ;
}

/*This function ll find out what command to do and call the functions*/ 
int command_execution(int commands_sum, char *commands, int commands_char_sum, int separator, row *sheet)
{
	char tempo_vars[TEMPO_VARS_MAX][TEMPO_VARS_LENGHT];
	char single_command[1000]; //max str that u can write to table is 1000 becouse max command size is 1000
	int last_command = 0;
	int error = 0;
	int row_from = 1, row_to = 1;
	int cell_from = 1, cell_to = 1;
	
	char command_separator = ';';

	for(last_command; last_command<=commands_sum;)
	{
		story_one_command(command_separator, single_command, commands, &last_command, commands_char_sum);
		error = call_command(sheet, single_command, &row_from, &row_to, &cell_from, &cell_to, tempo_vars, separator);
		if(error != 0) return -1;
	}
	printf("\n");
	return 0;
}

/*
This function just story one command that ll be executed 
*/
int story_one_command(char command_separator, char *single_command, char *commands, int *last_command, int commands_char_sum)
{
	int i = 0, help = 0, j = 0;
	if(help != *last_command)
	{
		for(;help<*last_command;i++)
		{
			if(commands[i] == command_separator)	
					if(commands[i-1] != '\\')
                        help++;
		}	
	}
	while (i < commands_char_sum)
	{
		if(commands[i] == command_separator)
			if(commands[i-1] != '\\')
				break;
		single_command[j] = commands[i];
		j++;
		i++;
	}
	single_command[j] = '\0';
	//TODO SMAZAT 
	printf("\n");
	for(int k = 0; single_command[k] != '\0';k++)
		putchar(single_command[k]);
	//TADY
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
	char command_separator = ';';
	
	if(is_there_separator == 1) check = COMMNDS_POSITION_WITH_SEPARATOR;
	else check = COMMNDS_POSITION_WITHOUT_SEPARATOR;
	
	for(int i = 0; argv[check][i] != '\0';i++)
	{
		
		*count_chars = *count_chars + 1 ;
		command_lenght++;
		if(argv[check][i] == command_separator)
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
void end_print_sheet(row *sheet, int row_counter)
{
	for(int i = 0; i < row_counter;i++)
	{
		for (int j = 0; j < sheet[i].row_size-1 ; j++)
		{
			if(sheet[i].one_row[j] == '\\')
				j++;
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
			//TODO CALL DESTRUCTOR
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
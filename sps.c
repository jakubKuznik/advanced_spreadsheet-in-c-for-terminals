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


int separe(int argc, char *argv[]); 				//Function gets cels separator. 

int main(int argc, char **argv)
{
	int d_separator;	 	
    d_separator = separe(argc, argv);
 
    return 0;
}
//In table we alwazs have some separator character this function ll get it.
//-d 
//separator is always next argument after -d 
//it there is not any default is ' ' space 
int separe(int argc, char *argv[])
{	
	int input_argument = ' ';
	for(int i = 0; i < argc; i++)  
	{
		if(strcmp(argv[i], "-d")==0)
			if(i+1 < argc)
			{
				input_argument = argv[i+1][0];
				break;
			}
	}
	return input_argument; 
}
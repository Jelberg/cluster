#include <stdio.h>
 
int main ( int argc, char **argv )
{
 	FILE *fp;
 	
 	char caracter;
 		
 	fp = fopen ( "fichero.txt", "a+t" ); 
 	//parametro para escritura al final y para file tipo texto
 	
 	printf("\nIntroduce un texto al fichero: ");
 	
 	while((caracter = getchar()) != '\n') // Se queda dentro del while hasta que se le da enter
 	{
 		printf("%c", fputc(caracter, fp));
 	}
 	
 	fclose ( fp );
 	
 	return 0;
}
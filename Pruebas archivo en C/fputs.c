#include <stdio.h>
 
int main ( int argc, char **argv )
{
 	FILE *fp;
 	
 	char cadena[] = "Mostrando el uso de fputs en un fichero.\n";
 	
 	fp = fopen ( "fichero2.txt", "w+" );
 	
 	fputs( cadena, fp );
 	
 	fclose ( fp );
 	
 	return 0;
}
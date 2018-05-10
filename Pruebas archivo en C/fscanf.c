#include <stdio.h>

 
int main ( int argc, char **argv )
{
 	FILE *archivo;
 	char caracter;
 	char buffer[100] = "Esto es un fichero. ";
 	int a = 0;
 	
 	archivo = fopen("archivo.txt","r+");
	 	 
	 	 while(feof(archivo) == 0)
	    	{
	    	   fscanf(archivo,"%c",&caracter);
	    	   
	    	   if (a == 1){
				fprintf(archivo,buffer);
	    	   	a=0;
	    	   }

			   if (caracter == '\n'){
				a=1;
			   } 
					
	    	}
 	
 	fclose ( archivo );
 	
 	return 0;
}
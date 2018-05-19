#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void cuentaPalabras(){
	FILE *archivo;
	char caracter;
	int fElement =0;
	archivo = fopen("archivo.txt","r");
	
	if (archivo == NULL)
        {
            printf("\nError de apertura del archivo. \n\n");
        }
        else
        {
            
     	    fscanf(archivo,"%c",&caracter);
     	    printf("%c",caracter);
     	    fscanf(archivo,"%c",&caracter);
     	    printf("%c ",caracter);
            while(feof(archivo) == 0)
	    	{
				fscanf(archivo,"%c",&caracter);
	    	   
				if (caracter == '\n'){
					fscanf(archivo,"%c",&caracter);
	    	    	printf("%c",caracter);
	    	    	fscanf(archivo,"%c",&caracter);
	    	    	printf("%c ",caracter);
				} 
					
	    	}
        }
        fclose(archivo);
	return 0;
}

int main()
{
	
	
	cuentaPalabras();
}
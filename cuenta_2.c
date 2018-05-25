#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


void main(int argc, char** argv){
	obtinePalabraDiccionario(1);
}


char *strlwr(char *str){
  unsigned char *p = (unsigned char *)str;

  while (*p) {
     *p = tolower((unsigned char)*p);
      p++;
  }

  return str;
}

void creaArchivoCantPalabras(char palabra[100], int cantidad, int nodo){
	FILE *fp;
	//Tamanio de la palabra por que es necesario para en strcpy y strcat
	int tamanio  = strlen(palabra);
	char dest[tamanio+7];
	
	//.............crear nombbre archivo ..............
	char numNodo[2] = {0};
	char nombreArchNodo[60];
	//transforma int a char
	sprintf(numNodo,"%d",nodo);
	
	//crea el nombre del archivo para el nodo
	strcpy(nombreArchNodo,"hola");
	strcat(nombreArchNodo,numNodo);
	strcat(nombreArchNodo,".txt");
	//........................................
	
	
	char scantidad[5];
	//Transforma un int en un char y lo copia en la variable scantidad 
	sprintf(scantidad,"%d",cantidad);
	
	strcpy(dest,palabra);
	strcat(dest," ");
	strcat(dest,scantidad);
	strcat(dest,"\n");
	
 	fp = fopen (nombreArchNodo, "a+" );

 	fputs(dest,fp);
 	
 	fclose ( fp );
}

void cuentaPalabras(char palabra[100],int nodo){
	FILE *archivo;	
 	char texto[500];
	int tp, tam;
	char word[100];
	int contador=0;
 	
 	archivo = fopen("diccionario.txt","r");
 	
 	if (archivo == NULL)
 		exit(1);
 	else{
 	 
 	    while (fscanf(archivo,"%s",word) != EOF){
			if (strcmp(palabra,word)==0){
				contador++;
			}
		}
		
		fclose(archivo);	
		
		creaArchivoCantPalabras(palabra,contador,nodo);
			 
    }
    
}

int cantFilas(char nombre[60]){
	FILE *archivo;
	char definicion[500]={0};
	int cont = 0;
	
	archivo = fopen(nombre,"r");
	
	if (archivo == NULL){
            printf("\nError de apertura del archivo. \n\n");
        }
        else{  
            while(feof(archivo) == 0)
	    	{		
				// Obtiene definicion 
				fgets(definicion,500,archivo);
				cont++;
	    	}
        }   
	fclose(archivo);
	return cont;	
}

void obtinePalabraDiccionario(int nodo){
	FILE *archivo;
	char *caracter[60]={0};
	char definicion[500]={0};
	int cont =0;
	
	int cantidad = cantFilas("hola.txt")-2;
	archivo = fopen("hola.txt","r");

	
	if (archivo == NULL)
        {
            printf("\nError de apertura del archivo. \n\n");
        }
        else
        {
            
            while(cont < cantidad)
	    	{		
				cont++;
				//Obtiene palabra
				
			    fscanf(archivo,"%s",&caracter);
				// Obtiene definicion 
				fgets(definicion,500,archivo);
			
				cuentaPalabras(strlwr(caracter),nodo);
				free(caracter);
	    	}
			printf("          TERMINA contabilizar palabras en proceso: %d \n",nodo);
        }   
	fclose(archivo);	
}
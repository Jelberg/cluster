#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Direccion del libro 
#define libroTXT "archivo.txt"

// Direccion archivo generado con cuentas de palabras
#define countWord "palabras_contabilizadas.txt"

		/****************************************************

			LISTA DE FUNCIONES Y PROCEDIMIENTOS COMENTADAS

		*****************************************************/

//Metodo compara la palabra a buscar con la oracion que se le manda
int loencontre(int pos, int tp, char texto[100], char palabra[]);

//Metodo para crear un archivo con las palabras contabilizadas 
void creaArchivoCantPalabras(char palabra[], int cantidad);

// Metodo llama a loencontre al cual le pasa la oracion de la fila, lleva la cuenta de la ocurrencia de las palabras
// Este metodo procesa solo 1 palabra
void cuentaPalabras(char palabra[]);
 
		/*******************************************

							MAIN

		*******************************************/
 
void main(){
 	cuentaPalabras("hola");
}

		/*******************************************

			        FUNCIONES Y PROCEDIMIENTOS

		*******************************************/

int loencontre(int pos, int tp, char texto[100], char palabra[]) {
  char aux[20] ;
  char anterior='a';
  int auxtam= pos+tp;
  int i, xi;
 
  // aux en la palabra que saca de la oracion de texto en base al tamanio de la palabra buscada 
  for ( i= pos, xi=0; i<= auxtam  ; i++,xi++){
    aux[xi]=tolower(texto[i]);
  }
 
  aux[tp]='\0';
  
  // Hace la comparacion 
  if ( strcmp(aux,palabra) ==0 )
      return 1;
 
  else
      return 0;
}

void creaArchivoCantPalabras(char palabra[], int cantidad){
	FILE *fp;
	//Tamanio de la palabra por que es necesario para en strcpy y strcat
	int tamanio  = strlen(palabra);
	char dest[tamanio+7];
	
	char scantidad[5];
	//Transforma un int en un char y lo copia en la variable scantidad 
	sprintf(scantidad,"%d",cantidad);
	
	strcpy(dest,palabra);
	strcat(dest," ");
	strcat(dest,scantidad);
	strcat(dest,"\n");
	
 	fp = fopen ( countWord , "a+" );
	
 	fputs(dest,fp);
 	
 	fclose ( fp );
}

 
void cuentaPalabras(char palabra[]){
	FILE *archivo;	
 	char texto[100];
	int tp, tam;
	int contador=0;
 	
 	archivo = fopen(libroTXT,"r");
 	
 	if (archivo == NULL)
 		exit(1);
 	else{
 	 
 	    while (feof(archivo) == 0){
			
			// fila se copia en texto
			fgets(texto,100,archivo);
			
			tp=strlen(palabra);
            tam=strlen(texto);
			
			//printf("\n palabra: %i texto: %i \n\n", tp,tam);  
			
			for (int i=0; i< tam ; i++){
			   
			   if ( loencontre(i,tp,texto,palabra)){
				   contador++;
	            }
			}
 	    }
		fclose(archivo);	
		
		creaArchivoCantPalabras(palabra,contador);
			 
    }
    
}
 
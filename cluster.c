#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

		/****************************************************

			LISTA DE FUNCIONES Y PROCEDIMIENTOS COMENTADAS

		*****************************************************/

//Metodo compara la palabra a buscar con la oracion que se le manda
int loencontre(int pos, int tp, char texto[100], char palabra[]);

//Metodo para crear un archivo con las palabras contabilizadas DA ERROR
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
  char aux[20] ={0};
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
	char dest[]={0};
    
	stpcpy(dest,palabra);
	//strcat(palabra," 20");
 	fp = fopen ( "palabras_contabilizadas.txt", "w+" );
 	
 	fputs(dest,fp);
 	
 	fclose ( fp );
}

 
void cuentaPalabras(char palabra[]){
	FILE *archivo;	
 	char texto[100]={0};
	int tp, tam;
	int contador=0;
 	
 	archivo = fopen("archivo.txt","r");
 	
 	if (archivo == NULL)
 		exit(1);
 	else{
 	 
 	    while (feof(archivo) == 0){
			
			fgets(texto,100,archivo);
			printf("%s",texto);
			
			tp=strlen(palabra);
            tam=strlen(texto);
			
			//printf("\n palabra: %i texto: %i \n\n", tp,tam);  
			
			for (int i=0; i< tam ; i++){
			   
			   if ( loencontre(i,tp,texto,palabra)){
				   contador++;
	            }
			}
 	    }
        printf("\n Se encontro la palabra %i veces \n\n", contador);  
		creaArchivoCantPalabras(palabra,contador);
    }
    fclose(archivo);
}
 
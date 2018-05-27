#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <mpi.h>

#define _palabrasXproc "/local_home/jelberg.12/diccionario_palabras_proc_"

// Direccion del libro 
#define _libroTXT "/local_home/jelberg.12/libro_medicina.txt"

// Direccion archivo generado con cuentas de palabras
#define _countWord "/local_home/jelberg.12/palabras_contabilizadas_1.txt"

// Direccion archivo generado con cuentas de palabras
#define _cuentas "/local_home/jelberg.12/count_words.txt"

// Direccion de diccionario de palabras 
#define _diccionario "/local_home/jelberg.12/palabras_libro_medicina.txt"

		/***************************************************************

			      LISTA DE FUNCIONES Y PROCEDIMIENTOS COMENTADAS

		****************************************************************/
char* nombreArchivoNodo(int nodo);
char* nombreArchivoCuentas(int nodo);
int loencontre(int pos, int tp, char texto[500], char palabra[60]);
void creaArchivoCantPalabras(char palabra[60], int cantidad, int nodo);
void cuentaPalabras(char palabra[60], int nodo);
int cantFilas(char nombre[60]);
void obtinePalabraDiccionario(int nodo);

		/*************************************************************

							        MAIN

		**************************************************************/
 
void main(int argc, char** argv){
	
MPI_Init(&argc, &argv);

	int my_id, nproc, i;

	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);

	if (my_id == 1){
	obtinePalabraDiccionario(1); // Contabiliza las palabras, lo ejecutan los demas nodos 
	
	}
	MPI_Finalize();
}


		/**************************************************************

			                FUNCIONES Y PROCEDIMIENTOS

		***************************************************************/	

char* nombreArchivoNodo(int nodo){
	char numNodo[2] = {0};
	char* nombreArchNodo = malloc(60);
	//transforma int a char
	sprintf(numNodo,"%d",nodo);
	
	//crea el nombre del archivo para el nodo
	strcpy(nombreArchNodo,_palabrasXproc);
	strcat(nombreArchNodo,numNodo);
	strcat(nombreArchNodo,".txt");
	
	return nombreArchNodo;
}

char* nombreArchivoCuentas(int nodo){
	char numNodo[2] = {0};
	char* nombreArchNodo = malloc(60);
	//transforma int a char
	sprintf(numNodo,"%d",nodo);
	
	//crea el nombre del archivo para el nodo
	strcpy(nombreArchNodo,_countWord);
	strcat(nombreArchNodo,numNodo);
	strcat(nombreArchNodo,".txt");
	
	return nombreArchNodo;
}
		
int loencontre(int pos, int tp, char texto[500], char palabra[60]) {
  char aux[60]={0};

  
  int auxtam= pos+tp;
  int i, xi;
 
  // aux en la palabra que saca de la oracion de texto en base al tamanio de la palabra buscada 
  for ( i= pos, xi=0; i<= auxtam  ; i++,xi++){
    aux[xi]=tolower(texto[i]);

  }
  aux[tp]='\0';
  if ( strcmp(aux,palabra) ==0 )
      return 1;
  else
      return 0;
}


void creaArchivoCantPalabras(char palabra[60], int cantidad, int nodo){
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
	//printf("%s\n",nombreArchivoCuentas(nodo));
 	fp = fopen ( nombreArchivoCuentas(nodo), "a+" );
	
 	fputs(dest,fp);
 	
 	fclose ( fp );
}

void cuentaPalabras(char palabra[60], int nodo){
	FILE *archivo;	
 	char texto[500];
	int tp, tam;
	int contador=0;
 	char word[60]={0}; 
 	
	archivo = fopen(_libroTXT,"r");
 	
 	if (archivo == NULL)
 		exit(1);
 	else{
		
		tp=strlen(palabra);
		for(int j = 0; j <= tp ; j++){// comvierte en minuscula la palabra
			word[j]=tolower(palabra[j]);
		}
 	 
 	    while (feof(archivo) == 0){
			
			// fila se copia en texto
			fgets(texto,500,archivo);
			
			
            tam=strlen(texto);
			
			//printf("\n palabra: %i texto: %i \n\n", tp,tam);  
			
			//Se usa un for para mandar la posicion i a loencontre() de tal manera que el 
			//concatene las siguientes palabras en base al tamo e la palabra buscada  
			for (int i=0; i< tam ; i++){
			   
			   if ( loencontre(i,tp,texto,word)){
				   contador++;
	            }
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
	printf("%s\n",nombreArchivoNodo(nodo));
	int cantidad = cantFilas( nombreArchivoNodo(nodo))-1;
	archivo = fopen(nombreArchivoNodo(nodo),"r");
	
	//int cantidad = cantPalabras(_diccionarioParticular)-2;
	//archivo = fopen(_diccionarioParticular,"r");
	
	if (archivo == NULL)
        {
            printf("\nError de apertura del archivo. \n\n");
        }
        else
        {
            
            while(cont < cantidad)
	    	{		
				printf("entrooo \n\n");
				cont++;
				//Obtiene palabra
			    fscanf(archivo,"%s",&caracter);
				// Obtiene definicion 
				fgets(definicion,500,archivo);
			    
				cuentaPalabras(caracter,nodo);

	    	}
        }   
	fclose(archivo);	
}


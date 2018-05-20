#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Direccion del libro 
#define libroTXT "libro_medicina.txt"

// Direccion archivo generado con cuentas de palabras
#define countWord "palabras_contabilizadas.txt"

// Direccion de diccionario de palabras 
#define diccionario "palabras_libro_medicina.txt"

		/****************************************************

			LISTA DE FUNCIONES Y PROCEDIMIENTOS COMENTADAS

		*****************************************************/

//--------------METODOS GLOBALES--------------
//Metodo ara convertir cadena de caracteres de mayusculas a minusculas
char *strlwr(char *str);

//Elimina ficheros con la direccion dada
void eliminaFichero(char dirArchivo[100]);

//-------------------------------------------		


//------METODOS PARA GENERAR ARCHIVOS DE PALABRAS CONTABILIZADAS------------
//Metodo para obtener la palabra el diccionario y llama metodo cuentapalabras() para contabilizar de una vez
void obtinePalabraDiccionario();		
		
//Metodo compara la palabra a buscar con la oracion que se le manda
// pos = es la posicion de la oracion de la fila 
// tp = tamao de la palabra que estoy buscando
// texto = fila del libro en el que  se busca lapalabra que no tendra mas de 500 caracteres
// palabra = es la palabra a buscar 
int loencontre(int pos, int tp, char texto[500], char palabra[]);

//Metodo para crear un archivo con las palabras contabilizadas 
void creaArchivoCantPalabras(char palabra[], int cantidad);

// Metodo llama a loencontre() al cual le pasa la oracion de la fila, lleva la cuenta de la ocurrencia de las palabras
// Este metodo procesa solo 1 palabra
// tambien llama metodo creaArchivoCantPalabras(), pasandole la palabra y la cantidad encontrada para que lo aaa al archivo
void cuentaPalabras(char palabra[]);
 
 //------------------------------------------------------------------
 
//---------------METODOS PARA DIVIDIR PALABRAS DEL DICCIONARIO PARA LOS NODOS---------USO PARA EL COORDINADOR------

//devuelve un int con la cantidad de palabras del diccionario
int cantPalabras();

// Metodo para crear archivos para diicionario particular tantos archivos como nodos haya para la reparticion de las palabras
void archivoPalabrasXnodo(int cantNodos);

//crea archivo con nombre de nodo y carga con palabras particulares, el cual llama creaArchivoDiccionarioNombre()
void creaArchivoDiccionarioNombre(int nodo, char texto[500]);

//-------------------------------------------------------------------------------- 
 
		/*******************************************

							MAIN

		*******************************************/
 
void main(){
 	//obtineDefinicionDiccionario();
	//obtinePalabraDiccionario();
    archivoPalabrasXnodo(6);
}

		/*******************************************

			        FUNCIONES Y PROCEDIMIENTOS

		*******************************************/
		
char *strlwr(char *str){
  unsigned char *p = (unsigned char *)str;

  while (*p) {
     *p = tolower((unsigned char)*p);
      p++;
  }

  return str;
}		
		
int loencontre(int pos, int tp, char texto[500], char palabra[]) {
  char aux[20];
  char auxword[]={0};
  int auxtam= pos+tp;
  int i, xi,j;
 
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
 	char texto[500];
	int tp, tam;
	int contador=0;
 	
 	archivo = fopen(libroTXT,"r");
 	
 	if (archivo == NULL)
 		exit(1);
 	else{
 	 
 	    while (feof(archivo) == 0){
			
			// fila se copia en texto
			fgets(texto,500,archivo);
			
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

void obtinePalabraDiccionario(){
	FILE *archivo;
	char *caracter[20]={0};
	char definicion[500]={0};
	
	archivo = fopen(diccionario,"r");
	
	if (archivo == NULL)
        {
            printf("\nError de apertura del archivo. \n\n");
        }
        else
        {
            
            while(feof(archivo) == 0)
	    	{		
				//Obtiene palabra
			    fscanf(archivo,"%s",&caracter);
				// Obtiene definicion 
				fgets(definicion,500,archivo);
			    
				cuentaPalabras(strlwr(caracter));
				//libera el espacio de memoria reservado de memoria
				free(caracter);
	    	}
        }   
	fclose(archivo);	
}

void obtineDefinicionDiccionario(char caracter, char definicion[500]){
		
}

int cantPalabras(){
	FILE *archivo;
	char definicion[500]={0};
	int cont = 0;
	
	archivo = fopen(diccionario,"r");
	
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

char* nombreArchivoNodo(int nodo){
	char numNodo[2] = {0};
	char* nombreArchNodo = malloc(35);
	//transforma int a char
	sprintf(numNodo,"%d",nodo);
	
	//crea el nombre del archivo para el nodo
	strcpy(nombreArchNodo,"diccionario_palabras_nodo_");
	strcat(nombreArchNodo,numNodo);
	strcat(nombreArchNodo,".txt");
	
	return nombreArchNodo;
}

void creaArchivoDiccionarioNombre(int nodo, char texto[500]){
	FILE *archivo;
	char numNodo[2] = {0};
	char nombreArchNodo[50];
  
    strcpy(nombreArchNodo,nombreArchivoNodo(nodo));
	
	archivo = fopen(nombreArchNodo,"a+");
	
	fputs(texto,archivo);
	//Libera el espacio de memoria reservado para el nombre del achivo 
	//free(nombreArchNodo);
	
	fclose(archivo);
}


void archivoPalabrasXnodo(int cantNodos){
	FILE *archivo;
	char definicion[500]={0};
	int totalPalabras = cantPalabras();
	// Divide la cantidad de palabras totales en basse a la cantidad de nodos existentes
	int cantidadParticular = totalPalabras/cantNodos + 1;
	// variable usada para llevar cuenta de la cantidad de palabras que se han aniadido al dicionario particular del nodo
	int sumPalabras =0;
	// variable usada para saber a que nodo se le estan asignado la lista e palabras 
	int nodoActual =1;
	
	archivo = fopen(diccionario,"r");
	
	if (archivo == NULL){
            printf("\nError de apertura del archivo. \n\n");
        }
        else{  
            while(feof(archivo) == 0)
	    	{		
				// Obtiene toda la fila 
				fgets(definicion,500,archivo);
				if (sumPalabras < cantidadParticular){
					creaArchivoDiccionarioNombre(nodoActual,definicion);
					sumPalabras++;
				}
				else {
					sumPalabras =0;
					nodoActual++;
				}
				
	    	}
        }   
	fclose(archivo);
}

void eliminaFichero(char dirArchivo[100]){
	
	if(remove(dirArchivo)==0) // Eliminamos el archivo
        printf("El archivo fue eliminado satisfactoriamente\n");
    else
        printf("No se pudo eliminar el archivo\n");
}

 
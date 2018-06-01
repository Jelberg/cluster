#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <mpi.h>
/***************************

PROGRAMA QUE MANDA A LOS NODOS LOS LIBROS Y LAS PALABRAS PARTICULARES

****************************/


//                             PARAMETROS FIjOS INICIAN CON "_"
//
#define _archivoXproc "/local_home/jelberg.12/diccionario_palabras_proc_"

// Direccion del libro 
#define _libroTXT "/local_home/jelberg.12/libro_medicina.txt"

// Direccion archivo generado con cuentas de palabras
#define _countWord "/local_home/jelberg.12/palabras_contabilizadas_"

// Direccion de diccionario de palabras 
#define _diccionario "/local_home/jelberg.12/palabras_libro_medicina.txt"

// Direccion de diccionrios particulares del nodo 
#define _diccionarioParticular "/local_home/jelberg.12/Palabras_Grupo02.txt"

		/***************************************************************

			      LISTA DE FUNCIONES Y PROCEDIMIENTOS COMENTADAS

		****************************************************************/

//##################METODOS COMUNES--------------

//Elimina ficheros con la direccion dada
void eliminaFichero(char dirArchivo[100]);

//Metodo para renombrar ficheros
void remonbraFichero(char nombreViejo[100], char nombreNuevo[100]);


//-------------------------------------------		


//###############   METODOS USADOS PARA EL ENVIO Y RECEPCION DE ARCHIVOS----------

//Metodo para saber la antidad de caracteres que posee el fichero dado
int cantidadCaracteres(char dir[]);

//Metodo para enviar archivos, mana caracter a caracter, se supone que es un metodo generalizado
// nodo = emisor 
// dir = direccion del archivo que se va a mandar 
void emisorArchivo(int nodo, char dir[]);

//Metodo para recepcion de archivos, funciona en conjunto con emisorArchivo(); Crea un archivo nuevo, dado una direccion
//el cual se encarga de copiar caracter a caracter de los que manda el emisor
// nodo = receptor, dir = direccion en donde se va a crear el archivo 
void receptorArchivo(int nodo, char dir[]);

//--------------------------------------------------------------------------
 
//##########   METODOS PARA DIVIDIR PALABRAS DEL DICCIONARIO PARA LOS NODOS---------USO PARA EL COORDINADOR------

//devuelve un int con la cantidad de filas del archivo dado
//IMPORTANTE : cuando llega al final del documento el asume el salto de linea hay que contabilizarlo
int cantFilas(char nombre[50]);

// Metodo divide las palabras el diccionario y llama a creaArchivoDiccionarioNombre() para que genere el archivo correspondiente al nodo
// cantNodos = es la cantidad de nodos por las que se va a dividir el archivo 
void archivoPalabrasXnodo(int cantNodos);// este es el principal 

//crea archivo con nombre de nodo y carga con palabras particulares
void creaArchivoDiccionarioNombre(int nodo, char texto[500]);

//-------------------------------------------------------------------------------- 
 
		/*************************************************************

							        MAIN

		**************************************************************/
 
void main(int argc, char** argv){
	
	//obtinePalabraDiccionario(1); // Contabiliza las palabras, lo ejecutan los demas nodos 
    //archivoPalabrasXnodo(6);  //Metodo independiente por que es el que activa el nodo coordinador el cual crea 6 archivos
	//sustituir(1); //Antes de sustituir de debe ejecutar archivosPalabrasXnodo porque generan los diccionarios partidular y el cual ejecuta nodo 1 por qe es el parametro que se le pasa
	MPI_Init(&argc, &argv);
	
	int my_id, nproc,i;
	double tiempo_inicial, tiempo_final;
    char numNodo[2] = {0};
	char nombreArchNodo[60];

	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	
	
	if (my_id == 0) {
		
		int i = 2; //Empieza en 3 porque el nodo 1 tiene ya lso archivos libro y diccionario, 
		// Node 1 tiene el proceso 1 y 0 que es el coordinador		
		printf("**************INICIA ENVIO DE LIBRO *****************\n");
		tiempo_inicial = MPI_Wtime();
		
		while (i < nproc){// Este while es para que mande el LIBRO a los procesos 
			emisorArchivo(i, _libroTXT);
			
			i= i+2;//Porque 2 procesos comparten disco
		}
		tiempo_final = MPI_Wtime();
		printf("\n\nTARDA EN MANDAR EL LIBRO A LOS NODOS %f seg\n\n",tiempo_final);
		
		// espues de mandar el libro genera archivos con la cantidad de palabras correspondientes por nodo
		tiempo_inicial = MPI_Wtime();
		
		archivoPalabrasXnodo(nproc-1);
		
		tiempo_final = MPI_Wtime();
		printf("\n\nTARDA EN generar el archivo de palabras particulares %f seg\n\n",tiempo_final);
		
		printf("**************finaliza ENVIO DE LIBRO *****************\n");
		
		printf("**************INICIA ENVIO DE PALABRAS *****************\n");
		i=2;
		
		tiempo_inicial = MPI_Wtime();
		
		while (i < nproc){ // para enviar palabras pariculares
		
			sprintf(numNodo,"%d",i); //trasnforma int  a char 
			strcpy(nombreArchNodo,_archivoXproc);
			strcat(nombreArchNodo,numNodo);
			strcat(nombreArchNodo,".txt"); //genera el nombre del archivo 
			
			emisorArchivo(i, nombreArchNodo);
			
			i++;
		}
		
		tiempo_final = MPI_Wtime();
		printf("\n\nTARDA EN ENVIAR el archivo de palabras particulares %f seg\n\n",tiempo_final);
		
		printf("**************finaliza ENVIO DE PALABRAS *****************\n");
		
	}
	else{
		
		if(my_id%2 == 0){ // Este if es especial solo para que reciban los procesos impares, ya que comparten disco con el anterior
		//Procesos reciben el LIBRO
		printf("Proceso que recibe archivo es %d\n",my_id);
		receptorArchivo(0, _libroTXT);
		}
		
		if (my_id != 1){ // para recibir palabras  particulares
		
			sprintf(numNodo,"%d",my_id); //trasnforma int  a char 
			strcpy(nombreArchNodo,_archivoXproc);
			strcat(nombreArchNodo,numNodo);
			strcat(nombreArchNodo,".txt"); //genera el nombre del archivo 
			
			receptorArchivo(0,nombreArchNodo);
			
		}
		
	}

MPI_Finalize();

	
}

		/**************************************************************

			                FUNCIONES Y PROCEDIMIENTOS

		***************************************************************/

void eliminaFichero(char dirArchivo[100]){
	
	if(remove(dirArchivo)!=0) // Eliminamos el archivo
	printf("No se pudo eliminar el archivo\n");
        
}

void remonbraFichero(char nombreViejo[100], char nombreNuevo[100]){
	
	if(rename(nombreViejo,nombreNuevo)!=0) // renombrado el archivo
         printf("No se pudo ser renombrado\n");
       
}

int cantFilas(char nombre[50]){
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




//----------------------------------------------------------------------------------------------------------


//-------------------------------crea archivos con los diccionarios particulares----------------------------

void creaArchivoDiccionarioNombre(int nodo, char texto[500]){
	FILE *archivo;
	
	char numNodo[2] = {0};
	char nombreArchNodo[60];
	sprintf(numNodo,"%d",nodo); //trasnforma int  a char 
	strcpy(nombreArchNodo,_archivoXproc);
	strcat(nombreArchNodo,numNodo);
	strcat(nombreArchNodo,".txt"); //genera el nombre del archivo 
  
	archivo = fopen(nombreArchNodo,"a+");
	//printf("\n Abrio direccion del documento abierto es: %s \n\n",nombreArchNodo);
	fputs(texto,archivo);
	//Libera el espacio de memoria reservado para el nombre del achivo 
	//free(nombreArchNodo);
	
	fclose(archivo);
}


void archivoPalabrasXnodo(int cantNodos){
	FILE *archivo;
	char definicion[500]={0};
	
	int totalPalabras = cantFilas(_diccionario);
	printf("%s\n",_diccionario);
	// Divide la cantidad de palabras totales en basse a la cantidad de nodos existentes
	int cantidadParticular = totalPalabras/cantNodos + 1;
	printf("CANTIDAD DE PALABRAS POR CADA NODO: %d\n",cantidadParticular);
	// variable usada para llevar cuenta de la cantidad de palabras que se han aniadido al dicionario particular del nodo
	int sumPalabras =0;
	// variable usada para saber a que nodo se le estan asignado la lista e palabras 
	int nodoActual =1;
	
	archivo = fopen(_diccionario,"r");
	
	if (archivo == NULL){
            printf("\nError de apertura del archivo. \n\n");
        }
        else{  
	    	//printf("\n Abrio el diccionario \n\n");
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
//---------------------------------------------------------------------------------------------------




//-------------------------------- enviar archivo------------------------------------------------

int cantidadCaracteres(char dir[]){
	FILE *archivo ;	
	char caracter;
	int xi =0;

	archivo = fopen(dir,"r");
	while(caracter=fgetc(archivo) != EOF) {
		xi++;
	}
	fclose(archivo);
	return xi;
	
	
}

//nodo= receptor
//dir= nombre e la direccion del archivo
void emisorArchivo(int nodo, char dir[]){
	FILE *file;
	int i=0;
	char caracter;
	int tam = cantidadCaracteres(dir);
	printf("Cantidad de caracteres en el documento es : %d\n", tam);
    file = fopen(dir,"r");
	printf("Inicio de proceso de emision de achivo al proceso %d\n",nodo);
	
	MPI_Send(&tam,1,MPI_INT,nodo,99,MPI_COMM_WORLD);
	
	while (i < tam){
		caracter = fgetc(file);
		
		MPI_Send(&caracter,1,MPI_CHAR,nodo,99,MPI_COMM_WORLD);
		//printf("Mando caracter : %c\n",caracter);
		i++;
	}
	printf("Fin de porceso de emision de achivo al proceso %d\n",nodo);
	fclose(file);
	
}

//nodo = receptor , direccion donde va a crearse el archivo 
void receptorArchivo(int nodo, char dir[]){
	FILE *file;
	char caracter;
	int tam;
	int i =0;
	MPI_Status status;
	
	file = fopen(dir,"a");
	//Recibe la cantidad de caracteres que se van a copiar en el fichero a crear
	printf("Inicio de porceso de RECEPCION de achivo que manda proceso %d\n",nodo);
	MPI_Recv(&tam,1,MPI_INT,nodo,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
	printf("DATO RECIBIO ES : %d\n", tam);
	
	while (i<tam){

		MPI_Recv(&caracter,1,MPI_CHAR,nodo,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
		//printf("RECIBO CARACTER: %c\n",caracter);
		i++;
		fputc(caracter,file);
	}
	printf("Fin de porceso de RECEPCION de achivo que manda proceso %d\n",nodo);
	fclose(file);
}

//-------------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <mpi.h>
/*********************************

PROGRAMA PARA MANDAR LOS ARCHIVOS CONTADOS AL NODO COORDINADOR para que lo meta en un solo archivo 

*********************************/


//                             PARAMETROS FIjOS INICIAN CON "_"

//Direccion de las palabras contabilizadas 
#define _archivoXproc "/local_home/jelberg.12/diccionario_palabras_proc_"

//Direccion del archivo de las palabras TOTALES contailizadas
#define _total "/local_home/jelberg.12/CONTADOS.txt"

// Direccion del libro 
#define _libroTXT "/local_home/jelberg.12/libro_medicina.txt"

// Direccion del libro MODIFICADO 
#define _MODIFICADO "/local_home/jelberg.12/libro_medicina_MODIFICADO.txt"

// Direccion archivo generado con cuentas de palabras
#define _countWord "/local_home/jelberg.12/palabras_contabilizadas_"

// Direccion de diccionario de palabras 
#define _diccionario "/local_home/jelberg.12/palabras_libro_medicina.txt"

// Direccion de diccionrios particulares del nodo 
#define _diccionarioParticular "/local_home/jelberg.12/Palabras_Grupo02.txt"

		/***************************************************************

			      LISTA DE FUNCIONES Y PROCEDIMIENTOS COMENTADAS

		****************************************************************/
		
int cantFilas(char nombre[50]);
void eliminaFichero(char dirArchivo[100]);

//-----------------ENVIA ARCHIVO ----------------
int cantidadCaracteres(char dir[]);
void emisorArchivo(int nodo, char dir[]);//nodo = a quien se va a mandar ; dir = direccion donde se encuentar el archivo
void receptorArchivo(int nodo, char dir[]);//nodo = de quien recibe los datos ; dir= donde se va a guardar el archivo
void receptorArchivoCoordinador(char dir[]);// dir = donde se va a guardar el archivo
//-----------------------------------------------

void archivoTotalCuentas();

		/*************************************************************

							        MAIN

		**************************************************************/		
		
		
void main(int argc, char** argv){
	
	MPI_Init(&argc, &argv);
	
	int my_id, nproc,i,j,k;

    char numNodo[2] = {0};
	char nombreArchNodo[60];
	char nombreArch[60];

	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	
	if (my_id==1) archivoTotalCuentas();
	
	MPI_Barrier(MPI_COMM_WORLD);// Espera hasta que nodo 1 genere el archivo de palabras 
	
	if (my_id == 0) {
		//archivoTotalCuentas();
		i =2;
		while (i < nproc){
		receptorArchivo(i,_total);
		i++;
		}
	}
	
	else{
		if (my_id != 1){ // para contar palabras  particulares

			sprintf(numNodo,"%d",my_id); //trasnforma int  a char 
			strcpy(nombreArch,_countWord);
			strcat(nombreArch,numNodo);
			strcat(nombreArch,".txt"); //genera el nombre del archivo 
			printf("%s\n",nombreArch);
			
			//eliminaFichero(_libroTXT);
			
			emisorArchivo(0, nombreArch);
		}
	}
	
	MPI_Finalize();
}	



//------------------------------envia archivos----------------------------------


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

//nodo = emisor , direccion donde va a crearse el archivo 
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



//------------------------------------------------------------------------------


void archivoTotalCuentas(){
	FILE *archivo, *final;
	char texto[200];
	
	char numNodo[2] = {0};
	char nombreArch[60];
	sprintf(numNodo,"%d",1); //trasnforma int  a char 
	strcpy(nombreArch,_countWord);
	strcat(nombreArch,numNodo);
	strcat(nombreArch,".txt"); //genera el nombre del archivo
	
	archivo = fopen(nombreArch,"r");
	final = fopen(_total,"a");
	
	while (fgets(texto,200,archivo) != NULL){
		fputs(texto,final);
	}
	
	fclose(archivo);
	fclose(final);
}


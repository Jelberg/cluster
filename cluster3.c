#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <mpi.h>

/*******************************************

PROGRAMA PARA SUSTITUIR LAS PALABRAS PARTICULARES DE LOS NODOS EN EL LIBRO, LA COMUNICACION ES EN FORMA DE ANILLO SECUENCIAL

*******************************************/


#define _archivoXproc "/local_home/jelberg.12/diccionario_palabras_proc_"

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


//--------------------ENVIA ARCHIVO ----------------
int cantidadCaracteres(char dir[]);
void emisorArchivo(int nodo, char dir[]);//nodo = a quien se va a mandar ; dir = direccion donde se encuentar el archivo
void receptorArchivo(int nodo, char dir[]);//nodo = de quien recibe los datos ; dir= donde se va a guardar el archivo
void receptorArchivoCoordinador(char dir[]);// dir = donde se va a guardar el archivo
//---------------------------------------------------

//-----------------SUSTITUIR-------------------------
void sustituir(int nodo);
void buscarPalabraLibro(char palabra[], char definicion[500], int row);
//------------------------------------------------------

int cantFilas(char nombre[50]);
void eliminaFichero(char dirArchivo[100]);

		/*************************************************************

							        MAIN

		**************************************************************/	

void main(int argc, char** argv){
	//-----------------------------FASE 2---------------------------------------
		
	MPI_Init(&argc, &argv);
	
	int my_id, nproc,i,j,k;
	int token=1;
	int valor;
	MPI_Status status;

	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);

	
		int next = (my_id+1)%nproc;
		int from = (my_id+nproc-1)%nproc;
		
		if (my_id ==1 ){
			sustituir(my_id);
		}
		
		MPI_Barrier(MPI_COMM_WORLD); // hasta que no termine el proceso 1 de sustituir no manda el libro
		
		if (my_id != 0){
			
		while(0 == 0){// Siguiente while es para simular el compartamiento de un anillo
	       
			if (token == my_id){
				
				token++;// TOKEN es para saber quien le toca mandar el proceso 
				
				printf("Mandando el token a my_id %d \n",next);
				
				MPI_Send(&token,1,MPI_INT,next,99,MPI_COMM_WORLD);
				
				if ((my_id%2 != 0)||(my_id== nproc-1)){ //procesos impares so los que mandan y el libro y el ultimo proceso tambien inependientemente de si es par o no
				
					printf("Proceso %d manda a proceso %d EL LIBRO\n",my_id,next);
					
					emisorArchivo(next,_libroTXT);
				}
			}
			
			else {
				
				MPI_Recv(&token,1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
				
				printf("Recibo el token que tiene valor: %d\n",token); 
				
				if (my_id%2 == 0){//procesos parea son los unicos en recibir el libro 
					
					receptorArchivo(from,_libroTXT);
					
					printf("	YO proceso %d recibo libro para modificar\n",my_id);
				}
				printf("====Sustituye proceso %d ====\n",my_id);
				sustituir(my_id);// independientemete de quien reciba el libro todos sustituyen, impares no tendran problema
			}
			
			if (token > my_id ) break;
		}//fin del while 
		printf("SALIO proceso :%d \n",my_id);
	}		
		else {// para que reciba el proceso 0 el libro del ultimo que lo mando 
			
			MPI_Recv(&token,1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
			
			receptorArchivo(from,_MODIFICADO);
		}
		//---------------------------------fin FASE 2----------------------------------------
	
	MPI_Finalize();
}
		/**************************************************************

			                FUNCIONES Y PROCEDIMIENTOS

		***************************************************************/

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

// este es especial para el coordinador ya que puede recibir de cuelquier proceso
void receptorArchivoCoordinador(char dir[]){
	FILE *file;
	char caracter;
	int tam;
	int i =0;
	MPI_Status status;
	
	file = fopen(dir,"a");
	//Recibe la cantidad de caracteres que se van a copiar en el fichero a crear
	printf("Inicio de porceso de RECEPCION de achivo para nodo0\n");
	MPI_Recv(&tam,1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
	printf("DATO RECIBIO ES : %d\n", tam);
	
	while (i<tam){

		MPI_Recv(&caracter,1,MPI_CHAR,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
		//printf("RECIBO CARACTER: %c\n",caracter);
		i++;
		fputc(caracter,file);
	}
	printf("Fin de porceso de RECEPCION de achivo PARA NODO0\n");
	fclose(file);
}


//-------------------------------------------------------------------------------------------------
		

//--------------------------------------sustituir---------------------------------------------------

void buscarPalabraLibro(char palabra[], char definicion[500], int row){
	FILE *archivo ;	
	char fila[500]={0};
	char aux[20];
	char word[60]={0};
	int tamPalbra=0;
	int tamFila =0;
	int i,tp;
	int seguir=0;
	int cont =0;
	int auxtam, xi,j;
	
 	// Direccion del libro 
 	archivo = fopen(_libroTXT,"rw+");
    int cantidad = cantFilas(_libroTXT)-1;
	 
 	if (archivo == NULL)
 		exit(1);
 	else{
 	    
 	    tp=strlen(palabra);
		for(int j = 0; j <= tp ; j++){// comvierte en minuscula la palabra
			word[j]=tolower(palabra[j]);
		}
		  //while en base a la cantidad de filas del libro 
 	      while (cont < cantidad){
			cont++;
			
			//Toma la fila del libro
			fgets(fila,500,archivo);
			
			tamPalbra = strlen(palabra);//redundante
			tamFila = strlen(fila);
			
			for (int i=0; i< tamFila ; i++){

					
					auxtam = i+tamPalbra;
					//for para concatenar la palabra  
					  for ( j= i, xi=0; j<= auxtam  ; j++,xi++){
						  
						aux[xi]=tolower(fila[j]);
						
					  }  
					  aux[tamPalbra]='\0';
										

				   if ( strcmp(aux,palabra) ==0){
					 printf("%d Se sustituyo : %s\n",row, palabra);  
					 fprintf(archivo,definicion);
					 
					 seguir=1;
				  }		
					// Esta pregunta es para que salga del ciclo for por que ya sustituyo la palabra 
					if (seguir)	break;
				
			}
			//Para romper el while y no se  quede preguntando 
			if (seguir)	break;	
 	    }

		fclose(archivo);
		
    }
    
}

void sustituir(int nodo){
	FILE *archivo;
	char *caracter[20]={0};
	char definicion[500]={0};
	int cont=0;
	
	char numNodo[2] = {0};
	
	char nombreArchNodo[60];
	sprintf(numNodo,"%d",nodo); //trasnforma int  a char 
	strcpy(nombreArchNodo,_archivoXproc);
	strcat(nombreArchNodo,numNodo);
	strcat(nombreArchNodo,".txt"); //genera el nombre del archivo 

	
	int cantidad = cantFilas(nombreArchNodo)-1;
	
	//Diireccion del diccionario particular
	archivo = fopen(nombreArchNodo,"r");
	
	if (archivo == NULL)
        {
            printf("\nError de apertura del archivo. \n\n");
        }
        else
        {
            //while en base a la cantidad de filas del diccionario particular
            while(cont < cantidad){	
			    cont++;
				//Obtiene palabra
			    fscanf(archivo,"%s",&caracter);
				// Obtiene definicion 
				fgets(definicion,500,archivo);
				//Se manda definicion y palabra bala buscar y sustituir
				buscarPalabraLibro(caracter,definicion,cont);

	
	    	}
        }   
	fclose(archivo);	
}

//-----------------------------------------------------------------------------------------

void eliminaFichero(char dirArchivo[100]){
	
	if(remove(dirArchivo)!=0) // Eliminamos el archivo
	printf("No se pudo eliminar el archivo\n");
        
}
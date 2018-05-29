#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <mpi.h>

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

//---------------CUENTA PALABRAS ---------------------------
int loencontre(int pos, int tp, char texto[500], char palabra[60]);
void creaArchivoCantPalabras(char palabra[60], int cantidad, int nodo);
void cuentaPalabras(char palabra[60], int nodo);
void obtinePalabraDiccionario(int nodo);//metodo principal
//------------------------------------------------------------

void archivoTotalCuentas();

//----------------SUSTITUIR---------------------------------------------
void sustituir(int nodo);
void buscarPalabraLibro(char palabra[], char definicion[500], int row);
//----------------------------------------------------------------------

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
	
	if (my_id == 0) {
		obtinePalabraDiccionario(1);// cuenta palabras
		//archivoTotalCuentas();
		i =2;
		while (i < nproc){
		receptorArchivo(i,_total);
		i++;
		}
	}
	
	else{
		if (my_id != 1){ // para recibir palabras  particulares
		   	obtinePalabraDiccionario(my_id); //cuenta las palabras 

			sprintf(numNodo,"%d",my_id); //trasnforma int  a char 
			strcpy(nombreArch,_countWord);
			strcat(nombreArch,numNodo);
			strcat(nombreArch,".txt"); //genera el nombre del archivo 
			
			//eliminaFichero(_libroTXT);
			
			emisorArchivo(0, nombreArch);
		}
		
		//-----------------------------FASE 2---------------------------------------
	/*	printf("*******INICIA FASE 2*******");
		int token=1;
		if (my_id != 0){
			int next = (my_id+1)%nproc;
			int from = (my_id+nproc-1)%nproc;
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
				printf("====Sustituye proceso %d ====\n",my_i);
				sustituir(my_id);// independientemete de quien reciba el libro todos sustituyen, impares no tendran problema
			}
			
			if (token > my_id ) break;
		}//fin del while 
		printf("SALIO proceso :%d \n",my_id);
	}		
		else {// para que reciba el proceso 0 el libro del ultimo que lo mando 
			
			MPI_Send(&token,1,MPI_INT,next,99,MPI_COMM_WORLD);
			
			receptorArchivo(from,_MODIFICADO);
		}
		//---------------------------------fin FASE 2----------------------------------------
		*/
	}
	
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

void eliminaFichero(char dirArchivo[100]){
	
	if(remove(dirArchivo)!=0) // Eliminamos el archivo
	printf("No se pudo eliminar el archivo\n");
        
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


//------------------------------------------------------------------------------
		
		

		
//----------------------------cuenta palabras-------------------------------------------
		
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
	
	char numNodo[2] = {0};
	char nombreArch[60];
	sprintf(numNodo,"%d",nodo); //trasnforma int  a char 
	strcpy(nombreArch,_countWord);
	strcat(nombreArch,numNodo);
	strcat(nombreArch,".txt"); //genera el nombre del archivo 
	
	//Transforma un int en un char y lo copia en la variable scantidad 
	sprintf(scantidad,"%d",cantidad);
	
	strcpy(dest,palabra);
	strcat(dest," ");
	strcat(dest,scantidad);
	strcat(dest,"\n");

 	fp = fopen ( nombreArch, "a+" );
	
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

void obtinePalabraDiccionario(int nodo){
	FILE *archivo;
	char *caracter[60]={0};
	char definicion[500]={0};
	int cont =0;
	
	char numNodo[2] = {0};
	char nombreArchNodo[60];
	sprintf(numNodo,"%d",nodo); //trasnforma int  a char 
	strcpy(nombreArchNodo,_archivoXproc);
	strcat(nombreArchNodo,numNodo);
	strcat(nombreArchNodo,".txt"); //genera el nombre del archivo 
	
	int cantidad = cantFilas( nombreArchNodo);
	archivo = fopen(nombreArchNodo,"r");
	
	
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
			    
				cuentaPalabras(caracter,nodo);

	    	}
        }   
	fclose(archivo);	
}


//--------------------------------------------------------------------------------------	


//------------------------------genera archivo total de cuentas solo para proceso 1--------------------

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

//-----------------------------------------------------------------------------------------------------	

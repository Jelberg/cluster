#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <mpi.h>

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
//Metodo ara convertir cadena de caracteres de mayusculas a minusculas
char *strlwr(char *str);

//Elimina ficheros con la direccion dada
void eliminaFichero(char dirArchivo[100]);

//Metodo para renombrar ficheros
void remonbraFichero(char nombreViejo[100], char nombreNuevo[100]);

// Devuelve nombre de archivo para el diccionario particular de los nodos 
//char* nombreArchivoNodo(int nodo);

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


//############METODOS PARA GENERAR ARCHIVOS DE PALABRAS CONTABILIZADAS------------
// devuelve un 1 si encuentra la palabra
int loencontre(int pos, int tp, char texto[500], char palabra[60]);

//crea el archivo de las palabras contadas del libro
void creaArchivoCantPalabras(char palabra[60], int cantidad, int nodo);

//busca las filas del libro el cal manda a loencontre para recorrer y comparar 
void cuentaPalabras(char palabra[60], int nodo);

//Saca las palabras del diccionario para pasarlo a cuentaPalabras
void obtinePalabraDiccionario(int nodo); // funcion principal
 
 //------------------------------------------------------------------
 
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
 
//################  METODO PARA SUSTIRUIR DEFINICIONES EN LIBRO -----------------

//Metodo compara palabra por palabra hasta conseguir la buscada y aniade definicion  
void buscarPalabraLibro(char palabra[], char definicion[500],int row);

//Metedo que saca palabra y definicion del diccionario particular para mandarlo al metodo buscaPalabraLibro()
// nodo = es el numero de nodo que va a ejecutarla sustitucion 
void sustituir(int nodo);//metdod principal 

//--------------------------------------------------------------------------------------
 
		/*************************************************************

							        MAIN

		**************************************************************/
 
void main(int argc, char** argv){
	
	//obtinePalabraDiccionario(1); // Contabiliza las palabras, lo ejecutan los demas nodos 
    //archivoPalabrasXnodo(6);  //Metodo independiente por que es el que activa el nodo coordinador el cual crea 6 archivos
	//sustituir(1); //Antes de sustituir de debe ejecutar archivosPalabrasXnodo porque generan los diccionarios partidular y el cual ejecuta nodo 1 por qe es el parametro que se le pasa
	MPI_Init(&argc, &argv);
	
	int my_id, nproc,i;
    char numNodo[2] = {0};
	char nombreArchNodo[60];

	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	
	
	if (my_id == 0) {
		
		int i = 2; //Empieza en 3 porque el nodo 1 tiene ya lso archivos libro y diccionario, 
		// Node 1 tiene el proceso 1 y 0 que es el coordinador		
		printf("**************INICIA ENVIO DE LIBRO *****************\n");
		while (i < nproc){// Este while es para que mande el LIBRO a los procesos 
			emisorArchivo(i, _libroTXT);
			
			i= i+2;//Porque 2 procesos comparten disco
		}
		// espues de mandar el libro genera archivos con la cantidad de palabras correspondientes por nodo
		archivoPalabrasXnodo(nproc-1);
		printf("**************finaliza ENVIO DE LIBRO *****************\n");
		
		printf("**************INICIA ENVIO DE PALABRAS *****************\n");
		i=2;
		while (i < nproc){ // para enviar palabras pariculares
		
			sprintf(numNodo,"%d",i); //trasnforma int  a char 
			strcpy(nombreArchNodo,_archivoXproc);
			strcat(nombreArchNodo,numNodo);
			strcat(nombreArchNodo,".txt"); //genera el nombre del archivo 
			
			emisorArchivo(i,nombreArchNodo);
			i++;
		}
		printf("**************finaliza ENVIO DE PALABRAS *****************\n");
	}
	else{
		if (my_id == 1){
			//obtinePalabraDiccionario(1);
		}
		
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
			
			obtinePalabraDiccionario(my_id); //cuenta las palabras 
		}
		
	}

MPI_Finalize();

	
}

		/**************************************************************

			                FUNCIONES Y PROCEDIMIENTOS

		***************************************************************/
		
char *strlwr(char *str){
  unsigned char *p = (unsigned char *)str;

  while (*p) {
     *p = tolower((unsigned char)*p);
      p++;
  }

  return str;
}		

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
	
	printf("%s\n",_diccionario);
	int totalPalabras = cantFilas(_diccionario)-1;
	// Divide la cantidad de palabras totales en basse a la cantidad de nodos existentes
	int cantidadParticular = totalPalabras/cantNodos + 1;
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




//--------------------------------------sustituir---------------------------------------------------

void buscarPalabraLibro(char palabra[], char definicion[500], int row){
	FILE *archivo ;	
	char fila[500]={0};
	char aux[20];
	int tamPalbra=0;
	int tamFila =0;
	int i;
	int seguir=0;
	int cont =0;
	int auxtam, xi,j;
	
 	// Direccion del libro 
 	archivo = fopen(_libroTXT,"rw+");
    int cantidad = cantFilas(_libroTXT)-1;
	 
 	if (archivo == NULL)
 		exit(1);
 	else{
		  //while en base a la cantidad de filas del libro 
 	      while (cont < cantidad){
			cont++;
			
			//Toma la fila del libro
			fgets(fila,500,archivo);
			
			tamPalbra = strlen(palabra);
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
	
	
	//int cantidad = cantFilas("Palabras_Grupo02.txt")-2;
	//archivo = fopen("Palabras_Grupo02.txt","r");
	
	
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
				buscarPalabraLibro(strlwr(caracter),definicion,cont);

	
	    	}
        }   
	fclose(archivo);	
}
//-----------------------------------------------------------------------------------------------





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
	printf("%s\n",nombreArch);
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
	
	int cantidad = cantFilas( nombreArchNodo)-1;
	archivo = fopen(nombreArchNodo,"r");
	
	
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


//--------------------------------------------------------------------------------------
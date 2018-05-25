#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <mpi.h>

//                             PARAMETROS FIjOS INICIAN CON "_"
// Path en general
#define _palabrasXproc "/local_home/jelberg.12/diccionario_palabras_proc_"

// Direccion del libro 
#define _libroTXT "/local_home/jelberg.12/libro_medicina.txt"

// Direccion archivo generado con cuentas de palabras
#define _countWord "/local_home/jelberg.12/palabras_contabilizadas_"

// Direccion archivo generado con cuentas de palabras
#define _cuentas "/local_home/jelberg.12/count_words.txt"

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

//Genera nombre de archivo de las palabras contabiliadas
//char* nombreArchivoCuentas(int nodo);

//-------------------------------------------		


//###############   METODOS USADOS PARA EL ENVIO Y RECEPCION DE ARCHIVOS----------

//Metodo para saber la antidad de caracteres que posee el fichero dado
int cantidadCaracteres(char dir[300]);

//Metodo para enviar archivos, mana caracter a caracter, se supone que es un metodo generalizado
// nodo = emisor 
// dir = direccion del archivo que se va a mandar 
void emisorArchivo(int nodo, char dir[300]);

//Metodo para recepcion de archivos, funciona en conjunto con emisorArchivo(); Crea un archivo nuevo, dado una direccion
//el cual se encarga de copiar caracter a caracter de los que manda el emisor
// nodo = receptor, dir = direccion en donde se va a crear el archivo 
void receptorArchivo(int nodo, char dir[300]);

//Metodo de recepcion de archivo solo para el corrdinador, dir es la direccion nueva de donde se va a guardar los datos recibidos
void receptorArchivoCoordinador(char dir[300]);

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

//############METODOS PARA GENERAR ARCHIVOS DE PALABRAS CONTABILIZADAS------------

//Metodo para obtener la palabra el diccionario y llama metodo cuentapalabras() para contabilizar de una vez
//nodo = es el nodo que esta ejecutando la contabilizacion
void obtinePalabraDiccionario(int nodo); //esta es la funcion principal	
		
//Metodo compara la palabra a buscar con la oracion que se le manda
// pos = es la posicion de la oracion de la fila 
// tp = tamao de la palabra que estoy buscando
// texto = fila del libro en el que  se busca lapalabra que no tendra mas de 500 caracteres
// palabra = es la palabra a buscar 
int loencontre(int pos, int tp, char texto[500], char palabra[100]);

//Metodo para crear un archivo con las palabras contabilizadas 
void creaArchivoCantPalabras(char palabra[100], int cantidad, int nodo);

// Metodo llama a loencontre() al cual le pasa la oracion de la fila, lleva la cuenta de la ocurrencia de las palabras
// Este metodo procesa solo 1 palabra
// tambien llama metodo creaArchivoCantPalabras(), pasandole la palabra y la cantidad encontrada para que lo aaa al archivo
void cuentaPalabras(char palabra[100], int nodo);
 
 //------------------------------------------------------------------
 
 
//################  METODO PARA SUSTIRUIR DEFINICIONES EN LIBRO -----------------

//Metodo compara palabra por palabra hasta conseguir la buscada y aniade definicion  
void buscarPalabraLibro(char palabra[100], char definicion[500],int row);

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
	
	int my_id, nproc, i;
	char numNodo[2] = {0};
	char nombreArchNodo[60];

	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	
//------------------------------FASE 1 ----------------------------------	
	if (my_id == 0) {
		
		i = 2; //Empieza en 2 porque el nodo 1 tiene ya lso archivos libro y diccionario, 
		// Node 1 tiene el proceso 1 y 0 que es el coordinador		
		
		while (i < nproc){// Este while es para que mande el LIBRO a los procesos 
			emisorArchivo(i, _libroTXT);
			
			i= i+2;//Porque 2 procesos comparten disco y  estan ligados a su impar siguiente
		}
		// espues de mandar el libro genera archivos con la cantidad de palabras correspondientes por nodo
		archivoPalabrasXnodo(nproc-1);
		
		i = 2; //Tiene que empezar en 2 porque proceso 1 ya tiene su diccionario  
		while (i < nproc){ // While para mandar DICCIONARIO partciculares 	
			//transforma int a char
			sprintf(numNodo,"%d",i);
			//crea el nombre del archivo para el nodo
			strcpy(nombreArchNodo,_countWord);
			strcat(nombreArchNodo,numNodo);
			strcat(nombreArchNodo,".txt");
			printf("ESTAAAAA ES LA DIRECCION 0 %s",nombreArchNodo);
			emisorArchivo(i,nombreArchNodo);
			//eliminaFichero(nombreArchivoNodo(i)); //Elimina el fichero que no es propio del nodo 1
			i++; // Este si aumenta de 1 en 1 por que todos los procesos deberian de tener sus diccionarios
		}
		
		obtinePalabraDiccionario(1); //Una vez mandado los palabras puede empezar a contabilizar
		
		//receptorArchivoCoordinador(_cuentas);// Recibe de cualquier proceso 
	}
	else { 
	    
		if(my_id%2 == 0){ // Este if es especial solo para que reciban los procesos pares, ya que comparten disco con el proceso impar siguiente 
		//Procesos reciben el LIBRO
			printf("Proceso que recibe archivo de Libro: %d\n",my_id);
			receptorArchivo(0, _libroTXT);
		}
		
		if (my_id != 1){ // Para RECIBIR DICCIONARIO particular
			printf("Proceso que recibe archivo de Diccionario particula: %d\n",my_id);
			
			sprintf(numNodo,"%d",my_id);
			//crea el nombre del archivo para el nodo
			strcpy(nombreArchNodo,_countWord);
			strcat(nombreArchNodo,numNodo);
			strcat(nombreArchNodo,".txt");
			printf("ESTAAAAA ES LA DIRECCION %s",nombreArchNodo);
			receptorArchivo(0,nombreArchNodo);
			obtinePalabraDiccionario(my_id);
		}	
		/*
		emisorArchivo(0,nombreArchivoCuentas(my_id)); //Manda al coordinador el txt con palabras contabilizadas 
		*/
	}
// ---------------------------FIN FASE 1------------------------------


//   --------------------------FASE 2----------------------------------
	
	if (my_id != 0){
		
	}
	else {
		
	}
	
//-----------------------------FIN FASE 2-----------------------------
MPI_Finalize();

	
}

		/**************************************************************

			                FUNCIONES Y PROCEDIMIENTOS

		***************************************************************/

/*char* nombreArchivoNodo(int nodo){
	char numNodo[2] = {0};
	//char* nombreArchNodo = malloc(60);
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
	//char* nombreArchNodo = malloc(60);
	//transforma int a char
	sprintf(numNodo,"%d",nodo);
	
	//crea el nombre del archivo para el nodo
	strcpy(nombreArchNodo,_countWord);
	strcat(nombreArchNodo,numNodo);
	strcat(nombreArchNodo,".txt");
	
	return nombreArchNodo;
}*/

		
char *strlwr(char *str){
  unsigned char *p = (unsigned char *)str;

  while (*p) {
     *p = tolower((unsigned char)*p);
      p++;
  }

  return str;
}		
		
int loencontre(int pos, int tp, char texto[500], char palabra[100]) {
  char aux[60];

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
	strcpy(nombreArchNodo,_countWord);
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
	int contador=0;
 	
 	archivo = fopen(_libroTXT,"r");
 	
 	if (archivo == NULL)
 		exit(1);
 	else{
 	 
 	    while (feof(archivo) == 0){
			
			// fila se copia en texto
			fgets(texto,500,archivo);
			
			tp=strlen(palabra);
            tam=strlen(texto);
			
			//Se usa un for para mandar la posicion i a loencontre() de tal manera que el 
			//concatene las siguientes palabras en base al tamo e la palabra buscada  
			for (int i=0; i< tam ; i++){
			   
			   if ( loencontre(i,tp,texto,palabra)){
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
	
	//........para generar el nombre del archivo .......
	
	char numNodo[2] = {0};
	char nombreArchNodo[60];
	//transforma int a char
	sprintf(numNodo,"%d",nodo);
	
	//crea el nombre del archivo para el nodo
	strcpy(nombreArchNodo,_palabrasXproc);
	strcat(nombreArchNodo,numNodo);
	strcat(nombreArchNodo,".txt");
	
	//..................................................
	
	printf("\n  %s \n",nombreArchNodo );
	printf("         Comienza a contabilizar palabras en proceso: %d \n",nodo);
	int cantidad = cantFilas(nombreArchNodo)-2;
	printf("         Cantidad de filas del diccionario de proceso %d es: %d \n",nodo,cantidad);
	archivo = fopen(nombreArchNodo,"r");
	
	
	//int cantidad = cantFilas(_diccionarioParticular)-2;
	//archivo = fopen(_diccionarioParticular,"r");
	
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


void creaArchivoDiccionarioNombre(int nodo, char texto[500]){
	FILE *archivo;
    
	//........para generar el nombre del archivo .......
	
	char numNodo[2] = {0};
	char nombreArchNodo[60];
	//transforma int a char
	sprintf(numNodo,"%d",nodo);
	
	//crea el nombre del archivo para el nodo
	strcpy(nombreArchNodo,_palabrasXproc);
	strcat(nombreArchNodo,numNodo);
	strcat(nombreArchNodo,".txt");
	
	//..................................................
	
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

void eliminaFichero(char dirArchivo[100]){
	
	if(remove(dirArchivo)!=0) // Eliminamos el archivo
	printf("No se pudo eliminar el archivo\n");
        
}

void remonbraFichero(char nombreViejo[100], char nombreNuevo[100]){
	
	if(rename(nombreViejo,nombreNuevo)!=0) // renombrado el archivo
         printf("No se pudo ser renombrado\n");
       
}


void buscarPalabraLibro(char palabra[100], char definicion[500], int row){
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
	
	//........para generar el nombre del archivo .......
	
	char numNodo[2] = {0};
	char nombreArchNodo[60];
	//transforma int a char
	sprintf(numNodo,"%d",nodo);
	
	//crea el nombre del archivo para el nodo
	strcpy(nombreArchNodo,_palabrasXproc);
	strcat(nombreArchNodo,numNodo);
	strcat(nombreArchNodo,".txt");
	
	//..................................................
	
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


int cantidadCaracteres(char dir[300]){
	FILE *archivo ;	
	char caracter;
	int xi =0;

	archivo = fopen(dir,"r");
	printf("DIRECCION %s\n", dir);
	while(caracter=fgetc(archivo) != EOF) {
		xi++;
	}
	fclose(archivo);
	return xi;
	
	
}

//nodo= receptor
//dir= nombre e la direccion del archivo
void emisorArchivo(int nodo, char dir[300]){
	FILE *file;
	int i=0;
	char caracter;
	int tam = cantidadCaracteres(dir);
	printf("Cantidad de caracteres en el documento es : %d\n", tam);
    file = fopen(dir,"r");
	
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
void receptorArchivo(int nodo, char dir[300]){
	FILE *file;
	char caracter;
	int tam;
	int i =0;
	MPI_Status status;
	
	file = fopen(dir,"a");
	//Recibe la cantidad de caracteres que se van a copiar en el fichero a crear
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

void receptorArchivoCoordinador(char dir[300]){
	FILE *file;
	char caracter;
	int tam;
	int i =0;
	MPI_Status status;
	
	file = fopen(dir,"a");
	//Recibe la cantidad de caracteres que se van a copiar en el fichero a crear
	MPI_Recv(&tam,1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
	printf("DATO RECIBIO ES : %d\n", tam);
	
	while (i<tam){

		MPI_Recv(&caracter,1,MPI_CHAR,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
		//printf("RECIBO CARACTER: %c\n",caracter);
		i++;
		fputc(caracter,file);
	}
	printf("Fin de porceso de RECEPCION de achivo en COORDINADOR\n");
	fclose(file);
}
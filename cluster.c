#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Direccion del libro 
#define libroTXT "archivo.txt"

// Direccion archivo generado con cuentas de palabras
#define countWord "palabras_contabilizadas.txt"

// Direccion de diccionario de palabras 
#define diccionario "Palabras_Grupo02.txt"

		/****************************************************

			LISTA DE FUNCIONES Y PROCEDIMIENTOS COMENTADAS

		*****************************************************/

//--------------METODOS GLOBALES--------------
//Metodo ara convertir cadena de caracteres de mayusculas a minusculas
char *strlwr(char *str);

//-------------------------------------------		

//------METODOS PARA GENERAR ARCHIVOS DEPALABRAS CONTADAS------------
//Metodo para obtener la palabra el diccionario 
void obtinePalabraDiccionario();		
		
//Metodo compara la palabra a buscar con la oracion que se le manda
int loencontre(int pos, int tp, char texto[300], char palabra[]);

//Metodo para crear un archivo con las palabras contabilizadas 
void creaArchivoCantPalabras(char palabra[], int cantidad);

// Metodo llama a loencontre al cual le pasa la oracion de la fila, lleva la cuenta de la ocurrencia de las palabras
// Este metodo procesa solo 1 palabra
void cuentaPalabras(char palabra[]);
 
 //------------------------------------------------------------------
 
		/*******************************************

							MAIN

		*******************************************/
 
void main(){
 	cuentaPalabras("hoLa");
	obtinePalabraDiccionario();
}

		/*******************************************

			        FUNCIONES Y PROCEDIMIENTOS

		*******************************************/

		
char *strlwr(char *str)
{
  unsigned char *p = (unsigned char *)str;

  while (*p) {
     *p = tolower((unsigned char)*p);
      p++;
  }

  return str;
}		
		
int loencontre(int pos, int tp, char texto[300], char palabra[]) {
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
 	char texto[300];
	int tp, tam;
	int contador=0;
 	
 	archivo = fopen(libroTXT,"r");
 	
 	if (archivo == NULL)
 		exit(1);
 	else{
 	 
 	    while (feof(archivo) == 0){
			
			// fila se copia en texto
			fgets(texto,300,archivo);
			
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
	char word[300]={0};
	int fElement =0;
	
	archivo = fopen(diccionario,"r");
	
	if (archivo == NULL)
        {
            printf("\nError de apertura del archivo. \n\n");
        }
        else
        {
            
            while(feof(archivo) == 0)
	    	{		
				//Toma la primera palabra 
			    fscanf(archivo,"%s",&caracter);
				// Necesario para poderer mover el apuntador al final de la fila
				fgets(word,300,archivo);
				
			
				printf("%s\n", strlwr(caracter));
			
	    	}
        }   
	fclose(archivo);	
}
 
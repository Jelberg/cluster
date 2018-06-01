#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void archivoTotalCuentas();

void main(int argc, char** argv){
	

	archivoTotalCuentas();
	
}




void archivoTotalCuentas(){
	FILE *archivo, *final;
	char texto[200];
	
	/*char numNodo[2] = {0};
	char nombreArch[60];
	sprintf(numNodo,"%d",1); //trasnforma int  a char 
	strcpy(nombreArch,_countWord);
	strcat(nombreArch,numNodo);
	strcat(nombreArch,".txt"); //genera el nombre del archivo
	*/
	
	archivo = fopen("hola.txt","r");
	final = fopen("copia.txt","a");
	
	while (fgets(texto,200,archivo) != NULL){
		printf("ENTROOOO\n");
		fputs(texto,final);
	}
	
	fclose(archivo);
	fclose(final);
}


#include <stdio.h>
#include <stdlib.h>
/*
-------------------------------------------

PROGRAMA PARA MANEJOO DE FICHEROS

-------------------------------------------

*/ 

main(int argc, char** argv)
{
	FILE *fp;
	fp = fopen ( "fichero.out", "w" );  
	// Agrega la siguiente linea en el fichero creado      
	fputs ("Salida para fichero",fp);
	fputs ("\n2DA LINEA DEL FICHERO",fp);
	fclose ( fp );

}


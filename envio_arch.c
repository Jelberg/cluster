#include <stdio.h>
#include <mpi.h>


int my_id;
FILE *arch;
int tam;
int tag =99;

MPI_Status status;

int tamFichero(){
	FILE *fich;

  fich=fopen("/local_home/jelberg.12/hola.txt","r");

  fseek(fich, 0L, SEEK_END);

  return ftell(fich);
  fclose(fich);
}

void main (int argc, char** argv)
{
	MPI_Init(&argc, &argv);
	
	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	
	if (my_id == 0) {
		arch= fopen("/local_home/jelberg.12/hola.txt","r");
		tam = tamFichero();
		
		MPI_Send(&tam,1,MPI_INT,1,tag,MPI_COMM_WORLD);
		
		MPI_Send(&arch,tam,MPI_BYTE,1,tag,MPI_COMM_WORLD);
		
		
	}
	else if (my_id == 6) {
		
		MPI_Recv(&tam,1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
		
		MPI_Recv(&arch,tam,MPI_BYTE,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
	}

MPI_Finalize();
}
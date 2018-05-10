#include <stdio.h>
#include <mpi.h>

int my_id, nproc, tag = 99, source;
MPI_Status status;

int main (int argc, char** argv){
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	MPI_Send(&my_id,1,MPI_INT,(my_id+1)%nproc,tag,MPI_COMM_WORLD);
	
	MPI_Recv(&source,1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,
	MPI_COMM_WORLD,&status);

    //siguiente Reciv asegura que recibe de un proceso predecesor, pero no es necesario por porque el send esta bien hecho
	/* MPI_Recv(&source,1,MPI_INT,(my_id+nproc-1)%nproc,tag,
	MPI_COMM_WORLD,&status); */


	printf("%d recibio mensaje de %d\n",my_id,source);
	MPI_Finalize();
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <mpi.h>


void main(int argc, char** argv){
	
	MPI_Init(&argc, &argv);
	
	int my_id, nproc,i,j,k;

	int valor;
	MPI_Status status;

	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	
		int token=1;
		valor = 0;
		if (my_id != 0){
			int next = (my_id+1)%nproc;
			int from = (my_id+nproc-1)%nproc;
		while(0 == 0){// Siguiente while es para simular el compartamiento de un anillo
	        
	        printf("if token: %d == my_id: %d\n",token,my_id);
			if (token == my_id){
				valor=valor+1;
				token++;
				printf("Mandando el token a my_id %d \n",next);
				MPI_Send(&token,1,MPI_INT,next,99,MPI_COMM_WORLD);
				printf("Proceso %d manda a proceso %d valor %d\n",my_id,next,valor);
				MPI_Send(&valor,1,MPI_INT,next,99,MPI_COMM_WORLD);
			}
			
			else {
				MPI_Recv(&token,1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
				printf("Recibo el token que tiene valor: %d\n",token); 
				MPI_Recv(&valor,1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
				printf("	YO proceso %d recibo valor %d\n",my_id,valor);
			}
			
			if (token > my_id ) break;
		}
		printf("SALIO proceso :%d \n",my_id);
	}
	
	MPI_Finalize();
}		
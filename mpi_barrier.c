#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

// Treat the processes as in a heap.
// Parent of process n is (n-1)/2
// Left child of process n is n*2+1
// Right child of process n is n*2+2

int getParent(int rank){
	return rank == 0 ? -1 : (rank-1)/2;
}
	
int getLeftChild(int rank, int size){
	return rank*2+1 >= size ? -1 : rank*2+1;
}
	
int getRightChild(int rank, int size){
	return rank*2+2 >= size ? -1 : rank*2+2;
}

void mpi_iittp_barrier(int rank, int size){

	printf("Process %d entering barrier\n",rank);

	int temp = 0;

	int p = getParent(rank), lc = getLeftChild(rank,size), rc = getRightChild(rank,size);
	
	// If the process has 'children' in the tree, receive a message from them

	if(lc+1)
		MPI_Recv(&temp, 1, MPI_INT, lc, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
	if(rc+1)
		MPI_Recv(&temp, 1, MPI_INT, rc, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	
	// If the process has a 'parent'(all except process 0), send a message and wait for response that all processes have reached barrier
	
	if(p+1){
		MPI_Send(&temp, 1, MPI_INT, p, 0, MPI_COMM_WORLD);
		MPI_Recv(&temp, 1, MPI_INT, p, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	
	// If the process has 'children', send a message to them to show all processes have reached barrier
	
	if(lc+1)
		MPI_Send(&temp, 1, MPI_INT, lc, 0, MPI_COMM_WORLD);
	
	if(rc+1)
		MPI_Send(&temp, 1, MPI_INT, rc, 0, MPI_COMM_WORLD);
		
	printf("Process %d leaving barrier\n",rank);

	return;
}

int main(int argc, char** argv) {
	
	MPI_Init(NULL, NULL);
	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
 	int world_size;
  	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  	
  	printf("Rank : %d\n",world_rank);
  	
  	mpi_iittp_barrier(world_rank, world_size);
  	
  	MPI_Finalize();
}

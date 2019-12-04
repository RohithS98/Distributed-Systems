#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// rank - Current rank
// n - Total number of elements
// world - World size
// Returns 0 if starting element is even indexed else 1
int getStart(int rank, int n, int world){
	return ((n/world)*rank)%2;
}

int main(int argc, char** argv) {
	
	MPI_Init(NULL, NULL);
	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
 	int world_size;
  	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  	
  	srand(0);
  	
  	// n - total number of elements
  	// s - number of elements for the respective process
  	int n, s;
  	int *a;
  	
  	//Randomly assigning elements in process 0 and sending to other processes
  	
  	if(world_rank == 0){
  	
  		printf("Enter number of elements: ");
  		scanf("%d",&n);
  		a = (int *)malloc(n*sizeof(int));
  		int i;
  		for(i = 0 ; i < n ; i++){
  			a[i] = rand()%100;
  			printf("%d ",a[i]);
  		}
  		printf("\n");
  		
  		int size = n/world_size, s1 = n - (world_size-1)*size;
  		s = size;
  		
  		for(i = 1; i < world_size-1 ; i++){
  			MPI_Send(&n, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
  			MPI_Send(&size, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
  			MPI_Send(&a[size*i], size, MPI_INT, i, 0, MPI_COMM_WORLD);
  		}
  		
  		MPI_Send(&n, 1, MPI_INT, world_size-1, 0, MPI_COMM_WORLD);
  		MPI_Send(&s1, 1, MPI_INT, world_size-1, 0, MPI_COMM_WORLD);
  		MPI_Send(&a[size*(world_size-1)], s1, MPI_INT, world_size-1, 0, MPI_COMM_WORLD);
  		
  	
  	}
  	else{
  		MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  		MPI_Recv(&s, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  		a = (int *)malloc(s*sizeof(int));
  		MPI_Recv(a, s, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  	}
  	
  	bool oddPhase = false, isSorted = false;
  	
  	bool flag = true;
  	
  	while(flag){
  		int start = getStart(world_rank, n, world_size) ^ oddPhase, i, isSorted = true;
  		
  		// Local sorting for each process
  		for(i = start ; i < s - 1 ; i=i+2){
  			if(a[i] > a[i+1]){
  				int temp = a[i];
  				a[i] = a[i+1];
  				a[i+1] = temp;
  				isSorted = false;
  			}
  		}
  		
  		// Even processes send last element(if needed) to odd processes
  		if( start && world_rank != 0 ){
  			int prev;
  			MPI_Recv(&prev, 1, MPI_INT, world_rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  			if(prev > a[0]){
  				isSorted = false;
  				int temp = a[0];
  				a[0] = prev;
  				prev = temp;
  			}
  			MPI_Send(&prev, 1, MPI_INT, world_rank-1, 0, MPI_COMM_WORLD);
  		}
  		
  		// Odd processes send last element(if needed) to even processes
  		if( (s-start)%2 && world_rank != world_size-1 ){
  			MPI_Send(&a[s-1], 1, MPI_INT, world_rank+1, 0, MPI_COMM_WORLD);
  			int next;
  			MPI_Recv(&next, 1, MPI_INT, world_rank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  			if(next != a[s-1]){
  				isSorted = false;
  				a[s-1] = next;
  			}
  		}
  		
  		oddPhase = !oddPhase;
  		
  		bool isDone = isSorted;
  		
  		if(world_rank == 0){
  			for(int i = 1; i < world_size ; i++){
  				bool t1;
  				MPI_Recv(&t1, 1, MPI_C_BOOL, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  				isDone = isDone && t1;
  			}
  			for(int i = 1; i < world_size ; i++){
  				MPI_Send(&isDone, 1, MPI_C_BOOL, i, 0, MPI_COMM_WORLD);
  			}
  		}
  		else{
  			MPI_Send(&isSorted, 1, MPI_C_BOOL, 0, 0, MPI_COMM_WORLD);
  			MPI_Recv(&isDone, 1, MPI_C_BOOL, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  		}
  		flag = !isDone;
  		
  	}
  	
  	if(world_rank != 0){
  		MPI_Send(a, s, MPI_INT, 0, 0, MPI_COMM_WORLD);
  	}
  	else{
  		int size = n/world_size, s1 = n - (world_size-1)*size, i;
  		
  		for(i = 1; i < world_size-1 ; i++)
  			MPI_Recv(&a[size*i], size, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
 
  		MPI_Recv(&a[size*(world_size-1)], s1, MPI_INT, world_size-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  		
  		
  		for(i = 0 ; i < n ; i++)
  			printf("%d ",a[i]);
  		printf("\n");
  	}

	free(a);
  	
  	MPI_Finalize();
}

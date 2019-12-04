#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){

	int p1,q1,p2,q2,i,j,k;
	
	printf("Enter dimensions of first matrix(space separated) : ");
	scanf("%d %d",&p1,&q1);
	
	printf("Enter dimensions of second matrix(space separated) : ");
	scanf("%d %d",&p2,&q2);
	
	if(q1 != p2){
		printf("Invalid dimensions\n");
		return 0;
	}
	
	int A[p1][q1], B[p2][q2], C[p1][q2];
	
	srand(1);
	
	printf("A:\n");
	
	for(i = 0 ; i < p1 ; i++){
		for(j = 0 ; j < q1 ; j++){
			A[i][j] = rand()%50;
			printf("%d ",A[i][j]);
		}
		printf("\n");
	}
	
	printf("\nB:\n");
	
	for(i = 0 ; i < p2 ; i++){
		for(j = 0 ; j < q2 ; j++){
			B[i][j] = rand()%50;
			printf("%d ",B[i][j]);
		}
		printf("\n");
	}

	#pragma omp parallel private(i,j,k)
	{
		for(i = 0 ; i < p1 ; i++){
			for(j = 0 ; j < q2 ; j++){
				C[i][j] = 0;
				for(k = 0 ; k < q1 ; k++){
					C[i][j] += A[i][k]*B[k][j];
				}
			}
		}
	}
	
	printf("\nC:\n");
	
	for(i = 0 ; i < p1 ; i++){
		for(j = 0 ; j < q2 ; j++){
			printf("%d ",C[i][j]);
		}
		printf("\n");
	}

}

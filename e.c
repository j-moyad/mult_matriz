#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

void llenamatriz(int *v, int va, int tam);
int multiplica(int *r, int *c, int tam);
void imprime(int *co, int tam);

int main (int argc, char **argv)
{
	int nproc, pid;	
	int *a, *b, *c, *re, *co,*to, N, i, total;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);
 
	MPI_Status status;
	MPI_Datatype renglon;
	MPI_Datatype columna;

		
	
	if(argc >1)
	{
		N = atoi(argv[1]);

		MPI_Type_vector(1,N,N,MPI_INT,&renglon);
         	MPI_Type_commit (&renglon);
          	MPI_Type_vector(N,1,N,MPI_INT,&columna);
          	MPI_Type_commit (&columna);

		if(N%(nproc-1)==0)
		{
			if(pid==0)
			{
				a=(int *)malloc(sizeof(int)*N*N);
				b=(int *)malloc(sizeof(int)*N*N);
				c=(int *)malloc(sizeof(int)*N*N);
		
				llenamatriz(a, 1, N);
				llenamatriz(b, 1, N);
				llenamatriz(c, 0, N);

				for(i=0; i<nproc-1;i++)
				{				
					MPI_Send(&a[0], 1, renglon, i+1, 1, MPI_COMM_WORLD);
				}

				for(i=0; i<nproc-1;i++)
				{				
					MPI_Send(&b[i], 1, columna, i+1, 1, MPI_COMM_WORLD);
				}
					
				for(i=0; i<nproc-1;i++)
				{				
					MPI_Recv(&c[i], 1, MPI_INT, i+1, 1, MPI_COMM_WORLD, &status);
				}

				imprime(a, N);
				imprime(b, N);
				imprime(c, N);					
			}
			else
			{
				re=(int *)malloc(sizeof(int)*N);
				co=(int *)malloc(sizeof(int)*N*N);
				to=(int *)malloc(sizeof(int)*N);
		
				MPI_Recv(re, 1, renglon, 0, 1, MPI_COMM_WORLD, &status);
				MPI_Recv(co, 1, renglon, 0, 1, MPI_COMM_WORLD, &status);

				printf("\n");
				for(i=0; i<N;i++)
				{				
					printf("[%02d] ",re[i]);
				}

				printf("\n");
				for(i=0; i<N;i++)
				{				
					printf("[%02d] ",co[i]);
				}

				total=multiplica(re, co, N);
				printf("\n\n%d\n\n",total);
				MPI_Send(&total, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);

			}			
		}
		else
		{	
			if(pid==0)
			{
				printf("\n\nEl numero de elementos no es divisble entre los procesos\n\n");
			}
		}

		MPI_Type_free(&renglon);
		MPI_Type_free(&columna);
	}
	else
	{
		if(pid==0)
		{
			printf("\n\nFaltan argumentos para el proyecto\n\n");
		}
	}  	
	

	MPI_Finalize();
	return 0;
}

void llenamatriz(int *v, int va, int tam)
{
	int i;
	for (i=0; i<tam*tam; i++)
	{
		if(va==1)
		{
			v[i]=rand()%10;
		}
		else
		{
			v[i]=0;
		}  
	}
	return;
}

int multiplica(int *re, int *co, int tam)
{
	int res=0, i;

	for(i=0; i<tam;i++)
	{
		res+=(re[i]*co[i]);
	}

	return res;   
}

void imprime(int *co, int tam)
{
	int i, j;
	printf("\n");	

	for (i=0, j=0; i<tam*tam; i++, j++)
	{
		printf("[%02d] ", co[i]);
		if(j==(tam-1))
		{
			printf("\n");
			j=-1;
		}	
	}
	return;
}

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
	int *a, *b, *c, *re, *co,*to;
	int x, y, N, i, j, total, tmp, op_terminadas = 0;
	clock_t start, end;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);
 
	MPI_Status status;
	MPI_Datatype renglon;
	MPI_Datatype columna;
	
	if(argc > 1)
	{
		N = atoi(argv[1]);

		MPI_Type_vector(1, N, N, MPI_INT, &renglon);
		MPI_Type_commit (&renglon);
		MPI_Type_vector(N, 1, N, MPI_INT, &columna);
		MPI_Type_commit (&columna);

		if(N % (nproc-1) == 0)
		{ 
			if(pid == 0)
			{
				a = (int *)malloc(sizeof(int) * N * N);
				b = (int *)malloc(sizeof(int) * N * N);
				c = (int *)malloc(sizeof(int) * N * N);
		
				llenamatriz(a, 1, N);
				llenamatriz(b, 1, N);
				llenamatriz(c, 0, N);
				
				start = clock();

				while(op_terminadas < nproc - 1) {
					MPI_Recv(&tmp, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
					 MPI_COMM_WORLD, &status);
					if(status.MPI_TAG == 0) // Tag 0 enviar renglon
					{
						int index = N * (tmp * (nproc-1) + status.MPI_SOURCE - 1);
						MPI_Send(&a[index], 1, renglon, status.MPI_SOURCE, status.MPI_SOURCE, MPI_COMM_WORLD);
					} 
					else if(status.MPI_TAG == 1) // Tag 1 enviar columna
					{
						MPI_Send(&b[tmp], 1, columna, status.MPI_SOURCE, status.MPI_SOURCE, MPI_COMM_WORLD);
					}
					else if(status.MPI_TAG == 2)// Tag 2 es que termino
					{
						op_terminadas++;
					}
					else // Cualquier otra tag indica la posicion del resultado
					{
						x = status.MPI_TAG % 10000;
						y = N * ((status.MPI_TAG / 10000 - 1) * (nproc-1) + status.MPI_SOURCE - 1);
						c[x+y] = tmp;
					}
				}
				imprime(c, N);
			}
			else
			{
				re=(int *)malloc(sizeof(int) * N);
				co=(int *)malloc(sizeof(int) * N);
				to=(int *)malloc(sizeof(int) * N);

				for(i=0; i<N / (nproc-1); i++)
				{
					MPI_Send(&i, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);			
					MPI_Recv(re, 1, renglon, 0, pid, MPI_COMM_WORLD, &status);
					
					for(j=0; j<N; j++)
					{
						MPI_Send(&j, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
						MPI_Recv(co, 1, renglon, 0, pid, MPI_COMM_WORLD, &status);
						
						total = multiplica(re, co, N);
						MPI_Send(&total, 1, MPI_INT, 0, (i+1)*10000+j, MPI_COMM_WORLD);
					}
				}
				
				MPI_Send(&total, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);

			}			
		}
		
		if(pid == 0) 
		{
			end = clock();

			int h,m;
			float s;
			
			s = (float)(end - start) / CLOCKS_PER_SEC;
			m = ((int)s / 60) % 60;
			h = s / 3600;
			
			while(s >= 60) // Como no es int no se puede usar modulo
			{
				s -= 60.0;
			}
			
			printf("\nTiempo total: %02d:%02d:%09.06f\n", h, m, s);
		}			
		else
		{	
			if(pid == 0)
			{
				printf("\n\nEl numero de elementos no es divisble entre los procesos\n\n");
			}
		}

		MPI_Type_free(&renglon);
		MPI_Type_free(&columna);
	}
	else
	{
		if(pid == 0)
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
		if(va == 1)
		{
			v[i] = (rand() % 10) + 1;
		}
		else
		{
			v[i] = 0;
		}  
	}
	
	if(va != 0)
		imprime(v, tam);
	
	return;
}

int multiplica(int *re, int *co, int tam)
{
	int res=0, i;

	for(i=0; i<tam; i++)
	{
		res += re[i] * co[i];
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
		if(j == tam - 1)
		{
			printf("\n");
			j = -1;
		}	
	}

	return;
}

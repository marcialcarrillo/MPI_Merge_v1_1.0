#include <mpi.h> 
#include <vector>
#include <stdlib.h>
#include <ctime>
#include <iostream>
#include <algorithm>
#include <random>
#include <iterator>

#include <functional>
using namespace std;

int input_number;
vector<int> vector_randoms;
int *array_randoms;
int *array_local_randoms;
vector<int> vector_local_random;
int range, process_quantity;

double local_start, local_finish, local_elapsed, elapsed;

//start of foos

void print1(vector<int>);
void print2(vector<int>);
void get_args(char* argv[], int& input_number);

//end of foos

int main(int argc, char* argv[])
{
	int process_id;

	/* Arrancar ambiente MPI */
	MPI_Init(&argc, &argv);             		/* Arranca ambiente MPI */
	MPI_Comm_rank(MPI_COMM_WORLD, &process_id); 		/* El comunicador le da valor a id (rank del proceso) */
	MPI_Comm_size(MPI_COMM_WORLD, &process_quantity);  /* El comunicador le da valor a p (número de procesos) */

	if (process_id == 0)
	{
		cin.ignore();
	}
	MPI_Barrier(MPI_COMM_WORLD);

	MPI_Barrier(MPI_COMM_WORLD);
	local_start = MPI_Wtime();
	/* Código a evaluar */

	//local_start = MPI_Wtime();

	get_args(argv, input_number);

	//int input_number = 1000;
	range = input_number / process_quantity;
	// First create an instance of an engine.
	random_device rnd_device;
	// Specify the engine and distribution.
	mt19937 mersenne_engine{ rnd_device() };  // Generates random integers
	uniform_int_distribution<int> dist{ 1, input_number };

	auto gen = [&dist, &mersenne_engine]()
	{
		return dist(mersenne_engine);
	};
	vector_randoms.resize(input_number);
	vector_local_random.resize(range);
	generate(begin(vector_randoms), end(vector_randoms), gen);

	/*for (auto i : vec)
	{
	cout << i << " ";
	}*/


	array_randoms = vector_randoms.data();
	array_local_randoms = vector_local_random.data();

	MPI_Scatter(array_randoms, range, MPI_INT, array_local_randoms, range, MPI_INT, 0, MPI_COMM_WORLD);

	vector<int> a, b;
	vector_randoms.clear();
	vector_randoms.resize(input_number);

	sort(vector_local_random.begin(), vector_local_random.end());

	MPI_Gather(array_local_randoms, range, MPI_INT, array_randoms, range, MPI_INT, 0, MPI_COMM_WORLD);

	local_finish = MPI_Wtime();
	local_elapsed = local_finish - local_start;
	MPI_Reduce(&local_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);


	MPI_Barrier(MPI_COMM_WORLD);

	if (process_id == 0)
	{
		//cout << "Prosess " << process_id << endl;
		print1(vector_randoms);
		//cout << vector_randoms.size() << endl;
		for (int i = 1; i < process_quantity; i++)
		{
			auto it_middle = vector_randoms.begin() + (i * range);
			auto it_end = vector_randoms.begin() + ((i + 1) * range);
			inplace_merge(vector_randoms.begin(), it_middle, it_end);
		}
		cout << endl << endl << "COMPLETE VECTOR: " << endl << endl;
		print2(vector_randoms);
		cout << endl << endl << "Total Time Elapsed: " << elapsed << endl;
		
	}

	if (process_id == 0)
		cin.ignore();
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	return 0;
}


void get_args(char* argv[], int& input_number)
{
	input_number = strtol(argv[1], NULL, 10);
}

void print1(vector<int> vec)
{
	int subvector = 0;
	for (int i = 0; i < vec.size(); i++)
	{
		if (i % range == 0) {
			cout << endl << endl << "Subvector Number: " << subvector;
			subvector++;
			cout << endl << endl << endl;
		}
		cout << vec[i] << " ";
	}
}

void print2(vector<int> vec)
{
	for (int i = 0; i < vec.size(); i++)
	{
		cout << vec[i] << " ";
	}
}




// Optional


#include <iostream>
#include <mpi.h>
#include <vector>
#include <fstream>
#include <cmath>
using namespace std; 
  
#define MASTER 0
#define INPUT "numbers"
#define TAG_DISTRIBUTE 0

// To heapify a subtree rooted with node i which is 
// an index in arr[]. n is size of heap 
void heapify(int arr[], int n, int i) { 
	int largest = i; // Initialize largest as root 
	int l = 2 * i + 1; // left = 2*i + 1 
	int r = 2 * i + 2; // right = 2*i + 2 
               
	// If left child is larger than root 
	if (l < n && arr[l] > arr[largest]) { 
		largest = l; 
	}
                                 
	// If right child is larger than largest so far 
	if (r < n && arr[r] > arr[largest]) {
		largest = r; 
	}
                                                   
	// If largest is not root 
	if (largest != i) { 
		swap(arr[i], arr[largest]); 
                                                                     
		// Recursively heapify the affected sub-tree 
		heapify(arr, n, largest); 
	} 
} 

void heapSort(int arr[], int n) { 
	// Build heap (rearrange array) 
	for (int i = n / 2 - 1; i >= 0; i--) { 
    		heapify(arr, n, i);
	} 

	// One by one extract an element from heap 
	for (int i = n - 1; i >= 0; i--) { 
		// Move current root to end 
		swap(arr[0], arr[i]); 
                           
		// call max heapify on the reduced heap 
		heapify(arr, i, 0); 
	} 
}

int next_power_of_two(int f){
    int b = f << 9 != 0; // If we're a power of two this is 0, otherwise this is 1

    f >>= 23; // remove factional part of floating point number
    f -= 127; // subtract 127 (the bias) from the exponent

    // adds one to the exponent if were not a power of two, 
    // then raises our new exponent to the power of two again.
    return (1 << (f + b)); 
}

vector<int> readInput() {
	int number;
	int invar = 0;
	vector<int> numbers;
	fstream fin; 
	fin.open(INPUT, ios::in); 

	while (fin.good()) {
		number = fin.get();
		if (!fin.good()) {
			break;
		}
		if (!numbers.empty()) {
			cout << " ";
		}
		cout << number;
		numbers.push_back(number);
	}
	fin.close();
	cout << endl;
	return numbers;
}

/**
 * Main function
 * @param argc number of arguments on command line
 * @param argv "array" of arguments
 * @return 0 if success, 1 otherwise
 */
int main(int argc, char *argv[]) {

	int numprocs;               // number of procesors
	int numberOfLeafs;	    // number of leaf processors
	int myid;                   // my rank
	int neighnumber;            // neighbour rank
	int inputSize;		    // number of input numbers
	int mynumber;               // my valuei
	int msgSize;
	int inputPowerOfTwo;
	vector<int> bucket;	    // all values of this process
	MPI_Status stat;            // struct- contains kod- source, tag, error

	//MPI INIT
	MPI_Init(&argc,&argv);                          // MPI init 
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);       // get number of running processes 
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);           // get rank of my process

	numberOfLeafs = (numprocs + 1) / 2;
	cout << "numberOfLeafs: " << numberOfLeafs << endl;	

	// master process
	if (myid == MASTER) {
		vector<int> numbers = readInput();
		inputSize = numbers.size();

		cout << "inputSize " << inputSize << endl; 
		int powerOfTwo = next_power_of_two(inputSize);
		inputPowerOfTwo = pow(2, powerOfTwo);

		msgSize = inputPowerOfTwo / numberOfLeafs;
		cout << "real msgSize " << msgSize << endl;
	
		for (int i = 0; i < inputPowerOfTwo - inputSize; i++) {
			numbers.push_back(-1);
		}
		for(int i=0; i < numbers.size(); ++i)
                        std::cout << numbers[i] << ' ';
		cout << "powe of two" << inputPowerOfTwo << endl;
		for (int i = 0; i < numberOfLeafs; i++) {
			cout << "sending data" << endl;
			for (int j = 0; j < msgSize; j++) {
				cout << i * numberOfLeafs + j << ": " << numbers[i * numberOfLeafs + j] << endl;
				MPI_Send(&numbers[i * numberOfLeafs + j], 1, MPI_INT, (i % numberOfLeafs) + (numberOfLeafs - 1), TAG_DISTRIBUTE, MPI_COMM_WORLD);
			}
		} 
	}
	MPI_Bcast(&msgSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);

	// leafs
	if (myid > numprocs - numberOfLeafs - 1) {

		cout << "msgSize " << msgSize << endl;

		cout << "wainting to receive\n";
		for (int i = 0; i < msgSize; i++) {
			MPI_Recv(&mynumber, 1, MPI_INT, 0, TAG_DISTRIBUTE, MPI_COMM_WORLD, &stat); //buffer,velikost,typ,rank odesilatele,tag, skupina, stat
			bucket.push_back(mynumber);
			cout<<"i am:"<<myid<< " number: " << mynumber << endl;
		}
	}

	MPI_Finalize(); 
	return 0;
}                                                                                  

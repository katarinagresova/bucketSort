#include <iostream>
#include <mpi.h>
#include <vector>
#include <fstream>
#include <cmath>
using namespace std; 
  
#define MASTER 0
#define INPUT "numbers"
#define TAG_DISTRIBUTE 0
#define TAG_REDUCE 1
#define MEASURE true

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

// compute power of two greater than or equal to n
unsigned nextPowerOf2(unsigned n) {
	// decrement n (to handle the case when n itself is a power of 2)
	n--;
	
	// Set all bits after the last set bit
	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	n |= n >> 8;
	n |= n >> 16;
	
	// increment n and return
	return ++n;
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
		if (!MEASURE) {
			if (!numbers.empty()) {
				cout << " ";
			}
			cout << number;
		}
		numbers.push_back(number);
	}
	fin.close();
	if (!MEASURE) {
		cout << endl;
	}
	return numbers;
}

vector<int> mergeSortedVectors(vector<int> a, vector<int> b) {
    int alen = a.size();
    int blen = b.size();
    int tlen = alen + blen;
    vector<int> c(tlen);
    int i = 0, j = 0, k = 0;
 
    while (i < alen && j < blen) {
        if (a[i] < b[j])
            c[k++] = a[i++];
        else
            c[k++] = b[j++];
    }
    while (i < alen)
        c[k++] = a[i++];
 
    while (j < blen)
        c[k++] = b[j++];
 
    return c;
}

/**
 * Measure time and print result to std out, it make diff of start time and end time
 * Source: http://www.guyrutenberg.com/2007/09/22/profiling-code-using-clock_gettime/
 * @param timeStart
 * @param timeEnd
 */
void measureTime(timespec timeStart, timespec timeEnd) {
    timespec timeTemp;
    if ((timeEnd.tv_nsec - timeStart.tv_nsec) < 0)
    {
        timeTemp.tv_sec = timeEnd.tv_sec - timeStart.tv_sec - 1;
        timeTemp.tv_nsec = 1000000000 + timeEnd.tv_nsec - timeStart.tv_nsec;
    } else
    {
        timeTemp.tv_sec = timeEnd.tv_sec - timeStart.tv_sec;
        timeTemp.tv_nsec = timeEnd.tv_nsec - timeStart.tv_nsec;
    }
    cout<< timeTemp.tv_sec << ":" << timeTemp.tv_nsec << endl;
}

/**
 * Main function
 * @param argc number of arguments on command line
 * @param argv "array" of arguments
 * @return 0 if success, 1 otherwise
 */
int main(int argc, char *argv[]) {

	int numprocs;               // number of procesors
	unsigned numberOfLeafs;	    // number of leaf processors
	int myid;                   // my rank
	unsigned inputSize;		    // number of input numbers
	int mynumber;               // my value
	unsigned msgSize;			// size of leaf bucket
	vector<int> bucket;	    	// all values of this process
	MPI_Status stat;            // struct- contains kod- source, tag, error

	//MPI INIT
	MPI_Init(&argc,&argv);                          // MPI init 
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);       // get number of running processes 
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);           // get rank of my process

	numberOfLeafs = (numprocs + 1) / 2;
	timespec timeStart;
    timespec timeEnd;

	// master process - reading and distributing input
	if (myid == MASTER) {
		vector<int> numbers = readInput();
		inputSize = numbers.size();

		// round to input to be able to evenly distribute it among lefs
		while (inputSize % numberOfLeafs != 0) {
			numbers.push_back(-1);
			inputSize++;
		}
		msgSize = inputSize / numberOfLeafs;
	
		if(MEASURE) {
		    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &timeStart);
		}

		// distribute numbers
		for (int i = 0; i < numberOfLeafs; i++) {
			for (int j = 0; j < msgSize; j++) {
				MPI_Send(&numbers[i * msgSize + j], 1, MPI_INT, (i % numberOfLeafs) + (numberOfLeafs - 1), TAG_DISTRIBUTE, MPI_COMM_WORLD);
			}
		} 
	}

	MPI_Bcast(&msgSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

	// leafs - sort its own part
	if (myid >= numprocs - numberOfLeafs) {
		for (int i = 0; i < msgSize; i++) {
			MPI_Recv(&mynumber, 1, MPI_INT, 0, TAG_DISTRIBUTE, MPI_COMM_WORLD, &stat); //buffer,velikost,typ,rank odesilatele,tag, skupina, stat
			bucket.push_back(mynumber);
		}
		heapSort(&bucket[0], msgSize);
	}

	int levelStart = numprocs - numberOfLeafs;
	int levelEnd = numprocs - 1;
	// main sorting loop
	for (int j = 0; j < log2(numberOfLeafs); j++) {

		// low level - sending numbers
		if (myid >= levelStart && myid <= levelEnd) {
			for(int i = 0; i < msgSize * pow(2, j); ++i) {
				MPI_Send(&bucket[i], 1, MPI_INT, floor((myid - 1) / 2), TAG_REDUCE, MPI_COMM_WORLD);
			}
		}

		int levelSize = levelEnd - levelStart + 1;
		levelEnd = levelStart - 1;
		levelStart = levelStart - (levelSize / 2);

		// high level - receiving numbers and merging
		if (myid >= levelStart && myid <= levelEnd) {
			vector<int> smallBucketOne;

			for (int i = 0; i < msgSize * pow(2, j); ++i) {
				MPI_Recv(&mynumber, 1, MPI_INT, 2 * myid + 1, TAG_REDUCE, MPI_COMM_WORLD, &stat);
				smallBucketOne.push_back(mynumber);
			}

			vector<int> smallBucketTwo;
			for(int i = 0; i < msgSize * pow(2, j); ++i) {
				MPI_Recv(&mynumber, 1, MPI_INT, 2 * myid + 2, TAG_REDUCE, MPI_COMM_WORLD, &stat);
				smallBucketTwo.push_back(mynumber);
			}

			bucket = mergeSortedVectors(smallBucketOne, smallBucketTwo);
		}
	}

	// master process - root - print result
	if (myid == MASTER) {
		if(MEASURE) {
	        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &timeEnd);
	        measureTime(timeStart, timeEnd);
	    } else {
			for(int i=0; i < bucket.size(); ++i) {
				if (bucket[i] != -1) {
	        		cout << bucket[i] << endl;
	        	}
			}
		}
	}

	MPI_Finalize(); 
	return 0;
}                                                                                  

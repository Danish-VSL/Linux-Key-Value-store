#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <pthread.h>
#include <sys/ioctl.h>

#include "ioctl_store.h"


#define NUM_THREADS 4


void *SetKeyValue(void *threadid) {

	long tid;
	tid = (intptr_t)threadid;
	printf("Thread ID : %ld \n" , tid);

	for(int j = tid*11; j < (tid*11)+11 ; j++)
	{
		struct key_value msg = {j, j*10};
		int dev = open("/dev/kvstore", O_WRONLY);
		if(dev == -1) {
			printf("Opening was not possible!\n");
			break;
		}
		ioctl(dev, WR_VALUE, &msg);
		close(dev);

		printf("Thread ID : %ld Update / Insert key : %d \n" , tid, j);
		sleep(1);

	}

	pthread_exit(NULL);
}

void *GetKeyValue(void *threadid) {

	long tid;
	tid = (intptr_t)threadid;
	printf("Thread ID : %ld \n" , tid);

	for(int j = (tid-2)*11; j < ((tid-2)*11)+11 ; j++)
	{
		int dev = open("/dev/kvstore", O_WRONLY);
		if(dev == -1) {
			printf("Opening was not possible!\n");
			break;
		}
		int keyvalue = j;
		ioctl(dev, GET_VALUE_FOR_KEY, &keyvalue);
		close(dev);

		printf("Thread ID : %ld fetched key : %d value : %d \n" , tid, j, keyvalue);
		sleep(1);
	}

	pthread_exit(NULL);
}


int main() {

 	int dev = open("/dev/kvstore", O_WRONLY);
	if(dev == -1) {
		printf("Opening was not possible!\n");
		return -1;
	}



	pthread_t threads[NUM_THREADS];

    int rc;
    int i;

	//Create 2 threads for Setting the Key-Values.
	for( i = 0; i < 2; i++ ) {
		printf("main() : creating thread, %d \n", i+1);

		rc = pthread_create(&threads[i], NULL, SetKeyValue, (void *) (intptr_t) i+1);

		if (rc) {
			printf("Error:unable to create thread, %d \n", rc);

			exit(-1);
		}
	}

	// Create 2 threads for Getting the Key-Values.
	for( i = 2; i < 4; i++ ) {
		printf("main() : creating thread, %d \n", i+1);

		rc = pthread_create(&threads[i], NULL, GetKeyValue, (void *) (intptr_t) i+1);

		if (rc) {
			printf("Error:unable to create thread, %d \n", rc);

			exit(-1);
		}
	}

	// main() waiting for all threads to finish for calling the Display command for printing the linked list.

	pthread_join(threads[0], NULL);
	pthread_join(threads[1], NULL);
	pthread_join(threads[2], NULL);
	pthread_join(threads[3], NULL);
	ioctl(dev,IOCTL_DISPLAY);
	pthread_exit(NULL);

}


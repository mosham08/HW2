#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#define MAX 20000

struct Earthquakes {
    char time[100];
    char latitude[20];
    char longitude[20];
    char depth[20];
    char mag[20];
};

struct Earthquakes *earthquakes[MAX];

struct Earthquakes *earthquakes1[10000];
struct Earthquakes *earthquakes2[10000];
struct Earthquakes *earthquakes3[10000];
struct Earthquakes *earthquakes4[10000];
struct Earthquakes *earthquakes5[10000];
struct Earthquakes *earthquakes6[10000];
struct Earthquakes *earthquakes7[10000];
struct Earthquakes *earthquakes8[10000];
struct Earthquakes *earthquakes9[10000];
struct Earthquakes *earthquakes10[10000];


int read_file(char *fileName) {
//    fprintf(stderr, "Try to open file!\n");
    FILE *file = fopen(fileName, "r");
    const size_t line_size = 2049;
    char *line = malloc(line_size);
    // checking file error
    if (!file) {
        fprintf(stderr, "Unable to open file!\n");
        return -1;
    }
// instrumentaion start
    time_t t;
    t = time(NULL);

    fgets(line, line_size, file);


    fgets(line, line_size, file);
    int index = 0;
    while (fgets(line, line_size, file) && (index < MAX)) {
        // allocate mem for the earthquakes structure
        earthquakes[index] = (struct Earthquakes *) malloc(sizeof(struct Earthquakes));

        char *token;
        // get token from line
        token = strtok(line, ",");
        strcpy(earthquakes[index]->time, token);
        token = strtok(NULL, ",");
        strcpy(earthquakes[index]->latitude, token);
        token = strtok(NULL, ",");
        strcpy(earthquakes[index]->longitude, token);
        token = strtok(NULL, ",");
        strcpy(earthquakes[index]->depth, token);
        token = strtok(NULL, ",");
        strcpy(earthquakes[index]->mag, token);

        index++;
    }
    free(line);
    return 1;
}


void split(int numPros) {

    if (numPros == 2) {
        for (int i = 0; i < 10000; ++i) {
            earthquakes1[i] = (struct Earthquakes *) malloc(sizeof(struct Earthquakes));
            earthquakes1[i] = earthquakes[i];
        }

        int j = 0;
        for (int i = 10000; i < 20000; ++i) {
            earthquakes2[j] = (struct Earthquakes *) malloc(sizeof(struct Earthquakes));
            earthquakes2[j] = earthquakes[i];
            j++;
        }
    }

    if (numPros == 4) {
        for (int i = 0; i < 5000; ++i) {
            earthquakes1[i] = (struct Earthquakes *) malloc(sizeof(struct Earthquakes));
            earthquakes1[i] = earthquakes[i];
        }

        int j = 0;
        for (int i = 5000; i < 10000; ++i) {
            earthquakes2[j] = (struct Earthquakes *) malloc(sizeof(struct Earthquakes));
            earthquakes2[j] = earthquakes[i];
            j++;
        }

        j = 0;
        for (int i = 10000; i < 15000; ++i) {
            earthquakes3[j] = (struct Earthquakes *) malloc(sizeof(struct Earthquakes));
            earthquakes3[j] = earthquakes[i];
            j++;
        }

        j = 0;
        for (int i = 15000; i < 20000; ++i) {
            earthquakes4[j] = (struct Earthquakes *) malloc(sizeof(struct Earthquakes));
            earthquakes4[j] = earthquakes[i];
            j++;
        }
    }

//    printf("First Earthquakes: \n");
//    for (int k = 0; k < 10; ++k) {
//        printEarthquake(earthquakes1[k]);
//    }
//
//    printf("Second Earthquakes: \n");
//    for (int k = 0; k < 10; ++k) {
//        printEarthquake(earthquakes2[k]);
//    }
}

void printEarthquake(struct Earthquakes *earthquakes) {
    printf("%-40s", earthquakes->time);
    printf("%-20s", earthquakes->latitude);
    printf("%-20s", earthquakes->longitude);
    printf("%-12s", earthquakes->depth);
    printf("%-12s\n", earthquakes->mag);
}

void Sort(struct Earthquakes *earth[], char *process, int size) {
    double time_spent = 0.0;
    clock_t begin = clock();
    struct Earthquakes *temp;
    for (int i = 1; i < size; i++) {
        for (int j = 0; j < size - i; j++) {
            if (atof(earth[j]->latitude) > atof(earth[j + 1]->latitude)) {
                temp = earth[j];
                earth[j] = earth[j + 1];
                earth[j + 1] = temp;
            }
        }
    }

    clock_t end = clock();
    time_spent += (double) (end - begin) / CLOCKS_PER_SEC;
    printf("\n%s: Time elpased is %f seconds\n", process, time_spent);

//    for (int k = 0; k < 10; ++k) {
//        printEarthquake(earth[k]);
//    }

}

void *create_shared_memory(size_t size) {
    // Our memory buffer will be readable and writable:
    int protection = PROT_READ | PROT_WRITE;

    // The buffer will be shared (meaning other processes can access it), but
    // anonymous (meaning third-party processes cannot obtain an address for it),
    // so only this process and its children will be able to use it:
    int visibility = MAP_SHARED | MAP_ANONYMOUS;

    // The remaining parameters to `mmap()` are not important for this use case,
    // but the manpage for `mmap` explains their purpose.
    return mmap(NULL, size, protection, visibility, -1, 0);
}

#define  NOT_READY  -1
#define  FILLED     0
#define  TAKEN      1

struct Memory {
    int status;
    struct Earthquakes *earthquakes1[10000];
};

void mergeArrays(struct Earthquakes *arr1[], struct Earthquakes *arr2[], int n1, int n2, struct Earthquakes *arr3[]) {
    int i = 0, j = 0, k = 0;

    // Traverse both array
    while (i < n1 && j < n2) {
        // Check if current element of first
        // array is smaller than current element
        // of second array. If yes, store first
        // array element and increment first array
        // index. Otherwise do same with second array
        if (atof(arr1[i]->latitude) < atof(arr2[j]->latitude)) {
            arr3[k++] = arr1[i++];
        } else {
            arr3[k++] = arr2[j++];
        }

    }

    // Store remaining elements of first array
    while (i < n1)
        arr3[k++] = arr1[i++];

    // Store remaining elements of second array
    while (j < n2)
        arr3[k++] = arr2[j++];
}

int main() {
    char *parent_message = "hello";  // parent process will write this message
    char *child_message = "goodbye"; // child process will then write this one

    void *shmem = create_shared_memory(128);

    memcpy(shmem, parent_message, sizeof(parent_message));

//    int pid = fork();
//
//    if (pid == 0) {
//        printf("Child read: %s\n", shmem);
//        memcpy(shmem, child_message, sizeof(child_message));
//        printf("Child wrote: %s\n", shmem);
//
//    } else {
//        printf("Parent read: %s\n", shmem);
//        sleep(1);
//        printf("After 1s, parent read: %s\n", shmem);
//    }

    char *earthquake_filename = "./all_month.csv";// "./all_month.csv";
    read_file(earthquake_filename);

    split(2);
    int shm_id, *shm_ptr;
    key_t key = 1234;
    shm_id = shmget(key, sizeof(struct Earthquakes), IPC_CREAT | 0666);

    struct Earthquakes *earthquakesN;
//    shm_ptr = shmat(shm_id, NULL, 0);

    earthquakesN = (struct Earthquakes *) shmat(shm_id, (void *) 0, 0);

//

    //////
    key_t ShmKEY, ShmKEYSplit2;
    int ShmID, ShmIDSplit2;
    struct Memory *ShmPTR, *ShmPTRSplit2;


    ShmKEY = ftok(".", 'x');
    ShmKEYSplit2 = ftok(".", 'y');

    ShmID = shmget(ShmKEY, sizeof(struct Memory), IPC_CREAT | 0666);
    ShmIDSplit2 = shmget(ShmKEYSplit2, sizeof(struct Memory), IPC_CREAT | 0666);

    printf("Server has received a shared memory of four integers...\n");

    ShmPTR = (struct Memory *) shmat(ShmID, NULL, 0);
    ShmPTRSplit2 = (struct Memory *) shmat(ShmIDSplit2, NULL, 0);

    printf("Server has attached the shared memory...\n");

    ShmPTR->status = NOT_READY;
    ShmPTRSplit2->status = NOT_READY;
//    ShmPTR->earthquakes1[0] = earthquakes[0];
//    printf("Server has filled %s %s  to shared memory...\n",
//           ShmPTR->earthquakes1[0]->longitude, ShmPTR->earthquakes1[0]->latitude);
//    ShmPTR->status = FILLED;
    for (int k = 0; k < 10; k++) {
        ShmPTR->earthquakes1[k] = earthquakes[k];
        strcpy(ShmPTR->earthquakes1[k]->time, "split#1");
    }
    int index = 0;
    for (int k = 10; k < 20; k++) {
        int currentIndex = index;
        ShmPTRSplit2->earthquakes1[index++] = earthquakes[k];
        strcpy(ShmPTRSplit2->earthquakes1[currentIndex]->time, "split#2");
    }

    printf("Please start the client in another window...\n");

//    Sort(ShmPTR->earthquakes1, "sorting shared memory", 10);
//    for(int k=0;k<10;k++){
//        printEarthquake(ShmPTR->earthquakes1[k]);
//    }

    //////
    printf("\nStarting fork (2): \n\n");
//    for (int k = 0; k < 10; k++) {
//        printEarthquake(ShmPTR->earthquakes1[k]);
//    }
    int pid = fork();
    if (pid >= 0) {
        if (pid == 0) {
//        printf("\nchild process:\n");
            Sort(ShmPTR->earthquakes1, "process #1", 10);
            printf("\nchild process: printing\n");
            ShmPTR->status = FILLED;

            //printf("\n\n");
        } else {
//        printf("\nparent:\n");
            Sort(ShmPTRSplit2->earthquakes1, "process #2", 10);
        }
        sleep(5);
        if (pid == 0 && ShmPTR->status == FILLED) {
            printf("\nHOW are you \n");
            printf("\nchild process: sorted\n");
//            for (int k = 0; k < 10; k++) {
//                printEarthquake(ShmPTR->earthquakes1[k]);
//            }
//            printf("\nparent process: sorted\n");
//            for (int k = 0; k < 10; k++) {
//                printEarthquake(ShmPTRSplit2->earthquakes1[k]);
//            }

            //int arr3[20];
            struct Earthquakes *earthquakes1[20];
            mergeArrays(ShmPTR->earthquakes1, ShmPTRSplit2->earthquakes1, 10, 10, earthquakes1);
            printf("\nArray after merging\n");
            for (int i = 0; i < 20; i++)
                printEarthquake(earthquakes1[i]);
        }
    } else {
        printf("\nNot a child process: printing\n");
    }

//    printf("\nparent: printing\n");
//    for (int k = 0; k < 10; k++) {
//        printEarthquake(ShmPTRSplit2->earthquakes1[k]);
//    }

    //////


//    time_t t;
//    char parent_message[] = "hello";  // parent process will write this message
//    char child_message[] = "goodbye"; // child process will then write this one
//
////    t = time(NULL);
////    printf("Driver Time: %s\n", ctime(&t));
//
//    char *earthquake_filename = "./all_month.csv";// "./all_month.csv";
//    read_file(earthquake_filename);
//
//    int numPros;
//    printf("Enter number of processes (2, 4, 10): ");
//    scanf("%d", &numPros);
//
//    split(numPros);
////    printf("Sorted earth1: \n");
////    Sort(earthquakes1);
////    printf("Sorted earth2: \n");
////    Sort(earthquakes2);
//    //void* shmem = create_shared_memory(128);
//
//    //memcpy(shmem, parent_message, sizeof(parent_message));
//
//    if (numPros == 2) {
//        printf("\nStarting fork (2): \n\n");
//        int pid = fork();
//
//        if (pid == 0) {
//            Sort(earthquakes1, "process #1", 10000);
//            printf("\n\n");
//        } else {
//            printf("\nsecond:\n");
//            Sort(earthquakes2, "process #2", 10000);
//        }
//    }
//
//    if (numPros == 4) {
//        printf("\nStarting fork (4): \n\n");
//        int pid = fork();
//
//        if (pid == 0) {
//            Sort(earthquakes1, "process #1", 5000);
//            Sort(earthquakes2, "process #2", 5000);
//            printf("\n\n");
//        } else {
//            printf("\nsecond:\n");
//            Sort(earthquakes3, "process #3", 5000);
//            Sort(earthquakes4, "process #4", 5000);
//        }
//    }
//
//
//    sleep(5);
//    for (int k = 0; k < 10; ++k) {
//        printEarthquake(earthquakes1[k]);
//    }
//    printf("\nhow are you:\n");
//    for (int k = 0; k < 10; ++k) {
//        printEarthquake(earthquakes2[k]);
//    }
//
////    printf("Starting first fork for earth2: \n\n");
////    //pid = fork();
////
////    if (pid == 0) {
////        Sort(earthquakes2);
////    } else {
////        sleep(1);
////    }
//
////    t = time(NULL);
////    printf("Driver Time again: %s\n", ctime(&t));
}
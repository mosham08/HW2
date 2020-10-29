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
#define  NOT_READY  -1
#define  FILLED     0
#define  TAKEN      1

struct Earthquakes {
    char time[100];
    char latitude[20];
    char longitude[20];
    char depth[20];
    char mag[20];
};

struct Memory {
    int status;
    struct Earthquakes *earthquakes1[10000];
};


struct Earthquakes *earthquakes[MAX];

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
}

void saveSortedData(struct Earthquakes *sortedArray[], int maxLimit2) {
    char *earthquake_filename = "./all_month_sorted.csv";
    FILE *file = fopen(earthquake_filename, "w+");
    fprintf(file, "time, latitude, longitude, depth, mag\n");
    for (int i = 0; i < maxLimit2; i++) {
        fprintf(file, "%s,%s,%s,%s,%s", sortedArray[i]->time, sortedArray[i]->latitude, sortedArray[i]->longitude,
                sortedArray[i]->depth, sortedArray[i]->mag);
    }
    fclose(file);

    printf("\n %s file created", earthquake_filename);
}

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

void printEarthquake(struct Earthquakes *earthquakes) {
    printf("%-40s", earthquakes->time);
    printf("%-20s", earthquakes->latitude);
    printf("%-20s", earthquakes->longitude);
    printf("%-12s", earthquakes->depth);
    printf("%-12s\n", earthquakes->mag);
}

int main() {
    char *earthquake_filename = "./all_month.csv";
    read_file(earthquake_filename);
    //////
    key_t ShmKEY, ShmKEYSplit2;
    int ShmID, ShmIDSplit2;
    struct Memory *ShmPTR, *ShmPTRSplit2;

    ShmKEY = ftok(".", 'x');        // first shared memory key
    ShmKEYSplit2 = ftok(".", 'y');  // second shared memory key

    // create the shared memory / allocated
    ShmID = shmget(ShmKEY, sizeof(struct Memory), IPC_CREAT | 0666);
    ShmIDSplit2 = shmget(ShmKEYSplit2, sizeof(struct Memory), IPC_CREAT | 0666);

    // get the shared memory pointer
    ShmPTR = (struct Memory *) shmat(ShmID, NULL, 0);
    ShmPTRSplit2 = (struct Memory *) shmat(ShmIDSplit2, NULL, 0);

    // nothing added to the shared memory. at the start
    ShmPTR->status = NOT_READY;
    ShmPTRSplit2->status = NOT_READY;

    int maxLimit1 = 10000;
    int maxLimit2 = 20000;

    // get the data into rhe shared memory
    for (int k = 0; k < maxLimit1; k++) {
        ShmPTR->earthquakes1[k] = earthquakes[k];
//        strcpy(ShmPTR->earthquakes1[k]->time, "split#1");
    }
    printf("Child read: 1 %s\n", "");
    int index = 0;
    for (int k = maxLimit1; k < maxLimit2; k++) {
        int currentIndex = index;
        ShmPTRSplit2->earthquakes1[index++] = earthquakes[k];
//        strcpy(ShmPTRSplit2->earthquakes1[currentIndex]->time, "split#2");
    }
    printf("Child read: %s\n", "");
    //////
    printf("\nStarting fork (2): \n\n");

    int pid = fork();
    if (pid >= 0) {
        if (pid == 0) {
            Sort(ShmPTR->earthquakes1, "process #1", maxLimit1);
            printf("\nchild process\n");
            ShmPTR->status = FILLED;
        } else {
            printf("\nparent process\n");
            Sort(ShmPTRSplit2->earthquakes1, "process #2", maxLimit1);
        }
        sleep(5);
        if (pid == 0 && ShmPTR->status == FILLED) {
            struct Earthquakes *earthquakes1[maxLimit2];
            mergeArrays(ShmPTR->earthquakes1, ShmPTRSplit2->earthquakes1, maxLimit1, maxLimit1, earthquakes1);
            printf("\nArray after merging\n");
            printf("\nStoring merged data into csv file\n");
            saveSortedData(earthquakes1, maxLimit2);
        }
    } else {
        printf("\nNot a child process: printing\n");
    }
}
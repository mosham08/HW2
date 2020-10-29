/******************************************************************************
* FILE: hello_arg2.c
* DESCRIPTION:
*   A "hello world" Pthreads program which demonstrates another safe way
*   to pass arguments to threads during thread creation.  In this case,
*   a structure is used to pass multiple arguments.
* AUTHOR: Blaise Barney
* LAST REVISED: 01/29/09
******************************************************************************/
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
#include <pthread.h>


#define NUM_THREADS    2

#define MAX 20000
char *messages[NUM_THREADS];

struct thread_data {
    int thread_id;
//    int sum;
//    char *message;
    struct Earthquakes *earthquakes1[10000];
    int size;
};
struct Earthquakes {
    char time[100];
    char latitude[20];
    char longitude[20];
    char depth[20];
    char mag[20];
};
struct thread_data thread_data_array[NUM_THREADS];

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

void *Sort(void *threadarg) {
    int taskid, sum, size;
    char *hello_msg;
    struct thread_data *my_data;
    struct Earthquakes *earth;

    sleep(1);
    my_data = (struct thread_data *) threadarg;
    taskid = my_data->thread_id;
//    sum = my_data->sum;
//    hello_msg = my_data->message;
    size = my_data->size;

////    printf("Thread %d: %s  Sum=%d  Time= %s\n", taskid, hello_msg, sum, my_data->earthquakes1[0]->time);
//    printf("Thread %d: %s  Sum=%d  Time= %s\n", taskid, "hello_msg", 0, my_data->earthquakes1[0]->time);

    /// sorting code
    double time_spent = 0.0;
    clock_t begin = clock();

    struct Earthquakes *temp;

    for (int i = 1; i < size; i++) {
        for (int j = 0; j < size - i; j++) {
            if (atof(my_data->earthquakes1[j]->latitude) > atof(my_data->earthquakes1[j + 1]->latitude)) {
                temp = my_data->earthquakes1[j];
                my_data->earthquakes1[j] = my_data->earthquakes1[j + 1];
                my_data->earthquakes1[j + 1] = temp;
            }
        }
    }

    clock_t end = clock();
    time_spent = (double) (end - begin) / CLOCKS_PER_SEC;
    printf("\n%s: Time elpased is %f seconds\n", "hello_msg", time_spent);
    /// end sorting code
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    char *earthquake_filename = "./all_month.csv";
    read_file(earthquake_filename);

    pthread_t threads[NUM_THREADS];
    int *taskids[NUM_THREADS];
    int rc, t, sum;

    sum = 0;
//    messages[0] = "English: Hello World!";
//    messages[1] = "French: Bonjour, le monde!";
//    messages[2] = "Spanish: Hola al mundo";
//    messages[3] = "Klingon: Nuq neH!";
//    messages[4] = "German: Guten Tag, Welt!";
//    messages[5] = "Russian: Zdravstvytye, mir!";
//    messages[6] = "Japan: Sekai e konnichiwa!";
//    messages[7] = "Latin: Orbis, te saluto!";

// this is my split
    for (t = 0; t < NUM_THREADS; t++){
        for (int k = 0; k < 10000; k++) {
            thread_data_array[t].earthquakes1[k] = earthquakes[k];
        }
    }

    for (t = 0; t < NUM_THREADS; t++) {
        sum = sum + t;
        thread_data_array[t].thread_id = t;
//        thread_data_array[t].sum = sum;
//        thread_data_array[t].message = messages[t];
        thread_data_array[t].size = 10000;

        printf("Creating thread %d\n", t);
        rc = pthread_create(&threads[t], NULL, Sort, (void *)
                &thread_data_array[t]);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }


    pthread_exit(NULL);
}
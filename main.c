
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//#define MAX 1000
int MAX = 1000;
struct Earthquakes {
    char time[100];
    char latitude[20];
    char longitude[20];
    char depth[20];
    char mag[20];
};

int read_file(char *fileName, char *processname) {
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
//    printf("Driver Time: %s\n", ctime(&t));
//    // instrumentation
//
//    fprintf(stderr, "The file was opened !\n");
    fgets(line, line_size, file);

    struct Earthquakes *earthquakes[MAX];

//    fprintf(stderr, "The file was opened !\n");
    fgets(line, line_size, file);
    int index = 0;
//    fprintf(stderr, "Reading one line at a time from the file!\n");
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

        // print the structure
//        printEarthquake(earthquakes[index]);
        //printf("Executing line: %-12d in\n", index);
        index++;
    }
    free(line);

//    printf("number of lines printed: %d\n", index);processname
//    for (int i1 = 0; i1 < MAX; i1++) {
//        printEarthquake(earthquakes[i1]);
//    }

    struct Earthquakes *temp;
    for (int i = 1; i < MAX; i++) {
        for (int j = 0; j < MAX - i; j++) {
            if (strcmp(earthquakes[j]->latitude, earthquakes[j + 1]->latitude) > 0) {
                temp = earthquakes[j];
                earthquakes[j] = earthquakes[j + 1];
                earthquakes[j + 1] = temp;
            }
        }
    }

//    printf("After: number of lines printed: %d\n", index);
//    for (int i1 = 0; i1 < MAX; i1++) {
//        printEarthquake(earthquakes[i1]);
//    }

    t = time(NULL);
//    printf("Driver Time again: %s\n", ctime(&t));
    return 1;
}

void printEarthquake(struct Earthquakes *earthquakes) {
    printf("%-40s", earthquakes->time);
    printf("%-20s", earthquakes->latitude);
    printf("%-20s", earthquakes->longitude);
    printf("%-12s", earthquakes->depth);
    printf("%-12s\n", earthquakes->mag);
}

//// sorting data using bubbleSort
//void bubble_sort()
//{
//    int i, j;
//    earthquakes lat;
//    for (i = 0; i < MAX-1; i++)
//    {
//        for (j = 0; j < MAX-i-1; j++)
//        {
//            if (data[j].latitude > data[j+1].latitude)
//            {
//                lat = data[j];
//                data[j] = data[j+1];
//                data[j+1] = lat;
//            }
//        }
//    }
//}

int main(int arcg, char *argv[]) {

    // to store execution time of codecreate_shared_memory
    double time_spent = 0.0;
    clock_t begin = clock();

//    fprintf(stderr, "In main! \n");
    char *earthquake_filename = argv[0];// "./all_month.csv";
    MAX = atoi(argv[1]);
    read_file(earthquake_filename, argv[2]);

    clock_t end = clock();

    // calculate elapsed time by finding difference (end - begin) and
    // dividing the difference by CLOCKS_PER_SEC to convert to seconds
    time_spent += (double) (end - begin) / CLOCKS_PER_SEC;



    printf("\n%s: Time elpased is %f seconds\n", argv[2], time_spent);
//    printf("main.c: Begin: %ld\n", begin);
//    printf("main.c: End: %ld\n", end);


    exit(0);
}
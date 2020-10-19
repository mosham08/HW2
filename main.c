
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 10

struct Earthquakes {
    char time[100];
    char latitude[20];
    char longitude[20];
    char depth[20];
    char mag[20];
};

int read_file(char *fileName) {
    fprintf(stderr, "Try to open file!\n");
    FILE *file = fopen(fileName, "r");
    const size_t line_size = 2049;
    char *line = malloc(line_size);
    // checking file error
    if (!file) {
        fprintf(stderr, "Unable to open file!\n");
        return -1;
    }

    fprintf(stderr, "The file was opened !\n");
    fgets(line, line_size, file);

    struct Earthquakes *earthquakes[MAX];

    fprintf(stderr, "The file was opened !\n");
    fgets(line, line_size, file);
    int index = 0;
    fprintf(stderr, "Reading one line at a time from the file!\n");
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
        printEarthquake(earthquakes[index]);
        index++;
    }
    free(line);

    printf("number of lines printed: %d\n", index);

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

int main() {
    fprintf(stderr, "In main!\n");

    char *earthquake_filename = "./all_month.csv";
    read_file(earthquake_filename);

    return 0;
}
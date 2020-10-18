#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#define MAX 30

struct earthquakes{
    char time[100];
    float latitude;
    float longitude;
    float depth;
    float mag;
    char magType[5];
    char nst[5];
    int gap;
    float dmin;
    float rms;
    char net[5];
    char id[20];
    char updated[100];
    char place[200];
    char type[10];
    float horizontalError;
    float depthError;
    float magError;
    int magNst;
    char status[10];
    char locationSource[5];
    char magSource[5];
};
earthquakes data[MAX];

int read_file(char* fileName)
{
    FILE* file = fopen(fileName,"r");
    size_t flag;
    char* line;
    size_t sz = 0;

    // checking file error
    if (!file)
    {
        fprintf(stderr, "Unable to open file!\n");
        return -1;
    }

    getline(&line, &sz, file);

    int i = 0;

    while((flag = getline( &line, &sz, file)) != -1)
    {
        char* token;
        token = strtok(line,",");
        strtok(data[i].time,token);
        token = strtok(NULL,",");
        data[i].latitude = atof(token);
        token = strtok(NULL,",");
        data[i].longitude = atof(token);
        token = strtok(NULL,",");
        data[i].depth = atof(token);
        token = strtok(NULL,",");
        data[i].mag = atof(token);
        token = strtok(NULL,",");
        strcpy(data[i].magType,token);
        token = strtok(NULL,",");
        strcpy(data[i].nst,token);
        token = strtok(NULL,",");
        data[i].gap = atoi(token);
        token = strtok(NULL,",");
        data[i].dmin=atof(token);
        token = strtok(NULL,",");
        data[i].rms=atof(token);
        token = strtok(NULL,",");
        strcpy(data[i].net,token);
        token = strtok(NULL,",");
        strcpy(data[i].id,token);
        token = strtok(NULL,",");
        strcpy(data[i].updated,token);
        token = strtok(NULL,",");
        strcpy(data[i].place,token);
        token = strtok(NULL,",");
        strcpy(data[i].type,token);
        token = strtok(NULL,",");
        data[i].horizontalError=atof(token);
        token = strtok(NULL,",");
        data[i].depthError=atof(token);
        token = strtok(NULL,",");
        data[i].magError=atof(token);
        token = strtok(NULL,",");
        data[i].magNst=atoi(token);
        token = strtok(NULL,",");
        strcpy(data[i].status,token);
        token = strtok(NULL,",");
        strcpy(data[i].locationSource,token);
        token = strtok(NULL,",");
        strcpy(data[i].magSource,token);
        if(i == MAX)
            break;
        i++;
    }
    return 1;
}

// sorting data using bubbleSort
void bubble_sort()
{
    int i, j;
    earthquakes lat;
    for (i = 0; i < MAX-1; i++)
    {
        for (j = 0; j < MAX-i-1; j++)
        {
            if (data[j].latitude > data[j+1].latitude)
            {
                lat = data[j];
                data[j] = data[j+1];
                data[j+1] = lat;
            }
        }
    }
}

int main() {
    char* earthquake_filename = "./all_month.csv";
    read_file(earthquake_filename);
    bubble_sort();
    return 0;
}

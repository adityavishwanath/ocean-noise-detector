#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>
#include <string.h>

typedef struct {
    float real;
    float imag;
    float normalized_values;
} COMPLEX_NUMBER;

COMPLEX_NUMBER *values;

//Set this value appropriately!
const float THRESHOLD = 5000000;

int main(int argc, char *argv[])
{
    printf("********************\n\n");

    char file_name[100];
    if (argc != 2) {
        printf("\nPlease enter the .wav file name as a command line argument!\n");
        exit(-1);
    }
    strcpy(file_name, argv[1]);
    //printf("%s\n", file_name);

    SNDFILE *sf;
    SF_INFO info;
    int num, num_items_total;
    int *buf;
    int f,sr,c;
    int i,j;
    int total_duration, num_10s_intervals;
    int num_samples_10s;
    FILE *out;
    
    /* Open the WAV file. */
    info.format = 0;
    sf = sf_open(file_name, SFM_READ, &info);
    if (sf == NULL)
    {
        printf("Failed to open the file.\n");
        exit(-1);
    }

    out = fopen(strcat(file_name, ".out"), "w");

    /* Print some of the info, and figure out how much data to read. */
    f = info.frames;
    sr = info.samplerate;
    c = info.channels;
    total_duration = f / sr;
    num_10s_intervals = total_duration / 10;
    printf("Total duration = %d seconds\n", total_duration);
    printf("Number of 10s intervals = %d\n", num_10s_intervals);
    printf("Frames=%d\n", f);
    printf("Samplerate=%d\n", sr);
    printf("Channels=%d\n", c);

    fprintf(out, "Total duration = %d seconds\n", total_duration);
    fprintf(out, "Number of 10s intervals = %d\n", num_10s_intervals);
    fprintf(out, "Frames=%d\n", f);
    fprintf(out, "Samplerate=%d\n", sr);
    fprintf(out, "Channels=%d\n", c);

    num_items_total = f * c;

    num_samples_10s = num_items_total / num_10s_intervals;

    printf("Number of samples every 10s = %d\n", num_samples_10s);
    printf("Total number of samples=%d\n", num_items_total);

    fprintf(out, "Number of samples every 10s = %d\n", num_samples_10s);
    fprintf(out, "Total number of samples=%d\n", num_items_total);

    //Allocate space for the values
    values = malloc(sizeof(COMPLEX_NUMBER) * (num_items_total / 2));

    /* Allocate space for the data to be read, then read it. */
    buf = (int *) malloc(num_items_total * sizeof(double));
    num = sf_read_int(sf, buf, num_items_total);
    sf_close(sf);
    printf("Read %d items\n", num);

    for (int x = 0; x < (num_items_total / c) / 32; x++) {
        float temp_in[16], temp_out[16];
        for (int y = 0, i = 0; y < 32; y+=c, i++) {
            temp_in[i] = buf[(x * 32) + y];
        }
        R16SRFFT(temp_in, temp_out);
        
        values[(x * 32) + 0].real = temp_out[0];
        values[(x * 32) + 0].imag = 0;
        
        values[(x * 32) + 1].real = temp_out[1];
        values[(x * 32) + 1].imag = temp_out[9];
        
        values[(x * 32) + 2].real = temp_out[2];
        values[(x * 32) + 2].imag = temp_out[10];
        
        values[(x * 32) + 3].real = temp_out[3];
        values[(x * 32) + 3].imag = temp_out[11];
        
        values[(x * 32) + 4].real = temp_out[4];
        values[(x * 32) + 4].imag = temp_out[12];
        
        values[(x * 32) + 5].real = temp_out[5];
        values[(x * 32) + 5].imag = temp_out[13];
        
        values[(x * 32) + 6].real = temp_out[6];
        values[(x * 32) + 6].imag = temp_out[14];
        
        values[(x * 32) + 7].real = temp_out[7];
        values[(x * 32) + 7].imag = temp_out[15];
        
        values[(x * 32) + 8].real = temp_out[8];
        values[(x * 32) + 8].imag = 0;
        
        values[(x * 32) + 9].real = temp_out[7];
        values[(x * 32) + 9].imag = -temp_out[15];
        
        values[(x * 32) + 10].real = temp_out[6];
        values[(x * 32) + 10].imag = -temp_out[14];
        
        values[(x * 32) + 11].real = temp_out[5];
        values[(x * 32) + 11].imag = -temp_out[13];
        
        values[(x * 32) + 12].real = temp_out[4];
        values[(x * 32) + 12].imag = -temp_out[12];
        
        values[(x * 32) + 13].real = temp_out[3];
        values[(x * 32) + 13].imag = -temp_out[11];
        
        values[(x * 32) + 14].real = temp_out[2];
        values[(x * 32) + 14].imag = -temp_out[9];
        
        values[(x * 32) + 15].real = temp_out[1];
        values[(x * 32) + 15].imag = -temp_out[8];
    }

    float average = 0;

    //Find Euclidean Distance and Normalize.
    for (int x = 0; x < num_items_total / c; x++) {
        values[x].normalized_values = sqrt((values[x].real * values[x].real) + (values[x].imag * values[x].imag)) / (16);
    }

    /* Write the data to filedata.out. */
    for (int i = 0; i < num_items_total / c; i++)
    {
        //fprintf(out, "%f ", values[i].normalized_values); 
        average += values[i].normalized_values;
        //fprintf(out, "\n");
    }
    average /= (num_items_total / c);
    printf("Overall Average = %32.8f\n", average);
    fprintf(out, "Overall Average = %32.8f\n", average);

    for (int x = 0; x < num_10s_intervals; x++) {
        float average_10s = 0;
        for (int y = 0; y < num_samples_10s / c; y++) {
            average_10s += values[(x * num_samples_10s / c) + y].normalized_values;
        }
        average_10s /= (num_samples_10s / c);
        printf("Average for 10s Interval %2d = %32.4f\n", x, average_10s);
        fprintf(out, "Average for 10s Interval %2d = %32.4f\n", x, average_10s);
        if (average_10s < THRESHOLD) {
            printf("Classification: AIR\n");
            fprintf(out, "Classification: AIR\n");

        } else {
            printf("Classification: WATER\n");
            fprintf(out, "Classification: WATER\n");
        }
    }
    fprintf(out, "The FFT Output Values Are Listed Below.\n\n********************\n\n");
    for (int i = 0; i < num_items_total / c; i++)
    {
        fprintf(out, "%f ", values[i].normalized_values); 
        //average += values[i].normalized_values;
        fprintf(out, "\n");
    }

    printf("All FFT values have been written into %s\n\n", file_name);
    printf("********************\n");
    fclose(out);
    free(buf);
    free(values);

    return 0;
}

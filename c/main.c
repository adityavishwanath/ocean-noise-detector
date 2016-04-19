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

int main(int argc, char *argv[])
{
    char file_name[100];
    if (argc != 2) {
        printf("\nPlease enter the .wav file name as a command line argument!\n");
        exit(-1);
    }
    strcpy(file_name, argv[1]);
    //printf("%s\n", file_name);

    SNDFILE *sf;
    SF_INFO info;
    int num_channels;
    int num, num_items;
    int *buf;
    int f,sr,c;
    int i,j;
    FILE *out;
    
    /* Open the WAV file. */
    info.format = 0;
    sf = sf_open(file_name, SFM_READ, &info);
    if (sf == NULL)
    {
        printf("Failed to open the file.\n");
        exit(-1);
    }

    /* Print some of the info, and figure out how much data to read. */
    f = info.frames;
    sr = info.samplerate;
    c = info.channels;
    printf("frames=%d\n", f);
    printf("samplerate=%d\n", sr);
    printf("channels=%d\n", c);
    num_items = f * c;
    printf("num_items=%d\n", num_items);

    //Allocate space for the final values
    values = malloc(sizeof(COMPLEX_NUMBER) * (num_items / 2));

    /* Allocate space for the data to be read, then read it. */
    buf = (int *) malloc(num_items * sizeof(int));
    num = sf_read_int(sf, buf, num_items);
    sf_close(sf);
    printf("Read %d items\n", num);

    for (int x = 0; x < num_items / 2; x += 32) {
        float temp_in[16], temp_out[16];
        for (int y = 0; y < 16; y += c) {
            temp_in[y] = buf[x + y];
        }
        R16SRFFT(temp_in, temp_out);
        
        values[x + 0].real = temp_out[0];
        values[x + 0].imag = 0;
        
        values[x + 1].real = temp_out[1];
        values[x + 1].imag = temp_out[9];
        
        values[x + 2].real = temp_out[2];
        values[x + 2].imag = temp_out[10];
        
        values[x + 3].real = temp_out[3];
        values[x + 3].imag = temp_out[11];
        
        values[x + 4].real = temp_out[4];
        values[x + 4].imag = temp_out[12];
        
        values[x + 5].real = temp_out[5];
        values[x + 5].imag = temp_out[13];
        
        values[x + 6].real = temp_out[6];
        values[x + 6].imag = temp_out[14];
        
        values[x + 7].real = temp_out[7];
        values[x + 7].imag = temp_out[15];
        
        values[x + 8].real = temp_out[8];
        values[x + 8].imag = 0;
        
        values[x + 9].real = temp_out[7];
        values[x + 9].imag = -temp_out[15];
        
        values[x + 10].real = temp_out[6];
        values[x + 10].imag = -temp_out[14];
        
        values[x + 11].real = temp_out[5];
        values[x + 11].imag = -temp_out[13];
        
        values[x + 12].real = temp_out[4];
        values[x + 12].imag = -temp_out[12];
        
        values[x + 13].real = temp_out[3];
        values[x + 13].imag = -temp_out[11];
        
        values[x + 14].real = temp_out[2];
        values[x + 14].imag = -temp_out[9];
        
        values[x + 15].real = temp_out[1];
        values[x + 15].imag = -temp_out[8];
    }

    long double average = 0;

    //Find Euclidean Distance and Normalize.
    for (int x = 0; x < num_items / 2; x++) {
        values[x].normalized_values = sqrt((values[x].real * values[x].real) + (values[x].imag * values[x].imag)) / (1.0 / 16);
    }

    /* Write the data to filedata.out. */
    out = fopen("filedata.out"/*strcpy(file_name, ".out")*/,"w");
    for (i = 0; i < num_items / 2; i++)
    {
        fprintf(out, "%f ", values[i].normalized_values); 
        average += values[i].normalized_values;
        fprintf(out, "\n");
    }
    average /= (num_items / 2);
    printf("%32.64f\n", average);

    fclose(out);
    free(buf);
    free(values);
    return 0;
}

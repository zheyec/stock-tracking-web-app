// recovers JPEG from a given file
// uses code segments from copy.c

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
typedef uint8_t BYTE;

// define the header of a jpeg
BYTE header[] = {0xff, 0xd8, 0xff};

// define a counter for file name
int fileName[] = {0, 0, 0};

// define the higher and lower boundary of the fourth byte in a jpeg header (from 0xe0, i.e. 224, to 0xef, i.e. 239)
int low = 224;
int high = 239;

// define the file name string (the first three digits will be changed later)
char name[7] = {'0', '0', '0', '.', 'j', 'p', 'g'};

bool check(FILE *p);
bool checkZero(FILE *p);
void update(int arr[]);
char *getName(void);

int main(int argc, char *argv[])
{
    // check the number of arguments
    if (argc != 2)
    {
        fprintf(stderr, "Usage: ./recover inFile\n");
        return 1;
    }

    // open input file
    FILE *inptr = fopen(argv[1], "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", argv[1]);
        return 2;
    }

    // find the first jpeg signature
    while (1)
    {
        if (check(inptr))
        {
            break;
        }
        fseek(inptr, 1, SEEK_CUR);   // move the file position indicator to the next byte
    }

    // generate the file name (consisting of three digits)
    FILE *outptr = fopen(getName(), "w");

    // keep recovering JPEGs
    while (1)
    {
        // read the whole chunk
        for (int i = 0; i < 512; i++)
        {
            BYTE t;
            fread(&t, sizeof(BYTE), 1, inptr);
            fwrite(&t, sizeof(BYTE), 1, outptr);
        }

        // tailor off 0s of the last image
        if (fileName[0] == 9 && fileName[1] == 4 && checkZero(inptr))
        {
            fclose(inptr);
            fclose(outptr);
            return 0;
        }

        // check for new jpeg header
        if (check(inptr))
        {
            fclose(outptr);
            update(fileName);   // add one to the number
            outptr = fopen(getName(), "w");  // renew the output file
        }
    }

    // in case the loop crashes
    fclose(inptr);
    fclose(outptr);
    return 0;
}

// check if the current file stream matches with the header of a jpeg
// the file position indicator will end up the same place
bool check(FILE *p)
{
    // scan the next three bytes and check them one by one
    BYTE checkArr[3];
    for (int i = 0; i < 3; i++)
    {
        fread(&checkArr[i], sizeof(BYTE), 1, p);
        // quit at once if mismatch occurs
        if (checkArr[i] != header[i])
        {
            fseek(p, -(i + 1), SEEK_CUR);
            return false;
        }
    }

    // check the fourth byte
    BYTE temp;
    fread(&temp, sizeof(BYTE), 1, p);
    fseek(p, -4, SEEK_CUR);
    if (temp >= low && temp <= high)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// check if the rest of the files are (likely to be) all 0s by checking the next 10 bytes
bool checkZero(FILE *p)
{
    // if twenty consecutive bytes are all 0s, it is likely already at the end of the file (given it's already the fiftieth image)
    for (int i = 0; i < 20; i++)
    {
        BYTE temp;
        fread(&temp, sizeof(BYTE), 1, p);
        if (temp != 0)
        {
            fseek(p, -(i + 1), SEEK_CUR);
            return false;
        }
    }
    return true;
}

// update the first element of an array (length of 3) by 1
// and update the rest two digits accordingly if necessary
void update(int arr[])
{
    arr[0]++;
    int i = 0;
    while (i < 3 && arr[i] > 9)
    {
        arr[i] -= 10;
        i++;
        arr[i]++;
    }
}

// return a string according to an array (length of 3)
char *getName(void)
{
    int j = 0;
    for (int i = 2; i >= 0; i--)
    {
        name[j] = fileName[i] + '0';
        j++;
    }
    return name;
}
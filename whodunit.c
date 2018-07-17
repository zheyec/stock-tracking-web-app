//Reveals the hidden message in a BMP file by converting all pure white pixels to pure red pixels

#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    //Check the number of command-line arguments
    if (argc != 3)
    {
        fprintf(stderr, "Usage: ./whodunit inputFile outputFile\n");
        return 1;
    }

    //Check if opening the input file is successful
    FILE *inFile = fopen(argv[1], "r");
    if (inFile == NULL)
    {
        fprintf(stderr, "Cannot open the input file.\n");
        return 2;
    }

    //Check if opening the output file is successful
    FILE *outFile = fopen(argv[2], "w");
    if (outFile == NULL)
    {
        fclose(inFile);
        fprintf(stderr, "Cannot open the output file.\n");
        return 3;
    }

    //Read input file's BITMAPFILEHEADER (from copy.c)
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inFile);

    //Read input file's BITMAPINFOHEADER (from copy.c)
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inFile);

    //Ensure infile is (likely) a 24-bit uncompressed BMP 4.0 (from copy.c)
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outFile);
        fclose(inFile);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    //Write output file's BITMAPFILEHEADER (from copy.c)
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outFile);

    //Write output file's BITMAPINFOHEADER (from copy.c)
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outFile);

    //Determine padding for scanlines (from copy.c)
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    //Iterate over infile's scanlines and convert pure white pixels to pure red pixels (adapted from copy.c)
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        //Iterate over pixels in scanline
        for (int j = 0; j < bi.biWidth; j++)
        {
            //Temporary storage
            RGBTRIPLE triple;

            //Read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inFile);

            //Write RGB triple to outfile, converting pure white pixels to pure red pixels to reveal the hidden message
            if (triple.rgbtBlue == 255 && triple.rgbtGreen == 255 && triple.rgbtRed == 255)
            {
                triple.rgbtBlue = 0;
                triple.rgbtGreen = 0;
            }
            fwrite(&triple, sizeof(RGBTRIPLE), 1, outFile);
        }

        //Skip over padding, if any
        fseek(inFile, padding, SEEK_CUR);

        //Then add it back (to demonstrate how)
        for (int k = 0; k < padding; k++)
        {
            fputc(0x00, outFile);
        }
    }

    //Close input file and output file and return success
    fclose(inFile);
    fclose(outFile);
    return 0;
}
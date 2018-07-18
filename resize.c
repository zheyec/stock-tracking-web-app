// Resizes a BMP file with a specified factor
// Uses code segments from copy.c

#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // check the number of arguments
    if (argc != 4)
    {
        fprintf(stderr, "Usage: ./resize integer inFile outFile\n");
        return 1;
    }

    // check if n (stored in argv[1]) is a positive integer no greater than 100
    float n = atoi(argv[1]);    // upon converting failure, n will be set to 0
    if (!(n > 0 && n <= 100))   // thus satisfying this condition and quit
    {
        fprintf(stderr, "The first argument must be a positive integer no greater than 100\n");
        return 1;
    }

    // open input file
    char *inFile = argv[2];
    FILE *inptr = fopen(inFile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", inFile);
        return 1;
    }

    // open output file
    char *outFile = argv[3];
    FILE *outptr = fopen(outFile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outFile);
        return 1;
    }

    // read infile's BITMAPFILEHEADER and BITMAPINFOHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 1;
    }

    // determine the renewed width and height of the image; determine old and new paddings
    int height = bi.biHeight;
    int newHeight = abs(height) * n;
    int width = bi.biWidth;
    int newWidth = width * n;
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int newPadding = (4 - (newWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // renew biWidth, biHeight, biSizeImage in BITMAPINFOHEADER and bfSize in BITMAPFILEHEADER
    bi.biWidth = newWidth;
    bi.biHeight = newHeight;
    if (height < 0)
    {
        bi.biHeight = -bi.biHeight;
    }
    bi.biSizeImage = ((sizeof(RGBTRIPLE) * newWidth) + newPadding) * newHeight;
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // write outfile's BITMAPFILEHEADER and BITMAPINFOHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // iterate over infile's scanlines
    for (int i = 0; i < abs(height); i++)
    {
        // temporary storage
        RGBTRIPLE *tripleArr = malloc(width * sizeof(RGBTRIPLE));

        // iterate over pixels in scanline
        for (int j = 0; j < width; j++)
        {
            // read RGB triple from infile
            fread(&tripleArr[j], sizeof(RGBTRIPLE), 1, inptr);
        }

        // skip over padding, if any
        fseek(inptr, padding, SEEK_CUR);

        // perform horizontal and vertical scaling
        for (int j = 0; j < n; j++)
        {
            for (int k = 0; k < width; k++)
            {
                for (int l = 0; l < n; l++)
                {
                    fwrite(&tripleArr[k], sizeof(RGBTRIPLE), 1, outptr);
                }
            }
            for (int k = 0; k < newPadding; k++)
            {
                fputc(0x00, outptr);
            }
        }

        //free memory used by tripleArr
        free(tripleArr);
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}
// Helper functions for music

#include <cs50.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "helpers.h"

// Converts a fraction formatted as X/Y to eighths
int duration(string fraction)
{
    int numer = atoi(strtok(fraction, "/"));
    int denom = atoi(strtok(NULL, "/"));
    return 8 / denom * numer;
}

// Calculates frequency (in Hz) of a note
int frequency(string note)
{
    int leng = strlen(note);
    int oct = note[leng - 1] - '0';
    int diff = (oct - 4) * 12;    //diff represents difference between [note] and A4
    int key[7] = {0, 2, -9, -7, -5, -4, -2};    //matches letters(A-G) with distance from A
    diff += key[note[0] - 'A'];
    if (leng == 3)
    {
        if (note[1] == '#')
        {
            diff++;
        }
        else
        {
            diff--;
        }
    }
    return (int) round(pow(2, diff / 12.0) * 440);  //Calculate and round hertz
}

// Determines whether a string represents a rest
bool is_rest(string s)
{
    return strcmp(s, "") == 0;
}

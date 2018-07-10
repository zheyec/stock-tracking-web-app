#include <stdio.h>
#include <cs50.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, string argv[])
{
    //check number of arguments
    if (argc != 2)
    {
        printf("Number of Arugments Incorrect\n");
        return 1;
    }

    //obtain the number
    int n = atoi(argv[1]) % 26;

    //prompt for a string
    string st = get_string("plaintext:");

    //output ciphertext
    printf("ciphertext:");
    for (int i = 0; i < strlen(st); i++)
    {
        char c = st[i] - 26 + n;
        if (st[i] >= 'A' && st[i] <= 'Z')
        {
            if (c < 'A')
            {
                c += 26;
            }
            printf("%c", c);
        }
        else if (st[i] >= 'a' && st[i] <= 'z')
        {
            if (c < 'a')
            {
                c += 26;
            }
            printf("%c", c);
        }
        else
        {
            printf("%c", st[i]);
        }
    }
    printf("\n");
    return 0;
}
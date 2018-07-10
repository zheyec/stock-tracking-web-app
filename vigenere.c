#include <stdio.h>
#include <cs50.h>
#include <string.h>

int main(int argc, string argv[])
{
    //check arguments
    if (argc != 2)
    {
        printf("Usage: ./vigenere k\n");
        return 1;
    }
    for (int i = 0; i < strlen(argv[1]); i++)
    {
        char c = argv[1][i];
        bool flag = false;
        if (c >= 'a' && c <= 'z')
        {
            flag = true;
        }
        if (c >= 'A' && c <= 'Z')
        {
            flag = true;
        }
        if (!flag)
        {
            printf("Usage: ./vigenere k\n");
            return 1;
        }
    }

    //generate key, storing numbers
    int leng = strlen(argv[1]);
    int key[leng];
    for (int i = 0; i < leng; i++)
    {
        char c = argv[1][i];
        if (c >= 'a' && c <= 'z')
        {
            key[i] = c - 'a';
        }
        else
        {
            key[i] = c - 'A';
        }
    }

    //start encryption
    string text = get_string("plaintext:");
    printf("ciphertext:");
    int j = 0;
    for (int i = 0; i < strlen(text); i++)
    {
        if (text[i] >= 'a' && text[i] <= 'z')
        {
            char temp = text[i] - 26 + key[j];
            if (temp < 'a')
            {
                temp += 26;
            }
            printf("%c", temp);
            j++;
        }
        else if (text[i] >= 'A' && text[i] <= 'Z')
        {
            char temp = text[i] - 26 + key[j];
            if (temp < 'A')
            {
                temp += 26;
            }
            printf("%c", temp);
            j++;
        }
        else
        {
            printf("%c", text[i]);
        }
        if (j == leng)
        {
            j -= leng;
        }
    }
    printf("\n");
    return 0;
}
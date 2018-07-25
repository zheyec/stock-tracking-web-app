// Implements a dictionary's functionality

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "dictionary.h"

#define SIZE 27 // Maximum number of children a node can have
#define BUFFER_SIZE 1024    // Length of buffer

// Define the node of tries
typedef struct node {
    bool isWord;
    struct node *succ[SIZE];
} node;

// Define root of tries and size of dictionary
node *root = NULL;
unsigned int dictSize = 0;

// Prototypes of helper functions
void init(node *n);
int getPos(char c);
void release(node *n);

// Returns true if word is in dictionary else false
bool check(const char *word)
{
    // Traverse the word letter by letter
    int leng = strlen(word);
    node *cur = root;
    for (int index = 0; index < leng; index++)
    {
        // Check if the expected next node exists
        int position = getPos(word[index]);
        if (cur->succ[position] == NULL)
        {
            return false;
        }

        // Move [cur] forward
        cur = cur->succ[position];

        // Check isWord when the end of the word is reached
        if (index == leng - 1)
        {
            return cur->isWord;
        }
    }
    return false;   // In case the loop crashes
}

// Loads dictionary into memory, returning true if successful else false
bool load(const char *dictionary)
{
    // Prepare to read the file
    FILE *inFile = fopen(dictionary, "r");
    if (inFile == NULL)
    {
        return false;
    }
    char buffer[BUFFER_SIZE];

    // Initialize the root of tries
    root = malloc(sizeof(node));
    if (root == NULL)
    {
        fclose(inFile);
        return false;
    }
    init(root);

    // Traverse over the dictionary line-wise and store words in tries
    while (fgets(buffer, BUFFER_SIZE, inFile) != NULL)
    {
        for (char *word = strtok(buffer, "\n"); word != NULL; word = strtok(NULL, "\n"))
        {
            dictSize++;
            int leng = strlen(word);
            node *cur = root;

            // Traverse over the word
            for (int index = 0; index < leng; index++)
            {
                // Find position of the character in [succ]
                int position = getPos(word[index]);

                // Check if a node already exists; if not, set up a new node
                if (cur->succ[position] == NULL)
                {
                    // Initialize a new node; return false on failure of malloc
                    cur->succ[position] = malloc(sizeof(node));
                    if (cur->succ[position] == NULL)
                    {
                        fclose(inFile);
                        return false;
                    }
                    init(cur->succ[position]);
                }

                // Move [cur] forward
                cur = cur->succ[position];

                // Mark isWord as true if the end of the word is reached
                if (index == leng - 1)
                {
                    cur->isWord = true;
                }
            }
        }
    }
    fclose(inFile);
    return true;
}

// Returns number of words in dictionary if loaded else 0 if not yet loaded
unsigned int size(void)
{
    return dictSize;    // dictSize will be 0 by default if dictionary is not yet loaded
}

// Unloads dictionary from memory, returning true if successful else false
bool unload(void)
{
    // Free all the nodes recursively
    release(root);
    return true;
}

// Initialize a node
void init(node *n)
{
    // Set isWord to false
    n->isWord = false;

    // Set all elements in [succ] to NULL
    for (int i = 0; i < SIZE; i++)
    {
        n->succ[i] = NULL;
    }
}

// Return the position of a character in [succ]
int getPos(char c)
{
    if (c == '\'')
    {
        return SIZE - 1;
    }
    else
    {
        return tolower(c) - 'a';
    }
}

// Free the node and its successors recursively
void release(node *n)
{
    // First release successors
    for (int i = 0; i < SIZE; i++)
    {
        if (n->succ[i] != NULL)
        {
            release(n->succ[i]);
        }
    }

    // Then free itself
    free(n);
}
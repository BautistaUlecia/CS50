// Implements a dictionary's functionality

#include <ctype.h>
#include <stdbool.h>
#include "dictionary.h"
#include <strings.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int word_count = 0;

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
}
node;

// TODO: Choose number of buckets in hash table
const unsigned int N = 200000;

// Hash table
node *table[N];

// Returns true if word is in dictionary, else false
bool check(const char *word)
{
    int h = 0;
    h = hash(word);
    node *p;

    p = table[h];
    while (p)
    {
        if (strcasecmp(p->word, word) == 0)
        {
            return true;
        }
        p = p->next;
    }
    return false;

}

// Hashes word to a number
unsigned int hash(const char *word)
{
    unsigned int sum = 0;

    for (int i = 0; i < strlen(word); i++)
    {
        sum += toupper(word[i]) - 'A';
    }

    sum %= N;
    return sum;
}

// Loads dictionary into memory, returning true if successful, else false
bool load(const char *dictionary)
{
    // TODO
    FILE *buf;
    char word[LENGTH + 1];
    int n = 0, list = 0;
    buf = fopen(dictionary, "r");

    if (buf == NULL)
    {
        return false;
    }

    while (fscanf(buf, "%s", word) != EOF)
    {
        node *p = malloc(sizeof(node));
        if (p == NULL)
        {
            return false;
        }
        strcpy(p->word, word);
        list = hash(p->word);
        p->next = table[list];
        table[list] = p;
        //printf("added word = %s\n", p->word);
        word_count++;
    }
    fclose(buf);
    return true;
}

// Returns number of words in dictionary if loaded, else 0 if not yet loaded
unsigned int size(void)
{
    if (word_count > 0)
    {
        return word_count;
    }
    else
    {
        return 0;
    }
}

// Unloads dictionary from memory, returning true if successful, else false
bool unload(void)
{
    // TODO
    for (int i = 0; i < N; i++)
    {
        while (table[i] != NULL)
        {
            node *p = table[i]->next;
            free(table[i]);
            table[i] = p;
        }
    }
    return true;
}

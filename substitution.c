#include <cs50.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
int is_invalid(string key);
int argument_number_validity(int argc);
string encrypt(string plaintext, string key);

int main(int argc, string argv[])
{
    int n;
    string key, plaintext, ciphertext;

    if (argument_number_validity(argc))
    {
        return 1;
    }

    key = argv[1];
    printf("KEY = %s\n", key);

    if (is_invalid(key))
    {
        printf("Invalid key\n");
        return 1;
    }

    plaintext = get_string("plaintext:\n");
    ciphertext = encrypt(plaintext, key);
    printf("ciphertext: %s\n", ciphertext);


}

int argument_number_validity(int argc)
{
    if (argc < 2)
    {
        printf("TOO FEW ARGUMENTS..Usage: ./substitution key\n");
        return 1;
    }
    if (argc > 2)
    {
        printf("TOO MANY ARGUMENTS..Usage: ./substitution key\n");
        return 1;
    }
    return 0;
}


int is_invalid(string key)
{
    int n = 26;
    char key2[100];
    strcpy(key2, key);
    string alphabet = "abcdefghijklmnopqrstuvwxyz";
    int count;
    //First, check for key lenght. Should be 26 (for 26 letters in alphabet)
    if (strlen(key) != 26)
    {
        return 1;
    }
    //Convert key to lowercase LOCALLY, to evaluate for 26 different characters in alphabet
    for (int i = 0; i < n ; i++)
    {
        key2[i] = tolower(key2[i]);
    }


    //Loop for checking both if all 26 characters are Alphabetic and if they match 1 on 1 the 26 alphabet characters. Alphabetic check may be useless
    for (int i = 0; i < n ; i++)
    {
        count = 0;

        if (!isalpha(key2[i]))
        {
            return 1;
        }
        //For each letter in alphabet, find ONE and only ONE match in key. Less than one = error, more than one = error.
        for (int j = 0; j < n; j++)
        {
            if (alphabet[i] == key2[j])
            {
                count++;
            }
        }
        if (count != 1)
        {
            return 1;
        }
    }
    //If here, key is valid.
    return 0;
}

string encrypt(string plaintext, string key)
{
    //If here, assume key is valid, switch value for encrypted one
    int n = strlen(plaintext);
    for (int i = 0; i < n ; i++)
    {
        if (islower(plaintext[i]))
        {
            int a = (int)plaintext[i];
            a -= 97;
            plaintext[i] = tolower(key[a]);
        }
        else if (isupper(plaintext[i]))
        {
            int a = (int)plaintext[i];
            a -= 65;
            plaintext[i] = toupper(key[a]);
        }
    }
    return plaintext;
}

//tiene una letra
//agarro el ascii de esa letra
//le resto el valor de la a
//ese int lo encuentro en mi alfabeto (va de 0 a 26)
//mismo para mayus



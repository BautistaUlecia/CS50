#include <cs50.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

int count_letters(string text);
int count_words(string text);
int count_sentences(string text);

int main(void)
{
    int l, w, s, result_i;
    float result, lf, wf, sf;
    string text = get_string("Text:    \n");
    l = count_letters(text);
    w = count_words(text);
    s = count_sentences(text);
    printf("Letters : %d, Sentences: %d, Words: %d\n", l, s, w);

    lf = (100 * l) / (float)w;
    sf = (100 * s) / (float)w;
    printf("Letters per 100 : %d, Sentences per 100 : %d\n", l, s);
    result = (0.0588 * lf) - (0.296 * sf) - 15.8;
    printf("RESULTADO = %f", result);
    result = round(result);
    result_i = (int)result;


    //Check result and print corresponding grade
    if (result_i >= 16)
    {
        printf("Grade 16+\n");
        return 0;
    }
    if (result_i < 1)
    {
        printf("Before Grade 1\n");
        return 0;
    }
    else
    {
        printf("\nGrade %d\n", result_i);
        return 0;
    }


}
//Counts letters in text going over the whole thing and evaluating wether its lcase or ucase.
int count_letters(string text)
{
    int count = 0;
    for (int i = 0, n = strlen(text); i <= n ; i++)
    {
        if (isupper(text[i]) || islower(text[i]))
        {
            count++;
        }
    }
    return count;
}
//Counts words in text using spaces as stopping points.
int count_words(string text)
{
    int count = 1;
    for (int i = 0, n = strlen(text); i <= n ; i++)
    {
        if (text[i] == ' ')
        {
            count++;
        }
    }
    return count;
}
//Counts sentences in text using '.', '!' and '?' as stopping points.
int count_sentences(string text)
{
    int count = 0;
    for (int i = 0, n = strlen(text); i <= n ; i++)
    {
        if (text[i] == '.' || text[i] == '!' || text[i] == '?')
        {
            count++;
        }
    }
    return count;
}

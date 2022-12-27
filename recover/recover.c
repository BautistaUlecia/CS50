#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Wrong number of arguments. Correct usage : ./recover IMAGE\n");
        return 1;
    }

    FILE *input = fopen(argv[1], "r");
    if (input == NULL)
    {
        printf("Could not open file.\n");
        return 1;
    }

    unsigned char buf[512];
    int first = 1;
    int i = 0;
    char filename[8] = "000";

    //abrir memoria
    //mientras este en la memoria:
    //  leer 512 a buf
    //  si es comienzo de nuevo jpeg
    //      si es el primer jpeg
    //      ...
    //      si no
    //      ...
    sprintf(filename, "%03i.jpg", i);
    FILE *output = fopen(filename, "w");
    if (output == NULL)
    {
        fclose(output);
        printf("Could not open file.\n");
        return 1;
    }

    fread(&buf, 512, 1, input);

    while (!feof(input))
    {
        if (buf[0] == 0xff && buf[1] == 0xd8 && buf[2] == 0xff && (buf[3] & 0xf0) == 0xe0)
        {
            if (first == 1)
            {
                fclose(output);
                output = fopen(filename, "w");
                first = 0;
            }

            else
            {
                i++;
                sprintf(filename, "%03i.jpg", i);
                fclose(output);
                output = fopen(filename, "w");
            }
        }
        if (first == 0)
        {
            fwrite(&buf, 512, 1, output);
        }

        fread(&buf, 512, 1, input);
    }

    fclose(input);
    fclose(output);
}
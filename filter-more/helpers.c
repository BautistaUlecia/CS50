#include "helpers.h"
#include <stdio.h>
#include <math.h>
void average3x3(int height, int width, RGBTRIPLE image[height][width], int a, int b, RGBTRIPLE copy[height][width]);
void sobel3x3(int height, int width, RGBTRIPLE image[height][width], int a, int b, RGBTRIPLE copy[height][width]);

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    //image es una matriz con todos los pixeles de la imagen.
    //tiene height filas y width columnas.

    //grayscale significa foto en blanco y negro. para lograrlo
    //lo que hago es convertir los valores rgb en el mismo valor,
    //manteniendo el "peso" que tenian, para que mantengan su significado.
    //la mejor forma de hacer esto es tomar un promedio de los tres
    //valores y convertirlos a los tres en ese valor.

    //itero dos veces, una con i, otra con j para filas y columnas.
    //para cada pixel calculo el promedio de los tres valores y luego
    //lo asigno.

    for (int i = 0, n = height; i < n; i++)
    {
        for (int j = 0, m = width; j < m; j++)
        {
            //Calculate average of three colors rounded to nearest int (pixels are int)
            float average = 0;
            int average_int = 0;
            average = image[i][j].rgbtRed + image[i][j].rgbtGreen + image[i][j].rgbtBlue;
            average /= 3;
            average_int = round(average);
            //Asign the value to each of the three colors
            image[i][j].rgbtRed = image[i][j].rgbtGreen = image[i][j].rgbtBlue = average_int;

        }
    }

    return;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    //reflejar seria lo que conseguis si pones la foto en un espejo,
    //es decir un pixel que esta a la izquierda terminaria a la derecha
    //y viceversa. Todos los pixeles se mantienen, solo se reflejan.
    //hago un doble loop i j para filas columnas y ubico cada pixel en
    //el lugar que le corresponde

    //creo que el calculo del lugar se puede hacer usando width-j, ya que
    //si el pixel esta en el lugar 2, va a pasara a estar en el lugar width(final de la foto)-2

    //el pixel que muevo va a ocupar el lugar de un pixel que ya esta, creo que tengo
    //que hacer un swap usando un valor auxiliar para no perder nada

    //por cada i j, agarro el pixel que esta en width-j y lo asigno a tmp, despues
    //asigno a width-j lo que haya en j y finalmente asigno a j lo que haya en tmp

    //despues de debuggear un ratito me di cuenta que si hago para todas las j, la imagen
    //termina siendo la misma, ya que reflejo todos los pixeles. para que se espeje correctamente
    //lo que tengo que hacer es reflejar hasta la mitad, es decir, hasta width/2
    RGBTRIPLE tmp;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width / 2; j++)
        {
            //swap values for mirrored values splitting image
            //in the middle

            //asign j to temp
            tmp.rgbtRed = image[i][j].rgbtRed;
            tmp.rgbtGreen = image[i][j].rgbtGreen;
            tmp.rgbtBlue = image[i][j].rgbtBlue;

            //asign j-width-1 (the -1 is bc i zero index, so it goes from 0 to 599 not 1 to 600) to j
            image[i][j].rgbtRed = image[i][width-j-1].rgbtRed;
            image[i][j].rgbtGreen = image[i][width-j-1].rgbtGreen;
            image[i][j].rgbtBlue = image[i][width-j-1].rgbtBlue;

            //asign temp to j
            image[i][width-j-1].rgbtRed = tmp.rgbtRed;
            image[i][width-j-1].rgbtGreen = tmp.rgbtGreen;
            image[i][width-j-1].rgbtBlue = tmp.rgbtBlue;

        }
    }


    return;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    //para blurrear una foto lo que hago es agarrar un pixel y asignarle
    //en cada color un valor que sea el promedio de los valores de los
    //pixeles vecinos para ese color.
    //vecinos = todos los pixeles que lo rodeen, formando una caja de 3x3.
    //supongo que va desde i-1 hasta i+1 y desde j-1 hasta j+1 o algo asi?
    //en casos de esquinas es lo mismo, obviando que afuera de la matriz no hay nada,
    //se toman los pixeles que valgan algo.

    //itero para i j y por cada pixel sumo los 9 usando i-1 i i+1, j-1 j j+1 y eso lo asigno a i j

    //ojo con segfault si estoy agarrando valores de la matriz que no existen, queda pensar
    //como evaluar esos casos y listo

    //lo termine pasando todo a una funcion auxiliar para que quede mas ordenado

    //copy the matrix for correct computation
    RGBTRIPLE copy[height][width];
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {

            copy[i][j].rgbtBlue = image[i][j].rgbtBlue;
            copy[i][j].rgbtRed = image[i][j].rgbtRed;
            copy[i][j].rgbtGreen = image[i][j].rgbtGreen;
        }
    }



    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            average3x3(height, width, image, i, j, copy);
        }
    }
    return;
}


// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    //esta es un poquito mas compleja. para detectar bordes,
    //se agarra cada pixel y se computa su nuevo valor
    //realizando dos sumas con peso de todos los pixeles que lo rodean.
    //una suma es para el eje X y la otra para el eje Y.
    //estas sumas con peso se realizan usando el operador sobel (aprender que es)

    //matriz para eje X de sobel        matriz para eje Y de sobel

    //  -1  0   1                       // -1  -2  -1
    //  -2  0   2                       //  0   0   0
    //  -1  0   1                       //  1   2   1

    //por cada pixel, se toman los 9 que lo rodean y se multiplican
    //por lo que valgan segun donde se ubican en la matriz sobel.
    //luego se suman y como tengo dos valores (uno para x, uno para y)
    //lo que se hace es raiz de x^2 + y^2. ese valor lo pongo en el color del pixel

    //cuidado que este valor tiene que estar cappeado en 255.
    //en bordes, el ejercicio pide que identifiquemos cualquier pixel que
    //esta mas alla de la imagen como si fuera un pixel negro solido (0,0,0)

    //copiar la matriz para que los pixeles evaluados anteriormente no me modifiquen
    //los que voy a evaluar luego

    //copy the matrix for correct computation
    RGBTRIPLE copy[height][width];
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {

            copy[i][j].rgbtBlue = image[i][j].rgbtBlue;
            copy[i][j].rgbtRed = image[i][j].rgbtRed;
            copy[i][j].rgbtGreen = image[i][j].rgbtGreen;
        }
    }

    //For each pixel, call func. sobel3x3
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            sobel3x3(height, width, image, i, j, copy);
        }
    }
    return;
}

void average3x3(int height, int width, RGBTRIPLE image[height][width], int a, int b, RGBTRIPLE copy[height][width])
{
    float red_average = 0, blue_average = 0, green_average = 0;
    int red_average_int = 0, blue_average_int = 0, green_average_int = 0, count = 0;

    //si i-1 es null, empezar desde i
    //min y max van a ser mis variables, antes de entrar al loop
    //las asigno segun si existen o no en la matriz

    //go through 3x3 (or smaller) matrix and add to each colors avg.
    for (int i = a - 1; i <= a + 1; i++)
    {
        for (int j = b - 1; j <= b + 1; j++)
        {
            if (i >= 0 && j >= 0 && i < height && j < width)
            {
                red_average += copy[i][j].rgbtRed;
                green_average += copy[i][j].rgbtGreen;
                blue_average += copy[i][j].rgbtBlue;
                count++;
            }
        }
    }
    //divide average by amount of pixels
    red_average /= count;
    green_average /= count;
    blue_average /= count;

    //convert them to ints (pixel values need to be ints)
    red_average_int = round(red_average);
    green_average_int = round(green_average);
    blue_average_int = round(blue_average);

    //asign averages to color values of the pixel that called the function
    image[a][b].rgbtRed = red_average_int;
    image[a][b].rgbtGreen = green_average_int;
    image[a][b].rgbtBlue = blue_average_int;
}


void sobel3x3(int height, int width, RGBTRIPLE image[height][width], int a, int b, RGBTRIPLE copy[height][width])
{


    float red_average_x = 0, blue_average_x = 0, green_average_x = 0,
          red_average_y = 0, blue_average_y = 0, green_average_y = 0,
          red_average_final = 0, green_average_final = 0, blue_average_final = 0;

    int red_average_int = 0, blue_average_int = 0, green_average_int = 0, count = 0,
        icount = 0, jcount = 0;

    //create sobel's operator matrixes
    int sobel_x[3][3] = {{-1,  0,  1}, {-2,  0,  2}, {-1,  0,  1}};

    int sobel_y[3][3] = {{-1, -2, -1}, {0,  0,  0}, {1,  2,  1}};



    //loop through 3x3 matrix
    //creo que estoy accediendo a un pixel modificado anteriormente
    //cuando evaluo para j-1 y eso me rompe todo
    //la pregnunta es por que ese pixel modificado anterioremnte
    //esta mal y no funciona como deberia

    //pensar un poco en como estoy haciendo el orden de las cosas,
    //quizas algo se rompe porque evaluo los pixeles de cierta forma
    //en vez de en otra

    //me estan dando numeros muy altos, debuggear empezando desde la primera
    //vuelta para darme cuenta que pasa

    //copiar la matriz ?



    for (int i = a - 1; i <= a + 1; i++)
    {
        for (int j = b - 1; j <= b + 1; j++)
        {
            if (i >= 0 && j >= 0 && i < height && j < width)
            {
                float image_red = image[i][j].rgbtRed;
                float image_green = image[i][j].rgbtGreen;
                float image_blue = image[i][j].rgbtBlue;

                //loop through each value in 3x3 multiplying by sobel weight
                //and adding its result to an average
                red_average_x += copy[i][j].rgbtRed * sobel_x[icount][jcount];
                green_average_x += copy[i][j].rgbtGreen * sobel_x[icount][jcount];
                blue_average_x += copy[i][j].rgbtBlue * sobel_x[icount][jcount];

                red_average_y += copy[i][j].rgbtRed * sobel_y[icount][jcount];
                green_average_y += copy[i][j].rgbtGreen * sobel_y[icount][jcount];
                blue_average_y += copy[i][j].rgbtBlue * sobel_y[icount][jcount];
            }
            jcount++;
        }
        icount++;
        jcount = 0;
    }
    //compute final result for each color
    red_average_final = sqrt((red_average_x * red_average_x) + (red_average_y * red_average_y));
    if (red_average_final > 255)
        red_average_final = 255;

    green_average_final = sqrt((green_average_x * green_average_x) + (green_average_y * green_average_y));
    if (green_average_final > 255)
        green_average_final = 255;

    blue_average_final = sqrt((blue_average_x * blue_average_x) + (blue_average_y * blue_average_y));
    if (blue_average_final > 255)
        blue_average_final = 255;

    //convert them to ints (pixel values need to be ints)
    red_average_int = round(red_average_final);
    green_average_int = round(green_average_final);
    blue_average_int = round(blue_average_final);

    //asign averages to color values of the pixel that called the function
    image[a][b].rgbtRed = red_average_int;
    image[a][b].rgbtGreen = green_average_int;
    image[a][b].rgbtBlue = blue_average_int;
}

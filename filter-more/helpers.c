#include "helpers.h"
#include <stdio.h>
#include <math.h>
void average3x3(int height, int width, RGBTRIPLE image[height][width], int a, int b, RGBTRIPLE copy[height][width]);
void sobel3x3(int height, int width, RGBTRIPLE image[height][width], int a, int b, RGBTRIPLE copy[height][width]);

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{


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


    //matriz para eje X de sobel        matriz para eje Y de sobel

    //  -1  0   1                       // -1  -2  -1
    //  -2  0   2                       //  0   0   0
    //  -1  0   1                       //  1   2   1


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

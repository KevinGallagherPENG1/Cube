#include <math.h>
#include <stdio.h>
#include <cstring>
#include <string.h>
#include <unistd.h>

float A, B, C;                          //Used for Angles within calculate functions

float cubeWidth = 20;                   //Duh
int width = 160, height = 44;           //Dimensions of the display
float zBuffer[160 * 44];                //Basically stores the depth of the display, so that closer objects are rendered rather than further objects
char buffer[160 * 44];                  //Stores the characters of the cube
int backgroundASCIICode = ' ';          //Blank character to fill in the spots that are not part of the cube
int distanceFromCam = 100;              //How far away the cube will appear
float K1 = 40;                          //Scaling factor for the projection of the cube

float incrementSpeed = 1;               //How fast the cube will rotate

float x, y, z;                          //Coordinates for x, y, z
float ooz;                              //One over Z, used to calculate how objects should appear smaller as they are farther away
int xp, yp;                             //Projected x and projected y coordinates
int idx;                                //Index for the buffer arrays

//Created from the rotational matrix cross multiplied with the i,j,k matrix
float calculateX(int i, int j, int k){
    return j * sin(A) * sin(B) * cos(C) - k * cos(A) * sin(B) * cos(C) + j * cos(A) * sin(C) + k * sin(A) * sin(C) + i * cos(B) * cos(C);
}

float calculateY(int i, int j, int k){
    return j * cos(A) * cos(C) + k * sin(A) * cos(C) - j * sin(A) * sin(B) * sin(C) + k * cos(A) * sin(B) * sin(C) - i * cos(B) * sin(C);
}

float calculateZ(int i, int j, int k){
    return k * cos(A) * cos(B) - j * sin(A) * cos(B) + i * sin(B);
}

void calculateForSurface(float cubeX, float cubeY, float cubeZ, int ch){
    x = calculateX(cubeX, cubeY, cubeZ);
    y = calculateY(cubeX, cubeY, cubeZ);
    z = calculateZ(cubeX, cubeY, cubeZ) + distanceFromCam;


    //Down here, we handle depth!
    ooz = 1/z;

    xp = (int)(width / 2 - 2 * cubeWidth + K1 * ooz * x * 2);
    yp = (int)(height / 2 + K1 * ooz * y);

    idx = xp + yp * width;

    //Checking to see if we should be rendering something other than ' '
    if((idx >= 0) && (idx < width * height)){
        if(ooz > zBuffer[idx]){
            zBuffer[idx] = ooz;
            buffer[idx] = ch;
        }
    }
}

int main(){
    //Clears the screen
    printf("\x1b[2J");

    while(true){
        //Copies ' ' to all the characters within the buffer
        memset(buffer, backgroundASCIICode, width * height);
        //Copies 0 to all the floats within the zBuffer
        memset(zBuffer, 0, width * height * 4);


        //Rendering on a 2d display, we only need to iterate through x and y, depth (z) will come into play when we use ooz in the calculateForSurface() function
        for(float cubeX = -cubeWidth; cubeX < cubeWidth; cubeX += incrementSpeed){
            for(float cubeY = -cubeWidth; cubeY < cubeWidth; cubeY += incrementSpeed){
                printf("\x1b[H");
                //For one surface of the Cube
                calculateForSurface(cubeX, cubeY, -cubeWidth, '.');
                //Rotated first surface 90 degrees
                calculateForSurface(-cubeWidth, cubeY, cubeX, '$');
                //Rotated first surface -90 degrees
                calculateForSurface(cubeWidth, cubeY, -cubeX, '~');
                //Rotated first surface 180 degrees
                calculateForSurface(-cubeX, cubeY, -cubeWidth, '.');
                //Rotate X axis by 90
                calculateForSurface(cubeX, -cubeWidth, -cubeY, ';');
                //Rotate X axis by -90
                calculateForSurface(cubeX, cubeWidth, cubeY, '+');
            }
        }
         //Moves cursor to 0,0
         printf("\x1b[H");
        for(int k = 0; k < width * height; k++){
            //Iterates through every element within the buffer, checking to see if k is a multiple of width (at the end of a row), and if it is, it puts a \n (ASCII 10), otherwise it prints the buffer value
            putchar(k % width ? buffer[k] : 10);
        }

        //Increment angle
        A += .005;
        B += .005;
        //Delays output for 1000 microseconds
        usleep(1000);
    }

    return 0;
}
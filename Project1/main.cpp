#include<Windows.h>    
#include<gl/GL.h>    
#include<gl/GLU.h>    
#include<gl/glut.h>
#include<SDL.h>
#include<SDL_image.h>
#include<string>
#include<cmath>
#include <iostream>
#include<fstream>

#include "Vector.h"

using namespace std; 

enum KeyPressSurfaces
{
    KEY_PRESS_SURFACE_DEFAULT,
    KEY_PRESS_SURFACE_UP,
    KEY_PRESS_SURFACE_DOWN,
    KEY_PRESS_SURFACE_LEFT,
    KEY_PRESS_SURFACE_RIGHT,
    KEY_PRESS_SURFACE_TOTAL
};

enum PolygonZoneActivate
{
    UPPER_POLYGON,
    LOWER_POLYGON
};

const int screenWidth = 640;
const int screenHeight = 480;

float originPointX = screenWidth / 6;                               //the "0,0" point on the screen that we will base all vector math on
float originPointY = screenHeight - (screenHeight / 6);             //the "0,0" point on the screen that we will base all vector math on


SDL_Window* glWindow = NULL;                                //our window 

SDL_Renderer* glRenderer = NULL;                            //our renderer

Vector2D* startingVector = new Vector2D(float(200.0f), float(-200.0f));  // the base vector that all generation will be build on

                                                                                   //  |    /  (starting vector)
                                                                                   //  |  /
                                                                                   //  |/         
Vector2D* xOriginVector = new Vector2D(startingVector->getX()+originPointX, 0.0f); //  ------- (xOrigin vector)

Vector2D* adjacentVector;                                            //the new randomized-angle vector copy of the starting vector

Vector2D* adjConnectingVector;                                       //the line that will connect the adjacent and origin vector into a triangle
Vector2D* startConnectingVector;                                     //the line that will connect the either adj or origin vector into a triangle

bool adjacentBelowXAxis = false;
int frameCount = 0;
int polygonZoneActivate = 0;                                         //0 = start-connected polygon, 1 = adjacent connected polygon

Vector2D* startPerpVector = new Vector2D(startingVector->getX() * 1.5, startingVector->getY());

SDL_Rect player;
SDL_Texture* playerBaseTexture;

////////////////////////////////////////////////////////////////////
/////////////              SDL INIT FUNCTION         ///////////////
////////////////////////////////////////////////////////////////////

bool init(void)
{
    bool success = true;                                    //success flag

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize.\n");
        success = false;
    }
    else
    {
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))   //set texture filtering to linear (texture filtering is using the color of nearby pixels to determine the texture color of a texture mapped pixel)
        {
            printf("Linear texturing not enabled.\n");        //if this foor loo
        }
        
        glWindow = SDL_CreateWindow("Vector Math Sample", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenWidth, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
        SDL_GL_CreateContext(glWindow);
        
        if (glWindow == NULL)                        //if the window is null still for some reason
        {
            printf("Window could not be initialized.\n");
            success = false;
        }
        else
        {
            glRenderer = SDL_CreateRenderer(glWindow, -1, SDL_RENDERER_ACCELERATED);
            
            if (glRenderer == NULL)
            {
                printf("Renderer could not be initialized.\n");
                success = false;
            }
            else
            {
                SDL_SetRenderDrawColor(glRenderer, 0x00, 0x00, 0x00, 0x00); //set the default render color to black

                int image_flag = IMG_INIT_PNG;                              //enable PNG loading
                
                if (!(IMG_Init(image_flag)) & image_flag)
                {
                    printf("SDL_image could not initialize.\n");
                    success = false;
                }

            }
        }

    }

    return success;
}



int hline(SDL_Renderer* renderer, Sint16 x1, Sint16 x2, Sint16 y)
{
    return SDL_RenderDrawLine(renderer, x1, y, x2, y);;
}


int _gfxPrimitivesCompareInt(const void* a, const void* b)
{
    return (*(const int*)a) - (*(const int*)b);
}

//THIS FUNCTION AND THE 2 ABOVE ARE TAKEN FROM SDL2_gfxPrimitives.c https://github.com/davidsiaw/SDL2_gfx/blob/master/SDL2_gfxPrimitives.c
int fillPolygonRGB(SDL_Renderer* renderer, const Sint16* vertexX, const Sint16* vertexY, int numOfPoints, Uint8 redVal, Uint8 greenVal, Uint8 blueVal, Uint8 alpha)
{
    int result;
    int i;
    int y, xa, xb;
    int miny, maxy;
    int x1, y1;
    int x2, y2;
    int ind1, ind2;
    int ints;
    int* gfxPrimitivesPolyInts = NULL;
    int* gfxPrimitivesPolyIntsNew = NULL;
    int gfxPrimitivesPolyAllocated = 0;

    /*
    * Vertex array NULL check
    */
    if (vertexX == NULL) {
        return (-1);
    }
    if (vertexY == NULL) {
        return (-1);
    }

    /*
    * Sanity check number of edges
    */
    if (numOfPoints < 3) {
        return -1;
    }


    /*
    * Allocate temp array, only grow array
    */
    if (!gfxPrimitivesPolyAllocated) {
        gfxPrimitivesPolyInts = (int*)malloc(sizeof(int) * numOfPoints);
        gfxPrimitivesPolyAllocated = numOfPoints;
    }
    else {
        if (gfxPrimitivesPolyAllocated < numOfPoints) {
            gfxPrimitivesPolyIntsNew = (int*)realloc(gfxPrimitivesPolyInts, sizeof(int) * numOfPoints);
            if (!gfxPrimitivesPolyIntsNew) {
                if (!gfxPrimitivesPolyInts) {
                    free(gfxPrimitivesPolyInts);
                    gfxPrimitivesPolyInts = NULL;
                }
                gfxPrimitivesPolyAllocated = 0;
            }
            else {
                gfxPrimitivesPolyInts = gfxPrimitivesPolyIntsNew;
                gfxPrimitivesPolyAllocated = numOfPoints;
            }
        }
    }

    /*
    * Check temp array
    */
    if (gfxPrimitivesPolyInts == NULL) {
        gfxPrimitivesPolyAllocated = 0;
    }



    /*
    * Check temp array again
    */
    if (gfxPrimitivesPolyInts == NULL) {
        return(-1);
    }

    /*
    * Determine Y maxima
    */
    miny = vertexY[0];
    maxy = vertexY[0];
    for (i = 1; (i < numOfPoints); i++) {
        if (vertexY[i] < miny) {
            miny = vertexY[i];
        }
        else if (vertexY[i] > maxy) {
            maxy = vertexY[i];
        }
    }

    /*
    * Draw, scanning y
    */
    result = 0;
    for (y = miny; (y <= maxy); y++) {
        ints = 0;
        for (i = 0; (i < numOfPoints); i++) {
            if (!i) {
                ind1 = numOfPoints - 1;
                ind2 = 0;
            }
            else {
                ind1 = i - 1;
                ind2 = i;
            }
            y1 = vertexY[ind1];
            y2 = vertexY[ind2];
            if (y1 < y2) {
                x1 = vertexX[ind1];
                x2 = vertexX[ind2];
            }
            else if (y1 > y2) {
                y2 = vertexY[ind1];
                y1 = vertexY[ind2];
                x2 = vertexX[ind1];
                x1 = vertexX[ind2];
            }
            else {
                continue;
            }
            if (((y >= y1) && (y < y2)) || ((y == maxy) && (y > y1) && (y <= y2))) {
                gfxPrimitivesPolyInts[ints++] = ((65536 * (y - y1)) / (y2 - y1)) * (x2 - x1) + (65536 * x1);
            }
        }

        qsort(gfxPrimitivesPolyInts, ints, sizeof(int), _gfxPrimitivesCompareInt);

        /*
        * Set color
        */
        result = 0;
        result |= SDL_SetRenderDrawBlendMode(renderer, (alpha == 255) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
        result |= SDL_SetRenderDrawColor(renderer, redVal, greenVal, blueVal, alpha);

        for (i = 0; (i < ints); i += 2) {
            xa = gfxPrimitivesPolyInts[i] + 1;
            xa = (xa >> 16) + ((xa & 32768) >> 15);
            xb = gfxPrimitivesPolyInts[i + 1] - 1;
            xb = (xb >> 16) + ((xb & 32768) >> 15);
            result |= hline(renderer, xa, xb, y);
        }
    }

    return (result);
}

void fireCollision()
{
    Vector2D* vectorScreenCoords[3] = {};                                            //list of vectors relative to screen positions (so more like coordinates on a screen)
    //NEXT STEP (MAKE A RAYCAST)


    vectorScreenCoords[0] = new Vector2D(originPointX, originPointY);
    //FIRST VECTOR = always the origin point

    if (polygonZoneActivate == UPPER_POLYGON)
    {
        vectorScreenCoords[1] = new Vector2D(startingVector->getX() + originPointX, startingVector->getY() + originPointY);      
        //SECOND VECTOR = since it is UPPER, the starting vector's "end" point on the screen

        vectorScreenCoords[2] = new Vector2D(startingVector->getX() + originPointX - startConnectingVector->getX(), startingVector->getY() + originPointY - startConnectingVector->getY());
        //THIRD VECTOR = since it is on UPPER, the starting connector's points "end" point on the screen
    }
    else if (polygonZoneActivate == LOWER_POLYGON)
    {
        vectorScreenCoords[1] = new Vector2D(adjacentVector->getX() + originPointX, adjacentVector->getY() + originPointY);
        //SECOND VECTOR = since it is LOWER, the adjacent vector's "end" point on the screen

        vectorScreenCoords[2] = new Vector2D(adjacentVector->getX() + originPointX + adjConnectingVector->getX(), adjacentVector->getY() + originPointY + adjConnectingVector->getY());
        //THIRD VECTOR = since it is on LOWER, the adjacent connector's points "end" point on the screen
    }

    int nextIndex = 0;              //since we will need to see any vertex in this list

    for (int i = 0; i < 3; i++)
    {
        nextIndex = i + 1;

        if (nextIndex == 3)         //to prevent wrap around at the end
        {
            nextIndex = 0;      
        }

        //remember that [i] will equal the current vector and nextIndex will equal the next one

        //if 
            //(((vectorScreenCoords[i]->getY() ))
    }
}

////////////////////////////////////////////////////////////////////
/////////////              MAIN LOOP FUNCTION         ///////////////
////////////////////////////////////////////////////////////////////

void mainLoop()
{
    bool quit = false;

    SDL_Event ev;       //event handler

    while (!quit)
    {
        
        while (SDL_PollEvent(&ev))                  
        {
            if (ev.type == SDL_QUIT)                    
            {
                quit = true;
            }
            else if (ev.type == SDL_KEYDOWN)            //KEY CONTROLS 
            {
                switch (ev.key.keysym.sym)
                {
                    case SDLK_a:
                        player.x -= 5;
                        break;

                    case SDLK_d:
                        player.x += 5;
                        break;

                    case SDLK_w:
                        player.y -= 5;
                        break;

                    case SDLK_s:
                        player.y += 5;
                        break;

                    default:
                        break;
                }
            }
        }

        ////////////////////////////////////////////////////
        ///         RENDER SECTION             /////////////
        ////////////////////////////////////////////////////


        SDL_SetRenderDrawColor(glRenderer, 0x00, 0x00, 0x00, 0xFF); //"clear" screen
        SDL_RenderClear(glRenderer);

        SDL_SetRenderDrawColor(glRenderer, 0xFF, 0x00, 0x00, 0xFF);                             //set render color to red

         /// <summary>    /////
        /// LINE UPDATES ////
        /// </summary> ////

        //(start pointx, startpointy, end pointx, end pointy)
        SDL_RenderDrawLine(glRenderer, originPointX, originPointY, startingVector->getX() + originPointX, startingVector->getY() + originPointY);                           //set up base (start, adjacent, and origin)
        SDL_RenderDrawLine(glRenderer, originPointX, originPointY, adjacentVector->getX() + originPointX, adjacentVector->getY() + originPointY);
        SDL_RenderDrawLine(glRenderer, originPointX, originPointY, xOriginVector->getX() + originPointX, xOriginVector->getY() + originPointY);
        SDL_RenderDrawLine(glRenderer, startingVector->getX() + originPointX, startingVector->getY() + originPointY, startPerpVector->getX() + originPointX, startPerpVector->getY() + originPointY);

        SDL_RenderDrawLine(glRenderer, adjacentVector->getX()+originPointX, adjacentVector->getY()+originPointY, adjacentVector->getX() + originPointX + adjConnectingVector->getX(), adjacentVector->getY() + originPointY + adjConnectingVector->getY());
        SDL_RenderDrawLine(glRenderer, startingVector->getX() + originPointX, startingVector->getY() + originPointY, startingVector->getX() + originPointX - startConnectingVector->getX(), startingVector->getY() + originPointY - startConnectingVector->getY());

        Sint16 xStartVertexes[3] = { originPointX, startingVector->getX() + originPointX, startingVector->getX() + originPointX - startConnectingVector->getX() };  //get x-coords for first polygon generated
        Sint16 yStartVertexes[3] = { originPointY, startingVector->getY() + originPointY, startingVector->getY() + originPointY - startConnectingVector->getY() };  //get y-coords for first polygon generated
            
        Sint16 xAdjVertexes[3] = { originPointX, adjacentVector->getX() + originPointX, adjacentVector->getX() + originPointX + adjConnectingVector->getX() };   //get x-coords for second polygon generated
        Sint16 yAdjVertexes[3] = { originPointY, adjacentVector->getY() + originPointY, adjacentVector->getY() + originPointY + adjConnectingVector->getY() };   //get y-coords for second polygon generated
        
        /// <summary>    /////
        /// FRAME-RELATED UPDATES ////
        /// </summary> ////

        frameCount++;
        
        if (frameCount == 480 * 4)                                                             //480 frames represents something close to a "second" * how many seconds                       
        {
            frameCount = 0;                                                                    //reset it

            switch (polygonZoneActivate)
            {
                case LOWER_POLYGON:
                    polygonZoneActivate = UPPER_POLYGON;
                    break;

                case UPPER_POLYGON:
                    polygonZoneActivate = LOWER_POLYGON;
                    break;
            }
        }
        
        if (polygonZoneActivate == UPPER_POLYGON)
        {
            fillPolygonRGB(glRenderer, xStartVertexes, yStartVertexes, 3, 255, 0, 0, 255);
        }
        else if (polygonZoneActivate == LOWER_POLYGON)
        {
            fillPolygonRGB(glRenderer, xAdjVertexes, yAdjVertexes, 3, 255, 0, 0, 255);
        }

        /// <summary>    /////
       /// OBJECT UPDATES ////
       /// </summary> ////
        
        SDL_SetRenderDrawColor(glRenderer, 0xFF, 0x00, 0x00, 0xFF);                             //set render color to red

        SDL_RenderCopy(glRenderer, playerBaseTexture, NULL, &player);

        SDL_RenderPresent(glRenderer);                                                       //update screen



    }
}

////////////////////////////////////////////////////////////////////
/////////////            MATHMETICAL FUNCTIONS       ///////////////
////////////////////////////////////////////////////////////////////


float calcuateAngleFunction(Vector2D* vec1, Vector2D* vec2)
{
    float dot = vec1->find2DDotProduct(*vec2);                           //start to find angle between starting and xOrigin (more in notes)
    float totalMag = vec1->findMagnitude() * vec2->findMagnitude();


    float totalAngle = (acos(dot / totalMag)) * 180.0 / 3.14159265;               //follow the equation as stateed in notes convert to degrees

    return totalAngle;
}

Vector2D& calculateAdjacentVector() 
{                                                                               

    int newAngleDegrees = rand() % 90 + 10;                                           //generate a new angle between 10 and 90 degrees
    float newAngleRadians = newAngleDegrees * 3.1415 / 180;                           //convert to radians to work with the trig functions

    printf("angle + %d\n", newAngleDegrees);

    float newVectorX = startingVector->getX() * cos(newAngleRadians) - startingVector->getY() * sin(newAngleRadians);     //generate a new vector by making a copy of the starting vector and rotating it
    float newVectorY = startingVector->getX() * sin(newAngleRadians) + startingVector->getY() * cos(newAngleRadians);     //

    Vector2D* adjacent = new Vector2D(newVectorX, newVectorY);

    return *adjacent;
}

void calculateAdjConnectingVector()
{
    float startingToOriginAngle = calcuateAngleFunction(startingVector, xOriginVector);
    float startingToAdjAngle = calcuateAngleFunction(startingVector, adjacentVector);

    Vector2D* adjConnectParse;

    if (startingToOriginAngle < startingToAdjAngle)                                                  //if the adjacent vector is below the x axis
    {
        //calculate the adj connecting vector to the origin with subtraction
        Vector2D xOriginCopy = *xOriginVector;                                                      //so the actual origin vector isn't subtracted from
        adjConnectParse = &xOriginCopy.Subtract(*adjacentVector);
        
        adjConnectingVector = new Vector2D(adjConnectParse->getX(), adjConnectParse->getY());

        adjacentBelowXAxis = true;                                                                     //since it's just a copy, free memory

        
    }
    else if (startingToOriginAngle > startingToAdjAngle)                                            //if the adjacent vector is above the x axis
    {
        Vector2D adjacentCopy = *adjacentVector;                                                    //same method as before; so the actual adjacent vector isn't changed
        adjConnectParse = &adjacentCopy.Subtract(*xOriginVector);

        adjConnectingVector = new Vector2D(adjConnectParse->getX(), adjConnectParse->getY());
    }

}

void calculateStartConnectingVector()
{

    Vector2D* startConnectParse;
    Vector2D startingCopy = *startingVector;

    if (adjacentBelowXAxis)                                                 
    {
        //calculate start to origin
        startConnectParse = &startingCopy.Subtract(*xOriginVector);

        startConnectingVector = new Vector2D(startConnectParse->getX(), startConnectParse->getY());
        printf("start connect x + %f\n", startConnectingVector->getX());
        printf("start connect y + % f\n", startConnectingVector->getY());
    }
    else if (!adjacentBelowXAxis)                                           
    {
        //calculate start to adj
        startConnectParse = &startingCopy.Subtract(*adjacentVector);

        startConnectingVector = new Vector2D(startConnectParse->getX(), startConnectParse->getY());
        printf("start connect x + %f\n", startConnectingVector->getX());
        printf("start connect y + % f\n", startConnectingVector->getY());
    }
}

void setupPlayer()
{
    player.w = 70;
    player.h = 80;

    player.x = 500;
    player.y = 300;

    SDL_Surface* playerBaseSurface = IMG_Load("BaseStance.png");
    playerBaseTexture = SDL_CreateTextureFromSurface(glRenderer, playerBaseSurface);

    if (playerBaseTexture == NULL)
    {
        printf("texture failure\n");
        printf("%s\n", SDL_GetError());
    }

}
////////////////////////////////////////////////////////////////////
/////////////          INITIAL SETUP FUNCTION         ///////////////
////////////////////////////////////////////////////////////////////


int main(int argc, char** argv)
{

    SDL_Init(SDL_INIT_VIDEO);
    if (!init())
    {
        printf("Failed to initialize.\n");
    }
    else
    {
        printf("origin x + %f\n", originPointX);
        printf("origin y + %f\n", originPointY);

        adjacentVector = &calculateAdjacentVector();                                //generate adjacent vector
        calculateAdjConnectingVector();
        calculateStartConnectingVector();
        setupPlayer();
        

        int shit = int(startingVector->getX());
        printf("starting vector x + %d\n", shit);
        int shit2 = int(startingVector->getY());
        printf("starting vector y + %d\n", shit2);
        int shit3 = int(adjacentVector->getX());
        printf("adjacent vector x + %d\n", shit3);
        int shit4 = int(adjacentVector->getY());
        printf("adjacent vector y + %d\n", shit4);
        printf("xOrigin + %f\n", xOriginVector->getX());

        mainLoop();
    }

    //if (!init_resources())
    //{
      //  return EXIT_FAILURE;
    //}

    return EXIT_SUCCESS;
   
}
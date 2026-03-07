#include "raylib.h"
#include <cassert>
#include <string.h>
#include <string>
#include <sys/types.h>

// TOC (table of content):
// FONT_SHEET
// STRUCTS_GLOBALS_DECLARATIONS
// MAIN
// DEFINITIONS

static bool running = true;

#define WINDOW_SIZE_X 1448
#define WINDOW_SIZE_Y 1072
#define FULL_REFRESH_TIME 3.0

// ===================================================================================
//
// FONT_SHEET
//
// ===================================================================================

const unsigned char font8x8[128][8] = {

/* 0-31 */
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},

/* 32 SPACE */
{0,0,0,0,0,0,0,0},

/* 33 ! */
{0b00010000,0b00010000,0b00010000,0b00010000,0b00010000,0,0b00010000,0},

/* 34 " */
{0b00100100,0b00100100,0b00100100,0,0,0,0,0},

/* 35 # */
{0b00100100,0b00100100,0b01111110,0b00100100,0b01111110,0b00100100,0b00100100,0},

/* 36 $ */
{0b00010000,0b00111100,0b01010000,0b00111100,0b00010100,0b00111100,0b00010000,0},

/* 37 % */
{0b01100010,0b01100100,0b00001000,0b00010000,0b00100000,0b01000110,0b10000110,0},

/* 38 & */
{0b00110000,0b01001000,0b00110000,0b01001010,0b01000100,0b01000100,0b00111010,0},

/* 39 ' */
{0b00010000,0b00010000,0b00100000,0,0,0,0,0},

/* 40 ( */
{0b00001000,0b00010000,0b00100000,0b00100000,0b00100000,0b00010000,0b00001000,0},

/* 41 ) */
{0b00100000,0b00010000,0b00001000,0b00001000,0b00001000,0b00010000,0b00100000,0},

/* 42 * */
{0,0b00100100,0b00011000,0b01111110,0b00011000,0b00100100,0,0},

/* 43 + */
{0,0b00010000,0b00010000,0b01111100,0b00010000,0b00010000,0,0},

/* 44 , */
{0,0,0,0b00110000,0b00110000,0b00010000,0b00100000,0},

/* 45 - */
{0,0,0,0b01111100,0,0,0,0},

/* 46 . */
{0,0,0,0,0,0b00110000,0b00110000,0},

/* 47 / */
{0b00000010,0b00000100,0b00001000,0b00010000,0b00100000,0b01000000,0,0},

/* 48 0 */
{0b00111100,0b01000010,0b01000110,0b01001010,0b01010010,0b01100010,0b01000010,0b00111100},

/* 49 1 */
{0b00011000,0b00101000,0b01001000,0b00001000,0b00001000,0b00001000,0b00001000,0b01111110},

/* 50 2 */
{0b00111100,0b01000010,0b00000010,0b00001100,0b00110000,0b01000000,0b01000000,0b01111110},

/* 51 3 */
{0b00111100,0b01000010,0b00000010,0b00011100,0b00000010,0b00000010,0b01000010,0b00111100},

/* 52 4 */
{0b00000100,0b00001100,0b00010100,0b00100100,0b01000100,0b01111110,0b00000100,0b00000100},

/* 53 5 */
{0b01111110,0b01000000,0b01000000,0b01111100,0b00000010,0b00000010,0b01000010,0b00111100},

/* 54 6 */
{0b00111100,0b01000010,0b01000000,0b01111100,0b01000010,0b01000010,0b01000010,0b00111100},

/* 55 7 */
{0b01111110,0b00000010,0b00000100,0b00001000,0b00010000,0b00100000,0b00100000,0b00100000},

/* 56 8 */
{0b00111100,0b01000010,0b01000010,0b00111100,0b01000010,0b01000010,0b01000010,0b00111100},

/* 57 9 */
{0b00111100,0b01000010,0b01000010,0b01000010,0b00111110,0b00000010,0b01000010,0b00111100},

/* 58 : */
{0,0b00110000,0b00110000,0,0b00110000,0b00110000,0,0},

/* 59 ; */
{0,0b00110000,0b00110000,0,0b00110000,0b00010000,0b00100000,0},

/* 60 < */
{0b00001100,0b00010000,0b00100000,0b01000000,0b00100000,0b00010000,0b00001100,0},

/* 61 = */
{0,0b01111100,0,0b01111100,0,0,0,0},

/* 62 > */
{0b01100000,0b00010000,0b00001000,0b00000100,0b00001000,0b00010000,0b01100000,0},

/* 63 ? */
{0b00111100,0b01000010,0b00000010,0b00001100,0b00010000,0,0b00010000,0},

/* 64 @ */
{0b00111100,0b01000010,0b01001110,0b01010010,0b01011110,0b01000000,0b00111100,0},

/* 65–90 A–Z */
{0b00111100,0b01000010,0b01000010,0b01111110,0b01000010,0b01000010,0b01000010,0},
{0b01111100,0b01000010,0b01000010,0b01111100,0b01000010,0b01000010,0b01000010,0b01111100},
{0b00111100,0b01000010,0b01000000,0b01000000,0b01000000,0b01000000,0b01000010,0b00111100},
{0b01111000,0b01000100,0b01000010,0b01000010,0b01000010,0b01000010,0b01000100,0b01111000},
{0b01111110,0b01000000,0b01000000,0b01111100,0b01000000,0b01000000,0b01000000,0b01111110},
{0b01111110,0b01000000,0b01000000,0b01111100,0b01000000,0b01000000,0b01000000,0b01000000},
{0b00111100,0b01000010,0b01000000,0b01000000,0b01001110,0b01000010,0b01000010,0b00111100},
{0b01000010,0b01000010,0b01000010,0b01111110,0b01000010,0b01000010,0b01000010,0},
{0b00111100,0b00010000,0b00010000,0b00010000,0b00010000,0b00010000,0b00010000,0b00111100},
{0b00011110,0b00000100,0b00000100,0b00000100,0b00000100,0b01000100,0b01000100,0b00111000},
{0b01000010,0b01000100,0b01001000,0b01110000,0b01001000,0b01000100,0b01000010,0},
{0b01000000,0b01000000,0b01000000,0b01000000,0b01000000,0b01000000,0b01000000,0b01111110},
{0b01000010,0b01100110,0b01011010,0b01000010,0b01000010,0b01000010,0b01000010,0},
{0b01000010,0b01100010,0b01010010,0b01001010,0b01000110,0b01000010,0b01000010,0},
{0b00111100,0b01000010,0b01000010,0b01000010,0b01000010,0b01000010,0b01000010,0b00111100},
{0b01111100,0b01000010,0b01000010,0b01111100,0b01000000,0b01000000,0b01000000,0},
{0b00111100,0b01000010,0b01000010,0b01000010,0b01001010,0b01000100,0b00111010,0},
{0b01111100,0b01000010,0b01000010,0b01111100,0b01001000,0b01000100,0b01000010,0},
{0b00111100,0b01000010,0b01000000,0b00111100,0b00000010,0b00000010,0b01000010,0b00111100},
{0b01111110,0b00010000,0b00010000,0b00010000,0b00010000,0b00010000,0b00010000,0},
{0b01000010,0b01000010,0b01000010,0b01000010,0b01000010,0b01000010,0b01000010,0b00111100},
{0b01000010,0b01000010,0b01000010,0b01000010,0b01000010,0b00100100,0b00011000,0},
{0b01000010,0b01000010,0b01000010,0b01011010,0b01011010,0b01100110,0b01000010,0},
{0b01000010,0b00100100,0b00011000,0b00011000,0b00011000,0b00100100,0b01000010,0},
{0b01000010,0b00100100,0b00011000,0b00010000,0b00010000,0b00010000,0b00010000,0},
{0b01111110,0b00000010,0b00000100,0b00001000,0b00010000,0b00100000,0b01000000,0b01111110},

/* 91–96 */
{0b00111100,0b00100000,0b00100000,0b00100000,0b00100000,0b00100000,0b00111100,0},
{0b01000000,0b00100000,0b00010000,0b00001000,0b00000100,0b00000010,0,0},
{0b00111100,0b00000100,0b00000100,0b00000100,0b00000100,0b00000100,0b00111100,0},
{0b00010000,0b00101000,0b01000100,0,0,0,0,0},
{0,0,0,0,0,0,0,0b11111111},
{0b00100000,0b00010000,0b00001000,0,0,0,0,0},

/* 97–122 a–z */
{0,0b00111100,0b00000010,0b00111110,0b01000010,0b01000010,0b00111110,0},
{0b01000000,0b01000000,0b01011100,0b01100010,0b01000010,0b01000010,0b01100010,0b01011100},
{0,0b00111100,0b01000010,0b01000000,0b01000000,0b01000010,0b00111100,0},
{0b00000010,0b00000010,0b00111010,0b01000110,0b01000010,0b01000010,0b01000110,0b00111010},
{0,0b00111100,0b01000010,0b01111110,0b01000000,0b01000010,0b00111100,0},
{0b00011100,0b00100010,0b00100000,0b01111100,0b00100000,0b00100000,0b00100000,0},
{0,0b00111010,0b01000110,0b01000010,0b01000010,0b00111110,0b00000010,0b00111100},
{0b01000000,0b01000000,0b01011100,0b01100010,0b01000010,0b01000010,0b01000010,0},
{0b00010000,0,0b00110000,0b00010000,0b00010000,0b00010000,0b00111000,0},
{0b00000100,0,0b00001100,0b00000100,0b00000100,0b01000100,0b01000100,0b00111000},
{0b01000000,0b01000100,0b01001000,0b01110000,0b01001000,0b01000100,0b01000010,0},
{0b00110000,0b00010000,0b00010000,0b00010000,0b00010000,0b00010000,0b00111000,0},
{0,0b01101100,0b01010010,0b01010010,0b01010010,0b01010010,0b01010010,0},
{0,0b01011100,0b01100010,0b01000010,0b01000010,0b01000010,0b01000010,0},
{0,0b00111100,0b01000010,0b01000010,0b01000010,0b01000010,0b00111100,0},
{0,0b01011100,0b01100010,0b01000010,0b01111100,0b01000000,0b01000000,0},
{0,0b00111010,0b01000110,0b01000010,0b00111110,0b00000010,0b00000010,0},
{0,0b01011100,0b01100010,0b01000000,0b01000000,0b01000000,0b01000000,0},
{0,0b00111110,0b01000000,0b00111100,0b00000010,0b01000010,0b00111100,0},
{0b00100000,0b00100000,0b01111100,0b00100000,0b00100000,0b00100010,0b00011100,0},
{0,0b01000010,0b01000010,0b01000010,0b01000010,0b01000110,0b00111010,0},
{0,0b01000010,0b01000010,0b01000010,0b00100100,0b00100100,0b00011000,0},
{0,0b01000010,0b01000010,0b01011010,0b01011010,0b01100110,0b01000010,0},
{0,0b01000010,0b00100100,0b00011000,0b00011000,0b00100100,0b01000010,0},
{0,0b01000010,0b01000010,0b00111110,0b00000010,0b01000010,0b00111100,0},
{0,0b01111110,0b00000100,0b00011000,0b00100000,0b01111110,0},

/* 123–126 */
{0b00001100,0b00010000,0b00010000,0b01100000,0b00010000,0b00010000,0b00001100,0},
{0b00010000,0b00010000,0b00010000,0,0b00010000,0b00010000,0b00010000,0},
{0b01100000,0b00010000,0b00010000,0b00001100,0b00010000,0b00010000,0b01100000,0},
{0b00110010,0b01001100,0,0,0,0,0,0},

/* 127 unused */
{0,0,0,0,0,0,0,0}
};

// ===================================================================================
//
// STRUCTS_GLOBALS_DECLARATIONS
//
// ===================================================================================

typedef struct 
{
    const uint sizeX = WINDOW_SIZE_X;
    const uint sizeY = WINDOW_SIZE_Y;
    bool pixels[WINDOW_SIZE_Y][WINDOW_SIZE_X] = {};
    bool stagedPixels[WINDOW_SIZE_Y][WINDOW_SIZE_X] = {};


    const double fullRefreshTime = FULL_REFRESH_TIME;
    double flushingPixelsStartTime = 0.0;
    uint flushingPixelsCount = 0;
    bool isFlushingPixels = false;

    struct
    {
        RenderTexture rendTexFront;
    }
    raylib;
}
Display;

Display display;

// CORE functions
void SetPixelSim(uint x, uint y, bool on);
void TogglePixelSim(uint x, uint y);
void FlushPixelChangesSim();
void UpdateDisplay();

// ADDON functions
void DrawLineSim(uint startX, uint startY, uint endX, uint endY, bool pixelsOn);
void DrawRectangleSim(uint startX, uint startY, uint width, uint height, bool pixelsOn);
void DrawRectangleToggleSim(uint startX, uint startY, uint width, uint height);
//void DrawCharacterSim(char c, uint x, uint y, int fontSize, bool pixelsOn);
//void DrawCharacterToggleSim(char c, uint x, uint y, int fontSize, bool pixelsOn);
void DrawCharacterIntScaledSim(char c, uint x, uint y, uint scale, bool pixelsOn);       // scale=1 means 8x8 pixel character
void DrawCharacterToggleIntScaledSim(char c, uint x, uint y, uint scale, bool pixelsOn); // scale=1 means 8x8 pixel character

// ===================================================================================
//
// MAIN
//
// ===================================================================================

int main()
{
    InitWindow(WINDOW_SIZE_X, WINDOW_SIZE_Y, "vid decoder testing");
    SetTargetFPS(60);

    display.raylib.rendTexFront = LoadRenderTexture(display.sizeX, display.sizeY);

    BeginDrawing();
    ClearBackground(WHITE);
    EndDrawing();

    while (!WindowShouldClose() && running)
    {

        for (uint y = 0; y < display.sizeY; y++)
        {
            for (uint x = 0; x < display.sizeX; x++)
            {
                TogglePixelSim(x, y);
            }
        }
        FlushPixelChangesSim();

        UpdateDisplay();
        BeginDrawing();
        ClearBackground(WHITE);
        DrawTexturePro(
                display.raylib.rendTexFront.texture, 
                (Rectangle) {
                    0.0f, 
                    0.0f, 
                    (float)display.raylib.rendTexFront.texture.width,
                    -(float)display.raylib.rendTexFront.texture.height
                }, 
                (Rectangle) {
                    0.0f, 
                    0.0f, 
                    (float)display.raylib.rendTexFront.texture.width,
                    (float)display.raylib.rendTexFront.texture.height
                }, 
                (Vector2) {0.0f, 0.0f}, 
                0.0f, 
                WHITE
            );
        //std::string time = std::to_string(GetTime());
        //DrawText(time.c_str(), 0, 0, 16, BLUE);
        EndDrawing();
    }

    UnloadRenderTexture(display.raylib.rendTexFront);

    CloseWindow();
}

// ===================================================================================
//
// DEFINITIONS
//
// ===================================================================================

void SetPixelSim(uint x, uint y, bool on)
{
    assert(x < display.sizeX && y < display.sizeY);
    if (display.isFlushingPixels)
        return;

    display.flushingPixelsCount++;
    display.stagedPixels[y][x] = on;
}

void TogglePixelSim(uint x, uint y)
{
    assert(x < display.sizeX && y < display.sizeY);
    if (display.isFlushingPixels)
        return;
    
    display.flushingPixelsCount++;
    display.stagedPixels[y][x] = display.pixels[y][x] ^ 1;
}

void FlushPixelChangesSim()
{
    if (display.isFlushingPixels)
        return;

    display.flushingPixelsStartTime = GetTime();
    display.isFlushingPixels = true;
}

void UpdateDisplay()
{
    if (display.isFlushingPixels)
    {
        double timeSinceFlushingStarted = GetTime() - display.flushingPixelsStartTime;

        uint totalPixelCount = display.sizeX * display.sizeY;
        double timeToFlush = ((double)display.flushingPixelsCount / totalPixelCount) * display.fullRefreshTime;

        if (timeSinceFlushingStarted > timeToFlush)
        {
            BeginTextureMode(display.raylib.rendTexFront);
            for (uint y = 0; y < display.sizeY; y++)
            {
                for (uint x = 0; x < display.sizeX; x++)
                {
                    if (display.pixels[y][x] != display.stagedPixels[y][x])
                    {
                        if (display.stagedPixels[y][x])
                            DrawPixel(x, y, BLACK);
                        else
                            DrawPixel(x, y, WHITE);
                    }
                }
            }
            EndTextureMode();
            memcpy(
                display.pixels, 
                display.stagedPixels, 
                display.sizeX * display.sizeY * sizeof(bool)
            );
            display.isFlushingPixels = false;
            display.flushingPixelsCount = 0;
        }
    }
}

// Bresenham's line algorithm (copy pasted from Wikipedia)
void DrawLineSim(uint startX, uint startY, uint endX, uint endY, bool pixelsOn)
{
    assert(startX < display.sizeX && startY < display.sizeY);
    assert(endX < display.sizeX && endY < display.sizeY);
    
    int dx = endX - startX;
    int dy = endY - startY;
    int D = 2*dy - dx;
    int y = startY;
    
    assert(dx >= 0);
    assert(dy >= 0);

    for (uint x = startX; x < endX; x++)
    {
        SetPixelSim(x, y, pixelsOn);
        if (D > 0)
        {
            y++;
            D = D + (2*(dy-dx));
        }
        else
            D = D + (2*dy);
    }
}

/*
void DrawCharacterSim(char c, uint x, uint y, int fontSize, bool pixelsOn)
{
    if (fontSize < 8)
        fontSize = 8;
    assert(c < 128);
    assert(x < display.sizeX);
    assert(y < display.sizeY);

    int yLinesRepeated = 0;

    for (uint yi = 0; yi < 8; yi++)
    {
        for (uint xi = 0; xi < 8; xi++)
        {
            bool shouldColorPixel = ((font8x8[c][yi]) >> (7-xi)) & 1;
            if (shouldColorPixel)
            {
                for                 
                SetPixelSim(uint x, uint y, pixelsOn);
            }
        }
    }
}

void DrawCharacterToggleSim(char c, uint x, uint y, int fontSize, bool pixelsOn)
{

}
*/

void DrawCharacterIntScaledSim(char c, uint x, uint y, uint scale, bool pixelsOn)
{
    assert((uint)c < 128);
    assert((x + 8*scale) < display.sizeX);
    assert((y + 8*scale) < display.sizeY);

    for (uint yi = 0; yi < 8; yi++)
    {
        for (uint xi = 0; xi < 8; xi++)
        {
            bool shouldColorPixel = ((font8x8[(uint)c][yi]) >> (7-xi)) & 1;
            if (shouldColorPixel)
                DrawRectangleSim(x + xi*scale, y + yi*scale, scale, scale, pixelsOn);
        }
    }
}

void DrawCharacterToggleIntScaledSim(char c, uint x, uint y, uint scale, bool pixelsOn)
{
    assert((uint)c < 128);
    assert((x + 8*scale) < display.sizeX);
    assert((y + 8*scale) < display.sizeY);

    for (uint yi = 0; yi < 8; yi++)
    {
        for (uint xi = 0; xi < 8; xi++)
        {
            bool shouldColorPixel = ((font8x8[(uint)c][yi]) >> (7-xi)) & 1;
            if (shouldColorPixel)
                DrawRectangleToggleSim(x + xi*scale, y + yi*scale, scale, scale);
        }
    }
}

void DrawRectangleSim(uint startX, uint startY, uint width, uint height, bool pixelsOn)
{
    assert((startX + width) < display.sizeX && (startY + height) < display.sizeY);

    for (uint y = startY; y < startY + height; y++)
        for (uint x = startX; x < startX + width; x++)
            SetPixelSim(x, y, pixelsOn);
}

void DrawRectangleToggleSim(uint startX, uint startY, uint width, uint height)
{
    assert((startX + width) < display.sizeX && (startY + height) < display.sizeY);

    for (uint y = startY; y < startY + height; y++)
        for (uint x = startX; x < startX + width; x++)
            TogglePixelSim(x, y);
}


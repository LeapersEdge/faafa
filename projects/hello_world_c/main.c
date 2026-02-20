#include "raylib.h"
#include <stdio.h>

int main()
{
    printf("hello world\n");

    InitWindow(400, 200, "hello world");
    SetTargetFPS(30);

    while (!WindowShouldClose()) 
    {
        BeginDrawing();
        ClearBackground(WHITE);
        DrawText("hello world!", 10, 10, 25, BLACK);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}


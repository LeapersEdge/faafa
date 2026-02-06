#include "raylib.h"
#include "rlImGui.h"
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui.h"

int main()
{
    InitWindow(800, 400, "hello imgui");
    SetTargetFPS(60);

    rlImGuiSetup(true);

    bool showDemoWindow = true;

    while (!WindowShouldClose()) 
    {
        BeginDrawing();
        ClearBackground(WHITE);

        rlImGuiBegin();
        igShowDemoWindow(&showDemoWindow);
        rlImGuiEnd();

        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
}


#include <assert.h>
#include <cstdio>
#include <sys/types.h>
#include <vector>
#include "imgui.h"
#include "raylib.h"
#include "rlImGui.h"

// IMPORTANT:
// - remember that raylib Y axis is positive when going down
// - polyline/points should be stored in "meters" metric and without inverted Y axis 
//   (pixel metric and inverted Y axis should only be handled on raylib render side)
//   (it doesnt matter how meshes are generated and stored since they are not exported
//   outside anyway but it would be nice if they followed the same convention for 
//   consistency)

// TODOs:
// - generate meshes on refresh value (currently not generated anywhere or anytime)

// table of contents:
// DEFS_n_STRUCTS
// GLOBALS
// FUNC_DECLS
// MAIN
// FUNC_DEFS

// ====================================================================
//
// DEFS_n_STRUCTS
//
// ====================================================================

#define global_persist static
#define local_persist  static

typedef struct
{
    Vector2 v1;
    Vector2 v2;
    Vector2 v3;
}
Triangle2D;

typedef Vector2                 Point2D;
typedef std::vector<Point2D>    Polyline2D;
typedef std::vector<Triangle2D> Mesh2D;

typedef enum
{
    PolylineEditMode_None = 0,
    PolylineEditMode_Brush,
    PolylineEditMode_Edit,
    PolylineEditMode_EndStub,
}
PolylineEditMode;

// ====================================================================
//
// GLOBALS
//
// ====================================================================

global_persist const uint PIXELS_PER_METER      = 64;
global_persist const Color GRIDLINE_COLOR       = (Color){ 2, 198, 224, 255 };
global_persist const Color SUBGIRDLINE_COLOR    = (Color){ 37, 103, 114, 255 };
global_persist const Color BACKGROUND_COLOR     = (Color){ 37, 53, 114, 255 };
global_persist const Color POLYLINE_COLOR       = (Color){ 255, 0, 0, 255};
global_persist const Color MESH_COLOR           = (Color){ 0, 0, 255, 255};
global_persist const Color MESH_TRIANGLE_COLOR  = (Color){ 0, 255, 0, 255};
global_persist const uint GRIDLINE_THICKNESS    = 3;
global_persist const uint SUBGRIDLINE_THICKNESS = 1;
global_persist const uint POLYLINE_THICKNESS    = 3;
global_persist const uint MESH_TRIANGLE_THICKNESS = 3;
global_persist Font agaveFont;

global_persist Camera2D cam2D;

global_persist std::vector<Polyline2D> obstaclePolylines = {};
global_persist std::vector<Mesh2D>     obstacleMeshes = {};

bool drawGuiWindow = false;
bool hoveringGui = false;

bool drawPolylines = true;
bool drawMeshes = true;
bool drawMeshTriangles = true;
int editPolylines = false;

// ====================================================================
//
// FUNC_DECLS
//
// ====================================================================

void DrawCamRelativeGrid(const Camera2D cam2D, const uint pixelsPerUnit, const uint subCellLines, const bool drawXYCoordinates);
void DrawTextFont(Font font, const char *text, int posX, int posY, int fontSize, Color color);
int MeasureTextFont(Font font, const char *text, int fontSize);

void DrawGUI();
void DrawPolylines();
void DrawMeshes();
void DrawMeshTriangles();

// ====================================================================
//
// MAIN
//
// ====================================================================

int main()
{
    printf("hello obstacle maker");
    InitWindow(800, 800, "obstacle maker");
    SetTargetFPS(60);
    rlImGuiSetup(true);

    agaveFont = LoadFont(ASSETS "Agave/AgaveNerdFontMono-Regular.ttf");

    cam2D.target = (Vector2){0.0f, 0.0f};
    cam2D.offset = (Vector2){GetScreenWidth()/2.0f, GetScreenHeight()/2.0f};
    cam2D.rotation = 0.0f;
    cam2D.zoom = 1.0f;

    while (!WindowShouldClose()) 
    {
        // ------------------------------------------
        // Update logic 

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !hoveringGui)
        {
            cam2D.target.x -= GetMouseDelta().x;
            cam2D.target.y -= GetMouseDelta().y;
        }


        // ------------------------------------------
        // Draw calls

        BeginDrawing();
        ClearBackground(BACKGROUND_COLOR);
        BeginMode2D(cam2D);
        DrawCamRelativeGrid(cam2D, PIXELS_PER_METER, 3, true);
        if (drawMeshes)         DrawMeshes();
        if (drawMeshTriangles)  DrawMeshTriangles();
        if (drawPolylines)      DrawPolylines();
        EndMode2D();
        DrawGUI();
        EndDrawing();
    }

    UnloadFont(agaveFont);
    rlImGuiShutdown();
    CloseWindow();
    return 0;
}

// ====================================================================
//
// FUNC_DEFS
//
// ====================================================================

void DrawCamRelativeGrid(const Camera2D cam2D, const uint pixelsPerUnit, const uint subCellLines, const bool drawXYCoordinates)
{
    int totalColumns = GetScreenWidth() / pixelsPerUnit + 1;
    int totalRows    = GetScreenHeight() / pixelsPerUnit + 1;

    int startScreenX = cam2D.target.x - cam2D.offset.x;
    int startScreenY = cam2D.target.y - cam2D.offset.y;

    int startRow    = cam2D.target.x - (int)cam2D.target.x % pixelsPerUnit - cam2D.offset.x + (int)cam2D.offset.x % pixelsPerUnit;
    int startColumn = cam2D.target.y - (int)cam2D.target.y % pixelsPerUnit - cam2D.offset.y + (int)cam2D.offset.y % pixelsPerUnit;

    // -----------------------------------
    // DRAW ORDER:
    // 1) draw sublines
    // 2) draw main lines
    // 3) draw coordinate texts

    // -----------------------------------
    // draw sublines

    // starts from -1 so that last subline of offscreen row/column gets drawn, if from 0, than it not get drawn
    // (do a bit more overdraw to get rid of error)
    for (int rows = -1; rows < totalRows; rows++)
    {
        int startX = startScreenX;
        int startY = startColumn + rows * pixelsPerUnit;
       
        for (uint i = 1; i <= subCellLines; i++)
        {
            int subStartY = startY + (int)((float)pixelsPerUnit/(float)(subCellLines+1) * (float)i);
            DrawRectangle(startX, subStartY-(SUBGRIDLINE_THICKNESS)/2, GetScreenWidth(), SUBGRIDLINE_THICKNESS, SUBGIRDLINE_COLOR);
        }
    }
    // starts from -1 so that last subline of offscreen row/column gets drawn, if from 0, than it not get drawn
    // (do a bit more overdraw to get rid of error)
    for (int cols = -1; cols <= totalColumns; cols++)
    {
        int startX = startRow + cols * pixelsPerUnit;
        int startY = startScreenY;

        for (uint i = 1; i <= subCellLines; i++)
        {
            int subStartX = startX + (int)((float)pixelsPerUnit/(float)(subCellLines+1) * (float)i);
            DrawRectangle(subStartX-(SUBGRIDLINE_THICKNESS+1)/2, startY, SUBGRIDLINE_THICKNESS, GetScreenHeight(), SUBGIRDLINE_COLOR);
        }
    }

    // -----------------------------------
    // draw main lines

    for (int rows = 0; rows < totalRows+1; rows++)
    {
        int startX = startScreenX;
        int startY = startColumn + rows * pixelsPerUnit;  
        DrawRectangle(startX, startY-(GRIDLINE_THICKNESS)/2, GetScreenWidth(), GRIDLINE_THICKNESS, GRIDLINE_COLOR);
    }
    for (int cols = 0; cols < totalColumns+1; cols++)
    {
        int startX = startRow + cols * pixelsPerUnit;
        int startY = startScreenY;
        DrawRectangle(startX-(GRIDLINE_THICKNESS+1)/2, startY, GRIDLINE_THICKNESS, GetScreenHeight(), GRIDLINE_COLOR);
    }
    
    // -----------------------------------
    // draw coordinate texts

    if (drawXYCoordinates)
    {
        const int fontSize = 14;

        BeginScissorMode(0, 0, 100, GetScreenWidth()-fontSize-20);
        for (int rows = 0; rows < totalRows+1; rows++)
        {
            int startX = startScreenX;
            int startY = startColumn + rows * pixelsPerUnit;
            
            char text[32];
            snprintf(text, 32, "(Y=%d)", -startY/(int)pixelsPerUnit); 
            
            int textWidth = MeasureTextFont(agaveFont, text, fontSize);

            Rectangle textFrame = {
                .x = startX + 4.0f,
                .y = startY - fontSize + 4*2.0f/2.0f,
                .width  = textWidth + 4*2.0f,
                .height = fontSize  + 4*2.0f
            };
            DrawRectangleRec(textFrame, BACKGROUND_COLOR);
            DrawRectangleLinesEx(textFrame, 1.0f, GRIDLINE_COLOR);
            DrawTextFont(agaveFont, text, textFrame.x + (textFrame.width-textWidth)/2, textFrame.y + (textFrame.height-fontSize)/2, fontSize, GRIDLINE_COLOR);
        }
        EndScissorMode();
        for (int cols = 0; cols < totalColumns+1; cols++)
        {
            int startX = startRow + cols * pixelsPerUnit;
            int startY = startScreenY;
            
            char text[32];
            snprintf(text, 32, "(X=%d)", startX/(int)pixelsPerUnit); 
            
            int textWidth = MeasureTextFont(agaveFont, text, fontSize);

            Rectangle textFrame = {
                .x = startX - textWidth/2.0f - 4*2.0f/2.0f,
                .y = startY - 4.0f + GetScreenWidth() - fontSize - 4*2.0f,
                .width  = textWidth + 4*2.0f,
                .height = fontSize  + 4*2.0f
            };
            DrawRectangleRec(textFrame, BACKGROUND_COLOR);
            DrawRectangleLinesEx(textFrame, 1.0f, GRIDLINE_COLOR);
            DrawTextFont(agaveFont, text, textFrame.x + (textFrame.width-textWidth)/2, textFrame.y + (textFrame.height-fontSize)/2, fontSize, GRIDLINE_COLOR);
        }
    }
}

void DrawTextFont(Font font, const char *text, int posX, int posY, int fontSize, Color color)
{
    // Check if default font has been loaded
    if (font.texture.id != 0)
    {
        Vector2 position = { (float)posX, (float)posY };

        int defaultFontSize = 10;   // Default Font chars height in pixel
        if (fontSize < defaultFontSize) fontSize = defaultFontSize;
        int spacing = fontSize/defaultFontSize;

        DrawTextEx(font, text, position, (float)fontSize, (float)spacing, color);
    }
}

int MeasureTextFont(Font font, const char *text, int fontSize)
{
    Vector2 textSize = { 0.0f, 0.0f };

    // Check if default font has been loaded
    if (font.texture.id != 0)
    {
        int defaultFontSize = 10;   // Default Font chars height in pixel
        if (fontSize < defaultFontSize) fontSize = defaultFontSize;
        int spacing = fontSize/defaultFontSize;

        textSize = MeasureTextEx(font, text, (float)fontSize, (float)spacing);
    }

    return (int)textSize.x;
}

void DrawGUI()
{
    // update which screens gets drawn
    {
        if (IsKeyPressed(KEY_ENTER)) drawGuiWindow = !drawGuiWindow;
    }

    // draw tooltip (raylib side)
    {
        const char* tooltipStr = "Enter - options menu";

        const int fontSize = 16;
        const int padding = 6;
        const int offsetX = 10;
        const int offsetY = 10;
        const float borderThickness = 1.0f;

        int tooltipWidth = MeasureTextFont(agaveFont, tooltipStr, fontSize);
       
        // background and border drawing
        Rectangle backgroundRect = {
            .x = (float)(GetScreenWidth() - tooltipWidth - offsetX - padding * 2 - borderThickness * 2),
            .y = (float)(offsetY),
            .width  = (float)(padding * 2 + tooltipWidth + borderThickness * 2),
            .height = (float)(padding * 2 + fontSize + borderThickness * 2)
        };
        DrawRectangleRec(backgroundRect, BACKGROUND_COLOR);
        DrawRectangleLinesEx(backgroundRect, borderThickness, WHITE);

        // text drawing
        int textPosX = GetScreenWidth() - padding - tooltipWidth - offsetX - borderThickness;
        int textPosY = offsetY + padding + borderThickness;
        DrawTextFont(agaveFont, tooltipStr, textPosX, textPosY, fontSize, GRIDLINE_COLOR);
    }

    // draw gui windows (dear imgui side)
    rlImGuiBegin();
    if (drawGuiWindow)
    {
        ImGui::Begin("Options Menu");
        {
            if (ImGui::Button("Create New Polyline"))
            {
                obstaclePolylines.push_back({});
                obstacleMeshes.push_back({});
            }
            ImGui::Button("Enable/Disable snapping for XYZ intervals"); // TODO: button function
            if (ImGui::CollapsingHeader("Render options"))
            {
                ImGui::SeparatorText("Draw properties");
                ImGui::SliderInt("Pixels Per Meter", (int*)&PIXELS_PER_METER, 0, 256);
                ImGui::SliderInt("Gridline thickness", (int*)&GRIDLINE_THICKNESS, 0, 32);
                ImGui::SliderInt("Subgridline thickness", (int*)&SUBGRIDLINE_THICKNESS, 0, 32);
                ImGui::SliderInt("Polyline thickness", (int*)&POLYLINE_THICKNESS, 0, 32);
                ImGui::SliderInt("Mesh triangle thickness", (int*)&MESH_TRIANGLE_THICKNESS, 0, 32);
                ImGui::Checkbox("Draw polylines", &drawPolylines);
                ImGui::Checkbox("Draw meshes", &drawMeshes);
                ImGui::Checkbox("Draw mesh triangles", &drawMeshTriangles);
                ImGui::SeparatorText("Color options");
                // TODO: color options
            }
        }        
        ImGui::End();

        ImGui::Begin("Polyline Selector/Editor");
        {
            local_persist int currentPolylineIndex = 0;
            if (ImGui::Button("<"))
            {
                currentPolylineIndex--;
                if (currentPolylineIndex < 0)
                    currentPolylineIndex = 0;
            }
            ImGui::SameLine();
            if (obstaclePolylines.size() == 0)
                ImGui::Text("0 of 0");
            else
                ImGui::Text("%d of %lu", currentPolylineIndex+1, obstaclePolylines.size());
            ImGui::SameLine();
            if (ImGui::Button(">"))
            {
                if (obstaclePolylines.size())
                    currentPolylineIndex = (currentPolylineIndex+1) % obstaclePolylines.size();
            }
            
            if (currentPolylineIndex < obstaclePolylines.size())
            {
                Polyline2D& polyline = obstaclePolylines[currentPolylineIndex];

                ImGui::SeparatorText("Polyline Commands");
                if (ImGui::Button("Add Point"))
                    polyline.push_back({0.0f, 0.0f});
                if (ImGui::Button("Jump to this polyline"))
                {
                    if (polyline.size())
                    {
                        // TODO: jump camera to center of polyline logic
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Delete this polyline"))
                {
                    obstaclePolylines.erase(obstaclePolylines.begin() + currentPolylineIndex);
                    obstacleMeshes.erase(obstacleMeshes.begin() + currentPolylineIndex);
                    currentPolylineIndex--;
                    if (currentPolylineIndex < 0)
                        currentPolylineIndex = 0;
                }

                ImGui::SeparatorText("Polyline Points");
                for (uint i = 0; i < polyline.size(); i++)
                {
                    ImGui::PushID(i);
                    ImGui::InputFloat2("", &polyline[i].x);
                    ImGui::SameLine();
                    if (ImGui::Button("^") && i != 0)
                    {
                        Point2D temp = polyline[i];
                        polyline[i] = polyline[i-1];
                        polyline[i-1] = temp;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("v") && i < (polyline.size()-1))
                    {
                        Point2D temp = polyline[i];
                        polyline[i] = polyline[i+1];
                        polyline[i+1] = temp;
                    }

                    ImGui::PopID();
                }
            }
        }
        ImGui::End();
    }
    hoveringGui = ImGui::GetIO().WantCaptureMouse;
    rlImGuiEnd();
}

void DrawPolylines()
{
    for (const auto& polyline : obstaclePolylines)
    {
        if (polyline.size())
        {
            for (int i = 0; i < polyline.size()-1; i++)
            {
                Vector2 point1 = polyline[i];
                Vector2 point2 = polyline[i+1];

                point1.x *= PIXELS_PER_METER;
                point2.x *= PIXELS_PER_METER;
                // Y axis multiplied by -1 becuase opengl (and by turn raylib) Y axis is down and not up
                point1.y *= -1 * (float)PIXELS_PER_METER;
                point2.y *= -1 * (float)PIXELS_PER_METER;
                DrawLineEx(point1, point2, POLYLINE_THICKNESS, POLYLINE_COLOR);
            }

            Vector2 point1 = polyline[polyline.size()-1];
            Vector2 point2 = polyline[0];

            point1.x *= PIXELS_PER_METER;
            point2.x *= PIXELS_PER_METER;
            // Y axis multiplied by -1 becuase opengl (and by turn raylib) Y axis is down and not up
            point1.y *= -1 * (float)PIXELS_PER_METER;
            point2.y *= -1 * (float)PIXELS_PER_METER;
            DrawLineEx(point1, point2, POLYLINE_THICKNESS, POLYLINE_COLOR);
        }
    }
}

void DrawMeshes()
{
    for (const auto& mesh : obstacleMeshes)
    {
        for (const auto& triangle : mesh)
        {
            Vector2 point1 = triangle.v1;
            Vector2 point2 = triangle.v2;
            Vector2 point3 = triangle.v3;
            
            point1.x *= PIXELS_PER_METER;
            point2.x *= PIXELS_PER_METER;
            point3.x *= PIXELS_PER_METER;
            // Y axis multiplied by -1 becuase opengl (and by turn raylib) Y axis is down and not up
            point1.y *= -1 * (float)PIXELS_PER_METER;
            point2.y *= -1 * (float)PIXELS_PER_METER;
            point3.y *= -1 * (float)PIXELS_PER_METER;

            // vX->vY->vZ order matters, this is done v3->v2->v1 because earcut.hpp sorts them as such
            DrawTriangle(point3, point2, point1, MESH_COLOR);
        }
    }
}

void DrawMeshTriangles()
{
    for (const auto& mesh : obstacleMeshes)
    {
        for (const auto& triangle : mesh)
        {
            Vector2 point1 = triangle.v1;
            Vector2 point2 = triangle.v2;
            Vector2 point3 = triangle.v3;
            
            point1.x *= PIXELS_PER_METER;
            point2.x *= PIXELS_PER_METER;
            point3.x *= PIXELS_PER_METER;
            // Y axis multiplied by -1 becuase opengl (and by turn raylib) Y axis is down and not up
            point1.y *= -1 * (float)PIXELS_PER_METER;
            point2.y *= -1 * (float)PIXELS_PER_METER;
            point3.y *= -1 * (float)PIXELS_PER_METER;
            
            DrawLineEx(point1, point2, MESH_TRIANGLE_THICKNESS, MESH_TRIANGLE_COLOR);
            DrawLineEx(point2, point3, MESH_TRIANGLE_THICKNESS, MESH_TRIANGLE_COLOR);
            DrawLineEx(point1, point3, MESH_TRIANGLE_THICKNESS, MESH_TRIANGLE_COLOR);
        }
    }
}


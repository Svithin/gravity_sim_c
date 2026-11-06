#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "raylib.h"
#include "gravity.h"

// Struct to hold the telemetry data read from the .csv file
typedef struct {
    int step;
    float time;
    char name[32];
    char symbol;
    float mass;
    float x;
    float y;
    float r;
    float v;
    float f;
    float TE;
} TelemetryData;

void simulation(char *filename)
{
    // Open .csv file to read the data
    FILE *file = fopen(filename, "r");
    if (file == NULL) 
    {
        perror("---Error opening file---\n");
        return;
    }

    // Skip the CSV header line
    char buffer[256];
    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        perror("---Error reading header---\n");
        fclose(file);
        return;
    }

    // First pass or predefined max coordinate estimation: 
    // For Earth-Moon / standard planetary orbits, we can dynamically track max extent, 
    // or use a safe default upper bound for scaling based on distance `r`.
    // Let's compute/track dynamic max distance encountered in the simulation file.
    
    // Set configuration flags and initialize window
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_TOPMOST);
    InitWindow(800, 600, "Gravity Simulation");
    SetTargetFPS(60);

    while (!WindowShouldClose()) 
    {
        int screenWidth = GetRenderWidth();
        int screenHeight = GetRenderHeight();
        int centerX = screenWidth / 2;
        int centerY = screenHeight / 2;
        float minDimension = fminf((float)screenWidth, (float)screenHeight);

        int fontSize = (int)(minDimension * 0.03f);
        if (fontSize < 12) fontSize = 12;
        int padding = (int)(minDimension * 0.03f);   

        TelemetryData obj1, obj2;

        // Read Object 1
        if (fscanf(file, "%d,%f,%31[^,], %c,%f,%f,%f,%f,%f,%f,%f", 
            &obj1.step, &obj1.time, obj1.name, &obj1.symbol, &obj1.mass, 
            &obj1.x, &obj1.y, &obj1.r, &obj1.v, &obj1.f, &obj1.TE) != 11) 
        {
            break;
        }

        // Read Object 2
        if (fscanf(file, "%d,%f,%31[^,], %c,%f,%f,%f,%f,%f,%f,%f", 
            &obj2.step, &obj2.time, obj2.name, &obj2.symbol, &obj2.mass, 
            &obj2.x, &obj2.y, &obj2.r, &obj2.v, &obj2.f, &obj2.TE) != 11) 
        {
            break;
        }

        // DYNAMIC SCALING: Use the current distance 'r' or max coordinate magnitude 
        // to fit the simulation cleanly inside ~80% of the window radius.
        float maxDist = fmaxf(obj1.r, 1.0f); // Fallback to avoid division by zero
        if (maxDist < 1e-5f)
            maxDist = fmaxf(fabsf(obj1.x), fabsf(obj1.y));
        if (maxDist < 1e-5f)
            maxDist = 1.0f;

        // Scale factor maps physical coordinates (meters) to screen pixels
        float scale = (minDimension * 0.4f) / maxDist;

        BeginDrawing();
            ClearBackground(BLACK);

            // Draw Object 1
            int x1 = centerX + (int)(obj1.x * scale);
            int y1 = centerY - (int)(obj1.y * scale);
            DrawCircle(x1, y1, 12, GOLD);
            
            char text1[128];
            snprintf(text1, sizeof(text1), "Name: %s\nPos: (%.2e, %.2e)\nVel: %.2f\nTime: %.1fs", 
                     obj1.name, obj1.x, obj1.y, obj1.v, obj1.time);
            DrawText(text1, padding, padding, fontSize, GOLD);

            // Draw Object 2
            int x2 = centerX + (int)(obj2.x * scale);
            int y2 = centerY - (int)(obj2.y * scale);
            DrawCircle(x2, y2, 8, WHITE);
            
            char text2[128];
            snprintf(text2, sizeof(text2), "Name: %s\nPos: (%.2e, %.2e)\nVel: %.2f", 
                     obj2.name, obj2.x, obj2.y, obj2.v);
            
            int textWidth = MeasureText(text2, fontSize); 
            DrawText(text2, screenWidth - textWidth - padding, padding, fontSize, WHITE);

        EndDrawing();
    }

    fclose(file);
    
    // Keep window open until user closes it
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("Simulation Finished. Close window to exit.", 50, 50, 20, GREEN);
        EndDrawing();
    }

    CloseWindow();
}
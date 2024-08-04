#include "raylib.h"
#include "game_state/states.h"

const int screenWidth = 1280;
const int screenHeight = 720;

GSM gsm{};

int main()
{
    InitWindow(screenWidth, screenHeight, "MVaniaJam 2024");
    InitAudioDevice();
    gsm.Init(SPLASH_STATE);
    SetTargetFPS(60);
    while (!WindowShouldClose()){
        float delta = GetFrameTime();
        gsm.Update(delta);
        BeginDrawing();
        gsm.Draw();
        EndDrawing();
    }
    CloseAudioDevice();
    CloseWindow();
    return 0;
}

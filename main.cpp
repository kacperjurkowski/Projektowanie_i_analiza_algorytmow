#include "raylib.h"

int main() {    

    const int screenWidth = 960;        //Stała szerokość
    const int screenHeight = 960;       //Stała wysokość


    InitWindow(screenWidth, screenHeight, "Warcaby");
    SetTargetFPS(60);

    Texture2D boardSprite = LoadTexture("assets/board.png");    //Plansza do gry
    Texture2D PawnBlack = LoadTexture("assets/PawnBlack.png");    //Plansza do gry
    Texture2D PawnRed = LoadTexture("assets/PawnRed.png");    //Plansza do gry

    //Wyłączone rozmycie pixelarta
    SetTextureFilter(boardSprite, TEXTURE_FILTER_POINT);    
    SetTextureFilter(PawnBlack, TEXTURE_FILTER_POINT);    
    SetTextureFilter(PawnRed, TEXTURE_FILTER_POINT);    

    const float scale = 3.0f;
    const int marginPx = 16;                        //Szerokość marginesu X
    const int marginPy = 30;                        //Szerokość marginesu Y
    
    const int fieldPx = 36;                         //Szerokość pola
    const int fieldPy = 32;                         //Wysokość pola

    const float screenMarginX = marginPx * scale;    //Margines X * skala
    const float screenMarginY = marginPy * scale;    //Margines Y * skala

    const float screenFieldX = fieldPx * scale;     // 108.0f
    const float screenFieldY = fieldPy * scale;     // 108.0f
    const float padding = 6.0f;                     // Nasze wyśrodkowanie piona na polu


    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTextureEx(boardSprite, {0, 0}, 0.0f, scale, WHITE);

        for (int logicalY = 0; logicalY < 8; logicalY++) {          //Wysokość 
            for (int logicalX = 0; logicalX < 8; logicalX++) {      //Szerokość
                
                //Piony stoją wyłącznie na ciemnych polach 
                if ((logicalX + logicalY) % 2 != 0) {               
                    
                    // Obliczenie dokładnej pozycji piksela na ekranie
                    float posX = screenMarginX + (logicalX * screenFieldX) + padding;
                    float posY = screenMarginY + (logicalY * screenFieldY) + padding;

                    // Górne 3 rzędy - Piony przeciwnika (Czarne) 
                    if (logicalY < 3) {
                        DrawTextureEx(PawnBlack, {posX, posY}, 0.0f, scale, WHITE);
                    }
                    // Dolne 3 rzędy - Piony gracza (Czerwone) 
                    else if (logicalY > 4) {
                        DrawTextureEx(PawnRed, {posX, posY}, 0.0f, scale, WHITE);
                    }
                }
            }
        }

        EndDrawing();
    }
    
    //Sprzątnięcie pamięci
    UnloadTexture(boardSprite);
    UnloadTexture(PawnBlack);
    UnloadTexture(PawnRed);

    CloseWindow();
    return 0;
}
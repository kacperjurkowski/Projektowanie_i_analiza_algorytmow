#include "raylib.h"
#include "board.hpp"
#include "ai.hpp"

using namespace std;

int main() {    

    const int screenWidth = 960;        //Stała szerokość
    const int screenHeight = 960;       //Stała wysokość


    InitWindow(screenWidth, screenHeight, "Warcaby");
    SetTargetFPS(60);
    
    
    InitAudioDevice();          //Inicjalizacja systemu audio

    //Ikonka gry
    Image windowIcon = LoadImage("assets/PawnRed.png");
    SetWindowIcon(windowIcon);
    UnloadImage(windowIcon);    

    Texture2D boardSprite = LoadTexture("assets/board.png");        //Plansza do gry
    Texture2D PawnBlack = LoadTexture("assets/PawnBlack.png");      //Czarny pion
    Texture2D PawnRed = LoadTexture("assets/PawnRed.png");          //Czerwony pion
    Texture2D PawnBlackQueen = LoadTexture("assets/PawnBlackQueen.png");      //Czerwona królowa
    Texture2D PawnRedQueen = LoadTexture("assets/PawnRedQueen.png");          //Czarny królowa
    Texture2D winSprite = LoadTexture("assets/Win.png");                //Wygrana
    Texture2D loseSprite = LoadTexture("assets/Lose.png");              //Przegrana
    Texture2D tieSprite = LoadTexture("assets/Tie.png");                //Remis


    Sound moveSound = LoadSound("sound_effects/move.wav");          //Dźwięk ruchu
    Sound captureSound = LoadSound("sound_effects/capture.wav");  //Dźwięk bicia
    Sound winSound = LoadSound("sound_effects/win.wav");            //Dźwięk wygranej
    Sound loseSound = LoadSound("sound_effects/lose.wav");        //Dźwięk przegranej
    Sound tieSound = LoadSound("sound_effects/tie.wav");        //Dźwięk remisu

    //Wyłączone rozmycie pixelarta przy skalowaniu 
    SetTextureFilter(boardSprite, TEXTURE_FILTER_POINT);    
    SetTextureFilter(PawnBlack, TEXTURE_FILTER_POINT);    
    SetTextureFilter(PawnRed, TEXTURE_FILTER_POINT);    
    SetTextureFilter(PawnBlackQueen, TEXTURE_FILTER_POINT);    
    SetTextureFilter(PawnRedQueen, TEXTURE_FILTER_POINT);    
    SetTextureFilter(winSprite, TEXTURE_FILTER_POINT);
    SetTextureFilter(loseSprite, TEXTURE_FILTER_POINT);
    SetTextureFilter(tieSprite, TEXTURE_FILTER_POINT); 

    const float scale = 3.0f;                       //Skala
    const int marginPx = 16;                        //Szerokość marginesu X
    const int marginPy = 30;                        //Szerokość marginesu Y
    
    const int fieldPx = 36;                         //Szerokość pola
    const int fieldPy = 32;                         //Wysokość pola

    const float screenMarginX = marginPx * scale;    //Margines X * skala
    const float screenMarginY = marginPy * scale;    //Margines Y * skala

    const float screenFieldX = fieldPx * scale;     // 108.0f
    const float screenFieldY = fieldPy * scale;     // 108.0f
    const float padding = 6.0f;                     // Nasze wyśrodkowanie piona na polu

    Board gameBoard;                                //Inicjalizacja silnika

    Player currentPlayer = Player::Red;             //Gracz - Czerwone pionki
    Position selectedPos = {-1, -1};                //Na starcie żaden pionek nie jest zaznaczony

    vector<Move> currentLegalMoves = gameBoard.getLegalMoves(currentPlayer); //Lista legalnych ruchów gracza

    // Stan gry
    bool gameOver = false;              //Flaga określająca czy gra zakończona czy gra trwa
    Player winner = Player::None;       //Informacja kto wygrał (czerwoni/czarni)
    bool isDragging = false;            //Czy gracz trzyma pionek

    float aiThinkTimer = 0.0f;          //Czas od rozpoczęcia tury komputera
    const float aiThinkLimit = 0.5f;    //Czas, w którym komputer "udaje że myśli"

    int movesWithoutCapture = 0;    //Licznik do remisu

    //Funkcja wykonująca ruch
    auto executeMove = [&](Move move, Player playerMoving) {
        gameBoard.applyMove(move); 
        
        // Zarządzanie Dźwiękiem i Licznikiem Remisu
        if (move.isCapture()) {
            PlaySound(captureSound);    //Odtworzenie dźwięku bicia
            movesWithoutCapture = 0;    //Resetujemy licznik przy biciu!
        } else {
            PlaySound(moveSound);       //Odtworzenie dźwięku ruchu
            movesWithoutCapture++;      //Zwykły ruch = licznik rośnie
        }

        //ZASADA REMISU: 20 ruchów bez żadnego bicia
        if (movesWithoutCapture >= 20) {
            gameOver = true;            //Koniec gry
            winner = Player::None;      //Player::None jako zwycięzca zinterpretujemy jako REMIS
            PlaySound(tieSound);        //Odtworzenie dźwięku remisu
        }

        //  Zmiana tury i odświeżenie zasad
        currentPlayer = (playerMoving == Player::Red) ? Player::Black : Player::Red;
        currentLegalMoves = gameBoard.getLegalMoves(currentPlayer); 
        
        //Zwykły koniec gry - komuś skończyły się ruchy
        if (!gameOver && currentLegalMoves.empty()) {
            gameOver = true;        //Koniec gry
            winner = playerMoving;  //Zwycięża ten, kto po swoim ruchu zablokował przeciwnika
            
            // Odtwarzamy dźwięk wygranej/przegranej
            PlaySound(winner == Player::Red ? winSound : loseSound); 
        }

        //Domyślnie ustawienie zaznaczenia i trzymania pionka
        selectedPos = {-1, -1}; 
        isDragging = false;
    };

    while (!WindowShouldClose()) {
    
        // --- KONTROLER: TURA SZTUCZNEJ INTELIGENCJI (CZARNE) ---
        if (!gameOver && currentPlayer == Player::Black) {
            aiThinkTimer += GetFrameTime();
            
            if (aiThinkTimer >= aiThinkLimit) {
                //Generuj ruch 
                Move bestMove = AI::getBestMove(gameBoard, Player::Black, 3); //poziom głębi przeszukiwania prze AI: 3)
                executeMove(bestMove, Player::Black);
                aiThinkTimer = 0.0f;
            }
        }
        // --- KONTROLER: OBSŁUGA MYSZKI GRACZA (CZERWONE) ---
        else if (!gameOver && currentPlayer == Player::Red) {
            Vector2 mousePos = GetMousePosition();
            
            int gridX = (mousePos.x - screenMarginX) / screenFieldX;
            int gridY = (mousePos.y - screenMarginY) / screenFieldY;
            bool isMouseOnGrid = (gridX >= 0 && gridX < 8 && gridY >= 0 && gridY < 8);

            // 1. NACIŚNIĘCIE PRZYCISKU MYSZY
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && isMouseOnGrid) {
                bool moveExecuted = false;

                //Click-to-move
                if (selectedPos.x != -1) {
                    for (const auto& move : currentLegalMoves) {
                        if (move.getStart() == selectedPos && move.getEnd().x == gridX && move.getEnd().y == gridY) {
                            executeMove(move, Player::Red);
                            moveExecuted = true;
                            break;
                        }
                    }
                }

                //Zaznaczanie i łapanie piona
                if (!moveExecuted) {
                    Piece p = gameBoard.getPiece(gridX, gridY);
                    if (p.player == currentPlayer) {
                        bool hasMoves = false;
                        for (const auto& move : currentLegalMoves) {
                            if (move.getStart().x == gridX && move.getStart().y == gridY) {
                                hasMoves = true; break;
                            }
                        }
                        
                        if (hasMoves) {
                            selectedPos = {gridX, gridY};
                            isDragging = true; 
                        } else {
                            selectedPos = {-1, -1}; 
                        }
                    } else {
                        selectedPos = {-1, -1}; 
                    }
                }
            }

            //2. UPUSZCZENIE PRZYCISKU MYSZY
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                if (isDragging) {
                    if (isMouseOnGrid && selectedPos.x != -1) {
                        for (const auto& move : currentLegalMoves) {
                            if (move.getStart() == selectedPos && move.getEnd().x == gridX && move.getEnd().y == gridY) {
                                executeMove(move, Player::Red);
                                break;
                            }
                        }
                    }
                    isDragging = false; 
                }
            }
        }

        // --- WIDOK: RYSOWANIE GRAFIKI ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTextureEx(boardSprite, {0, 0}, 0.0f, scale, WHITE);     

        // Rysowanie kropek dozwolonego ruchu
        if (selectedPos.x != -1) {
            for (const auto& move : currentLegalMoves) {
                if (move.getStart() == selectedPos) {
                    Position end = move.getEnd();
                    
                    float centerX = screenMarginX + (end.x * screenFieldX) + (screenFieldX / 2.0f);
                    float centerY = screenMarginY + (end.y * screenFieldY) + (screenFieldY / 2.0f);
                    DrawCircle(centerX, centerY, 12.0f, Fade(BLACK, 0.4f));
                }   
            }
        }

        //Rysowanie stojących pionów
        for (int logicalY = 0; logicalY < 8; logicalY++) {          
            for (int logicalX = 0; logicalX < 8; logicalX++) {      
                
                if (isDragging && selectedPos.x == logicalX && selectedPos.y == logicalY) {
                    continue; 
                }

                Piece p = gameBoard.getPiece(logicalX, logicalY);

                if (!p.isEmpty()) {               
                    float posX = screenMarginX + (logicalX * screenFieldX) + padding;
                    float posY = screenMarginY + (logicalY * screenFieldY) + padding;

                    if (selectedPos.x == logicalX && selectedPos.y == logicalY && !isDragging) {
                        DrawRectangle(screenMarginX + logicalX * screenFieldX, 
                                    screenMarginY + logicalY * screenFieldY + 6.0f, 
                                    screenFieldX, screenFieldY, Fade(GREEN, 0.4f));
                    }

                    Texture2D texToDraw;
                    if (p.player == Player::Red) {
                        texToDraw = (p.type == PieceType::Queen) ? PawnRedQueen : PawnRed;
                    } else {
                        texToDraw = (p.type == PieceType::Queen) ? PawnBlackQueen : PawnBlack;
                    }

                    DrawTextureEx(texToDraw, {posX, posY}, 0.0f, scale, WHITE);
                }
            }
        }

        //Rysowanie przeciąganego pionka
        if (isDragging && selectedPos.x != -1) {
            Piece p = gameBoard.getPiece(selectedPos.x, selectedPos.y);
            Texture2D texToDraw;
            if (p.player == Player::Red) {
                texToDraw = (p.type == PieceType::Queen) ? PawnRedQueen : PawnRed;
            } else {
                texToDraw = (p.type == PieceType::Queen) ? PawnBlackQueen : PawnBlack;
            }

            Vector2 mousePos = GetMousePosition();
            float dragPosX = mousePos.x - (32 * scale / 2.0f);
            float dragPosY = mousePos.y - (32 * scale / 2.0f);
            
            DrawTextureEx(texToDraw, {dragPosX, dragPosY}, 0.0f, scale, WHITE);
        }

        // --- EKRAN KOŃCOWY Z WŁASNYMI SPRITAMI ---
        if (gameOver) {
            //Zaciemnienie ekranu
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.7f));
            
            Texture2D endSpriteToDraw;
            
            if (winner == Player::Red) {
                endSpriteToDraw = winSprite;
            } else if (winner == Player::Black) {
                endSpriteToDraw = loseSprite;
            } else {
                endSpriteToDraw = tieSprite; // Remis
            }

            // Automatyczne centrowanie Twojego sprita na środku ekranu
            float spriteX = (screenWidth - endSpriteToDraw.width) / 2.0f;
            float spriteY = (screenHeight - endSpriteToDraw.height) / 2.0f;
            
            DrawTexture(endSpriteToDraw, spriteX, spriteY, WHITE);
        }

        EndDrawing();
    }
    
    //Sprzątnięcie pamięci dźwiękowej
    UnloadSound(moveSound);
    UnloadSound(captureSound);
    UnloadSound(winSound);
    UnloadSound(loseSound);
    UnloadSound(tieSound);
    CloseAudioDevice();

    //Sprzątnięcie pamięci po teksturach
    UnloadTexture(boardSprite);
    UnloadTexture(PawnBlack);
    UnloadTexture(PawnRed);
    UnloadTexture(PawnRedQueen);
    UnloadTexture(PawnBlackQueen);
    UnloadTexture(winSprite);
    UnloadTexture(loseSprite);
    UnloadTexture(tieSprite);

    CloseWindow();
    return 0;
}
#include "board.hpp"

// --- 1. KONSTRUKTOR I NARZĘDZIA ---

Board::Board() {
    setupInitialBoard();
}

void Board::setupInitialBoard() {
    // Najpierw czyścimy całą planszę (ustawiamy powietrze)
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            grid[y][x] = {Player::None, PieceType::None};
        }
    }

    // Następnie układamy piony na ciemnych polach (jak w main.cpp!)
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            if ((x + y) % 2 != 0) {
                if (y < 3) {
                    grid[y][x] = {Player::Black, PieceType::Man};
                } else if (y > 4) {
                    grid[y][x] = {Player::Red, PieceType::Man}; // White to Twoje Czerwone
                }
            }
        }
    }
}

bool Board::isValidPos(int x, int y) const {
    return x >= 0 && x < 8 && y >= 0 && y < 8;
}

Piece Board::getPiece(int x, int y) const {
    if (!isValidPos(x, y)) return {Player::None, PieceType::None};
    return grid[y][x];
}

void Board::setPiece(int x, int y, Piece p) {
    if (isValidPos(x, y)) {
        grid[y][x] = p;
    }
}


// --- 2. FIZYCZNE WYKONANIE RUCHU ---

void Board::applyMove(const Move& move) {
    Position start = move.getStart();
    Position end = move.getEnd();
    
    // 1. Podnosimy piona ze startu
    Piece p = getPiece(start.x, start.y);
    setPiece(start.x, start.y, {Player::None, PieceType::None});
    
    // 2. Kładziemy na polu docelowym
    setPiece(end.x, end.y, p);

    // 3. Usuwamy wszystkie zbite piony przeciwnika (zamieniamy w powietrze)
    for (const Position& cap : move.getCaptures()) {
        setPiece(cap.x, cap.y, {Player::None, PieceType::None});
    }

    // 4. PROMOCJA NA DAMKĘ (Jeśli doszedł na koniec planszy)
    if (p.type == PieceType::Man) {
        if (p.player == Player::Red && end.y == 0) {
            setPiece(end.x, end.y, {Player::Red, PieceType::Queen});
        } else if (p.player == Player::Black && end.y == 7) {
            setPiece(end.x, end.y, {Player::Black, PieceType::Queen});
        }
    }
}


// --- 3. SZTUCZNA INTELIGENCJA ZASAD (GENEROWANIE RUCHÓW) ---

std::vector<Move> Board::getLegalMoves(Player player) const {
    std::vector<Move> moves;
    
    // Najpierw szukamy, czy są jakiekolwiek bicia
    getCaptureMoves(player, moves);

    // ZASADA BEZWZGLĘDNEGO BICIA: 
    // Jeśli wektor 'moves' jest pusty (nie ma bić), to dopiero wtedy szukamy zwykłych ruchów.
    if (moves.empty()) {
        getNormalMoves(player, moves);
    }

    return moves;
}

void Board::getNormalMoves(Player player, std::vector<Move>& moves) const {
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            Piece p = getPiece(x, y);
            if (p.player != player) continue; // Ignorujemy piony przeciwnika

            // Ustalenie dozwolonych kierunków w osi Y (Góra/Dół)
            std::vector<int> dirsY;
            if (p.type == PieceType::Queen) {
                dirsY = {-1, 1}; // Damka idzie i w górę i w dół
            } else if (player == Player::Red) {
                dirsY = {-1};    // Czerwone idą tylko w górę ekranu (Y maleje)
            } else {
                dirsY = {1};     // Czarne idą tylko w dół ekranu (Y rośnie)
            }

            // Sprawdzamy przekątne
            for (int dy : dirsY) {
                for (int dx : {-1, 1}) { // Lewo (-1) i Prawo (1)
                    int nx = x + dx;
                    int ny = y + dy;

                    if (p.type == PieceType::Queen) {
                        // DAMKA: Leci na dowolną ilość pustych pól
                        while (isValidPos(nx, ny) && getPiece(nx, ny).isEmpty()) {
                            moves.push_back(Move({x, y}, {nx, ny}));
                            nx += dx;
                            ny += dy;
                        }
                    } else {
                        // ZWYKŁY PIONEK: Tylko jedno pole
                        if (isValidPos(nx, ny) && getPiece(nx, ny).isEmpty()) {
                            moves.push_back(Move({x, y}, {nx, ny}));
                        }
                    }
                }
            }
        }
    }
}

void Board::getCaptureMoves(Player player, std::vector<Move>& moves) const {
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            Piece p = getPiece(x, y);
            if (p.player == player) {
                std::vector<Position> emptyPath;
                std::vector<Position> emptyCaptures;
                // Rozpoczynamy rekurencyjne szukanie bić dla tego pionka
                findCaptures({x, y}, {x, y}, player, p.type == PieceType::Queen, emptyPath, emptyCaptures, moves);
            }
        }
    }
}

void Board::findCaptures(Position startPos, Position currentPos, Player player, bool isQueen, std::vector<Position> currentPath, std::vector<Position> capturedPieces, std::vector<Move>& allMoves) const {
    bool foundAnyFurtherCapture = false;
    int directions[4][2] = {{-1, -1}, {1, -1}, {-1, 1}, {1, 1}}; // 4 kierunki po przekątnych

    for (auto& dir : directions) {
        int dx = dir[0];
        int dy = dir[1];
        
        if (!isQueen) {
            // ZWYKŁY PIONEK - bicie tylko o 2 pola
            int overX = currentPos.x + dx;
            int overY = currentPos.y + dy;
            int landX = currentPos.x + 2 * dx;
            int landY = currentPos.y + 2 * dy;

            if (isValidPos(landX, landY)) {
                Piece overPiece = getPiece(overX, overY);
                Piece landPiece = getPiece(landX, landY);

                // Zabezpieczenie: Czy tego przeciwnika już nie zbiliśmy w tym łańcuchu?
                bool alreadyCaptured = false;
                for (const auto& c : capturedPieces) {
                    if (c.x == overX && c.y == overY) { alreadyCaptured = true; break; }
                }

                if (!overPiece.isEmpty() && overPiece.player != player && !alreadyCaptured) {
                    // Pole docelowe musi być puste (lub być naszym punktem startowym - bicie "w kółko")
                    if (landPiece.isEmpty() || (landX == startPos.x && landY == startPos.y)) {
                        foundAnyFurtherCapture = true;
                        
                        std::vector<Position> newPath = currentPath;
                        newPath.push_back({landX, landY});
                        std::vector<Position> newCaptures = capturedPieces;
                        newCaptures.push_back({overX, overY});
                        
                        // Zaczepia samą siebie z nowej pozycji!
                        findCaptures(startPos, {landX, landY}, player, isQueen, newPath, newCaptures, allMoves);
                    }
                }
            }
        } else {
            // DAMKA - lotna, bada nieskończoną linię
            int step = 1;
            bool enemyFound = false;
            Position enemyPos = {-1, -1};

            while (true) {
                int nx = currentPos.x + dx * step;
                int ny = currentPos.y + dy * step;

                if (!isValidPos(nx, ny)) break;
                
                Piece p = getPiece(nx, ny);
                
                bool alreadyCaptured = false;
                for (const auto& c : capturedPieces) {
                    if (c.x == nx && c.y == ny) { alreadyCaptured = true; break; }
                }

                if (!p.isEmpty() && !alreadyCaptured) {
                    if (p.player == player) {
                        break; // Własny pion blokuje linię
                    } else {
                        if (enemyFound) break; // Drugi wróg z rzędu w tej samej linii
                        enemyFound = true;
                        enemyPos = {nx, ny};
                    }
                } else {
                    // Puste pole za wrogiem - każde kolejne takie pole to nowe legalne lądowanie
                    if (enemyFound) {
                        foundAnyFurtherCapture = true;
                        
                        std::vector<Position> newPath = currentPath;
                        newPath.push_back({nx, ny});
                        std::vector<Position> newCaptures = capturedPieces;
                        newCaptures.push_back(enemyPos);
                        
                        // Z każdej opcji lądowania sprawdzamy dalsze ewentualne bicia
                        findCaptures(startPos, {nx, ny}, player, isQueen, newPath, newCaptures, allMoves);
                    }
                }
                step++;
            }
        }
    }

    // Dodaj ostateczny ruch bicia tylko na samym końcu łańcucha (wymuszenie maksymalnego bicia)
    if (!foundAnyFurtherCapture && !capturedPieces.empty()) {
        bool isDuplicate = false;
        for (const auto& m : allMoves) {
            if (m.getStart() == startPos && m.getEnd() == currentPos && m.getCaptures().size() == capturedPieces.size()) {
                isDuplicate = true; break;
            }
        }
        if (!isDuplicate) {
            allMoves.push_back(Move(startPos, currentPos, capturedPieces));
        }
    }
}
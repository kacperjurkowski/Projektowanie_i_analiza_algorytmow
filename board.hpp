#pragma once
#include "piece.hpp" //Pionek i pozycja
#include <vector>    //Do trzymania list

using namespace std;

class Move {
private:
    Position startPos;              //Pozycja na początku ruchu
    Position endPos;                //Pozycja pod koniec ruchu
    vector<Position> capturedPieces; //Lista kratek, na których zbiliśmy piony wroga

public:
    //Konstruktor ruchu
    Move(Position start, Position end, vector<Position> captures = {})
        : startPos(start), endPos(end), capturedPieces(captures) {}

    //Metody dostępu ("Gettery")
    Position getStart() const { return startPos; }
    Position getEnd() const { return endPos; }
    vector<Position> getCaptures() const { return capturedPieces; }
    
    //Szybkie sprawdzenie: czy ten ruch kogokolwiek bije?
    bool isCapture() const { return !capturedPieces.empty(); }
};


class Board {
private:
    //Pamięć naszej planszy - siatka 8x8 przechowująca obiekty Piece
    Piece grid[8][8]; 

    //Narzędzia wewnętrzne silnika
    bool isValidPos(int x, int y) const;
    void setupInitialBoard();

    //Szukanie ruchów
    void getNormalMoves(Player player, vector<Move>& moves) const;
    void getCaptureMoves(Player player, vector<Move>& moves) const;
    
    // NOWA FUNKCJA: Rekurencyjne szukanie bicia wielokrotnego (w tym dla damek)
    void findCaptures(Position startPos, Position currentPos, Player player, bool isKing, vector<Position> currentPath, vector<Position> capturedPieces, vector<Move>& allMoves) const;

public:
    //Konstruktor
    Board();        

    //Podstawowa komunikacja z planszą
    Piece getPiece(int x, int y) const;
    void setPiece(int x, int y, Piece p);

    //Wykonuje ruch
    void applyMove(const Move& move);

    //Główny mózg silnika
    vector<Move> getLegalMoves(Player player) const;
};
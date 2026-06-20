#pragma once

enum class Player { 
    None,       //Pole bez pionka
    Red,      //Kolor czerwony
    Black       //Kolor czarny
};

enum class PieceType { 
    None,  //Pole bez pionka
    Man,   //Zwykły pion
    Queen   //Damka
};

//Struktura reprezentująca pojedynczego piona (lub puste pole) na planszy
struct Piece {
    Player player = Player::None;
    PieceType type = PieceType::None;

    // Szybka metoda sprawdzająca, czy pole jest puste
    bool isEmpty() const { 
        return player == Player::None; 
    }
};

//Struktura reprezentująca współrzędne na szachownicy (indeksy 0-7)
struct Position {
    int x;
    int y;

    // Przeciążenie operatorów porównania (niezbędne do weryfikacji, czy kliknięto w dobre pole)
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Position& other) const {
        return !(*this == other);
    }
};
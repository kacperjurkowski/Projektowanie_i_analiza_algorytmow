#pragma once
#include "board.hpp"
#include <vector>

class AI {
private:
    //Funkcja oceniająca - komputer patrzy na planszę i liczy "punkty"
    static int evaluate(const Board& board, Player aiPlayer);

    //Główny algorytm Minimax
    static int minimax(Board board, int depth, int alpha, int beta, bool isMaximizing, Player aiPlayer);

public:
    //Zwraca najlepszy znaleziony ruch
    static Move getBestMove(const Board& board, Player aiPlayer, int depth);
};
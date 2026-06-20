#include "ai.hpp"
#include <algorithm>
#include <limits>

int AI::evaluate(const Board& board, Player aiPlayer) {
    int score = 0;
    Player oppPlayer = (aiPlayer == Player::Red) ? Player::Black : Player::Red;
    
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            Piece p = board.getPiece(x, y);
            if (p.isEmpty()) continue;
            
            // Damka jest znacznie cenniejsza niż zwykły pion
            int val = (p.type == PieceType::Queen) ? 5 : 1; 
            
            if (p.player == aiPlayer) score += val;
            else if (p.player == oppPlayer) score -= val;
        }
    }
    return score;
}

int AI::minimax(Board board, int depth, int alpha, int beta, bool isMaximizing, Player aiPlayer) {
    Player currentPlayer = isMaximizing ? aiPlayer : ((aiPlayer == Player::Red) ? Player::Black : Player::Red);
    std::vector<Move> moves = board.getLegalMoves(currentPlayer);

    // Koniec symulacji (osiągnięto limit głębokości lub koniec gry)
    if (depth == 0 || moves.empty()) {
        int score = evaluate(board, aiPlayer);
        // Jeśli nie mamy ruchów, to przegraliśmy - komputer ucieka od tego scenariusza (-1000 pkt)
        if (moves.empty()) {
            return isMaximizing ? -1000 : 1000;
        }
        return score;
    }

    if (isMaximizing) {
        int maxEval = std::numeric_limits<int>::min(); // Minus nieskończoność
        for (const Move& move : moves) {
            Board newBoard = board; // Kopiujemy planszę na potrzeby symulacji
            newBoard.applyMove(move);
            int eval = minimax(newBoard, depth - 1, alpha, beta, false, aiPlayer);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) break; // Cięcie Alfa-Beta (przyspiesza algorytm)
        }
        return maxEval;
    } else {
        int minEval = std::numeric_limits<int>::max(); // Plus nieskończoność
        for (const Move& move : moves) {
            Board newBoard = board;
            newBoard.applyMove(move);
            int eval = minimax(newBoard, depth - 1, alpha, beta, true, aiPlayer);
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) break;
        }
        return minEval;
    }
}

Move AI::getBestMove(const Board& board, Player aiPlayer, int depth) {
    std::vector<Move> moves = board.getLegalMoves(aiPlayer);
    
    // Jeśli z jakiegoś powodu jest tylko 1 ruch, bierzemy go od razu (np. wymuszone bicie)
    if (moves.size() == 1) return moves[0];

    Move bestMove = moves[0];
    int maxEval = std::numeric_limits<int>::min();

    for (const Move& move : moves) {
        Board newBoard = board;
        newBoard.applyMove(move);
        int eval = minimax(newBoard, depth - 1, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), false, aiPlayer);
        
        if (eval > maxEval) {
            maxEval = eval;
            bestMove = move;
        }
    }
    return bestMove;
}
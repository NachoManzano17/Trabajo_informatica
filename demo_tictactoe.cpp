// Demo mínima del módulo de IA.
//
// NO es ARCHON. Es un tres en raya cuyo único propósito es probar
// que la IA funciona contra un estado concreto, sin depender del
// trabajo de tus compañeros. Una vez validada la IA aquí, el
// siguiente paso es escribir un adaptador sobre el tablero real.
//
// Compilar (desde la carpeta ai_module):
//   cl /std:c++17 /EHsc demo_tictactoe.cpp RandomStrategy.cpp
//      HeuristicStrategy.cpp MinimaxStrategy.cpp AIPlayer.cpp

#include <array>
#include <iostream>
#include <memory>

#include "AIPlayer.h"
#include "IGameState.h"

class TicTacToeState : public IGameState {
public:
    TicTacToeState() { board.fill(-1); }

    std::vector<Move> getLegalMoves() const override {
        std::vector<Move> moves;
        if (winnerCached() != -1 || isFull()) return moves;
        for (int i = 0; i < 9; ++i) {
            if (board[i] == -1) {
                Move m;
                m.kind = Move::Kind::Move;
                m.toRow = i / 3;
                m.toCol = i % 3;
                moves.push_back(m);
            }
        }
        return moves;
    }

    std::unique_ptr<IGameState> clone() const override {
        return std::make_unique<TicTacToeState>(*this);
    }

    void applyMove(const Move& m) override {
        const int idx = m.toRow * 3 + m.toCol;
        board[idx] = turn;
        turn = 1 - turn;
    }

    bool isTerminal() const override {
        return winnerCached() != -1 || isFull();
    }

    int currentPlayer() const override { return turn; }

    int winner() const override { return winnerCached(); }

    double evaluate(int me) const override {
        const int w = winnerCached();
        if (w == me) return 100.0;
        if (w != -1) return -100.0;
        // Heurística simple: cuenta líneas todavía ganables.
        double score = 0.0;
        static const int lines[8][3] = {
            {0,1,2},{3,4,5},{6,7,8},
            {0,3,6},{1,4,7},{2,5,8},
            {0,4,8},{2,4,6}
        };
        for (auto& line : lines) {
            int mine = 0, theirs = 0;
            for (int i : line) {
                if (board[i] == me)     ++mine;
                else if (board[i] != -1) ++theirs;
            }
            if (theirs == 0) score += mine;
            if (mine == 0)   score -= theirs;
        }
        return score;
    }

    void print() const {
        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < 3; ++c) {
                char ch = '.';
                if (board[r*3+c] == 0) ch = 'X';
                if (board[r*3+c] == 1) ch = 'O';
                std::cout << ch << ' ';
            }
            std::cout << '\n';
        }
        std::cout << '\n';
    }

private:
    std::array<int, 9> board{};
    int turn = 0;

    bool isFull() const {
        for (int v : board) if (v == -1) return false;
        return true;
    }

    int winnerCached() const {
        static const int lines[8][3] = {
            {0,1,2},{3,4,5},{6,7,8},
            {0,3,6},{1,4,7},{2,5,8},
            {0,4,8},{2,4,6}
        };
        for (auto& line : lines) {
            if (board[line[0]] != -1
                && board[line[0]] == board[line[1]]
                && board[line[1]] == board[line[2]]) {
                return board[line[0]];
            }
        }
        return -1;
    }
};

int main() {
    // IA 'Difícil' (jugador 0, X) contra IA 'Fácil' (jugador 1, O).
    AIPlayer xPlayer(Difficulty::Hard, 9);
    AIPlayer oPlayer(Difficulty::Easy);

    TicTacToeState s;
    s.print();

    while (!s.isTerminal()) {
        Move m;
        if (s.currentPlayer() == 0) m = xPlayer.chooseMove(s);
        else                        m = oPlayer.chooseMove(s);
        s.applyMove(m);
        s.print();
    }

    const int w = s.winner();
    if (w == 0)      std::cout << "Gana X (Hard)\n";
    else if (w == 1) std::cout << "Gana O (Easy)\n";
    else             std::cout << "Empate\n";
    return 0;
}

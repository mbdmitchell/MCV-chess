//
// Created by Max Mitchell on 12/10/2023.
//

#include "Game.h"

Game::Game(const Game &other)
        : gameState{other.gameState}
        , player1{other.player1}
        , player2{other.player2}
        , enPassantTargetSquare{other.enPassantTargetSquare}
        , whiteCastingAvailability{other.whiteCastingAvailability}
        , blackCastingAvailability{other.blackCastingAvailability}
{
    for (const auto& pair : other.board) {
        board[pair.first] = pair.second->clone();
    }
}

Game &Game::operator=(const Game &other) {
    if (this != &other) {  // Check for self-assignment
        Game temp(other); // Create a temporary copy
        std::swap(*this, temp);   // Swap contents with the temporary
    }
    return *this;
}

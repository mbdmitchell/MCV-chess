//
// Created by Max Mitchell on 12/10/2023.
//

#include "Game.h"

Game::Game(const Game &other)
        : gameState{other.gameState}
        , enPassantTargetSquare{other.enPassantTargetSquare}
        , whiteCastingAvailability{other.whiteCastingAvailability}
        , blackCastingAvailability{other.blackCastingAvailability}
        , activePlayer{other.activePlayer}
{
    for (const auto& pair : other.board) {
        board[pair.first] = pair.second->clone();
    }
}

Game &Game::operator=(const Game &other) {
    if (this != &other) {

        gameState = other.gameState;
        enPassantTargetSquare = other.enPassantTargetSquare;
        whiteCastingAvailability = other.whiteCastingAvailability;
        blackCastingAvailability = other.blackCastingAvailability;
        for (const auto& pair : other.board) {
            board[pair.first] = pair.second->clone();
        }
        /*Game temp(other); // TODO: copy-swap idiom preferred way but swap relies on `=` which causes recursion
        std::swap(*this, temp);*/
    }
    return *this;
}

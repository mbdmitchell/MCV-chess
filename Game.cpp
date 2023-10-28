//
// Created by Max Mitchell on 12/10/2023.
//

#include "Game.h"

Game::Game(const Game &other)
        : gameState{other.gameState}
        , enPassantTargetSquare{other.enPassantTargetSquare}
        , whiteCastlingAvailability{other.whiteCastlingAvailability}
        , blackCastlingAvailability{other.blackCastlingAvailability}
        , activePlayer{other.activePlayer}
{
    for (const auto& pair : other.board) {
        board[pair.first] = pair.second->clone();
    }
}

Game &Game::operator=(const Game &other) {
    if (this != &other) {
        Game temp(other);
        std::swap(*this, temp);
    }
    return *this;
}

std::string Game::gameStateAsString(Game::GameState gs) noexcept {
    switch (gs) {
        case GameState::WHITE_WIN: return "White Wins";
        case GameState::BLACK_WIN: return "Black Wins";
        case GameState::DRAW: return "Draw";
        case GameState::STALEMATE: return "Draw by Stalemate";
        case GameState::IN_PROGRESS: return "Game In Progress";
    }
}

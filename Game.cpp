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

std::string Game::gameStateAsString(Game::GameState gs) {
    switch (gs) {
        case GameState::WHITE_WIN: return "White Wins";
        case GameState::BLACK_WIN: return "Black Wins";
        case GameState::DRAW: return "Draw";
        case GameState::STALEMATE: return "Draw by Stalemate";
        case GameState::IN_PROGRESS: return "Game In Progress";
    }
}

Player Game::getPlayerWithColour(Piece::Colour colour) const {
    return (colour == Piece::Colour::WHITE) ? player1 : player2;
}

bool Game::isValidPromotionPiece(const Piece *&promotionPiece, const Player &player) {
    if (promotionPiece == nullptr) return false;

    if (dynamic_cast<const King*>(promotionPiece) != nullptr
        || dynamic_cast<const Pawn*>(promotionPiece) != nullptr
        || promotionPiece->getColour() != player.getColour()) {
        return false;
    }

    return true;
}

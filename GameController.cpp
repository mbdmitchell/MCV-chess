//
// Created by Max Mitchell on 12/10/2023.
//

#include "GameController.h"

void GameController::setup() {
    const auto& BLACK = Piece::Colour::BLACK;
    const auto& WHITE = Piece::Colour::WHITE;
    auto& board = game.board;
    // PAWNS
    for (Location location{"A2"}; location <= Location{"H2"}; ++location) {
        board.insert(location, std::make_unique<Pawn>(WHITE));
    }
    for (Location location{"A7"}; location <= Location{"H7"}; ++location) {
        board.insert(location, std::make_unique<Pawn>(BLACK));
    }
    // ROOKS
    for (Location location : {Location{"A1"}, Location{"H1"}}) {
        board.insert(location, std::make_unique<Rook>(WHITE));
    }
    for (Location location : {Location{"A8"}, Location{"H8"}}) {
        board.insert(location, std::make_unique<Rook>(BLACK));
    }
    // KNIGHTS
    for (Location location : {Location{"B1"}, Location{"G1"}}) {
        board.insert(location, std::make_unique<Knight>(WHITE));
    }
    for (Location location : {Location{"B8"}, Location{"G8"}}) {
        board.insert(location, std::make_unique<Knight>(BLACK));
    }
    // BISHOPS
    for (Location location : {Location{"C1"}, Location{"F1"}}) {
        board.insert(location, std::make_unique<Bishop>(WHITE));
    }
    for (Location location : {Location{"C8"}, Location{"F8"}}) {
        board.insert(location, std::make_unique<Bishop>(BLACK));
    }
    board.insert(Location{"D1"}, std::make_unique<Queen>(WHITE));
    board.insert(Location{"D8"}, std::make_unique<Queen>(BLACK));
    board.insert(Location{"E1"}, std::make_unique<King>(WHITE));
    board.insert(Location{"E8"}, std::make_unique<King>(BLACK));
}

void GameController::displayBoard() const {
    gameView->viewBoard(game.board);
}

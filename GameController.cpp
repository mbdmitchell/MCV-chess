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

void GameController::makeMove(const Location<> &source, const Location<> &destination) {

    Board& board = game.board;

    const auto handleRookCastlingMove = [&]() -> void {
        if (destination == Location{"C1"}) { //whiteCastingAvailability.queenSide;
            board.insert(Location{"D1"}, board.pieceAt(Location{"A1"})->clone());
            board.erase(Location{"A1"});
        }
        else if (destination == Location{"G1"}) { //whiteCastingAvailability.kingSide;
            board.insert(Location{"F1"}, board.pieceAt(Location{"H1"})->clone());
            board.erase(Location{"H1"});
        }
        else if (destination == Location{"C8"}) { //blackCastingAvailability.queenSide;
            board.insert(Location{"D8"}, board.pieceAt(Location{"A8"})->clone());
            board.erase(Location{"A8"});
        }
        else { // (destination == Location("G8")) //blackCastingAvailability.kingSide;
            board.insert(Location{"F8"}, board.pieceAt(Location{"H8"})->clone());
            board.erase(Location{"H8"});
        }
    };

    board.erase(destination); // if piece already there
    board.insert(destination, std::move(board[source]));
    board.erase(source);

    // logistics for special moves

    // 1. en passant
    /*if (isEnPassant(source, destination)) {
        board.erase(game.enPassantTargetSquare);
        return;
    }*/

    // 2. castling
    /*if (isCastlingAttempt(source, destination)) {
        handleRookCastlingMove();
        //updateCastingAvailability(MOVING_KING, MOVING_ROOK, SOURCE);
    }*/

    /*
    // 3. pawn promotion
    if (const int DESTINATION_ROW = DESTINATION.getRow(); MOVING_PAWN && (DESTINATION_ROW == 0 || DESTINATION_ROW == 7)){
        board.addPiece(DESTINATION, move.getPromotionPiece());
    }*/
}

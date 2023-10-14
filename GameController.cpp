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

void GameController::submitMove(const Location<> &source, const Location<> &destination) {
    // validation
    if (!isValidMove(source, destination)) return;
    //if (moveLeavesMoverInCheck(source, destination)) return;
    // move
    makeMove(source, destination);
    // handle any post-move things that need sorting
    setEnPassantTargetSquare(source, destination);
    swapActivePlayer();
}

void GameController::swapActivePlayer() {
    game.activePlayer = ((game.activePlayer.getColour() == game.player1.getColour()) ? game.player2 : game.player1);
}

bool GameController::isValidMove(const Location<> &source, const Location<> &destination) {
    const auto& board = game.board;
    const auto& moversColour = game.activePlayer.getColour();
    const bool isDirectCapture = board.thereExistsPieceAt(destination); // i.e. capture that's not an en passant

    // universal conditions
    if ( game.gameState != Game::IN_PROGRESS
         ||   Board::isLocationOutOfBounds(source)
         ||   Board::isLocationOutOfBounds(destination)
         || ! board.thereExistsPieceAt(source)
         ||  (board.thereExistsPieceAt(destination)
              && board[destination]->getColour() == moversColour)
         ||   board.pieceAt(source)->getColour() != moversColour
         || ! board[source]->isValidMovePath(source, destination, game.enPassantTargetSquare, isDirectCapture)
         ||   board.isPathBlocked(source, destination) )
    {
        return false;
    }

    // piece-dependant conditions

    // castling
    //if (isCastlingAttempt(game, source, destination) && !isValidCastling(source, destination)) {
    //    return false;
    //}

    // TODO: pawn promotion

    return true;
Location<> GameController::getLocationOfKing(Piece::Colour kingColour) {
    const auto& board = game.board.board;
    auto it = std::find_if(board.begin(), board.end(), [&](const auto& entry) {
        const auto& piece = entry.second;
        return (piece->getColour() == kingColour) && (dynamic_cast<King*>(piece.get()) != nullptr);
    });

    return (it != board.end() ? it->first : Location{});
}

bool GameController::isUnderAttackBy(Location<> target, const Piece::Colour& opponentsColour) {
    // NB: a square isn't marked as under attack if the attacker has a piece there.
    // En passant target squares are also not accounted for, but as isUnderAttack is a used in inCheck() and validMove()
    // and you cant castle through an en passant target square, this is a moot issue
    const auto& board = game.board.board;

    auto isOpponentPieceAttackingTarget = [&](const auto& it) -> bool {
        const auto& [source, sourcePiece] = it;
        if (sourcePiece.get()->getColour() != opponentsColour) return false;
        return GameController::isValidMove(source, target);
    };

    return std::any_of(board.cbegin(), board.cend(), isOpponentPieceAttackingTarget);
}

void GameController::setEnPassantTargetSquare(const Location<> &source, const Location<> &destination) {
    const Location<>::RowColumnDifferences locationDifferences = Location<>::calculateRowColumnDifferences(source, destination);
    if (dynamic_cast<Pawn*>(game.board.pieceAt(destination)) != nullptr && abs(locationDifferences.rowDifference) == 2) {
        game.enPassantTargetSquare = destination;
    } else {
        game.enPassantTargetSquare = Location{};
    }
}

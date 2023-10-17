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

    board.erase(destination); // in case piece already there
    board.insert(destination, std::move(board[source]));
    board.erase(source);

    // logistics for special moves

    // 1. en passant
    if (isEnPassant(source, destination)) {
        board.erase(game.enPassantTargetSquare);
        return;
    }

    // 2. castling
    if (isCastlingAttempt(source, destination)) {
        handleRookCastlingMove(destination);
        return;
    }

    /*
    // 3. pawn promotion
    if (const int DESTINATION_ROW = DESTINATION.getRow(); MOVING_PAWN && (DESTINATION_ROW == 0 || DESTINATION_ROW == 7)){
        board.addPiece(DESTINATION, move.getPromotionPiece());
    }*/
}

void GameController::submitMove(const Location<> &source, const Location<> &destination) {
    // validation
    if (!isValidMove(source, destination)) return;
    if (moveLeavesMoverInCheck(source, destination)) return;

    const std::unique_ptr<Piece> pieceMoved = game.board.pieceAt(source)->clone(); // now we know it's valid we're safe to assign pieceMoved

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
    if (    ! board.thereExistsPieceAt(source)
         ||  (board.thereExistsPieceAt(destination) && board[destination]->getColour() == moversColour)
         ||   board.pieceAt(source)->getColour() != moversColour
         || ! board[source]->isValidMovePath(source, destination, game.enPassantTargetSquare, isDirectCapture)
         ||   board.isPathBlocked(source, destination) )
    {
        return false;
    }

    // piece-dependant conditions

    // castling
    if (isCastlingAttempt(source, destination) && !isValidCastling(source, destination)) {
        return false;
    }

    // TODO: pawn promotion

    return true;
}

bool GameController::moveLeavesMoverInCheck(const Location<> &source, const Location<> &destination) {

    GameController copy {*this}; // store controller (inc. game state) in copy

    const auto& moverColour = game.activePlayer.getColour();
    const auto& opponentColour = moverColour == Piece::Colour::WHITE ? Piece::Colour::BLACK : Piece::Colour::WHITE;

    makeMove(source, destination);
    swapActivePlayer(); // must be done manually as move not handled through submitMove()

    bool returnValue = isUnderAttackBy(getLocationOfKing(moverColour),opponentColour);
    swapActivePlayer(); // TODO: this bug fix makes me sad... (without this line, moveLeavesMoverInCheck had side-effect of swapping the game.activePlayer)

    *this = copy;
    return returnValue;
}

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

bool GameController::isValidCastling(const Location<> &source, const Location<> &destination) {

    if (!isCastlingAttempt(source, destination)) return false;
    // if (kingAlreadyMoved() return false;

    if (destination == Location{"C1"}) {
        return game.whiteCastingAvailability.queenSide && !game.board.thereExistsPieceAt(Location{"B1"}); // !game.board.thereExistsPieceAt(Location{"B1"}) as not handled by isPathBlocked()
    } else if (destination == Location{"C8"}) {
        return game.blackCastingAvailability.queenSide && !game.board.thereExistsPieceAt(Location{"B8"}); // !game.board.thereExistsPieceAt(Location{"B1"}) as not handled by isPathBlocked()
    } else if (destination == Location{"G1"}) {
        return game.whiteCastingAvailability.kingSide;
    } else if (destination == Location{"G8"}) {
        return game.blackCastingAvailability.kingSide;
    }

    return false;
}

bool GameController::isCastlingAttempt(const Location<> &source, const Location<> &destination) {
    const auto [rowDifference, columnDifference] { Location<>::calculateRowColumnDifferences(source, destination) };
    if ((dynamic_cast<King*>(game.board.pieceAt(source)) != nullptr)
        || rowDifference != 0
        || abs(columnDifference) != 2
        || (!(source == Location{"E1"} || source == Location{"E8"})))
    {
        return false;
    }
    return true;
}

void GameController::updateCastingAvailability(const gsl::not_null<Piece*> pieceMoved, const Location<> &source) {
    if (dynamic_cast<const King*>(pieceMoved.get()) != nullptr) { // if pieceMoved's type == King
        if (game.activePlayer.getColour() == Piece::Colour::WHITE) {
            game.whiteCastingAvailability = {false, false};
        } else {
            game.blackCastingAvailability = { false, false };
        }
    } else if (dynamic_cast<const Rook*>(pieceMoved.get()) != nullptr) { // todo: remove `!=/== nullptr` throughout. (if you want to keep the bool "look", `static_cast<bool>(dynamic_cast...)`
        if (game.activePlayer.getColour() == Piece::Colour::WHITE) {
            if (source == Location("A1")) {
                game.whiteCastingAvailability.queenSide = false;
            } else if (source == Location("H1")) {
                game.whiteCastingAvailability.kingSide = false;
            }
        } else {
            if (source == Location("A8")) {
                game.blackCastingAvailability.queenSide = false;
            } else if (source == Location("H8")) {
                game.blackCastingAvailability.kingSide = false;
            }
        }
    }

}

bool GameController::isEnPassant(const Location<> &source, const Location<> &destination) const {
    const auto& [sourceRow, sourceColumn] = source;
    const auto& [destinationRow, destinationColumn] = destination;
    return (dynamic_cast<Pawn*>(game.board.pieceAt(destination)) != nullptr
            && game.enPassantTargetSquare == Location<>{sourceRow.value(), destinationColumn.value()});
}

void GameController::handleRookCastlingMove(const Location<> &destination) {
    Board& board = game.board;
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
}

Game::GameState GameController::calculateGameState() const {
    if (thereExistsValidMove(game.activePlayer)) {
        /*if (isDrawByInsufficientMaterial() || isThreeFoldRepetition() || isFiftyMoveRule())
            return GameState::DRAW*/
        return Game::GameState::IN_PROGRESS;
    }
    else {
        //const auto& activePlayerColour = game.activePlayer.getColour();
        //const auto opposingPlayerColour = activePlayerColour == Piece::Colour::WHITE ? Piece::Colour::BLACK : Piece::Colour::WHITE;
        if (inCheck(game.activePlayer)) { // TODO: Test inCheck()
        //if (isUnderAttackBy(getLocationOfKing(activePlayerColour),opposingPlayerColour)) {
            return game.activePlayer.getColour() == Piece::Colour::WHITE ? Game::GameState::BLACK_WIN : Game::GameState::WHITE_WIN;
        }
        else {
            return Game::GameState::STALEMATE;
        }
    }
}

bool GameController::thereExistsValidMove(const Player& activePlayer) const {
    GameController copy {*this};
    for (const auto& [source, piece] : copy.game.board) {
        if (piece->getColour() != activePlayer.getColour()) continue;
        for (Location destination = Location{"A1"}; destination <= Location{"H8"}; ++destination) {
            if (copy.isValidMove(source, destination) && !copy.moveLeavesMoverInCheck(source, destination)) return true;
        }
    }
    return false;
}

void GameController::initGameLoop() {
    while (game.gameState == Game::GameState::IN_PROGRESS){
        displayBoard();
        std::cout << '\n';
        try {
            submitMove(Location{gameView->readInput("Type source square: ")}, Location{gameView->readInput("Type destination square: ")});
            game.gameState = calculateGameState();
        }
        catch (const std::exception& e) { gameView->logException(e); }
    }
}

bool GameController::inCheck(const Player& player) const {
    const Piece::Colour playerColour = player.getColour();
    const Piece::Colour opponentColour = playerColour == Piece::Colour::WHITE ? Piece::Colour::BLACK : Piece::Colour::WHITE;
    return isUnderAttackBy(getLocationOfKing(playerColour),opponentColour);
}

#include "GameController.h"

void GameController::setup() {
    const auto& BLACK = Piece::Colour::BLACK;
    const auto& WHITE = Piece::Colour::WHITE;
    auto& board = game.board;

    for (const auto& colour : {WHITE, BLACK}){

        const std::string pawnRow = (colour == WHITE ? "2" : "7");  // NB: didn't use gsl::index as don't want to risk confusing with ACTUAL indices (1, 6 respectively)
        const std::string pieceRow = (colour == WHITE ? "1" : "8");

        for (Location location{"A" + pawnRow}; location <= Location{"H" + pawnRow}; ++location) {
            board.insert(location, std::make_unique<Pawn>(colour));
        }
        for (Location location: {Location{"A" + pieceRow}, Location{"H" + pieceRow}}) {
            board.insert(location, std::make_unique<Rook>(colour));
        }
        for (Location location: {Location{"B" + pieceRow}, Location{"G" + pieceRow}}) {
            board.insert(location, std::make_unique<Knight>(colour));
        }
        for (Location location: {Location{"C" + pieceRow}, Location{"F" + pieceRow}}) {
            board.insert(location, std::make_unique<Bishop>(colour));
        }
        board.insert(Location{"D" + pieceRow}, std::make_unique<Queen>(colour));
        board.insert(Location{"E" + pieceRow}, std::make_unique<King>(colour));
    }
}

void GameController::setupSimple() {
    /// For debugging
    const auto& BLACK = Piece::Colour::BLACK;
    const auto& WHITE = Piece::Colour::WHITE;
    auto& board = game.board;
    board.insert(Location{"H7"}, std::make_unique<Pawn>(WHITE));
    board.insert(Location{"H2"}, std::make_unique<Pawn>(BLACK));
    board.insert(Location{"A3"}, std::make_unique<King>(WHITE));
    board.insert(Location{"D2"}, std::make_unique<Knight>(WHITE));
    board.insert(Location{"A1"}, std::make_unique<King>(BLACK));
}

void GameController::makeMove(const Location<> &source, const Location<> &destination, const Piece* promotionPiece = nullptr) {

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

    // 3. pawn promotion
    if (promotionPiece == nullptr) return;
    board.erase(destination); // erase pawn at back row
    board.insert(destination, promotionPiece->clone());
}

void GameController::submitMove(const Location<> &source, const Location<> &destination, const Piece* const promotionPiece = nullptr) {

    // pre-move validation <- todo: make one function to call both
    if (!isValidMove(game.activePlayer, source, destination, promotionPiece)) return;
    if (moveLeavesMoverInCheck(source, destination)) return;

    const std::unique_ptr<Piece> pieceMoved = game.board.pieceAt(source)->clone(); // now we know it's valid we're safe to assign pieceMoved

    // move
    makeMove(source, destination, promotionPiece);

    // post-move things that need sorting
    updateCastingAvailability(pieceMoved.get(), source);
    setEnPassantTargetSquare(source, destination);
    swapActivePlayer();
}

void GameController::swapActivePlayer() {
    game.activePlayer = ((game.activePlayer.getColour() == game.player1.getColour()) ? game.player2 : game.player1);
}

bool GameController::isValidMove(Player player, const Location<> &source, const Location<> &destination, const Piece* promotionPiece = nullptr) const {
    const auto& board = game.board;
    const auto& moversColour = player.getColour();
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

    // pawn promotion
    // if pawn move to back row ...
    if (dynamic_cast<const Pawn*>(board.pieceAt(source)) && isBackRow(destination, player)) {
        return Game::isValidPromotionPiece(promotionPiece, player);
    }

    return promotionPiece == nullptr;
}

bool GameController::moveLeavesMoverInCheck(const Location<> &source, const Location<> &destination) const {

    GameController copy {*this};
    copy.makeMove(source, destination);
    bool returnValue = copy.inCheck(copy.game.activePlayer);

    return returnValue;
}

Location<> GameController::getLocationOfKing(Piece::Colour kingColour) const { // TODO: pass a player, not a colour
    const auto& board = game.board.board;

    auto it = std::find_if(board.begin(), board.end(), [&](const auto& entry) {
        const auto& piece = entry.second;
        return (piece->getColour() == kingColour) && (dynamic_cast<King*>(piece.get()) != nullptr);
    });

    return (it != board.end() ? it->first : Location{});
}

bool GameController::isUnderAttackBy(Location<> target, const Piece::Colour& opponentsColour) const { // TODO: pass a player, not a colour
    // NB: a square isn't marked as under attack if the attacker has a piece there.
    // En passant target squares are also not accounted for, but as isUnderAttack is a used in inCheck() and validMove()
    // and you cant castle through an en passant target square, this is a moot issue
    const auto& board = game.board.board;
    auto isOpponentPieceAttackingTarget = [&](const auto& it) -> bool {
        const auto& [source, sourcePiece] = it;
        if (sourcePiece.get()->getColour() != opponentsColour) return false;
        return isValidMove(game.getPlayerWithColour(opponentsColour), source, target);
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

bool GameController::isValidCastling(const Location<> &source, const Location<> &destination) const {

    if (!isCastlingAttempt(source, destination)) return false;

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

// TODO: move to King class
bool GameController::isCastlingAttempt(const Location<> &source, const Location<> &destination) const {
    const auto [rowDifference, columnDifference] { Location<>::calculateRowColumnDifferences(source, destination) };
    return (
         ((dynamic_cast<King*>(game.board.pieceAt(source)) != nullptr)
            && rowDifference == 0
            && abs(columnDifference) == 2
            && (source == Location{"E1"} || source == Location{"E8"})));
}

void GameController::updateCastingAvailability(const gsl::not_null<Piece*> pieceMoved, const Location<> &source) {
    if (dynamic_cast<const King*>(pieceMoved.get()) != nullptr) { // if pieceMoved's type == King
        if (game.activePlayer.getColour() == Piece::Colour::WHITE) {
            game.whiteCastingAvailability = { false, false };
        } else {
            game.blackCastingAvailability = { false, false };
        }
    } else if (dynamic_cast<const Rook*>(pieceMoved.get()) != nullptr) { // todo: remove `!=/== nullptr` throughout
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
        if (inCheck(game.activePlayer)) {
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
            if (copy.isValidMove(activePlayer, source, destination)
            && !copy.moveLeavesMoverInCheck(source, destination)) {
                return true;
            }

        }
    }
    return false;
}

void GameController::initGameLoop() {
    while (game.gameState == Game::GameState::IN_PROGRESS) {

        gameView->viewBoard(game.board);
        std::cout << '\n'; // TODO: no std::cout! handle with gameView

        try {
            const Player preMoveActivePlayer = game.activePlayer;
            gameView->displayTurn(preMoveActivePlayer);

            const auto source = Location{gameView->readInput("Type source square: ")};
            const auto destination = Location{gameView->readInput("Type destination square: ")};

            auto promotionPiece = std::invoke([&]() -> std::unique_ptr<Piece> {
                if (dynamic_cast<Pawn*>(game.board.pieceAt(source)) == nullptr || !isBackRow(destination, preMoveActivePlayer)) {
                    return nullptr;
                }
                while (true) {
                    const char pieceChar = gameView->readInput("Enter promotion piece char (eg. 'Q', 'R'): ")[0];
                    const char pieceCode = toupper(pieceChar, std::locale());
                    if (pieceFactories.find(pieceCode) != pieceFactories.end()) {
                        return pieceFactories.at(pieceCode)(preMoveActivePlayer.getColour());
                    } else {
                        gameView->displayException(std::runtime_error("Entered invalid char"));
                        std::cout << '\n'; // TODO: no std::cout! handle with gameView
                    }
                }
            });

            submitMove(source, destination, promotionPiece.get());
            if (preMoveActivePlayer != game.activePlayer) {
                game.gameState = calculateGameState();
            }
        }
        catch (const std::exception& e) {
            gameView->displayException(e);
        }
    }
    gameView->viewBoard(game.board);
    gameView->displayEndOfGameMessage(game.gameState);
}

bool GameController::inCheck(const Player& player) const {
    const Piece::Colour playerColour = player.getColour();
    const Piece::Colour opponentColour = playerColour == Piece::Colour::WHITE ? Piece::Colour::BLACK : Piece::Colour::WHITE;
    return isUnderAttackBy(getLocationOfKing(playerColour),opponentColour);
}

void GameController::manualSetup() {

    gameView->viewBoard(game.board);
    std::cout << '\n';

    while (toupper(gameView->readInput("Add another piece? (Any key for Yes, 'N' for no): ")[0], std::locale()) != 'N') {

        std::unique_ptr<Piece> selectedPiece = [&]{
            while (true) {
                try {
                    const char pieceChar = gameView->readInput("Enter piece char (eg. 'K', 'k'): ")[0];
                    const char pieceCode = toupper(pieceChar, std::locale());
                    if (pieceFactories.find(pieceCode) != pieceFactories.end()) {
                        if (isupper(pieceChar)) {
                            return pieceFactories.at(pieceCode)(Piece::Colour::WHITE);
                        } else {
                            return pieceFactories.at(pieceCode)(Piece::Colour::BLACK);
                        }
                    }
                    throw std::runtime_error("Invalid piece character");
                } catch (const std::exception& e) {
                    gameView->displayException(e);
                }
            }
        }();

        const Location selectedLocation = [&]{
            while (true) {
                try {
                    const auto location {Location{gameView->readInput("Place at location: ")}};
                    return location;
                }
                catch (const std::exception& e) {
                    gameView->displayException(e);
                }
            }
        }();

        game.board.insert(selectedLocation, std::move(selectedPiece));
        gameView->viewBoard(game.board);

    }

}

GameController::GameController(const GameController &rhs)
        : game{rhs.game}, gameView{std::make_unique<GameViewCLI>()} { }

GameController &GameController::operator=(const GameController &rhs) {
    gameView = std::make_unique<GameViewCLI>();
    game.board.board.clear(); // bug fix for moveLeavesMoverInCheck() where `*this = copy` didn't remove the moved piece
    game = rhs.game;
    return *this;
}

std::map<char, PieceFactory> GameController::createPieceFactories() {
    std::map<char, PieceFactory> temp;
    temp['P'] = [](Piece::Colour color) { return std::make_unique<Pawn>(color); };
    temp['B'] = [](Piece::Colour color) { return std::make_unique<Bishop>(color); };
    temp['N'] = [](Piece::Colour color) { return std::make_unique<Knight>(color); };
    temp['R'] = [](Piece::Colour color) { return std::make_unique<Rook>(color); };
    temp['Q'] = [](Piece::Colour color) { return std::make_unique<Queen>(color); };
    temp['K'] = [](Piece::Colour color) { return std::make_unique<King>(color); };
    return temp;
}

const std::map<char, PieceFactory> GameController::pieceFactories = createPieceFactories();

bool GameController::isBackRow(const Location<> &square, const Player &player) {
    if (player.getColour() == Piece::Colour::WHITE) {
        return square.getBoardRowIndex() == Location<>::getMaxRowIndex();
    }
    return square.getBoardRowIndex() == 0;
}

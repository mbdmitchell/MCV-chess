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

void GameController::makeMove(const Location &source, const Location &destination, const Piece* promotionPiece = nullptr) {

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
    if (King::isValidCastlingPath(source, destination)) {
        handleRookCastlingMove(destination);
        return;
    }

    // 3. pawn promotion
    if (promotionPiece == nullptr) return;
    board.erase(destination); // erase pawn at back row
    board.insert(destination, promotionPiece->clone());
}

void GameController::submitMove(const Location &source, const Location &destination, const Piece* const promotionPiece = nullptr) {

    // pre-move validation <- todo: make one function to call both
    if (!isValidMove(game.activePlayer, source, destination, promotionPiece)) return;
    if (moveLeavesMoverInCheck(source, destination)) return;

    const std::unique_ptr<Piece> pieceMoved = game.board.pieceAt(source)->clone(); // now we know it's valid we're safe to assign pieceMoved

    // move
    makeMove(source, destination, promotionPiece);

    // post-move things that need sorting
    updateCastingAvailability(*pieceMoved, source);
    setEnPassantTargetSquare(source, destination);
    swapActivePlayer();
}

void GameController::swapActivePlayer() {
    game.activePlayer = ((game.activePlayer == game.whitePlayer) ? game.blackPlayer : game.whitePlayer);
}

bool GameController::isValidMove(const Player& player, const Location &source, const Location &destination, const Piece* promotionPiece = nullptr) const {
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
    if (King::isValidCastlingPath(source, destination) && !isValidCastling(source, destination)) {
        return false;
    }

    // pawn promotion
    if (isType<Pawn>(*board.pieceAt(source)) && isBackRow(destination, player)) { // if (pawn moves to back row) ...
        return isValidPromotionPiece(promotionPiece, player);
    }

    return promotionPiece == nullptr;
}

bool GameController::moveLeavesMoverInCheck(const Location &source, const Location &destination) const {

    GameController copy {*this};
    copy.makeMove(source, destination);
    bool returnValue = copy.inCheck(copy.game.activePlayer);

    return returnValue;
}

void GameController::setEnPassantTargetSquare(const Location &source, const Location &destination) {
    const Location::RowColumnDifferences locationDifferences = Location::calculateRowColumnDifferences(source, destination);

    game.enPassantTargetSquare = [&](){
        if (isType<Pawn>(*game.board.pieceAt(destination)) && abs(locationDifferences.rowDifference) == 2) {
            return destination;
        }
        return Location{};
    }();

}

bool GameController::isValidCastling(const Location &source, const Location &destination) const {

    if (!King::isValidCastlingPath(source, destination)) return false;

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

void GameController::updateCastingAvailability(const Piece& pieceMoved, const Location &source) {
    if (isType<King>(pieceMoved)) {
        if (game.activePlayer.getColour() == Piece::Colour::WHITE) {
            game.whiteCastingAvailability = { false, false };
        } else {
            game.blackCastingAvailability = { false, false };
        }
    } else if (isType<Rook>(pieceMoved)) {
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

bool GameController::isEnPassant(const Location &source, const Location &destination) const {
    const auto& [sourceRow, sourceColumn] = source;
    const auto& [destinationRow, destinationColumn] = destination;
    return (isType<Pawn>(*game.board.pieceAt(destination))
            && game.enPassantTargetSquare == Location{sourceRow.value(), destinationColumn.value()});
}

void GameController::handleRookCastlingMove(const Location &destination) {
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
        // todo: implement additional draw conditions
        /*if (isDrawByInsufficientMaterial() || isThreeFoldRepetition() || isFiftyMoveRule())
            return GameState::DRAW*/
        return Game::GameState::IN_PROGRESS;
    }
    else {
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
            gameView->displayTurn(game.activePlayer);

            const auto& [source, destination, promotionPiece] = getMoveInfoFromUser();
            const Player preMoveActivePlayer = game.activePlayer;

            submitMove(source, destination, promotionPiece.get());
            // TODO: submitMove -> bool submitMoveAndReturnSuccessStatus()??
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
    const Player& opponent = (player.getColour() == Piece::Colour::WHITE ? game.blackPlayer : game.whitePlayer);
    return isUnderAttackBy(getLocationOfKing(player),opponent);
}

void GameController::manualSetup() {

    gameView->viewBoard(game.board);
    std::cout << '\n';

    while (toupper(gameView->readInput("Add another piece? (Any key for Yes, 'N' for no): ")[0], std::locale()) != 'N') {

        std::unique_ptr<Piece> selectedPiece = getPieceFromUser("Enter piece char (eg. 'K', 'k'): ");
        const Location selectedLocation = getLocationFromUser("Place at location: ");

        game.board.insert(selectedLocation, std::move(selectedPiece));
        gameView->viewBoard(game.board);

    }
    // TODO: add isValidBoard() for things like exactly 1 king of each colour
    // TODO: selectStartingTurn()
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

bool GameController::isBackRow(const Location &square, const Player &player) const {
    if (player == game.whitePlayer) {
        return square.getBoardRowIndex() == Location::getMaxRowIndex();
    }
    return square.getBoardRowIndex() == 0;
}

Game::MoveInfo GameController::getMoveInfoFromUser() const {

    const auto source = getLocationFromUser("Type source square: ");
    const auto destination = getLocationFromUser("Type destination square: ");

    auto promotionPiece = std::invoke([&]() -> std::unique_ptr<Piece> {
        if (!isType<Pawn>(*game.board.pieceAt(source)) || !isBackRow(destination, game.activePlayer)) {
            return nullptr;
        }
        while (true) {
            auto piece = getPieceFromUser("Enter promotion piece char (eg. 'Q', 'R'): ");

            if (!isType<King>(*piece)
                && !isType<Pawn>(*piece)
                && piece->getColour() == game.activePlayer.getColour()){
                return piece;
            } else {
                gameView->displayException(std::runtime_error("Invalid Promotion Piece (wrong piece type and/or colour)"));
            }
        }

    });

    return {source, destination, std::move(promotionPiece)};
}

Location GameController::getLocationFromUser(std::string_view message) const {
    while (true) {
        try {
            return Location{gameView->readInput(message)};
        }
        catch (const std::exception& e) {
            gameView->displayException(e);
        }
    }
}

std::unique_ptr<Piece> GameController::getPieceFromUser(std::string_view message) const {
    while (true) {
        const char pieceChar = gameView->readInput(message)[0];
        const char pieceCode = toupper(pieceChar, std::locale());
        if (pieceFactories.find(pieceCode) != pieceFactories.end()) {
            const auto colour = (isupper(pieceChar) ? Piece::Colour::WHITE : Piece::Colour::BLACK);
            return pieceFactories.at(pieceCode)(colour);
        } else {
            gameView->displayException(std::runtime_error("Entered invalid char"));
            std::cout << '\n'; // TODO: no std::cout! handle with gameView
        }
    }
}

Location GameController::getLocationOfKing(const Player &player) const {
    const auto& board = game.board.board;
    const auto& kingColour = player.getColour();

    auto it = std::find_if(board.begin(), board.end(), [&](const auto& entry) {
        const auto& piece = entry.second;
        return (piece->getColour() == kingColour) && (isType<King>(*piece));
    });

    return (it != board.end() ? it->first : Location{});
}

bool GameController::isUnderAttackBy(Location target, const Player &opponent) const {
    // NB: a square isn't marked as under attack if the attacker has a piece there.
    // En passant target squares are also not accounted for, but as isUnderAttack is a used in inCheck() and validMove()
    // and you cant castle through an en passant target square, this is a moot issue
    const auto& board = game.board.board;
    auto isOpponentPieceAttackingTarget = [&](const auto& it) -> bool {
        const auto& [source, sourcePiece] = it;
        if (sourcePiece.get()->getColour() != opponent.getColour()) return false;
        return isValidMove(opponent, source, target);
    };

    return std::any_of(board.cbegin(), board.cend(), isOpponentPieceAttackingTarget);
}

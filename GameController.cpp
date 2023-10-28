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

    // pre-move validation <- TODO: Low priority, put calcMoveValidityStatus and moveLeavesMoverInCheck in one function
    if (auto result = calcMoveValidityStatus(game.activePlayer, source, destination, promotionPiece); !result.isValid) {
        gameView->displayException(std::runtime_error("ERROR: " + result.reason));
        return;
    }
    if (moveLeavesMoverInCheck(source, destination)) {
        gameView->displayException(std::runtime_error("ERROR: Move leaves mover in check"));
        return;
    }

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

GameController::MoveValidityStatus GameController::calcMoveValidityStatus(const Player& player, const Location &source, const Location &destination, const Piece* promotionPiece = nullptr) const {
    const auto& board = game.board;
    const auto& moversColour = player.getColour();
    const bool isDirectCapture = board.thereExistsPieceAt(destination); // i.e. capture that's not an en passant

    // universal conditions

    if (!board.thereExistsPieceAt(source)) {
        return {.isValid = false, .reason = "No piece at source square"};
    }
    if (board.pieceAt(source)->getColour() != moversColour) {
        return {.isValid = false, .reason = "Moving wrong colour piece"};
    }
    if (board.thereExistsPieceAt(destination) && board[destination]->getColour() == moversColour) {
        return {.isValid = false, .reason = "Can't take your own piece"};
    }
    if (!board[source]->isValidMovePath(source, destination, game.enPassantTargetSquare, isDirectCapture)) {
        return {.isValid = false, .reason = "Piece can't move that way"};
    }
    if (board.isPathBlocked(source, destination)) {
        return {.isValid = false, .reason = "Path blocked"};
    }

    // piece-dependant conditions

    // castling
    if (King::isValidCastlingPath(source, destination) && !isValidCastling(source, destination)) {
        return {.isValid = false, .reason = "Invalid castling attempt"};
    }

    // pawn promotion
    if (isType<Pawn>(*board.pieceAt(source)) && isBackRow(destination, player)) { // if (pawn moves to back row) ...
        const bool validPromotionPiece = isValidPromotionPiece(promotionPiece, player);
        if (!validPromotionPiece) {
            return {.isValid = false, .reason = "Invalid promotion piece"};
        }
        return {.isValid = true};
    }
    if (promotionPiece) {
        return {.isValid = false, .reason = "Move includes promotion piece but can't promote"};
    }

    return {.isValid = true};
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
        return game.whiteCastlingAvailability.queenSide && !game.board.thereExistsPieceAt(Location{"B1"}); // !game.board.thereExistsPieceAt(Location{"B1"}) as not handled by isPathBlocked()
    } else if (destination == Location{"C8"}) {
        return game.blackCastlingAvailability.queenSide && !game.board.thereExistsPieceAt(Location{"B8"}); // !game.board.thereExistsPieceAt(Location{"B1"}) as not handled by isPathBlocked()
    } else if (destination == Location{"G1"}) {
        return game.whiteCastlingAvailability.kingSide;
    } else if (destination == Location{"G8"}) {
        return game.blackCastlingAvailability.kingSide;
    }

    return false;
}

void GameController::updateCastingAvailability(const Piece& pieceMoved, const Location &source) {
    if (isType<King>(pieceMoved)) {
        if (game.activePlayer.getColour() == Piece::Colour::WHITE) {
            game.whiteCastlingAvailability = { .kingSide = false, .queenSide = false };
        } else {
            game.blackCastlingAvailability = { .kingSide = false, .queenSide = false };
        }
    } else if (isType<Rook>(pieceMoved)) {
        if (game.activePlayer.getColour() == Piece::Colour::WHITE) {
            if (source == Location("A1")) {
                game.whiteCastlingAvailability.queenSide = false;
            } else if (source == Location("H1")) {
                game.whiteCastlingAvailability.kingSide = false;
            }
        } else {
            if (source == Location("A8")) {
                game.blackCastlingAvailability.queenSide = false;
            } else if (source == Location("H8")) {
                game.blackCastlingAvailability.kingSide = false;
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
        if (isDrawByInsufficientMaterial() /*|| isThreeFoldRepetition() || isFiftyMoveRule()*/) {
            return Game::GameState::DRAW;
        }
        return Game::GameState::IN_PROGRESS;
    }
    else {
        if (inCheck(game.activePlayer)) {
            return game.activePlayer.getColour() == Piece::Colour::WHITE
                ? Game::GameState::BLACK_WIN
                : Game::GameState::WHITE_WIN;
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
            if (copy.calcMoveValidityStatus(activePlayer, source, destination).isValid
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

        try {
            gameView->displayTurn(game.activePlayer);

            const auto& [source, destination, promotionPiece] = getMoveInfoFromUser();
            const Player preMoveActivePlayer = game.activePlayer;

            submitMove(source, destination, promotionPiece.get());
            // TODO: Low priority, submitMove -> bool submitMoveAndReturnSuccessStatus()??
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
    /// NB: Implementing has the potential to be a huge rabbit-hole so *mostly* assumes a valid position
    /// (e.g. doesn't cover both players in check/checkmate, pawns on back ranks, person in check when opponent's turn)

    /// Checks players have exactly one king as, out of all the ways to be invalid, this one would be most problematic

    gameView->viewBoard(game.board);
    std::cout << '\n';

    while (toupper(gameView->readInput("Add another piece? (Any key for Yes, 'N' for no): ")[0], std::locale()) != 'N') {

        std::unique_ptr<Piece> selectedPiece = getPieceFromUser("Enter piece char (eg. 'K', 'k'): ");
        const Location selectedLocation = getLocationFromUser("Place at location: ");

        game.board.insert(selectedLocation, std::move(selectedPiece));
        gameView->viewBoard(game.board);

    }

    const bool eachHaveExactlyOneKing = std::invoke([&](){
        const auto& board = game.board.board;

        size_t whiteKingCount = 0, blackKingCount = 0;

        std::for_each(cbegin(board), cend(board), [&](const auto& it){
            const auto& piece = it.second;
            if (isType<King>(*piece)) {
                (piece->getColour() == Piece::Colour::WHITE) ? ++whiteKingCount : ++blackKingCount;
            }
        });

        return whiteKingCount == 1 && blackKingCount == 1;
    });

    if (!eachHaveExactlyOneKing) {
        gameView->displayException(std::runtime_error("Invalid Position: Each player must have exactly one king. Clearing board..."));
        game.board.board.clear();
        return;
    }

    game.activePlayer = getStartingPlayer();
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
        return calcMoveValidityStatus(opponent, source, target).isValid;
    };

    return std::any_of(board.cbegin(), board.cend(), isOpponentPieceAttackingTarget);
}

Player GameController::getStartingPlayer() const {
    while (true) {
        const char startColour = toupper(gameView->readInput("Enter starting colour ('W' or 'B'): ")[0], std::locale());
        if (startColour == 'W') { return game.whitePlayer; }
        else if (startColour == 'B') { return game.blackPlayer; }
        else { gameView->displayException(std::runtime_error("Starting colour char not recognised"));}
    }
}

bool GameController::isDrawByInsufficientMaterial() const {
    /** NB: This innocent-seeming function is more complex to implement in accordance with FIDE rules than it
     * may seem, due to the following also being classed as "Insufficient Material"
     *
     * FIDE rules (article 6.9)
     * "However, the game is drawn if the position is such that the opponent cannot checkmate the player’s king
     * by any possible series of legal moves."
     *
     * This means the position 4k3/8/8/1p2p2p/1P2P2P/8/8/4K3 (FEN notation) is also classed as insufficient material,
     * despite many pieces being on the board.
     *
     * It's worth noting that this is so low-priority that, at the time of writing this (28 Oct 2023),
     * neither of the two largest chess sites chess.com and lichess.com have implemented this rule
     * TODO: Low priority, Integrate a 'helpmate analyser' to fix this.
     */

     const bool thereExistsAPawnOrMajorPiece = [&](){
         auto& board = game.board.board;
         return std::any_of(begin(board), end(board), [](const auto& it) {
             const auto& piece = it.second;
             return isType<Rook>(*piece) || isType<Queen>(*piece) || isType<Pawn>(*piece);
         });
     }();

    if (thereExistsAPawnOrMajorPiece) return false;

    struct MinorPieceCount {
        size_t whiteBishopCount, whiteKnightCount, blackBishopCount, blackKnightCount;
    };

    const MinorPieceCount minorPieceCount = [&](){

        MinorPieceCount count {0,0,0,0};

        auto& board = game.board.board;

        std::for_each(cbegin(board), cend(board), [&](const auto& it){
            const auto& piece = it.second;
            if (isType<Bishop>(*piece)) {
                (piece->getColour() == Piece::Colour::WHITE) ? ++count.whiteBishopCount : ++count.blackBishopCount;
            }
            else if (isType<Knight>(*piece)) {
                (piece->getColour() == Piece::Colour::WHITE) ? ++count.whiteKnightCount : ++count.blackKnightCount;
            }
        });

        return count;

    }();

    const auto totalWhiteMinorPieces = minorPieceCount.whiteBishopCount + minorPieceCount.whiteKnightCount;
    const auto totalBlackMinorPieces = minorPieceCount.blackBishopCount + minorPieceCount.blackKnightCount;

    // trivial draw conditions
    if (totalWhiteMinorPieces == 1 && totalBlackMinorPieces == 1) return true;
    if (totalWhiteMinorPieces > 2 || totalBlackMinorPieces > 2) return true;

    // if either side has a lone king
    if (totalWhiteMinorPieces == 0 || totalBlackMinorPieces == 0) {
        if ((totalWhiteMinorPieces == totalBlackMinorPieces)
            || (totalWhiteMinorPieces == 1 || totalBlackMinorPieces == 1)
            || (minorPieceCount.whiteKnightCount == 2 || minorPieceCount.blackKnightCount == 2)) {
            return true;
        }
    }

    // if (1 minor VS 2 minor)
    // 2 minor pieces against one results in a draw, except when the stronger side has a bishop Pair
    if (totalWhiteMinorPieces == 1 && totalBlackMinorPieces == 2
    || totalWhiteMinorPieces == 2
    && totalBlackMinorPieces == 1)
    {
        return minorPieceCount.whiteBishopCount != 2 && minorPieceCount.blackBishopCount != 2;
    }

    return false;
}

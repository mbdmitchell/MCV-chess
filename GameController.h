#pragma once

#include "Game.h"
#include "GameView.h"

using PieceFactory = std::function<std::unique_ptr<Piece>(Piece::Colour)>;

class GameController {

    struct MoveValidityStatus {
        bool isValid;
        std::string reason;
    };

    /// DATA MEMBERS
    Game game;
    std::unique_ptr<GameView> gameView = std::make_unique<GameViewCLI>();
    static const std::map<char, PieceFactory> pieceFactories;

    /// CONSTRUCTORS / OVERLOADS
public:
    GameController() = default;
    explicit GameController(gsl::not_null<GameView*> gv) : gameView{gv}, game{} { }

    GameController(const GameController& rhs); // Change if implementing other `GameView`s
    GameController& operator=(const GameController& rhs);

    /// SETUP
    void setup() noexcept;
    void manualSetup() noexcept;
    void setupSimple() noexcept; // TODO: remove from public API

    /// MISC.
    // TODO: submitMove(...) -> submitMove(Game::MoveInfo)
    void submitMove(const Location &source, const Location &destination, const Piece* promotionPiece) noexcept;
    void initGameLoop() noexcept;
    void displayAllUnderAttackBy(const Player& player) noexcept;

private:

    /// VALIDATION
    // TODO: isValidMove(Player, ...) -> submitMove(Player, Game::MoveInfo)    
    [[nodiscard]] GameController::MoveValidityStatus calcMoveValidityStatus(const Player& player, const Location &source, const Location &destination, const Piece *promotionPiece) const noexcept;
    [[nodiscard]] bool isValidCastling(const Location &source, const Location &destination) const noexcept;
    [[nodiscard]] bool isEnPassant(const Location &source, const Location &destination) const noexcept;
    [[nodiscard]] bool isBackRow(const Location& square, const Player& player) const noexcept;

    template <typename T>
    [[nodiscard]] static bool isType(const Piece& piece) {
        return dynamic_cast<const T*>(&piece);
    }

    [[nodiscard]] static bool isValidPromotionPiece(const Piece* promotionPiece, const Player &player) noexcept {
        if (promotionPiece == nullptr) return false;
        if (isType<King>(*promotionPiece)
            || isType<Pawn>(*promotionPiece)
            || promotionPiece->getColour() != player.getColour()) {
            return false;
        }
        return true;
    }
    /// CHECK
    [[nodiscard]] bool inCheck(const Player& player) const noexcept;
    [[nodiscard]] bool moveLeavesMoverInCheck(const Location &source, const Location &destination) const noexcept;

    /// GET / CALCULATE

    [[nodiscard]] Location getLocationOfKing(const Player& player) const noexcept;
    [[nodiscard]] Game::GameState calculateGameState() const noexcept;
    [[nodiscard]] bool isUnderAttackBy(Location target, const Player& opponent) const noexcept;
    [[nodiscard]] bool thereExistsValidMove(const Player& activePlayer) const noexcept;

    /// ... get from user
    [[nodiscard]] Game::MoveInfo getMoveInfoFromUser() const noexcept;
    [[nodiscard]] Location getLocationFromUser(std::string_view message) const noexcept;
    [[nodiscard]] std::unique_ptr<Piece> getPieceFromUser(std::string_view message) const noexcept;

    [[nodiscard]] Player getStartingPlayer() const noexcept;


    /// MANIPULATE GAME / BOARD
    // TODO: makeMove(...) -> makeMove(Game::MoveInfo)
    void makeMove(const Location &source, const Location &destination, const Piece* promotionPiece) noexcept;

    void setEnPassantTargetSquare(const Location &source, const Location &destination) noexcept;
    void updateCastingAvailability(const Piece& pieceMoved, const Location &source) noexcept;
    void handleRookCastlingMove(const Location &destination) noexcept;

    void swapActivePlayer() noexcept;

    /// MISC.
    static std::map<char, PieceFactory> createPieceFactories() noexcept;

    [[nodiscard]] bool isDrawByInsufficientMaterial() const noexcept;
};



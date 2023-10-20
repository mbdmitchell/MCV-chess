#pragma once

#include "Game.h"
#include "GameView.h"

using PieceFactory = std::function<std::unique_ptr<Piece>(Piece::Colour)>;

class GameController {

    /// DATA MEMBERS
    Game game;
    std::unique_ptr<GameView> gameView = std::make_unique<GameViewCLI>(); // Change if implementing other `GameView`s
    static const std::map<char, PieceFactory> pieceFactories;

    /// CONSTRUCTORS / OVERLOADS
public:
    GameController() = default;
    GameController(const GameController& rhs); // Change if implementing other `GameView`s
    GameController& operator=(const GameController& rhs);

    /// SETUP
    void setup();
    void manualSetup();
    void setupSimple(); // TODO: remove from public API

    /// MISC.
    void submitMove(const Location<> &source, const Location<> &destination, const Piece* const promotionPiece);
    void initGameLoop();
    void displayAllUnderAttackBy(const Player& player) {
        Game copy {game};
        auto col = player.getColour();
        copy.board.board.clear();
        for (Location i = Location{"A1"}; i <= Location{"H8"}; ++i) {
            if (isUnderAttackBy(i, col)) {
                copy.board.insert(i, std::make_unique<Pawn>(Piece::Colour::WHITE));
            }
        }
        gameView->viewBoard(copy.board);
    }

private:

    /// VALIDATION
    [[nodiscard]] bool isValidMove(Player player, const Location<> &source, const Location<> &destination, const Piece *promotionPiece) const;
    [[nodiscard]] bool isValidCastling(const Location<> &source, const Location<> &destination) const;
    [[nodiscard]] static bool isBackRow(const Location<>& square, const Player& player);
    /// MOVE TYPE
    [[nodiscard]] bool isEnPassant(const Location<> &source, const Location<> &destination) const; // TODO: move to Pawn::isEnPassant()

    /// CHECK
    [[nodiscard]] bool inCheck(const Player& player) const;
    [[nodiscard]] bool moveLeavesMoverInCheck(const Location<> &source, const Location<> &destination) const;

    /// GET / CALCULATE
    [[nodiscard]] Location<> getLocationOfKing(Piece::Colour kingColour) const;
    [[nodiscard]] Game::GameState calculateGameState() const;
    [[nodiscard]] bool isUnderAttackBy(Location<> location, const Piece::Colour& opponentsColour) const;
    [[nodiscard]] bool thereExistsValidMove(const Player& activePlayer) const;

    /// MANIPULATE GAME / BOARD
    void makeMove(const Location<> &source, const Location<> &destination, const Piece* promotionPiece);

    void setEnPassantTargetSquare(const Location<> &source, const Location<> &destination);
    void updateCastingAvailability(gsl::not_null<Piece*> pieceMoved, const Location<> &source);
    void handleRookCastlingMove(const Location<> &destination);

    void swapActivePlayer();

    /// MISC.
    static std::map<char, PieceFactory> createPieceFactories();

};



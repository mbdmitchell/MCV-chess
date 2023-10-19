#pragma once

#include "Game.h"
#include "GameView.h"

class GameController {
/// STRUCTS / ENUMS

/// DATA MEMBERS
    Game game;
    std::unique_ptr<GameView> gameView = std::make_unique<GameViewCLI>(); // Change if implementing other `GameView`s
/// FRIENDS

/// CONSTRUCTORS / OVERLOADS
public:
    GameController() = default;
    GameController(const GameController& rhs)
        : game{rhs.game}, gameView{std::make_unique<GameViewCLI>()} { } // Change if implementing other `GameView`s
    GameController& operator=(const GameController& rhs) {
        gameView = std::make_unique<GameViewCLI>();
        game.board.board.clear(); // bug fix for moveLeavesMoverInCheck() where `*this = copy` didn't remove the moved piece
        game = rhs.game;
        return *this;
    }
/// GETTERS

/// OPERATORS

/// VALIDATION

/// MISC.
public:
    void setup();
    void manualSetup();
    void setupSimple(); // TODO: remove from public API
    void displayBoard() const;
    void submitMove(const Location<> &source, const Location<> &destination);
    void initGameLoop();
private:
    void makeMove(const Location<> &source, const Location<> &destination);

    void swapActivePlayer();

    [[nodiscard]] bool isValidMove(Piece::Colour playerColour, const Location<> &source, const Location<> &destination) const;

    [[nodiscard]] bool moveLeavesMoverInCheck(const Location<> &source, const Location<> &destination) const;

    [[nodiscard]] Location<> getLocationOfKing(Piece::Colour kingColour) const;

    [[nodiscard]] bool isUnderAttackBy(Location<> location, const Piece::Colour& opponentsColour) const;

    void setEnPassantTargetSquare(const Location<> &source, const Location<> &destination);

    [[nodiscard]] bool isValidCastling(const Location<> &source, const Location<> &destination) const;

    [[nodiscard]] bool isCastlingAttempt(const Location<> &source, const Location<> &destination) const;
    void updateCastingAvailability(gsl::not_null<Piece*> pieceMoved, const Location<> &source);

    [[nodiscard]] bool isEnPassant(const Location<> &source, const Location<> &destination) const;

    void handleRookCastlingMove(const Location<> &destination);

    [[nodiscard]] Game::GameState calculateGameState() const;

    [[nodiscard]] bool thereExistsValidMove(const Player& activePlayer) const;

    [[nodiscard]] bool inCheck(const Player& player) const;
};

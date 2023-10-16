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
    GameController& operator=(const GameController& rhs){
        gameView = std::make_unique<GameViewCLI>();
        game = rhs.game;
        return *this;
    }
/// GETTERS

/// OPERATORS

/// VALIDATION

/// MISC.
public:
    void setup();
    void displayBoard() const;
    void submitMove(const Location<> &source, const Location<> &destination);
private:
    void makeMove(const Location<> &source, const Location<> &destination);

    void swapActivePlayer();

    bool isValidMove(const Location<> &source, const Location<> &destination);

    bool moveLeavesMoverInCheck(const Location<> &source, const Location<> &destination);

    Location<> getLocationOfKing(Piece::Colour kingColour);
    bool isUnderAttackBy(Location<> location, const Piece::Colour& opponentsColour);
    void setEnPassantTargetSquare(const Location<> &source, const Location<> &destination);
};

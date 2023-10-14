#pragma once

#include "Game.h"
#include "GameView.h"

class GameController {
/// STRUCTS / ENUMS

/// DATA MEMBERS
    Game game;
    std::unique_ptr<GameView> gameView = std::make_unique<GameViewCLI>(); // Change if implementing other `GameView`s
/// FRIENDS

/// CONSTRUCTORS

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
};

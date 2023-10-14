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

    GameController() = default;
    GameController(const GameController& rhs)
        : game{rhs.game}, gameView{std::make_unique<GameViewCLI>()} { } // Change if implementing other `GameView`s
/// GETTERS

/// OPERATORS

/// VALIDATION

/// MISC.
public:
    void setup();
    void displayBoard() const;
};

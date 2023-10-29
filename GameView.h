#pragma once

#include "Board.h"
#include "Game.h"
#include "glfw-3.3.8/include/GLFW/glfw3.h"

class GameView {
public:
    virtual ~GameView() = default;

    virtual void viewBoard(const Board& b) const = 0;
    virtual void viewPiece(const Piece& piece) const = 0;

    [[nodiscard]] virtual std::string readInput(std::string_view message) const = 0;

    virtual void displayEndOfGameMessage(Game::GameState gameState) const = 0;
    virtual void displayTurn(const Player& player) const = 0;

    virtual void displayException(const std::exception& e) const = 0;
    [[nodiscard]] virtual std::unique_ptr<GameView> clone() const noexcept = 0;
};

class GameViewCLI : public GameView {
public:
    void viewBoard(const Board &b) const override;
    void viewPiece(const Piece& piece) const override;

    [[nodiscard]] std::string readInput(std::string_view message) const override;

    void displayEndOfGameMessage(Game::GameState gameState) const override;
    void displayTurn(const Player& player) const override;

    void displayException(const std::exception& e) const override;
    [[nodiscard]] std::unique_ptr<GameView> clone() const noexcept override {
        return std::make_unique<GameViewCLI>(*this);
    }
};

class GameViewOpenGL : public GameView {
private:
    GLFWwindow* window;
public:

    GameViewOpenGL();

    ~GameViewOpenGL() override;

    void viewBoard(const Board &b) const override {
        // TODO: Remove code duplication w/ GameViewCLI::viewBoard()

    }
    void viewPiece(const Piece& piece) const override {}

    [[nodiscard]] std::string readInput(std::string_view message) const override {}

    void displayEndOfGameMessage(Game::GameState gameState) const override {}
    void displayTurn(const Player& player) const override {}

    void displayException(const std::exception& e) const override {}
    [[nodiscard]] std::unique_ptr<GameView> clone() const noexcept override {
        return std::make_unique<GameViewOpenGL>(*this);
    }
};
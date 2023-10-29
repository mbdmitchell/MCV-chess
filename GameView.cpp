//
// Created by Max Mitchell on 12/10/2023.
//

#include "GameView.h"

std::string GameViewCLI::readInput(std::string_view message) const {
    std::string input;
    std::cout << message << '\n';
    std::cin >> input;
    return input;
}

void GameViewCLI::viewPiece(const Piece& piece) const {
    std::cout << static_cast<char>(piece);
}

void GameViewCLI::viewBoard(const Board &b) const {

    const auto& board { b.board };

    const gsl::index maxRowIndex { Location::getMaxRowIndex() };
    const gsl::index maxColumnIndex { Location::getMaxColumnIndex() };

    for (gsl::index row = maxRowIndex; row >= 0; --row) { // top-left to bottom-right
        for (gsl::index col = 0; col <= maxColumnIndex; ++col) {

            const Location location {row, col};

            if (board.contains(location)) {
                const gsl::not_null<Piece*> piece = board.at(location).get(); // not_null not strictly necessary here
                viewPiece(*piece);
            } else {
                std::cout << '.';
            }

            std::cout << ' ';
            if (location.getBoardColumnIndex() == maxColumnIndex) {
                std::cout << '\n';
            }

        }
    }

    std::cout << '\n';

}

void GameViewCLI::displayException(const std::exception &e) const {
    std::cout << e.what() << '\n';
}

void GameViewCLI::displayEndOfGameMessage(const Game::GameState gameState) const {
    std::cout << std::format("End of Game: {}", Game::gameStateAsString(gameState)) << '\n';
}

void GameViewCLI::displayTurn(const Player &player) const {
    std::string colourString = (player.getColour() == Piece::Colour::WHITE ? "White" : "Black");
    std::cout << std::format("{}'s turn\n", colourString);
}

GameViewOpenGL::GameViewOpenGL() {

    if (!glfwInit()) { throw std::runtime_error("Cannot initialise GLFW"); }

    // Set GLFW window hints before creating the window

    window = glfwCreateWindow(640, 480, "Chess", nullptr, nullptr);

    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window)) {

        // Your game rendering logic goes here
        // Clear the screen, render your chessboard, pieces, and other elements

        glfwSwapBuffers(window);
        glfwPollEvents();                           // Poll for and process events
    }

    // Cleanup GLFW and other resources in the destructor
}

GameViewOpenGL::~GameViewOpenGL() {
    glfwTerminate(); // Cleanup and terminate GLFW when the object is destroyed
}


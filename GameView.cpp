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

    const auto& board = b.board;

    for (gsl::index row = Location<>::getMaxRowIndex(); row >= 0; --row) {
        for (Location location {row,0}; location <= Location{row,Location<>::getMaxColumnIndex()}; ++location) {

            if (board.contains(location)) {
                const gsl::not_null<Piece*> piece = board.at(location).get(); // not_null not strictly necessary here
                viewPiece(*piece);
            } else {
                std::cout << '.';
            }

            std::cout << ' ';
            if (Location<>::getMaxColumnIndex() == location.getBoardColumnIndex()) {
                std::cout << '\n';
            }

        }
    }

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

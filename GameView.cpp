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

void GameViewCLI::logError(const Error::Type& error) const {
    using enum Error::Type;
    switch (error) {
        case LOCATION_OUT_OF_BOUNDS: std::cout << "LOCATION_OUT_OF_BOUNDS"; break;
        case MOVE_WRONG_COLOUR_PIECE: std::cout << "MOVE_WRONG_COLOUR_PIECE"; break;
        case NO_PIECE_AT_SOURCE: std::cout << "NO_PIECE_AT_SOURCE"; break;
        case PIECE_OF_MOVERS_COLOUR_ALREADY_AT_DESTINATION: std::cout << "PIECE_OF_MOVERS_COLOUR_ALREADY_AT_DESTINATION"; break;
        case INVALID_MOVE_PATH: std::cout << "INVALID_MOVE_PATH"; break;
        case PATH_BLOCKED: std::cout << "PATH_BLOCKED"; break;
        default: throw std::runtime_error("Attempt to log error of an unknown type");
    }
}

void GameViewCLI::viewPiece(const gsl::not_null<const Piece *> piece) const {
    std::cout << static_cast<char>(*piece);
}

void GameViewCLI::viewBoard(const Board &b) const {

    const auto& board = b.board;

    for (gsl::index row = Location<>::getMaxRowIndex(); row != std::numeric_limits<size_t>::max(); --row) {
        for (Location location {row,0}; location <= Location{row,Location<>::getMaxColumnIndex()}; ++location) {

            if (board.contains(location)) {
                const gsl::not_null<Piece*> piece = board.at(location).get();
                viewPiece(piece);
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

void GameViewCLI::logException(const std::exception &e) const {
    std::cout << e.what() << '\n';
}

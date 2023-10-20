//
// Created by Max Mitchell on 11/10/2023.
//

#include "Board.h"

bool Board::isPathBlocked(const Location<> &source, const Location<> &destination) const {

    const auto totalRowColumnDifferences = Location<>::calculateRowColumnDifferences(source, destination);
    const auto minimalDistanceMoveForGivenDirection = calculateMinimalDistanceMoveForGivenDirection(totalRowColumnDifferences);

    auto toNextSquare = [](const Location<>& location, const Location<>::RowColumnDifferences& minimal) -> Location<> {
        const auto& [row, column] = location;

        const gsl::index rSum = row.value() + minimal.rowDifference;
        const gsl::index cSum = column.value() + minimal.columnDifference;

        return {rSum, cSum};
    };

    Location current = toNextSquare(source, minimalDistanceMoveForGivenDirection);
    while (current != destination) {
        if (thereExistsPieceAt(current)) {
            return true;
        }
        current = toNextSquare(current, minimalDistanceMoveForGivenDirection);
    }

    return false;
}

bool Board::thereExistsPieceAt(const Location<> &location) const {
    return board.contains(location);
}

Piece* Board::pieceAt(const Location<> &location) const {
    if (!board.contains(location)) {
        return nullptr;
    }
    return board.at(location).get();
}

void Board::erase(const Location<> &location) {
    board.erase(location);
}

void Board::insert(const Location<> &location, std::unique_ptr<Piece> piece) {
    board.insert({location,std::move(piece)});
}

Location<>::RowColumnDifferences Board::calculateMinimalDistanceMoveForGivenDirection(
        const Location<>::RowColumnDifferences &totalRowColumnDifferences) {
    const auto& [totalChangeInRow, totalChangeInColumn] = totalRowColumnDifferences;
    if (totalChangeInRow == 0 && totalChangeInColumn == 0) {
        return Location<>::RowColumnDifferences {0, 0};
    } else if (totalChangeInRow == 0) {
        return Location<>::RowColumnDifferences {0, totalChangeInColumn/abs(totalChangeInColumn)};
    } else if (totalChangeInColumn == 0) {
        return Location<>::RowColumnDifferences {totalChangeInRow / abs(totalChangeInRow), 0};
    } else {
        auto gcd = std::gcd(abs(totalChangeInRow), abs(totalChangeInColumn));
        return Location<>::RowColumnDifferences {totalChangeInRow / gcd, totalChangeInColumn / gcd};
    }
}

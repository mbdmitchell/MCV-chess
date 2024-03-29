#include "Board.h"

bool Board::isPathBlocked(const Location &source, const Location &destination) const noexcept {

    const auto totalRowColumnDifferences = Location::calculateRowColumnDifferences(source, destination);
    const auto minimalDistanceMoveForGivenDirection = calculateMinimalDistanceMove(totalRowColumnDifferences);

    auto toNextSquare = [&minimal = minimalDistanceMoveForGivenDirection](const Location& location) {
        const auto& [row, column] = location;

        const gsl::index rSum = row.value() + minimal.rowDifference;
        const gsl::index cSum = column.value() + minimal.columnDifference;

        return Location{rSum, cSum};
    };

    Location current = toNextSquare(source);
    while (current != destination) {
        if (thereExistsPieceAt(current)) {
            return true;
        }
        current = toNextSquare(current);
    }

    return false;
}

bool Board::thereExistsPieceAt(const Location &location) const noexcept{
    return board.contains(location);
}

Piece* Board::pieceAt(const Location &location) const noexcept{
    if (!board.contains(location)) {
        return nullptr;
    }
    return board.at(location).get();
}

void Board::erase(const Location &location) noexcept {
    board.erase(location);
}

void Board::insert(const Location &location, std::unique_ptr<Piece> piece) noexcept {
    board.insert({location,std::move(piece)});
}

Location::RowColumnDifferences Board::calculateMinimalDistanceMove(const Location::RowColumnDifferences& totalRowColumnDifferences) noexcept {
    const auto& [totalChangeInRow, totalChangeInColumn] = totalRowColumnDifferences;
    if (totalChangeInRow == 0 && totalChangeInColumn == 0) {
        return Location::RowColumnDifferences {0, 0};
    } else if (totalChangeInRow == 0) {
        return Location::RowColumnDifferences {0, totalChangeInColumn/abs(totalChangeInColumn)};
    } else if (totalChangeInColumn == 0) {
        return Location::RowColumnDifferences {totalChangeInRow / abs(totalChangeInRow), 0};
    } else {
        auto gcd = std::gcd(abs(totalChangeInRow), abs(totalChangeInColumn));
        return Location::RowColumnDifferences {totalChangeInRow / gcd, totalChangeInColumn / gcd};
    }
}

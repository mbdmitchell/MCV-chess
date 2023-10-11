#pragma once

#include "Piece.h"
#include "Location.h"
#include <map>
#include <numeric>

class Board {

    /// DATA MEMBERS
    std::map<Location<>, std::unique_ptr<Piece>> board;


    /// FRIENDS
    friend class GameController;


    /// STRUCTS

    /// CONSTRUCTORS

    /// GETTERS

    /// OPERATORS
public:
    // "function 'operator[]' with deduced return type cannot be used before it is defined"
    auto& operator[](const Location<>& location) {
        if (board.find(location) == board.end()) {
            board[location] = nullptr;
        }
        return board[location];
    }
    const auto& operator[](const Location<>& location) const {
        // Check if 'index' exists in the 'board' before accessing it.
        if (board.find(location) == board.end()) {
            // Handle the case where 'index' does not exist, e.g., by throwing an exception or returning a default value.
            // You need to decide what's appropriate for your use case.
            throw std::out_of_range("Location not found in board");
        }
        return board.at(location);
    }

    /// ITERATORS

    [[nodiscard]] auto begin() const {
        return board.begin();
    }
    [[nodiscard]] auto end() const {
        return board.end();
    }
    [[nodiscard]] auto cbegin() const {
        return board.cbegin();
    }
    [[nodiscard]] auto cend() const {
        return board.cend();
    }


    /// VALIDATION

    [[nodiscard]] static bool isLocationOutOfBounds(const Location<>& location) {
        const auto& [row, col] = location;
        return (row > location.getMaxRowIndex() || col > location.getMaxColumnIndex());
    }


    /// MISC.

    bool isPathBlocked(const Location<> &source, const Location<> &destination) const {

        const auto totalRowColumnDifferences = Location<>::calculateRowColumnDifferences(source, destination);

        const auto minimalDistanceMoveForGivenDirection = [&] {
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
        }();

        auto toNextSquare = [](const Location<>& location, const Location<>::RowColumnDifferences& minimal) -> Location<> {
            const auto& [row, column] = location;

            const gsl::index r = row.value() + minimal.rowDifference;
            const gsl::index c = column.value() + minimal.columnDifference;

            return {r, c};
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

    [[nodiscard]] bool thereExistsPieceAt(const Location<> &location) const {
        return board.contains(location);
    }

    Piece* pieceAt(const Location<>& location) const {
        if (!board.contains(location)) {
            return nullptr;
        }
        return board.at(location).get();
    }

    void erase(const Location<>& location) {
        board.erase(location);
    }

    void insert(const Location<>& location, std::unique_ptr<Piece> piece) {
        board.insert({location,std::move(piece)});
    }

};


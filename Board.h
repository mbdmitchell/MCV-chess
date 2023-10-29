#pragma once

#include "Piece.h"
#include "Location.h"
#include <map>
#include <numeric>

class Board {

    /// DATA MEMBERS
    std::map<Location, std::unique_ptr<Piece>> board;

    /// FRIENDS
    friend class GameController;
    friend class GameViewCLI;
    friend class GameViewOpenGL;

    /// OPERATORS
public:
    // "function 'operator[]' with deduced return type cannot be used before it is defined"
    auto& operator[](const Location& location) noexcept {
        if (board.find(location) == board.end()) {
            board[location] = nullptr;
        }
        return board[location];
    }
    const auto& operator[](const Location& location) const {
        if (board.find(location) == board.end()) {
            throw std::runtime_error("Location not found in board");
        }
        return board.at(location);
    }

    /// ITERATORS

    [[nodiscard]] auto begin() const noexcept { return board.begin(); }
    [[nodiscard]] auto end() const noexcept { return board.end(); }
    [[nodiscard]] auto cbegin() const noexcept { return board.cbegin(); }
    [[nodiscard]] auto cend() const noexcept { return board.cend(); }

    /// MISC.

    [[nodiscard]] bool isPathBlocked(const Location &source, const Location &destination) const noexcept;

    [[nodiscard]] bool thereExistsPieceAt(const Location &location) const noexcept;
    [[nodiscard]] Piece* pieceAt(const Location& location) const noexcept;

    void erase(const Location& location) noexcept;
    void insert(const Location& location, std::unique_ptr<Piece> piece) noexcept;

private:

    static Location::RowColumnDifferences calculateMinimalDistanceMove(const Location::RowColumnDifferences& totalRowColumnDifferences) noexcept;

};


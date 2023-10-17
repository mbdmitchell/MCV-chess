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
    friend class GameViewCLI;

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


    /// MISC.

    [[nodiscard]] bool isPathBlocked(const Location<> &source, const Location<> &destination) const;

    [[nodiscard]] bool thereExistsPieceAt(const Location<> &location) const;

    Piece* pieceAt(const Location<>& location) const;

    void erase(const Location<>& location);

    void insert(const Location<>& location, std::unique_ptr<Piece> piece);

};


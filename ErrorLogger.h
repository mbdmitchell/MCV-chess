#pragma once

#include <memory>
#include <map>

namespace Error {
    enum class Type {
        LOCATION_OUT_OF_BOUNDS
        , NO_PIECE_AT_SOURCE
        , MOVE_WRONG_COLOUR_PIECE
        , PIECE_OF_MOVERS_COLOUR_ALREADY_AT_DESTINATION
        , INVALID_MOVE_PATH
        , PATH_BLOCKED
    };
}

// Singleton
class ErrorLogger {
protected:
    ErrorLogger() = default;
    static std::unique_ptr<ErrorLogger> singleton;
    // data members
public:
    ErrorLogger(ErrorLogger &other) = delete;
    void operator=(const ErrorLogger &) = delete;
    static ErrorLogger* GetInstance() {
        return singleton.get();
    }

};

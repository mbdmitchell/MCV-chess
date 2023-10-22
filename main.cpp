#include "GameController.h"

// TODO: Idea: instead of unique_ptr's, construct needed objects in main() and classes have non-owning const references
// TODO: `noexcept` all the things!
// TODO: `final` things that should be `final`
// TODO: remove param names of unused params in overloaded functions eg isValidPath to better express that they aren't used
// - e.g., bool Bishop::isValidMovePath(const Location<> &source, const Location<> &destination, const Location<> &/*enPassantTargetSquare*/, bool /*isCapture*/)
// TODO: "Prefer pass-by-value for parameters that a function inherently would copy, but only if the parameter is 
// of a type that supports move semantics. Otherwise, use reference-to-const parameters."

int main() {
    GameController g{};
    g.setup();
    g.initGameLoop();
    return EXIT_SUCCESS;
}

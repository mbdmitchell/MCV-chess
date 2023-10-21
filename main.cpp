#include "GameController.h"

// TODO: Idea: instead of unique_ptr's, construct needed objects in main() and classes have non-owning const references
// TODO: `noexcept` all the things!
// TODO: `final` things that should be `final`
// TODO: remove param names of unused params, 
// - e.g., bool Bishop::isValidMovePath(const Location<> &source, const Location<> &destination, const Location<> &/*enPassantTargetSquare*/, bool /*isCapture*/)
int main() {
    GameController g{};
    g.setup();
    g.initGameLoop();
    return EXIT_SUCCESS;
}

#include "GameController.h"

// TODO: Idea: instead of unique_ptr's, construct needed objects in main() and classes have non-owning const references
// TODO: `noexcept` all the things!
// TODO: `final` things that should be `final`
int main() {
    GameController g{};
    g.setup();
    g.initGameLoop();
    return EXIT_SUCCESS;
}

#include "staged_tab.hpp"

using namespace ftxui;

namespace slayergit::ui {

Element StagedTab::render() {
    return vbox({
        text("Staged Changes") | bold | color(Color::Green),
        separator(),
        text("A  tests/test_window.cpp") | color(Color::Green),
        text("M  CMakeLists.txt") | color(Color::Yellow),
        separator(),
        text("Press 'u' to unstage selected file (not implemented yet)") | dim,
    });
}

} // namespace slayergit::ui

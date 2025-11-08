#include "stashes_view.hpp"
using namespace ftxui;
namespace slayergit::ui {
Element StashesView::render() {
    return vbox({text("Stashes") | bold, separator(), text("stash@{0}: WIP on main: Add feature"), text("stash@{1}: WIP on develop: Fix bug")});
}
} // namespace slayergit::ui

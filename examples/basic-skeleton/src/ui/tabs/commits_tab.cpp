#include "commits_tab.hpp"
using namespace ftxui;
namespace slayergit::ui {
Element CommitsTab::render() {
    return vbox({text("Commit History") | bold, separator(), text("* abc1234 Add UI panels (2h ago)"), text("  def5678 Initial commit (1d ago)")});
}
} // namespace slayergit::ui

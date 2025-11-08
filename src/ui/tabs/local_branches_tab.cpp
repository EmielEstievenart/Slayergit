#include "local_branches_tab.hpp"
using namespace ftxui;
namespace slayergit::ui {
Element LocalBranchesTab::render() {
  return vbox({
      text("Local Branches") | bold,
      separator(),
      text("* main") | color(Color::Green),
      text("  develop"),
      text("  feature/ui-panels"),
      text("  feature/git-wrapper"),
  });
}
} // namespace slayergit::ui

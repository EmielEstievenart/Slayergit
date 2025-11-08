#include "remote_branches_tab.hpp"
using namespace ftxui;
namespace slayergit::ui {
Element RemoteBranchesTab::render() {
  return vbox({text("Remote Branches") | bold, separator(),
               text("  origin/main"), text("  origin/develop")});
}
} // namespace slayergit::ui

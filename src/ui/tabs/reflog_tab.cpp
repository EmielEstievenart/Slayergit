#include "reflog_tab.hpp"
using namespace ftxui;
namespace slayergit::ui {
Element ReflogTab::render() {
  return vbox({text("Reflog") | bold, separator(),
               text("abc1234 HEAD@{0}: commit: Add UI panels"),
               text("def5678 HEAD@{1}: commit (initial): Initial commit")});
}
} // namespace slayergit::ui

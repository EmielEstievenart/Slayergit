#include "tags_tab.hpp"
using namespace ftxui;
namespace slayergit::ui {
Element TagsTab::render() {
  return vbox({text("Tags") | bold, separator(), text("  v1.0.0"),
               text("  v0.9.0"), text("  v0.8.5")});
}
} // namespace slayergit::ui

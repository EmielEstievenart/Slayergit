#include "unstaged_tab.hpp"

using namespace ftxui;

namespace slayergit::ui {

Element UnstagedTab::render() {
  // Placeholder implementation with fake data
  return vbox({
      text("Unstaged Changes") | bold | color(Color::Yellow),
      separator(),
      text(" M  src/main.cpp") | color(Color::Red),
      text(" M  src/ui/window.cpp") | color(Color::Red),
      text(" ?? README.md") | color(Color::GreenLight),
      text(" ?? docs/architecture.md") | color(Color::GreenLight),
      separator(),
      text("Press 's' to stage selected file (not implemented yet)") | dim,
  });
}

} // namespace slayergit::ui

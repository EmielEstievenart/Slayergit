#include "details_view.hpp"
using namespace ftxui;
namespace slayergit::ui {
Element DetailsView::render() {
    return vbox({
        text("File Details") | bold,
        separator(),
        text("File: src/main.cpp"),
        separator(),
        text("--- a/src/main.cpp") | color(Color::Red),
        text("+++ b/src/main.cpp") | color(Color::Green),
        text("@@ -1,3 +1,4 @@"),
        text("+ #include <iostream>") | color(Color::Green),
        text("  int main() {"),
        text("      return 0;"),
        text("  }"),
    });
}
} // namespace slayergit::ui

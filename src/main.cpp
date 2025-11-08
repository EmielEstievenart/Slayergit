#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

int main() {
  using namespace ftxui;

  // Create a simple interactive button
  auto screen = ScreenInteractive::Fullscreen();

  std::string quit_text = "Press 'q' to quit";
  bool should_quit = false;

  auto component = Renderer([&] {
    return vbox({
               text("SlayerGit") | bold | center,
               separator(),
               text("A blazing-fast Git TUI") | center,
               separator(),
               text("Built with FTXUI and Modern C++") | center,
               filler(),
               text(quit_text) | dim | center,
           }) |
           border | center;
  });

  // Handle 'q' key to quit
  component = CatchEvent(component, [&](Event event) {
    if (event == Event::Character('q')) {
      should_quit = true;
      screen.ExitLoopClosure()();
      return true;
    }
    return false;
  });

  screen.Loop(component);

  return 0;
}

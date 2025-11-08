#include "ui/window_manager.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

using namespace ftxui;
using namespace slayergit::ui;

int main() {
  // Create the screen
  auto screen = ScreenInteractive::Fullscreen();

  // Create window manager (sets up all windows and tabs)
  auto window_manager = std::make_unique<WindowManager>();

  // Create FTXUI component
  auto component = Renderer([&] { return window_manager->render(); });

  // Capture keyboard events
  component = CatchEvent(component, [&](Event event) {
    // Handle quit key
    if (event == Event::Character('q') || event == Event::Character('Q')) {
      screen.ExitLoopClosure()();
      return true;
    }

    // Handle refresh key (F5)
    if (event == Event::F5) {
      window_manager->trigger_refresh();
      return true;
    }

    // Pass event to window manager
    return window_manager->handle_event(event);
  });

  // Start the UI loop
  screen.Loop(component);

  return 0;
}

#include "ui/input_handler.hpp"
#include "ui/window_manager.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;
using namespace slayergit::ui;

int main() {
  auto screen = ScreenInteractive::Fullscreen();

  // Create the window manager
  WindowManager wm;

  // Create Window 1 with tabs
  auto window1 = wm.add_window("Window 1");
  window1->add_tab("Status");
  window1->add_tab("Changes");
  window1->add_tab("Staged");
  window1->add_tab("test");

  // Create Window 2 with tabs
  auto window2 = wm.add_window("Window 2");
  window2->add_tab("Log");
  window2->add_tab("Branches");
  window2->add_tab("Remotes");

  // Create Window 3 with tabs
  auto window3 = wm.add_window("Window 3");
  window3->add_tab("Diff");
  window3->add_tab("Stash");
  window3->add_tab("Reflog");

  // Create the input handler
  InputHandler input_handler(wm);
  input_handler.set_quit_callback([&screen] { screen.ExitLoopClosure()(); });

  // Create the main component from window manager
  auto main_component = wm.create_component();

  main_component = CatchEvent(main_component, [&](Event event) {
    auto result = input_handler.handle_event(event);
    return result.handled;
  });

  screen.Loop(main_component);

  return 0;
}

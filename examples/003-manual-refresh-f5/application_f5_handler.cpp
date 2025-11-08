// application_f5_handler.cpp
// Reference implementation of F5 key handling in Application class

#include "ui/application.hpp"

namespace slayergit::ui {

bool Application::handle_global_event(ftxui::Event event) {
  // F5 - Full refresh
  if (event == ftxui::Event::F5) {
    _app_state->refresh_all();
    show_status_message("Refreshing repository data...");
    return true;
  }

  // q - Quit
  if (event == ftxui::Event::Character('q')) {
    _should_quit = true;
    return true;
  }

  return false;
}

void Application::run() {
  auto screen = ftxui::ScreenInteractive::Fullscreen();

  auto component = ftxui::Renderer([&] { return _window_manager->render(); });

  // Wrap component to intercept events
  component = ftxui::CatchEvent(component, [&](ftxui::Event event) {
    // Check global keys first (F5, q, etc.)
    if (handle_global_event(event)) {
      return true;
    }

    // Otherwise pass to window manager for window-specific handling
    return _window_manager->handle_event(event);
  });

  screen.Loop(component);
}

} // namespace slayergit::ui

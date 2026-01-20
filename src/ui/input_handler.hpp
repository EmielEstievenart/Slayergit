#pragma once

#include <ftxui/component/event.hpp>

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace slayergit::ui {

// Forward declaration
class WindowManager;

// Command types that can be executed
enum class Command {
  None,
  Quit,
  FocusWindow1,
  FocusWindow2,
  FocusWindow3,
  FocusWindow4,
  FocusWindow5,
  FocusWindow6,
  FocusWindow7,
  FocusWindow8,
  FocusWindow9,
  FocusNextWindow,
  FocusPreviousWindow,
  NextTab,
  PreviousTab,
};

// Result of handling an event
struct InputResult {
  bool handled = false;
  Command command = Command::None;
};

// Handles user input and translates to commands
class InputHandler {
public:
  using QuitCallback = std::function<void()>;

  explicit InputHandler(WindowManager &wm);

  void set_quit_callback(QuitCallback callback) {
    quit_callback_ = std::move(callback);
  }

  // Process an event and return the result
  InputResult handle_event(const ftxui::Event &event);

private:
  void execute_command(Command cmd);

  WindowManager &window_manager_;
  QuitCallback quit_callback_;
};

} // namespace slayergit::ui

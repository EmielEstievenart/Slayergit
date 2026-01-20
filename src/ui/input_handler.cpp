#include "input_handler.hpp"

#include "window_manager.hpp"

namespace slayergit::ui {

InputHandler::InputHandler(WindowManager &wm) : window_manager_(wm) {}

InputResult InputHandler::handle_event(const ftxui::Event &event) {
  InputResult result;

  // Check for quit
  if (event == ftxui::Event::Character('q') ||
      event == ftxui::Event::Character('Q')) {
    result.handled = true;
    result.command = Command::Quit;
    execute_command(result.command);
    return result;
  }

  // Window focus: 1-9 keys
  if (event == ftxui::Event::Character('1')) {
    result.handled = true;
    result.command = Command::FocusWindow1;
    execute_command(result.command);
    return result;
  }
  if (event == ftxui::Event::Character('2')) {
    result.handled = true;
    result.command = Command::FocusWindow2;
    execute_command(result.command);
    return result;
  }
  if (event == ftxui::Event::Character('3')) {
    result.handled = true;
    result.command = Command::FocusWindow3;
    execute_command(result.command);
    return result;
  }
  if (event == ftxui::Event::Character('4')) {
    result.handled = true;
    result.command = Command::FocusWindow4;
    execute_command(result.command);
    return result;
  }
  if (event == ftxui::Event::Character('5')) {
    result.handled = true;
    result.command = Command::FocusWindow5;
    execute_command(result.command);
    return result;
  }
  if (event == ftxui::Event::Character('6')) {
    result.handled = true;
    result.command = Command::FocusWindow6;
    execute_command(result.command);
    return result;
  }
  if (event == ftxui::Event::Character('7')) {
    result.handled = true;
    result.command = Command::FocusWindow7;
    execute_command(result.command);
    return result;
  }
  if (event == ftxui::Event::Character('8')) {
    result.handled = true;
    result.command = Command::FocusWindow8;
    execute_command(result.command);
    return result;
  }
  if (event == ftxui::Event::Character('9')) {
    result.handled = true;
    result.command = Command::FocusWindow9;
    execute_command(result.command);
    return result;
  }

  // Tab navigation
  if (event == ftxui::Event::Tab) {
    result.handled = true;
    result.command = Command::NextTab;
    execute_command(result.command);
    return result;
  }
  if (event == ftxui::Event::TabReverse) {
    result.handled = true;
    result.command = Command::PreviousTab;
    execute_command(result.command);
    return result;
  }

  return result;
}

void InputHandler::execute_command(Command cmd) {
  switch (cmd) {
  case Command::Quit:
    if (quit_callback_) {
      quit_callback_();
    }
    break;

  case Command::FocusWindow1:
    window_manager_.focus_window(0);
    break;
  case Command::FocusWindow2:
    window_manager_.focus_window(1);
    break;
  case Command::FocusWindow3:
    window_manager_.focus_window(2);
    break;
  case Command::FocusWindow4:
    window_manager_.focus_window(3);
    break;
  case Command::FocusWindow5:
    window_manager_.focus_window(4);
    break;
  case Command::FocusWindow6:
    window_manager_.focus_window(5);
    break;
  case Command::FocusWindow7:
    window_manager_.focus_window(6);
    break;
  case Command::FocusWindow8:
    window_manager_.focus_window(7);
    break;
  case Command::FocusWindow9:
    window_manager_.focus_window(8);
    break;

  case Command::FocusNextWindow:
    window_manager_.focus_next_window();
    break;
  case Command::FocusPreviousWindow:
    window_manager_.focus_previous_window();
    break;

  case Command::NextTab:
    if (auto window = window_manager_.get_focused_window()) {
      window->select_next_tab();
    }
    break;
  case Command::PreviousTab:
    if (auto window = window_manager_.get_focused_window()) {
      window->select_previous_tab();
    }
    break;

  case Command::None:
    break;
  }
}

} // namespace slayergit::ui

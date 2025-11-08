#pragma once

#include "tab.hpp"
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <memory>
#include <vector>


namespace slayergit::ui {

/// A window contains one or more tabs
/// Pressing the window's number key cycles through its tabs
class Window {
public:
  Window(int number, std::vector<std::unique_ptr<Tab>> tabs);

  /// Get window number (1-5)
  int get_number() const { return _number; }

  /// Get window title (includes tab names)
  std::string get_title() const;

  /// Cycle to next tab (wraps around)
  void next_tab();

  /// Get current tab index
  size_t get_current_tab_index() const { return _current_tab_index; }

  /// Get pointer to current tab
  Tab *get_current_tab();

  /// Render the window (border + tabs + content)
  ftxui::Element render();

  /// Called when window gains focus
  void on_focus();

  /// Called when window loses focus
  void on_blur();

  /// Handle keyboard events
  bool handle_event(ftxui::Event event);

private:
  int _number;
  std::vector<std::unique_ptr<Tab>> _tabs;
  size_t _current_tab_index = 0;
  bool _has_focus = false;
};

} // namespace slayergit::ui

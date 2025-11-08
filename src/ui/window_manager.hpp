#pragma once

#include "window.hpp"
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <memory>
#include <vector>


namespace slayergit::ui {

/// Manages all windows and focus
/// Handles numbered key navigation (1-5)
class WindowManager {
public:
  WindowManager();

  /// Add a window to the manager
  void add_window(std::unique_ptr<Window> window);

  /// Focus window by number
  void focus_window(int number);

  /// Get currently focused window
  Window *get_focused_window() { return _focused_window; }

  /// Render the complete layout (top bar + windows)
  ftxui::Element render();

  /// Handle keyboard events
  bool handle_event(ftxui::Event event);

  /// Trigger refresh (F5) - placeholder for Git integration
  void trigger_refresh();

  /// Check if currently refreshing
  bool is_refreshing() const { return _is_refreshing; }

private:
  std::vector<std::unique_ptr<Window>> _windows;
  Window *_focused_window = nullptr;
  bool _is_refreshing = false;

  // Helper methods
  void handle_window_key(int number);
  ftxui::Element render_top_bar();
  ftxui::Element render_main_layout();
};

} // namespace slayergit::ui

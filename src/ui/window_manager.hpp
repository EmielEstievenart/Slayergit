#pragma once

#include "window.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

#include <memory>
#include <vector>

namespace slayergit::ui {

class WindowManager {
public:
  WindowManager() = default;

  // Window management
  void add_window(WindowPtr window);
  WindowPtr add_window(const std::string &title);
  void remove_window(size_t index);
  void remove_window(const std::string &title);
  [[nodiscard]] size_t window_count() const { return windows_.size(); }
  [[nodiscard]] WindowPtr get_window(size_t index) const;
  [[nodiscard]] WindowPtr get_focused_window() const;

  // Focus management
  [[nodiscard]] int focused_window_index() const { return focused_window_; }
  void focus_window(int index);
  void focus_next_window();
  void focus_previous_window();

  // Component creation - creates a vertical stack of all windows
  [[nodiscard]] ftxui::Component create_component();

private:
  std::vector<WindowPtr> windows_;
  std::vector<ftxui::Component> window_components_;
  int focused_window_ = 0;
};

using WindowManagerPtr = std::shared_ptr<WindowManager>;

} // namespace slayergit::ui

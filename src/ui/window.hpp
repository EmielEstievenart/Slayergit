#pragma once

#include "window_tab.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

#include <memory>
#include <string>
#include <vector>

namespace slayergit::ui {

class Window {
public:
  explicit Window(std::string title);

  [[nodiscard]] const std::string &title() const { return title_; }
  void set_title(std::string title) { title_ = std::move(title); }

  // Tab management
  void add_tab(WindowTabPtr tab);
  void add_tab(const std::string &name);
  void remove_tab(size_t index);
  void remove_tab(const std::string &name);
  [[nodiscard]] size_t tab_count() const { return tabs_.size(); }
  [[nodiscard]] WindowTabPtr get_tab(size_t index) const;
  [[nodiscard]] WindowTabPtr get_current_tab() const;

  // Selection
  [[nodiscard]] int selected_tab_index() const { return selected_tab_; }
  void select_tab(int index);
  void select_next_tab();
  void select_previous_tab();

  // Active state
  void set_active(bool active) { is_active_ = active; }
  [[nodiscard]] bool is_active() const { return is_active_; }

  // Component creation (call once)
  [[nodiscard]] ftxui::Component create_component();

  // Render the window
  [[nodiscard]] ftxui::Element render();

private:
  void rebuild_tab_names();

  std::string title_;
  std::vector<WindowTabPtr> tabs_;
  std::vector<std::string> tab_names_; // For Toggle component
  int selected_tab_ = 0;
  bool is_active_ = false;
  ftxui::Component tab_toggle_;
};

using WindowPtr = std::shared_ptr<Window>;

} // namespace slayergit::ui

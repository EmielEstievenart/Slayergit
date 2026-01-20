#include "window.hpp"

#include <algorithm>
#include <stdexcept>

namespace slayergit::ui {

Window::Window(std::string title) : title_(std::move(title)) {}

void Window::add_tab(WindowTabPtr tab) {
  tabs_.push_back(std::move(tab));
  rebuild_tab_names();
}

void Window::add_tab(const std::string &name) {
  add_tab(std::make_shared<WindowTab>(name));
}

void Window::remove_tab(size_t index) {
  if (index >= tabs_.size()) {
    return;
  }

  tabs_.erase(tabs_.begin() + static_cast<ptrdiff_t>(index));
  rebuild_tab_names();

  // Adjust selected tab if necessary
  if (!tabs_.empty()) {
    if (selected_tab_ >= static_cast<int>(tabs_.size())) {
      selected_tab_ = static_cast<int>(tabs_.size()) - 1;
    }
  } else {
    selected_tab_ = 0;
  }
}

void Window::remove_tab(const std::string &name) {
  auto it = std::find_if(
      tabs_.begin(), tabs_.end(),
      [&name](const WindowTabPtr &tab) { return tab->name() == name; });

  if (it != tabs_.end()) {
    remove_tab(static_cast<size_t>(std::distance(tabs_.begin(), it)));
  }
}

WindowTabPtr Window::get_tab(size_t index) const {
  if (index >= tabs_.size()) {
    return nullptr;
  }
  return tabs_[index];
}

WindowTabPtr Window::get_current_tab() const {
  return get_tab(static_cast<size_t>(selected_tab_));
}

void Window::select_tab(int index) {
  if (index >= 0 && index < static_cast<int>(tabs_.size())) {
    selected_tab_ = index;
  }
}

void Window::select_next_tab() {
  if (!tabs_.empty()) {
    selected_tab_ = (selected_tab_ + 1) % static_cast<int>(tabs_.size());
  }
}

void Window::select_previous_tab() {
  if (!tabs_.empty()) {
    selected_tab_ = (selected_tab_ - 1 + static_cast<int>(tabs_.size())) %
                    static_cast<int>(tabs_.size());
  }
}

void Window::rebuild_tab_names() {
  tab_names_.clear();
  tab_names_.reserve(tabs_.size());
  for (const auto &tab : tabs_) {
    tab_names_.push_back(tab->name());
  }
}

ftxui::Component Window::create_component() {
  using namespace ftxui;

  // Create a custom menu for tabs with proper styling
  auto option = MenuOption::HorizontalAnimated();
  option.entries_option.transform = [this](EntryState state) {
    Element e = text(state.label);

    // state.active = this is the selected entry
    if (state.active) {
      e = e | bold | underlined;
    }

    if (is_active_) {
      e = e | color(Color::Green);
    }

    e = e | hcenter;
    return e;
  };

  tab_toggle_ = Menu(&tab_names_, &selected_tab_, option);
  auto container = Container::Vertical({tab_toggle_});

  return Renderer(container, [this] { return render(); });
}

ftxui::Element Window::render() {
  using namespace ftxui;

  Element content;
  if (auto current_tab = get_current_tab()) {
    content = current_tab->render();
  } else {
    content = text("No tabs") | center;
  }

  Element tab_element = tab_toggle_ ? tab_toggle_->Render() : text("");

  if (is_active_) {
    // Active window: green border
    auto win = window(tab_element, content | flex) | flex | color(Color::Green);
    return win;
  }

  auto win = window(tab_element, content | flex) | flex;
  return win;
}

} // namespace slayergit::ui

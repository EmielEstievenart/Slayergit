#include "window_manager.hpp"

#include <algorithm>

namespace slayergit::ui {

void WindowManager::add_window(WindowPtr window) {
  windows_.push_back(std::move(window));
}

WindowPtr WindowManager::add_window(const std::string &title) {
  auto window = std::make_shared<Window>(title);
  add_window(window);
  return window;
}

void WindowManager::remove_window(size_t index) {
  if (index >= windows_.size()) {
    return;
  }

  windows_.erase(windows_.begin() + static_cast<ptrdiff_t>(index));

  // Adjust focused window if necessary
  if (!windows_.empty()) {
    if (focused_window_ >= static_cast<int>(windows_.size())) {
      focused_window_ = static_cast<int>(windows_.size()) - 1;
    }
  } else {
    focused_window_ = 0;
  }
}

void WindowManager::remove_window(const std::string &title) {
  auto it = std::find_if(
      windows_.begin(), windows_.end(),
      [&title](const WindowPtr &window) { return window->title() == title; });

  if (it != windows_.end()) {
    remove_window(static_cast<size_t>(std::distance(windows_.begin(), it)));
  }
}

WindowPtr WindowManager::get_window(size_t index) const {
  if (index >= windows_.size()) {
    return nullptr;
  }
  return windows_[index];
}

WindowPtr WindowManager::get_focused_window() const {
  return get_window(static_cast<size_t>(focused_window_));
}

void WindowManager::focus_window(int index) {
  if (index >= 0 && index < static_cast<int>(windows_.size())) {
    focused_window_ = index;
  }
}

void WindowManager::focus_next_window() {
  if (!windows_.empty()) {
    focused_window_ = (focused_window_ + 1) % static_cast<int>(windows_.size());
  }
}

void WindowManager::focus_previous_window() {
  if (!windows_.empty()) {
    focused_window_ =
        (focused_window_ - 1 + static_cast<int>(windows_.size())) %
        static_cast<int>(windows_.size());
  }
}

ftxui::Component WindowManager::create_component() {
  using namespace ftxui;

  // Create components for all windows once
  window_components_.clear();
  for (const auto &window : windows_) {
    window_components_.push_back(window->create_component());
  }

  // Create a vertical container with all window components
  auto container = Container::Vertical(window_components_, &focused_window_);

  return Renderer(container, [this] {
    // Update active state before rendering
    for (size_t i = 0; i < windows_.size(); ++i) {
      windows_[i]->set_active(static_cast<int>(i) == focused_window_);
    }

    Elements elements;
    elements.reserve(window_components_.size());

    for (const auto &comp : window_components_) {
      elements.push_back(comp->Render() | flex);
    }

    return vbox(std::move(elements)) | flex;
  });
}

} // namespace slayergit::ui

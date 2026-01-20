#include "window_tab.hpp"

namespace slayergit::ui {

WindowTab::WindowTab(std::string name)
    : name_(std::move(name)), content_renderer_(nullptr) {}

WindowTab::WindowTab(std::string name, ContentRenderer content_renderer)
    : name_(std::move(name)), content_renderer_(std::move(content_renderer)) {}

ftxui::Element WindowTab::render() const {
  if (content_renderer_) {
    return content_renderer_();
  }
  return ftxui::text("Tab: " + name_) | ftxui::center;
}

} // namespace slayergit::ui

#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

#include <functional>
#include <memory>
#include <string>

namespace slayergit::ui {

class WindowTab {
public:
  using ContentRenderer = std::function<ftxui::Element()>;

  explicit WindowTab(std::string name);
  WindowTab(std::string name, ContentRenderer content_renderer);

  [[nodiscard]] const std::string &name() const { return name_; }
  void set_name(std::string name) { name_ = std::move(name); }

  void set_content_renderer(ContentRenderer renderer) {
    content_renderer_ = std::move(renderer);
  }

  [[nodiscard]] ftxui::Element render() const;

private:
  std::string name_;
  ContentRenderer content_renderer_;
};

using WindowTabPtr = std::shared_ptr<WindowTab>;

} // namespace slayergit::ui

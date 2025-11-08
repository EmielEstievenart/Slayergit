#pragma once
#include "../tab.hpp"
namespace slayergit::ui {
class TagsTab : public Tab {
public:
  std::string name() const override { return "Tags"; }
  ftxui::Element render() override;
};
} // namespace slayergit::ui

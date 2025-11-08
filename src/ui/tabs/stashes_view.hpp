#pragma once
#include "../tab.hpp"
namespace slayergit::ui {
class StashesView : public Tab {
public:
  std::string name() const override { return "Stashes"; }
  ftxui::Element render() override;
};
} // namespace slayergit::ui

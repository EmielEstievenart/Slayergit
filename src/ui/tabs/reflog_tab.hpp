#pragma once
#include "../tab.hpp"
namespace slayergit::ui {
class ReflogTab : public Tab {
public:
  std::string name() const override { return "Reflog"; }
  ftxui::Element render() override;
};
} // namespace slayergit::ui

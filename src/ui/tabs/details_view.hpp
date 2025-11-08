#pragma once
#include "../tab.hpp"
namespace slayergit::ui {
class DetailsView : public Tab {
public:
  std::string name() const override { return "Details"; }
  ftxui::Element render() override;
};
} // namespace slayergit::ui

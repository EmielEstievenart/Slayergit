#pragma once
#include "../tab.hpp"

namespace slayergit::ui {
class LocalBranchesTab : public Tab {
public:
  std::string name() const override { return "Local"; }
  ftxui::Element render() override;
};
} // namespace slayergit::ui

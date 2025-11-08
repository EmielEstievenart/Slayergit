#pragma once
#include "../tab.hpp"
namespace slayergit::ui {
class RemoteBranchesTab : public Tab {
public:
    std::string name() const override { return "Remote"; }
    ftxui::Element render() override;
};
} // namespace slayergit::ui

#pragma once
#include "../tab.hpp"
namespace slayergit::ui {
class CommitsTab : public Tab {
public:
    std::string name() const override { return "Commits"; }
    ftxui::Element render() override;
};
} // namespace slayergit::ui

#pragma once

#include "../tab.hpp"

namespace slayergit::ui {

class StagedTab : public Tab {
public:
    std::string name() const override { return "Staged"; }
    ftxui::Element render() override;
};

} // namespace slayergit::ui

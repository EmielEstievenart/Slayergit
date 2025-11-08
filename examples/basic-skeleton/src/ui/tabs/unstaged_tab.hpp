#pragma once

#include "../tab.hpp"

namespace slayergit::ui {

/// Unstaged changes tab (placeholder implementation)
class UnstagedTab : public Tab {
public:
    std::string name() const override { return "Unstaged"; }
    ftxui::Element render() override;
};

} // namespace slayergit::ui

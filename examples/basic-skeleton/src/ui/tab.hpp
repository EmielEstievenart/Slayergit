#pragma once

#include <string>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>

namespace slayergit::ui {

/// Base interface for all tabs
/// Each window contains one or more tabs that implement this interface
class Tab {
public:
    virtual ~Tab() = default;

    /// Return the name of this tab (shown in tab bar)
    virtual std::string name() const = 0;

    /// Render the tab content
    virtual ftxui::Element render() = 0;

    /// Handle keyboard input (optional, return true if handled)
    virtual bool handle_event(ftxui::Event event) { return false; }

    /// Called when tab gains focus
    virtual void on_focus() {}

    /// Called when tab loses focus
    virtual void on_blur() {}
};

} // namespace slayergit::ui

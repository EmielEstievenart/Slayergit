#include "window.hpp"
#include <sstream>

using namespace ftxui;

namespace slayergit::ui {

Window::Window(int number, std::vector<std::unique_ptr<Tab>> tabs)
    : _number(number), _tabs(std::move(tabs)) {
    if (_tabs.empty()) {
        throw std::invalid_argument("Window must have at least one tab");
    }
}

std::string Window::get_title() const {
    // Build title with tab names: "1: Status [Unstaged | Staged]"
    std::stringstream ss;
    ss << _number << ": ";

    if (_tabs.size() == 1) {
        ss << _tabs[0]->name();
    } else {
        // Show all tab names with active one in brackets
        for (size_t i = 0; i < _tabs.size(); ++i) {
            if (i > 0) ss << " | ";
            if (i == _current_tab_index) {
                ss << "[" << _tabs[i]->name() << "]";
            } else {
                ss << _tabs[i]->name();
            }
        }
    }

    return ss.str();
}

void Window::next_tab() {
    if (_tabs.size() <= 1) return;

    _tabs[_current_tab_index]->on_blur();
    _current_tab_index = (_current_tab_index + 1) % _tabs.size();
    _tabs[_current_tab_index]->on_focus();
}

Tab* Window::get_current_tab() {
    return _tabs[_current_tab_index].get();
}

Element Window::render() {
    auto current_tab = get_current_tab();

    // Render: Title + Content with border
    // Use different color if focused
    auto content = vbox({
        text(get_title()) | bold,
        separator(),
        current_tab->render() | flex,
    });

    // Add border with color based on focus
    if (_has_focus) {
        return content | border | color(Color::Blue);
    } else {
        return content | border;
    }
}

void Window::on_focus() {
    _has_focus = true;
    get_current_tab()->on_focus();
}

void Window::on_blur() {
    _has_focus = false;
    get_current_tab()->on_blur();
}

bool Window::handle_event(Event event) {
    // Forward events to current tab
    return get_current_tab()->handle_event(event);
}

} // namespace slayergit::ui

#include "window_manager.hpp"
#include "tabs/unstaged_tab.hpp"
#include "tabs/staged_tab.hpp"
#include "tabs/local_branches_tab.hpp"
#include "tabs/remote_branches_tab.hpp"
#include "tabs/tags_tab.hpp"
#include "tabs/commits_tab.hpp"
#include "tabs/reflog_tab.hpp"
#include "tabs/stashes_view.hpp"
#include "tabs/details_view.hpp"
#include <algorithm>
#include <chrono>
#include <thread>

using namespace ftxui;

namespace slayergit::ui {

WindowManager::WindowManager() {
    // Create Window 1: Status with Unstaged and Staged tabs
    std::vector<std::unique_ptr<Tab>> status_tabs;
    status_tabs.push_back(std::make_unique<UnstagedTab>());
    status_tabs.push_back(std::make_unique<StagedTab>());
    add_window(std::make_unique<Window>(1, std::move(status_tabs)));

    // Create Window 2: Branches with Local, Remote, and Tags tabs
    std::vector<std::unique_ptr<Tab>> branch_tabs;
    branch_tabs.push_back(std::make_unique<LocalBranchesTab>());
    branch_tabs.push_back(std::make_unique<RemoteBranchesTab>());
    branch_tabs.push_back(std::make_unique<TagsTab>());
    add_window(std::make_unique<Window>(2, std::move(branch_tabs)));

    // Create Window 3: History with Commits and Reflog tabs
    std::vector<std::unique_ptr<Tab>> history_tabs;
    history_tabs.push_back(std::make_unique<CommitsTab>());
    history_tabs.push_back(std::make_unique<ReflogTab>());
    add_window(std::make_unique<Window>(3, std::move(history_tabs)));

    // Create Window 4: Stashes (single tab)
    std::vector<std::unique_ptr<Tab>> stash_tabs;
    stash_tabs.push_back(std::make_unique<StashesView>());
    add_window(std::make_unique<Window>(4, std::move(stash_tabs)));

    // Create Window 5: Details (single tab)
    std::vector<std::unique_ptr<Tab>> details_tabs;
    details_tabs.push_back(std::make_unique<DetailsView>());
    add_window(std::make_unique<Window>(5, std::move(details_tabs)));

    // Focus first window by default
    if (!_windows.empty()) {
        _focused_window = _windows[0].get();
        _focused_window->on_focus();
    }
}

void WindowManager::add_window(std::unique_ptr<Window> window) {
    _windows.push_back(std::move(window));
}

void WindowManager::focus_window(int number) {
    auto it = std::find_if(_windows.begin(), _windows.end(),
        [number](const auto& w) { return w->get_number() == number; });

    if (it != _windows.end()) {
        if (_focused_window) {
            _focused_window->on_blur();
        }
        _focused_window = it->get();
        _focused_window->on_focus();
    }
}

Element WindowManager::render() {
    return vbox({
        render_top_bar(),
        separator(),
        render_main_layout() | flex,
    });
}

bool WindowManager::handle_event(Event event) {
    // Handle window number keys (1-5)
    if (event == Event::Character('1')) {
        handle_window_key(1);
        return true;
    }
    if (event == Event::Character('2')) {
        handle_window_key(2);
        return true;
    }
    if (event == Event::Character('3')) {
        handle_window_key(3);
        return true;
    }
    if (event == Event::Character('4')) {
        handle_window_key(4);
        return true;
    }
    if (event == Event::Character('5')) {
        handle_window_key(5);
        return true;
    }

    // Pass other events to focused window
    if (_focused_window) {
        return _focused_window->handle_event(event);
    }

    return false;
}

void WindowManager::handle_window_key(int number) {
    auto it = std::find_if(_windows.begin(), _windows.end(),
        [number](const auto& w) { return w->get_number() == number; });

    if (it == _windows.end()) return;

    Window* window = it->get();

    if (_focused_window == window) {
        // Same window - cycle to next tab
        window->next_tab();
    } else {
        // Different window - focus it
        if (_focused_window) {
            _focused_window->on_blur();
        }
        _focused_window = window;
        _focused_window->on_focus();
    }
}

void WindowManager::trigger_refresh() {
    _is_refreshing = true;
    // TODO: In Slice 02, this will call AppState::refresh_all()
    // For now, simulate refresh work
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    _is_refreshing = false;
}

Element WindowManager::render_top_bar() {
    if (_is_refreshing) {
        return hbox({
            text("SlayerGit") | bold,
            text(" | "),
            text("Refreshing...") | color(Color::Yellow),
            filler(),
            text("F5: Refresh | q: Quit") | dim,
        }) | bgcolor(Color::Blue) | color(Color::White);
    }

    return hbox({
        text("SlayerGit") | bold,
        text(" | "),
        text("Branch: main"),
        text(" | "),
        text("Status: Clean"),
        filler(),
        text("F5: Refresh | q: Quit") | dim,
    }) | bgcolor(Color::Blue) | color(Color::White);
}

Element WindowManager::render_main_layout() {
    // Get window pointers (assuming 5 windows: 0-4)
    auto window1 = _windows.size() > 0 ? _windows[0]->render() : text("Window 1");
    auto window2 = _windows.size() > 1 ? _windows[1]->render() : text("Window 2");
    auto window3 = _windows.size() > 2 ? _windows[2]->render() : text("Window 3");
    auto window4 = _windows.size() > 3 ? _windows[3]->render() : text("Window 4");
    auto window5 = _windows.size() > 4 ? _windows[4]->render() : text("Window 5");

    // Left column: windows 1-4 stacked vertically
    auto left_column = vbox({
        window1 | flex_grow,
        separator(),
        window2 | flex_grow,
        separator(),
        window3 | flex_grow,
        separator(),
        window4 | flex_grow,
    });

    // Right column: window 5 (details)
    auto right_column = window5;

    // Horizontal split: 40% left, 60% right
    return hbox({
        left_column | size(WIDTH, EQUAL, 40),
        separator(),
        right_column | flex,
    });
}

} // namespace slayergit::ui

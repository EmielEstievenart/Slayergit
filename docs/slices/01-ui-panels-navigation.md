# Vertical Slice 01: UI Panels and Navigation

**Status:** 🔵 Ready for Implementation
**Priority:** P0 - Foundation
**Estimated Effort:** 3-5 days

---

## Goal

Create the complete UI layout with all numbered windows and tabs. The UI should be navigable using number keys, but **no Git integration required**. All panels show placeholder/mock data.

**What You'll Build:**
- 5 numbered windows with focus management
- Tab cycling within windows (press same number repeatedly)
- Proper FTXUI layout matching the kickoff design
- Keyboard navigation (1-5 keys)
- Visual feedback showing which window/tab is active

**What You Won't Build:**
- No Git operations
- No real data loading
- No AppState or observers (just hardcoded placeholder data)
- No async operations

---

## Success Criteria

✅ Pressing `1` focuses Window 1 (Status)
✅ Pressing `1` again cycles between "Unstaged" and "Staged" tabs
✅ Pressing `2` focuses Window 2 (Branches)
✅ Pressing `2` again cycles between "Local", "Remote", and "Tags" tabs
✅ Pressing `3` focuses Window 3 (History)
✅ Pressing `3` again cycles between "Commits" and "Reflog" tabs
✅ Pressing `4` focuses Window 4 (Stashes) - single tab
✅ Pressing `5` focuses Window 5 (Details) - right panel
✅ Pressing `F5` shows "Refreshing..." message (placeholder for future Git integration)
✅ Pressing `q` quits the application
✅ Active window and tab are visually highlighted
✅ Layout matches the kickoff.md design

---

## Architecture Components Needed

### Minimal Component Set

```
src/ui/
├── main.cpp                    # Entry point
├── window_manager.cpp/.hpp     # Manages windows and focus
├── window.cpp/.hpp             # Window with tabs
├── tab.hpp                     # Tab interface
└── tabs/
    ├── unstaged_tab.cpp/.hpp   # Placeholder tab implementations
    ├── staged_tab.cpp/.hpp
    ├── local_branches_tab.cpp/.hpp
    ├── remote_branches_tab.cpp/.hpp
    ├── tags_tab.cpp/.hpp
    ├── commits_tab.cpp/.hpp
    ├── reflog_tab.cpp/.hpp
    ├── stashes_view.cpp/.hpp
    └── details_view.cpp/.hpp
```

**Note:** Example skeleton code is available in `examples/basic-skeleton/`.

### What's NOT Needed Yet
- ❌ Infrastructure layer (Git CLI wrapper)
- ❌ Core layer (GitRepository, domain models)
- ❌ Application layer (AppState, Commands)
- ❌ Any actual Git operations

---

## Implementation Steps

### Step 1: Setup CMake (1 hour)

Create a minimal CMakeLists.txt that builds just the UI:

```cmake
cmake_minimum_required(VERSION 3.20)
project(SlayerGit VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add FTXUI submodule
add_subdirectory(submodules/FTXUI)

# UI library (just this for now)
add_library(slayergit_ui STATIC
    src/ui/window_manager.cpp
    src/ui/window.cpp
    src/ui/tabs/unstaged_tab.cpp
    src/ui/tabs/staged_tab.cpp
    src/ui/tabs/local_branches_tab.cpp
    src/ui/tabs/remote_branches_tab.cpp
    src/ui/tabs/tags_tab.cpp
    src/ui/tabs/commits_tab.cpp
    src/ui/tabs/reflog_tab.cpp
    src/ui/tabs/stashes_view.cpp
    src/ui/tabs/details_view.cpp
)

target_link_libraries(slayergit_ui PUBLIC
    ftxui::screen
    ftxui::dom
    ftxui::component
)

target_include_directories(slayergit_ui PUBLIC src/ui)

# Main executable
add_executable(slayergit src/ui/main.cpp)
target_link_libraries(slayergit PRIVATE slayergit_ui)
```

### Step 2: Implement Tab Interface (30 minutes)

Create the base `Tab` interface that all tabs implement:

```cpp
// tab.hpp
#pragma once
#include <string>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

namespace slayergit::ui {

class Tab {
public:
    virtual ~Tab() = default;

    // Tab name shown in header
    virtual std::string name() const = 0;

    // Render the tab content
    virtual ftxui::Element render() = 0;

    // Handle keyboard events (optional)
    virtual bool handle_event(ftxui::Event event) { return false; }

    // Called when tab gains focus
    virtual void on_focus() {}

    // Called when tab loses focus
    virtual void on_blur() {}
};

} // namespace slayergit::ui
```

### Step 3: Implement Window Class (2 hours)

The `Window` class contains multiple tabs and handles cycling:

**Key Behaviors:**
- Stores vector of tabs
- Tracks current tab index
- `next_tab()` cycles to next tab (wraps around)
- Renders current tab with tab indicator
- Forwards events to current tab

**See:** `examples/basic-skeleton/src/ui/window.cpp` for implementation.

### Step 4: Implement WindowManager Class (3 hours)

The `WindowManager` orchestrates all windows:

**Key Behaviors:**
- Manages 5 windows
- Tracks which window has focus
- Handles number key presses (1-5)
- If pressed number == focused window → cycle tabs
- If pressed number != focused window → switch focus
- Renders the layout (left column + right details panel)

**See:** `examples/basic-skeleton/src/ui/window_manager.cpp` for implementation.

### Step 5: Implement Placeholder Tabs (4 hours)

Create each tab with hardcoded placeholder data:

**Example: UnstagedTab**
```cpp
class UnstagedTab : public Tab {
public:
    std::string name() const override { return "Unstaged"; }

    ftxui::Element render() override {
        using namespace ftxui;
        return vbox({
            text("Unstaged Changes") | bold,
            separator(),
            text(" M  src/main.cpp"),
            text(" M  src/ui.cpp"),
            text(" ?? README.md"),
        }) | border;
    }
};
```

**Repeat for all 9 tabs** with appropriate placeholder content:
- Status tabs: Show fake file changes
- Branch tabs: Show fake branch names
- Commits/Reflog: Show fake commit hashes
- Stashes: Show fake stash entries
- Details: Show fake diff content

### Step 6: Implement Layout Rendering (2 hours)

The layout should match kickoff.md:

```
┌─────────────────────────────────────────────┐
│ Top Bar: SlayerGit | Branch: main          │
├──────────────────┬──────────────────────────┤
│ 1: Status        │                          │
├──────────────────┤                          │
│ 2: Branches      │                          │
├──────────────────┤   5: Details             │
│ 3: History       │   (Right Panel)          │
├──────────────────┤                          │
│ 4: Stashes       │                          │
└──────────────────┴──────────────────────────┘
```

**FTXUI Layout Code:**
```cpp
auto left_column = vbox({
    windows[0]->render() | flex_grow,  // Status
    separator(),
    windows[1]->render() | flex_grow,  // Branches
    separator(),
    windows[2]->render() | flex_grow,  // History
    separator(),
    windows[3]->render() | flex_grow,  // Stashes
});

auto right_panel = windows[4]->render();  // Details

auto main_layout = hbox({
    left_column | size(WIDTH, EQUAL, 40),
    separator(),
    right_panel | flex,
});
```

### Step 7: Main Loop and Event Handling (1 hour)

Wire everything together in `main.cpp`:

```cpp
int main() {
    using namespace ftxui;

    auto screen = ScreenInteractive::Fullscreen();
    auto window_manager = std::make_unique<WindowManager>();

    auto component = Renderer([&] {
        return window_manager->render();
    });

    component = CatchEvent(component, [&](Event event) {
        // Quit
        if (event == Event::Character('q')) {
            screen.ExitLoopClosure()();
            return true;
        }

        // Refresh (F5) - placeholder for future Git integration
        if (event == Event::F5) {
            window_manager->trigger_refresh();
            return true;
        }

        return window_manager->handle_event(event);
    });

    screen.Loop(component);
    return 0;
}
```

### Step 8: Implement Refresh Indicator (30 minutes)

Add F5 refresh functionality to WindowManager:

**In WindowManager class:**
```cpp
class WindowManager {
public:
    // ... existing methods

    void trigger_refresh() {
        _is_refreshing = true;
        // TODO: In future slice, this will call AppState::refresh_all()
        // For now, just toggle the flag to show UI feedback
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Simulate work
        _is_refreshing = false;
    }

private:
    bool _is_refreshing = false;
    // ... rest of members
};
```

**Update render_top_bar() to show refresh status:**
```cpp
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
        text(" | Branch: main | Status: Clean"),
        filler(),
        text("F5: Refresh | q: Quit") | dim,
    }) | bgcolor(Color::Blue) | color(Color::White);
}
```

**Note:** This is a placeholder implementation. In **Slice 02** (Git Status Integration), this will actually call `AppState::refresh_all()` to fetch real Git data.

### Step 9: Visual Feedback (1 hour)

Add highlighting for active window and tab:

**Active Window:**
- Different border color (use `color(Color::Blue)`)
- Or bold border
- Or title background

**Active Tab:**
- Show tab indicator: `[Unstaged | Staged]` with active one highlighted
- Or underline the active tab name

---

## Testing Your Implementation

### Manual Testing Checklist

1. **Window Focus**
   - [ ] Launch app, Window 1 (Status) should be focused by default
   - [ ] Press `2`, Window 2 (Branches) should gain focus
   - [ ] Press `3`, Window 3 (History) should gain focus
   - [ ] Press `4`, Window 4 (Stashes) should gain focus
   - [ ] Press `5`, Window 5 (Details) should gain focus

2. **Tab Cycling**
   - [ ] Focus Window 1, press `1` again → cycle to "Staged"
   - [ ] Press `1` again → cycle back to "Unstaged"
   - [ ] Focus Window 2, press `2` repeatedly → cycle through Local/Remote/Tags
   - [ ] Focus Window 3, press `3` repeatedly → cycle between Commits/Reflog

3. **Layout**
   - [ ] Left column shows 4 windows stacked vertically
   - [ ] Right panel shows Details window
   - [ ] Top bar shows repository info
   - [ ] All borders and separators are visible

4. **Visual Feedback**
   - [ ] Active window has different appearance (color/bold)
   - [ ] Current tab name is highlighted in window header
   - [ ] Tab indicators show which tab is active

5. **Refresh (F5)**
   - [ ] Press `F5` → top bar shows "Refreshing..." in yellow
   - [ ] After ~500ms, message disappears
   - [ ] Top bar shows "F5: Refresh | q: Quit" hint

6. **Quitting**
   - [ ] Press `q` → app should exit cleanly

### Build and Run

```bash
# Configure
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build build

# Run
./build/slayergit
```

**Expected Result:**
A full-screen TUI with 5 windows, navigable with number keys, showing placeholder data.

---

## Common Pitfalls

### Pitfall 1: Tab Index Out of Bounds
**Problem:** Pressing number key crashes when cycling tabs
**Solution:** Always check `if (index >= tabs.size()) index = 0;`

### Pitfall 2: Focus Not Updating Visually
**Problem:** Pressing numbers switches focus but no visual change
**Solution:** Make sure `on_focus()` and `on_blur()` update some state that affects rendering

### Pitfall 3: Layout Not Splitting Correctly
**Problem:** Windows don't divide space evenly
**Solution:** Use `| flex_grow` on elements that should expand, and `size(WIDTH, EQUAL, N)` for fixed sizes

### Pitfall 4: Events Not Being Captured
**Problem:** Number keys don't do anything
**Solution:** Make sure `handle_event()` returns `true` when it handles an event, otherwise it passes through

---

## Example Code Reference

Full example implementations available in:
```
examples/basic-skeleton/
├── src/ui/
│   ├── main.cpp                    # Main loop with window manager
│   ├── window_manager.cpp/.hpp     # Focus management, event handling
│   ├── window.cpp/.hpp             # Tab container and cycling
│   └── tabs/
│       ├── unstaged_tab.cpp/.hpp   # All placeholder tab implementations
│       └── ...
```

**To use the examples:**
1. Copy files from `examples/basic-skeleton/src/` to your `src/` directory
2. Follow the CMakeLists.txt setup above
3. Build and run
4. Modify to fit your style

---

## Next Steps After Completing This Slice

Once you can navigate all panels and tabs:
1. ✅ You have a working UI foundation
2. ➡️ **Next Slice:** Add Git status integration (Slice 02)
3. Start building the Infrastructure layer (Git CLI wrapper)
4. Add AppState to hold real data instead of placeholders

---

## Questions?

**Q: Do I need to implement the Application or Core layers?**
A: No! This slice is UI-only. Hardcode everything.

**Q: Should tabs do anything when focused?**
A: Not yet. Just render placeholder content. We'll add interaction later.

**Q: What if I want to use different colors/styling?**
A: Go for it! The UX pattern (numbered windows/tabs) is what matters.

**Q: Can I use different key bindings?**
A: For this slice, stick to 1-5 for windows and `q` to quit. We'll make it configurable later.

**Q: Do I need to handle window resizing?**
A: FTXUI handles this automatically with `flex` and `flex_grow`. Just make sure your layout uses them.

---

## Success Checklist

Before marking this slice complete:

- [ ] All 5 windows render with placeholder content
- [ ] Pressing 1-5 switches window focus
- [ ] Pressing same number cycles tabs within that window
- [ ] Active window is visually distinct
- [ ] Active tab is shown in window header
- [ ] Layout matches kickoff.md design (left column + right panel)
- [ ] Pressing `q` quits cleanly
- [ ] Code compiles without warnings
- [ ] Example code is available in `examples/basic-skeleton/`

**Once all checked:** You're ready for Slice 02! 🎉

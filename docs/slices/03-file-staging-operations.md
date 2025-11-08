# Vertical Slice 03: File Staging Operations

**Status:** 🔵 Ready for Implementation  
**Priority:** P0 - Core Functionality  
**Estimated Effort:** 1-2 days  
**Prerequisites:** Slice 02 (Git Status Integration)

---

## User Value Statement

As a user, I want to stage and unstage files using keyboard shortcuts so that I can prepare commits without leaving the TUI.

---

## Prerequisites

- ✅ Slice 02 completed: Git status integration working
- `AppState` and observer pattern in place
- `GitRepository` can execute Git commands

---

## Scope - Frontend

**Update Tabs:**
- `UnstagedTab` - Add selection navigation (↑/↓ arrows), stage file on `s` key
- `StagedTab` - Add selection navigation (↑/↓ arrows), unstage file on `u` key
- Both tabs highlight selected file
- Both tabs show keybinding hints in footer

**Event Handling:**
- `s` key → stage selected file (if in UnstagedTab)
- `u` key → unstage selected file (if in StagedTab)
- `Shift+S` → stage all files
- `Shift+U` → unstage all files

---

## Scope - Backend (Core Layer)

**Extend GitRepository:**
```cpp
class GitRepository {
public:
    // ... existing methods

    void stage_file(const std::string& file_path);
    void unstage_file(const std::string& file_path);
    void stage_all();
    void unstage_all();
};
```

**Git Commands:**
- `git add <file>` - Stage file
- `git reset HEAD <file>` - Unstage file
- `git add -A` - Stage all
- `git reset HEAD` - Unstage all

---

## Scope - Application Layer

**New Files:**
- `src/lib/app/command.hpp` - Command interface
- `src/lib/app/command_dispatcher.cpp/.hpp` - Execute commands
- `src/lib/app/commands/stage_file_command.cpp/.hpp`
- `src/lib/app/commands/unstage_file_command.cpp/.hpp`

**Command Pattern:**
```cpp
class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0; // For future undo feature
};

class StageFileCommand : public Command {
public:
    StageFileCommand(
        std::shared_ptr<core::GitRepository> repo,
        AppState* app_state,
        const std::string& file_path
    );

    void execute() override;
    void undo() override;

private:
    std::shared_ptr<core::GitRepository> _repo;
    AppState* _app_state;
    std::string _file_path;
};
```

**CommandDispatcher:**
```cpp
class CommandDispatcher {
public:
    void execute(std::unique_ptr<Command> command);
    void execute_async(std::unique_ptr<Command> command);

private:
    std::vector<std::unique_ptr<Command>> _history; // For future undo
};
```

---

## Acceptance Criteria

**Functional:**
- [ ] Select file in Unstaged tab using ↑/↓ arrows
- [ ] Press `s` → file is staged → file moves from Unstaged to Staged tab
- [ ] Select file in Staged tab, press `u` → file is unstaged → file moves back to Unstaged
- [ ] Press `Shift+S` in Unstaged tab → all files are staged
- [ ] Press `Shift+U` in Staged tab → all files are unstaged
- [ ] Status automatically refreshes after stage/unstage operations
- [ ] Selected file is visually highlighted (different background color)
- [ ] Footer shows available keybindings: `[s] Stage | [u] Unstage | [S] Stage All | [U] Unstage All`

**Quality:**
- [ ] Stage/unstage operations complete in < 100ms for single file
- [ ] UI remains responsive during Git operations
- [ ] Error handling: staging non-existent file → show error message
- [ ] No flickering when refreshing status after operation

---

## Implementation Approach

### Tracer Bullet Phase

**Goal:** Get basic stage/unstage working for single file.

**Steps:**
1. Add simple `stage_file()` and `unstage_file()` to `GitRepository`
2. Create minimal `StageFileCommand` that directly calls `GitRepository`
3. Add selection tracking (index) to `UnstagedTab` and `StagedTab`
4. Handle `s` key in `UnstagedTab`: create command, execute, refresh status
5. Handle `u` key in `StagedTab`: create command, execute, refresh status

**Success:** Can stage and unstage one file at a time.

### Completion Phase

**Steps:**
1. Implement full Command pattern with `CommandDispatcher`
2. Add `stage_all()` and `unstage_all()` methods
3. Create `StageAllCommand` and `UnstageAllCommand`
4. Add visual selection highlighting (background color change)
5. Add footer with keybinding hints
6. Implement targeted refresh (refresh only status, not all data)
7. Add error handling for git command failures
8. Add async execution for commands (optional, if operations are slow)

**Success:** All acceptance criteria met.

---

## Testing Scenarios

### Happy Path
1. Open repo with 3 unstaged files
2. Press ↓ to select second file
3. Press `s` → file is staged and moves to Staged tab
4. Switch to Staged tab, select the file
5. Press `u` → file is unstaged and moves back to Unstaged tab

### Stage All / Unstage All
1. Unstaged tab has 5 files
2. Press `Shift+S` → all 5 files move to Staged tab
3. Press `Shift+U` → all 5 files move back to Unstaged tab

### Error Cases
1. Try to stage a file that was deleted externally → error message shown
2. Git command fails (repo corrupted) → error message shown, app doesn't crash

### Edge Cases
1. Empty Unstaged tab → arrow keys do nothing, `s` key does nothing
2. Stage last file in Unstaged tab → selection moves to previous file (if any)
3. Rapid key presses (spam `s` key) → operations queue properly, no race conditions

---

## Integration Points

### Consumes From:
- Slice 02: Uses `GitRepository`, `AppState`, observer pattern

### Provides For:
- Slice 04 (Commit): Requires files to be staged before committing
- Future slices: Command pattern foundation for all user actions
- Future undo feature: Command history enables undo/redo

---

## Files to Create

**Application Layer:**
```
src/lib/app/
├── command.hpp
├── command_dispatcher.cpp
├── command_dispatcher.hpp
└── commands/
    ├── stage_file_command.cpp
    ├── stage_file_command.hpp
    ├── unstage_file_command.cpp
    └── unstage_file_command.hpp
```

---

## Files to Modify

**Core Layer:**
```
src/lib/core/git_repository.cpp/.hpp    # Add stage/unstage methods
```

**UI Layer:**
```
src/ui/tabs/unstaged_tab.cpp/.hpp       # Add selection, handle 's' key
src/ui/tabs/staged_tab.cpp/.hpp         # Add selection, handle 'u' key
src/ui/main.cpp                         # Create CommandDispatcher, pass to tabs
```

**Build System:**
```
src/lib/app/CMakeLists.txt              # Add command files
```

---

## Notes for Implementation Agent

### Selection Implementation in Tabs

```cpp
class UnstagedTab : public Tab, public AppState::Observer {
public:
    UnstagedTab(AppState* app_state, CommandDispatcher* dispatcher)
        : _app_state(app_state), _dispatcher(dispatcher), _selected_index(0) {
        _app_state->add_observer(this);
    }

    bool handle_event(ftxui::Event event) override {
        if (event == ftxui::Event::ArrowDown) {
            if (_selected_index < _files.size() - 1) {
                _selected_index++;
            }
            return true;
        }

        if (event == ftxui::Event::ArrowUp) {
            if (_selected_index > 0) {
                _selected_index--;
            }
            return true;
        }

        if (event == ftxui::Event::Character('s') && !_files.empty()) {
            auto cmd = std::make_unique<StageFileCommand>(
                _app_state->get_repo(),
                _app_state,
                _files[_selected_index].path
            );
            _dispatcher->execute(std::move(cmd));
            return true;
        }

        return false;
    }

    ftxui::Element render() override {
        using namespace ftxui;
        
        std::vector<ftxui::Element> lines;
        for (size_t i = 0; i < _files.size(); ++i) {
            auto line = text(_files[i].path);
            if (i == _selected_index) {
                line = line | bgcolor(Color::Blue) | color(Color::White);
            }
            lines.push_back(line);
        }

        return vbox({
            text("Unstaged Changes") | bold,
            separator(),
            vbox(lines),
            filler(),
            text("[s] Stage | [S] Stage All") | dim
        }) | border;
    }

private:
    AppState* _app_state;
    CommandDispatcher* _dispatcher;
    std::vector<core::FileStatus> _files;
    size_t _selected_index;
};
```

### Command Implementation Pattern

```cpp
// stage_file_command.cpp
StageFileCommand::StageFileCommand(
    std::shared_ptr<core::GitRepository> repo,
    AppState* app_state,
    const std::string& file_path
) : _repo(repo), _app_state(app_state), _file_path(file_path) {}

void StageFileCommand::execute() {
    try {
        _repo->stage_file(_file_path);
        _app_state->refresh_status(); // Targeted refresh
    } catch (const std::exception& e) {
        _app_state->notify_error("Failed to stage file: " + std::string(e.what()));
    }
}

void StageFileCommand::undo() {
    // For future undo feature
    _repo->unstage_file(_file_path);
    _app_state->refresh_status();
}
```

### Git Commands in GitRepository

```cpp
void GitRepository::stage_file(const std::string& file_path) {
    auto result = _executor->execute({"add", file_path});
    if (result.exit_code != 0) {
        throw GitCommandException("git add", result.exit_code, result.stderr_output);
    }
}

void GitRepository::unstage_file(const std::string& file_path) {
    auto result = _executor->execute({"reset", "HEAD", file_path});
    if (result.exit_code != 0) {
        throw GitCommandException("git reset", result.exit_code, result.stderr_output);
    }
}

void GitRepository::stage_all() {
    auto result = _executor->execute({"add", "-A"});
    if (result.exit_code != 0) {
        throw GitCommandException("git add -A", result.exit_code, result.stderr_output);
    }
}

void GitRepository::unstage_all() {
    auto result = _executor->execute({"reset", "HEAD"});
    if (result.exit_code != 0) {
        throw GitCommandException("git reset HEAD", result.exit_code, result.stderr_output);
    }
}
```

### Targeted Refresh

Instead of `refresh_all()`, use targeted refresh to avoid unnecessary Git commands:

```cpp
// In AppState
void AppState::refresh_status() {
    auto future = _repo->get_status_async();
    // ... handle async result
    notify_status_changed();
    // Don't refresh branches, commits, etc.
}
```

### CommandDispatcher Implementation

```cpp
class CommandDispatcher {
public:
    void execute(std::unique_ptr<Command> command) {
        command->execute();
        _history.push_back(std::move(command)); // For future undo
    }

    void execute_async(std::unique_ptr<Command> command) {
        std::async(std::launch::async, [cmd = std::move(command)]() mutable {
            cmd->execute();
        });
        // Note: Can't store in history for async (ownership moved)
    }

    // Future: undo/redo methods
    void undo() {
        if (!_history.empty()) {
            _history.back()->undo();
            _history.pop_back();
        }
    }

private:
    std::vector<std::unique_ptr<Command>> _history;
};
```

---

## Common Pitfalls

**Pitfall 1: Selection Out of Bounds**
- **Problem:** Staging last file, selection index becomes invalid
- **Solution:** After refresh, clamp selection: `_selected_index = std::min(_selected_index, _files.size() - 1)`

**Pitfall 2: Race Condition on Rapid Key Presses**
- **Problem:** User presses `s` 5 times rapidly, multiple stage commands execute on same file
- **Solution:** Disable key handling while command is executing, or use command queue

**Pitfall 3: Status Not Refreshing**
- **Problem:** File is staged but still shows in Unstaged tab
- **Solution:** Make sure `refresh_status()` is called after command execution and observers are notified

**Pitfall 4: Observers Not Updating Selection**
- **Problem:** After refresh, selected item changes unexpectedly
- **Solution:** Store selected file path, not index; after refresh, find matching path and update index

---

## Success Checklist

Before marking this slice complete:

- [ ] Can stage individual files with `s` key
- [ ] Can unstage individual files with `u` key
- [ ] Can stage all files with `Shift+S`
- [ ] Can unstage all files with `Shift+U`
- [ ] Selection is visible (highlighted row)
- [ ] Arrow keys navigate selection
- [ ] Status refreshes automatically after operations
- [ ] Error handling works (show error message on failure)
- [ ] No crashes on edge cases (empty list, rapid key presses)
- [ ] Code follows Command pattern
- [ ] Footer shows keybinding hints

**Once all checked:** You're ready for Slice 04 (Commit Creation)! 🎉

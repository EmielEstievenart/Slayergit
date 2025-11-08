# Vertical Slice 04: Commit Creation

**Status:** 🔵 Ready for Implementation  
**Priority:** P0 - Core Functionality  
**Estimated Effort:** 1-2 days  
**Prerequisites:** Slice 03 (File Staging Operations)

---

## User Value Statement

As a user, I want to commit staged files with a commit message so that I can save my changes to the repository history.

---

## Prerequisites

- ✅ Slice 03 completed: File staging working
- Command pattern in place
- `GitRepository` can execute Git commands

---

## Scope - Frontend

**New Components:**
- `InputDialog` - Reusable text input dialog for commit messages
- Multi-line text editor for commit message

**Update Tabs:**
- `StagedTab` - Add `c` key handler to open commit dialog

**Commit Flow:**
1. User presses `c` in Staged tab (when files are staged)
2. Input dialog appears overlaying main UI
3. User types commit message (multi-line supported)
4. User presses `Ctrl+Enter` to confirm or `Esc` to cancel
5. Commit executes → Status refreshes → Staged tab empties

---

## Scope - Backend (Core Layer)

**Extend GitRepository:**
```cpp
class GitRepository {
public:
    // ... existing methods

    void commit(const std::string& message);
    void amend_commit(const std::string& message); // For future use
};
```

**Git Commands:**
- `git commit -m "message"` - Create commit with message
- `git commit --amend -m "message"` - Amend last commit (future feature)

---

## Scope - Application Layer

**New Files:**
- `src/lib/app/commands/commit_command.cpp/.hpp`

**CommitCommand:**
```cpp
class CommitCommand : public Command {
public:
    CommitCommand(
        std::shared_ptr<core::GitRepository> repo,
        AppState* app_state,
        const std::string& message
    );

    void execute() override;
    void undo() override; // Future: soft reset

private:
    std::shared_ptr<core::GitRepository> _repo;
    AppState* _app_state;
    std::string _message;
    std::string _commit_hash; // Store for undo
};
```

---

## Scope - UI Components

**New Files:**
- `src/ui/components/input_dialog.cpp/.hpp` - Reusable text input overlay

**InputDialog:**
```cpp
class InputDialog {
public:
    InputDialog(const std::string& title, const std::string& placeholder);
    
    ftxui::Component component();
    bool is_open() const;
    void show();
    void hide();
    std::string get_input() const;
    
private:
    std::string _title;
    std::string _input;
    bool _is_open;
};
```

---

## Acceptance Criteria

**Functional:**
- [ ] Press `c` in Staged tab (with staged files) → commit dialog opens
- [ ] Press `c` in Staged tab (no staged files) → error message "No files staged"
- [ ] Type commit message in dialog (multi-line supported)
- [ ] Press `Ctrl+Enter` → commit is created → dialog closes → status refreshes
- [ ] Press `Esc` → dialog closes without committing
- [ ] After successful commit, Staged tab is empty
- [ ] Commit appears in History tab (Window 3)
- [ ] Empty commit message → show error "Commit message required"
- [ ] Very long commit message (> 1000 chars) → accepted (git allows it)

**Quality:**
- [ ] Commit operation completes in < 200ms
- [ ] Dialog is centered on screen
- [ ] Dialog is keyboard-navigable (no mouse required)
- [ ] Error messages are clear and actionable
- [ ] No crashes on git commit failures

---

## Implementation Approach

### Tracer Bullet Phase

**Goal:** Get basic commit working with single-line message.

**Steps:**
1. Add simple `commit()` method to `GitRepository`
2. Create basic `CommitCommand`
3. Add simple `InputDialog` with single-line input
4. Handle `c` key in `StagedTab`: show dialog, execute commit
5. Refresh status after commit

**Success:** Can commit with a simple message.

### Completion Phase

**Steps:**
1. Upgrade `InputDialog` to support multi-line input
2. Add `Ctrl+Enter` to submit, `Esc` to cancel
3. Add validation: empty message → error
4. Add validation: no staged files → error
5. Add commit message preview in dialog
6. Add async commit execution (optional)
7. Add success notification in status bar
8. Add error handling for git commit failures

**Success:** All acceptance criteria met.

---

## Testing Scenarios

### Happy Path
1. Stage 2 files
2. Press `c` in Staged tab → dialog opens
3. Type "Add new feature"
4. Press `Ctrl+Enter` → commit created
5. Staged tab is now empty
6. History tab shows new commit

### Multi-Line Commit Message
1. Stage files
2. Press `c`
3. Type first line: "Add authentication"
4. Press `Enter` for new line
5. Type second line: "- Add login form"
6. Type third line: "- Add password validation"
7. Press `Ctrl+Enter` → commit with multi-line message

### Error Cases
1. Press `c` with no staged files → error "No files staged"
2. Open dialog, press `Ctrl+Enter` with empty message → error "Message required"
3. Git commit fails (e.g., pre-commit hook rejection) → error message shown

### Edge Cases
1. Commit message with special characters (quotes, newlines) → handled correctly
2. Very long commit message (> 1000 chars) → accepted
3. Rapid `c` key presses → dialog doesn't open multiple times

---

## Integration Points

### Consumes From:
- Slice 03: Uses Command pattern, CommandDispatcher
- Slice 02: Uses AppState and refresh mechanism

### Provides For:
- Slice 06 (Commit History): Commits appear in history after creation
- Future slices: InputDialog is reusable for branch names, tag messages, etc.

---

## Files to Create

**Application Layer:**
```
src/lib/app/commands/
├── commit_command.cpp
└── commit_command.hpp
```

**UI Components:**
```
src/ui/components/
├── input_dialog.cpp
└── input_dialog.hpp
```

---

## Files to Modify

**Core Layer:**
```
src/lib/core/git_repository.cpp/.hpp    # Add commit() method
```

**UI Layer:**
```
src/ui/tabs/staged_tab.cpp/.hpp         # Handle 'c' key, show dialog
src/ui/main.cpp                         # Integrate InputDialog into component tree
```

**Build System:**
```
src/lib/app/CMakeLists.txt              # Add commit_command files
src/ui/CMakeLists.txt                   # Add input_dialog files
```

---

## Notes for Implementation Agent

### InputDialog Implementation (FTXUI)

```cpp
class InputDialog {
public:
    InputDialog(const std::string& title, const std::string& placeholder)
        : _title(title), _placeholder(placeholder), _is_open(false) {
        
        _input_component = ftxui::Input(&_input, _placeholder);
    }

    ftxui::Component component() {
        return ftxui::Renderer(_input_component, [this] {
            if (!_is_open) {
                return ftxui::emptyElement();
            }

            using namespace ftxui;
            return vbox({
                text(_title) | bold | center,
                separator(),
                _input_component->Render(),
                separator(),
                text("Ctrl+Enter: Confirm | Esc: Cancel") | dim | center
            }) | border | center | size(WIDTH, EQUAL, 60) | size(HEIGHT, EQUAL, 10);
        });
    }

    bool handle_event(ftxui::Event event) {
        if (!_is_open) return false;

        if (event == ftxui::Event::Escape) {
            hide();
            return true;
        }

        if (event.is_character() && event.character() == "\n" && event.control()) {
            // Ctrl+Enter
            _confirmed = true;
            hide();
            return true;
        }

        return _input_component->OnEvent(event);
    }

    void show() { _is_open = true; _confirmed = false; _input.clear(); }
    void hide() { _is_open = false; }
    bool is_confirmed() const { return _confirmed; }
    std::string get_input() const { return _input; }

private:
    std::string _title;
    std::string _placeholder;
    std::string _input;
    bool _is_open;
    bool _confirmed;
    ftxui::Component _input_component;
};
```

### Multi-Line Input Dialog

For multi-line support, use `ftxui::Input` with multiline option:

```cpp
_input_component = ftxui::Input(&_input, _placeholder) | ftxui::size(HEIGHT, EQUAL, 5);
```

Or manually handle Enter key to add newlines:

```cpp
if (event == ftxui::Event::Return) {
    _input += "\n";
    return true;
}
```

### StagedTab Integration

```cpp
class StagedTab : public Tab, public AppState::Observer {
public:
    StagedTab(AppState* app_state, CommandDispatcher* dispatcher)
        : _app_state(app_state), 
          _dispatcher(dispatcher),
          _commit_dialog("Commit Message", "Enter commit message...") {}

    bool handle_event(ftxui::Event event) override {
        // Handle commit dialog events first
        if (_commit_dialog.is_open()) {
            if (_commit_dialog.handle_event(event)) {
                if (_commit_dialog.is_confirmed()) {
                    std::string message = _commit_dialog.get_input();
                    if (message.empty()) {
                        _app_state->notify_error("Commit message required");
                    } else {
                        auto cmd = std::make_unique<CommitCommand>(
                            _app_state->get_repo(),
                            _app_state,
                            message
                        );
                        _dispatcher->execute(std::move(cmd));
                    }
                }
                return true;
            }
        }

        // Handle 'c' key to open commit dialog
        if (event == ftxui::Event::Character('c')) {
            if (_files.empty()) {
                _app_state->notify_error("No files staged");
            } else {
                _commit_dialog.show();
            }
            return true;
        }

        // ... other event handling (arrow keys, etc.)
        return false;
    }

    ftxui::Element render() override {
        using namespace ftxui;
        
        auto main_content = vbox({
            text("Staged Changes") | bold,
            separator(),
            render_file_list(),
            filler(),
            text("[c] Commit | [u] Unstage") | dim
        }) | border;

        // Overlay commit dialog if open
        if (_commit_dialog.is_open()) {
            return dbox({
                main_content | dim,  // Dim background
                _commit_dialog.component()->Render()
            });
        }

        return main_content;
    }

private:
    AppState* _app_state;
    CommandDispatcher* _dispatcher;
    InputDialog _commit_dialog;
    std::vector<core::FileStatus> _files;
};
```

### Git Commit Implementation

```cpp
void GitRepository::commit(const std::string& message) {
    // Escape message for shell (or use proper process execution)
    auto result = _executor->execute({"commit", "-m", message});
    
    if (result.exit_code != 0) {
        throw GitCommandException("git commit", result.exit_code, result.stderr_output);
    }
}
```

**Security Note:** Using `-m` flag with message string is safer than piping message to stdin, but be careful with special characters. Proper process execution with argv array is recommended.

### CommitCommand Implementation

```cpp
void CommitCommand::execute() {
    try {
        _repo->commit(_message);
        
        // Extract commit hash from git output (optional, for undo)
        // _commit_hash = parse_commit_hash_from_output();
        
        // Refresh status AND commits
        _app_state->refresh_status();
        _app_state->refresh_commits(); // New method in Slice 06
        
        _app_state->notify_success("Committed successfully");
    } catch (const std::exception& e) {
        _app_state->notify_error("Commit failed: " + std::string(e.what()));
    }
}

void CommitCommand::undo() {
    // Future: implement soft reset
    // _repo->reset_soft("HEAD~1");
}
```

### Success Notification

Add to AppState:

```cpp
class AppState {
public:
    class Observer {
    public:
        virtual void on_error(const std::string& message) = 0;
        virtual void on_success(const std::string& message) = 0; // New
    };

    void notify_success(const std::string& message) {
        for (auto* obs : _observers) {
            obs->on_success(message);
        }
    }
};
```

Display in status bar (top bar of WindowManager).

---

## Common Pitfalls

**Pitfall 1: Special Characters in Commit Message**
- **Problem:** Message with quotes breaks git command
- **Solution:** Use proper argv array passing, not shell string concatenation

**Pitfall 2: Dialog Not Closing**
- **Problem:** Dialog stays open after commit
- **Solution:** Make sure `hide()` is called after confirmation

**Pitfall 3: Event Handling Order**
- **Problem:** `c` key opens dialog AND types 'c' in input field
- **Solution:** Return `true` from event handler to stop propagation

**Pitfall 4: Multi-Line Input Not Working**
- **Problem:** Enter key submits instead of adding newline
- **Solution:** Check for Ctrl+Enter for submit, plain Enter for newline

---

## Success Checklist

Before marking this slice complete:

- [ ] Can open commit dialog with `c` key
- [ ] Can type commit message (multi-line supported)
- [ ] Can submit with `Ctrl+Enter`
- [ ] Can cancel with `Esc`
- [ ] Validation: empty message → error
- [ ] Validation: no staged files → error
- [ ] After commit, staged files are cleared
- [ ] Status bar shows success message
- [ ] Error handling works (git failures)
- [ ] Dialog is visually centered and clear
- [ ] No crashes on edge cases

**Once all checked:** You're ready for Slice 05 (Branch Viewing & Switching)! 🎉

# Vertical Slice 05: Branch Viewing & Switching

**Status:** 🔵 Ready for Implementation  
**Priority:** P1 - Important Functionality  
**Estimated Effort:** 2 days  
**Prerequisites:** Slice 02 (Git Status Integration)

---

## User Value Statement

As a user, I want to view all local and remote branches and switch between them so that I can work on different features or review remote work.

---

## Prerequisites

- ✅ Slice 02 completed: AppState and observer pattern working
- `GitRepository` can execute Git commands
- Command pattern in place

---

## Scope - Frontend

**Update Tabs:**
- `LocalBranchesTab` - Display real local branches, navigate with arrows, checkout with Enter
- `RemoteBranchesTab` - Display real remote branches (read-only for now)
- `TagsTab` - Display real tags (read-only for now)

**Branch Display:**
- Current branch highlighted with `*` indicator
- Tracking info shown (e.g., "origin/main [ahead 2, behind 1]")
- Last commit message preview (optional)

**Checkout Flow:**
1. Navigate to branch in LocalBranchesTab
2. Press `Enter` → checkout branch
3. Status refreshes → current branch updates

---

## Scope - Backend (Infrastructure Layer)

**New Files:**
- `src/lib/infra/parsers/branch_parser.cpp/.hpp`
- `src/lib/infra/parsers/tag_parser.cpp/.hpp`

**Branch Parser:**
```cpp
class BranchParser {
public:
    static std::vector<core::Branch> parse_local(const std::string& output);
    static std::vector<core::Branch> parse_remote(const std::string& output);
};
```

**Tag Parser:**
```cpp
class TagParser {
public:
    static std::vector<core::Tag> parse(const std::string& output);
};
```

---

## Scope - Backend (Core Layer)

**New Domain Models:**
```cpp
enum class BranchType {
    Local,
    Remote
};

struct Branch {
    std::string name;
    BranchType type;
    bool is_current;
    std::string tracking_branch; // e.g., "origin/main"
    int ahead_count = 0;
    int behind_count = 0;
    std::string last_commit_hash;
    std::string last_commit_subject;
};

struct Tag {
    std::string name;
    std::string commit_hash;
    std::string message; // Annotated tag message
};
```

**Extend GitRepository:**
```cpp
class GitRepository {
public:
    // ... existing methods

    std::vector<Branch> get_local_branches();
    std::vector<Branch> get_remote_branches();
    std::vector<Tag> get_tags();
    
    void checkout_branch(const std::string& branch_name);
    void checkout_new_branch(const std::string& branch_name); // Future: Slice 10
};
```

**Git Commands:**
- `git branch -vv --no-color` - List local branches with tracking info
- `git branch -r -vv --no-color` - List remote branches
- `git tag -l --format="%(refname:short)%x00%(objectname)%x00%(subject)"` - List tags
- `git checkout <branch>` - Checkout branch

---

## Scope - Application Layer

**Extend AppState:**
```cpp
class AppState {
public:
    class Observer {
    public:
        virtual void on_branches_changed(
            const std::vector<core::Branch>& local,
            const std::vector<core::Branch>& remote
        ) = 0;
        virtual void on_tags_changed(const std::vector<core::Tag>& tags) = 0;
        // ... existing methods
    };

    const std::vector<core::Branch>& get_local_branches() const;
    const std::vector<core::Branch>& get_remote_branches() const;
    const std::vector<core::Tag>& get_tags() const;

    void refresh_branches();
    void refresh_tags();

private:
    std::vector<core::Branch> _local_branches;
    std::vector<core::Branch> _remote_branches;
    std::vector<core::Tag> _tags;
};
```

**New Command:**
- `src/lib/app/commands/checkout_branch_command.cpp/.hpp`

---

## Acceptance Criteria

**Functional:**
- [ ] Window 2, Local Branches tab shows all local branches
- [ ] Current branch marked with `*` and highlighted
- [ ] Tracking info shown (e.g., "[origin/main ↑2 ↓1]")
- [ ] Navigate branches with ↑/↓ arrows
- [ ] Press `Enter` on branch → checkout → current branch updates
- [ ] Remote Branches tab shows all remote branches (e.g., "origin/develop")
- [ ] Tags tab shows all tags
- [ ] F5 refreshes branch list
- [ ] Checkout updates status tab (shows new branch name)
- [ ] Error handling: checkout fails → error message shown

**Quality:**
- [ ] Branch list loads in < 500ms
- [ ] UI remains responsive during checkout
- [ ] No crashes on checkout failures (e.g., uncommitted changes)
- [ ] Branch names are properly truncated if too long

---

## Implementation Approach

### Tracer Bullet Phase

**Goal:** Display local branches and checkout.

**Steps:**
1. Add `get_local_branches()` to `GitRepository` with basic parsing
2. Add `refresh_branches()` to `AppState`
3. Update `LocalBranchesTab` to display branches from AppState
4. Implement simple checkout command
5. Handle Enter key in LocalBranchesTab

**Success:** Can see local branches and checkout.

### Completion Phase

**Steps:**
1. Implement full BranchParser with tracking info parsing
2. Add remote branch parsing
3. Add tag parsing
4. Update RemoteBranchesTab and TagsTab
5. Add visual indicators (*, ahead/behind counts)
6. Add async branch loading
7. Add error handling for checkout failures
8. Add unit tests for BranchParser

**Success:** All acceptance criteria met.

---

## Testing Scenarios

### Happy Path
1. Open repo with 3 local branches
2. Window 2 shows all branches
3. Current branch marked with `*`
4. Select different branch, press Enter → checkout succeeds
5. Current branch indicator updates

### Remote Branches
1. Repo with remote branches
2. Remote Branches tab shows "origin/main", "origin/develop", etc.
3. Cannot checkout remote branch directly (read-only for now)

### Tags
1. Repo with tags
2. Tags tab shows all tags with commit hashes
3. Navigate tags with arrow keys

### Error Cases
1. Checkout branch with uncommitted changes → git fails → error "Uncommitted changes"
2. Checkout fails (detached HEAD, etc.) → error message shown

### Edge Cases
1. No branches (empty repo) → show "No branches" message
2. Very long branch names → truncated with "..."
3. Detached HEAD state → show commit hash as current "branch"

---

## Integration Points

### Consumes From:
- Slice 02: Uses AppState, observer pattern, GitRepository

### Provides For:
- Slice 10 (Branch Management): Extends with create/delete branch operations
- Slice 11 (Remote Operations): Branch list needed for push/pull operations

---

## Files to Create

**Infrastructure Layer:**
```
src/lib/infra/parsers/
├── branch_parser.cpp
├── branch_parser.hpp
├── tag_parser.cpp
└── tag_parser.hpp
```

**Core Layer:**
```
src/lib/core/models/
├── branch.hpp
└── tag.hpp
```

**Application Layer:**
```
src/lib/app/commands/
├── checkout_branch_command.cpp
└── checkout_branch_command.hpp
```

**Tests:**
```
tests/lib/infra/parsers/
├── branch_parser_test.cpp
└── tag_parser_test.cpp
```

---

## Files to Modify

**Core Layer:**
```
src/lib/core/git_repository.cpp/.hpp    # Add branch/tag methods
```

**Application Layer:**
```
src/lib/app/app_state.cpp/.hpp          # Add branch/tag state
```

**UI Layer:**
```
src/ui/tabs/local_branches_tab.cpp/.hpp     # Display real branches, handle checkout
src/ui/tabs/remote_branches_tab.cpp/.hpp    # Display real remote branches
src/ui/tabs/tags_tab.cpp/.hpp               # Display real tags
```

---

## Notes for Implementation Agent

### Git Branch Output Format

**Local branches:**
```
* main                  abc1234 [origin/main: ahead 2, behind 1] Latest commit message
  feature/new-auth      def5678 Initial auth implementation
  bugfix/crash-fix      789abcd Fix critical bug
```

**Parsing pattern:**
- `*` indicates current branch
- Branch name, commit hash, tracking info, commit message

**Remote branches:**
```
  origin/main           abc1234 Latest commit
  origin/develop        def5678 Development work
```

### BranchParser Implementation

```cpp
std::vector<core::Branch> BranchParser::parse_local(const std::string& output) {
    std::vector<core::Branch> branches;
    std::istringstream stream(output);
    std::string line;

    while (std::getline(stream, line)) {
        if (line.empty()) continue;

        core::Branch branch;
        branch.type = core::BranchType::Local;
        branch.is_current = (line[0] == '*');

        // Remove leading "* " or "  "
        line = line.substr(2);

        // Parse: "name hash [tracking] message"
        std::istringstream line_stream(line);
        line_stream >> branch.name >> branch.last_commit_hash;

        // Parse tracking info if present: [origin/main: ahead 2, behind 1]
        if (line.find('[') != std::string::npos) {
            size_t start = line.find('[');
            size_t end = line.find(']', start);
            std::string tracking = line.substr(start + 1, end - start - 1);
            
            // Extract tracking branch name
            size_t colon = tracking.find(':');
            if (colon != std::string::npos) {
                branch.tracking_branch = tracking.substr(0, colon);
                
                // Parse ahead/behind counts
                std::string counts = tracking.substr(colon + 1);
                if (counts.find("ahead") != std::string::npos) {
                    // Extract number after "ahead"
                    // branch.ahead_count = ...
                }
                if (counts.find("behind") != std::string::npos) {
                    // Extract number after "behind"
                    // branch.behind_count = ...
                }
            }
        }

        branches.push_back(branch);
    }

    return branches;
}
```

### LocalBranchesTab Implementation

```cpp
class LocalBranchesTab : public Tab, public AppState::Observer {
public:
    LocalBranchesTab(AppState* app_state, CommandDispatcher* dispatcher)
        : _app_state(app_state), _dispatcher(dispatcher), _selected_index(0) {
        _app_state->add_observer(this);
    }

    void on_branches_changed(
        const std::vector<core::Branch>& local,
        const std::vector<core::Branch>& remote
    ) override {
        _branches = local;
        // Clamp selection
        if (_selected_index >= _branches.size()) {
            _selected_index = _branches.size() > 0 ? _branches.size() - 1 : 0;
        }
    }

    bool handle_event(ftxui::Event event) override {
        if (event == ftxui::Event::ArrowDown) {
            if (_selected_index < _branches.size() - 1) {
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

        if (event == ftxui::Event::Return && !_branches.empty()) {
            const auto& branch = _branches[_selected_index];
            if (!branch.is_current) {
                auto cmd = std::make_unique<CheckoutBranchCommand>(
                    _app_state->get_repo(),
                    _app_state,
                    branch.name
                );
                _dispatcher->execute(std::move(cmd));
            }
            return true;
        }

        return false;
    }

    ftxui::Element render() override {
        using namespace ftxui;

        std::vector<Element> lines;
        for (size_t i = 0; i < _branches.size(); ++i) {
            const auto& br = _branches[i];
            
            std::string indicator = br.is_current ? "* " : "  ";
            std::string tracking = "";
            if (!br.tracking_branch.empty()) {
                tracking = " [" + br.tracking_branch;
                if (br.ahead_count > 0 || br.behind_count > 0) {
                    tracking += ": ";
                    if (br.ahead_count > 0) tracking += "↑" + std::to_string(br.ahead_count);
                    if (br.behind_count > 0) tracking += "↓" + std::to_string(br.behind_count);
                }
                tracking += "]";
            }

            auto line = text(indicator + br.name + tracking);
            
            if (i == _selected_index) {
                line = line | bgcolor(Color::Blue) | color(Color::White);
            } else if (br.is_current) {
                line = line | color(Color::Green) | bold;
            }

            lines.push_back(line);
        }

        return vbox({
            text("Local Branches") | bold,
            separator(),
            vbox(lines),
            filler(),
            text("[Enter] Checkout | [↑↓] Navigate") | dim
        }) | border;
    }

private:
    AppState* _app_state;
    CommandDispatcher* _dispatcher;
    std::vector<core::Branch> _branches;
    size_t _selected_index;
};
```

### CheckoutBranchCommand Implementation

```cpp
void CheckoutBranchCommand::execute() {
    try {
        _repo->checkout_branch(_branch_name);
        
        // Refresh status (current branch changed)
        _app_state->refresh_status();
        
        _app_state->notify_success("Checked out branch: " + _branch_name);
    } catch (const GitCommandException& e) {
        // Parse git error for user-friendly message
        std::string error_msg = e.stderr_output();
        if (error_msg.find("Please commit") != std::string::npos) {
            _app_state->notify_error("Cannot checkout: uncommitted changes");
        } else {
            _app_state->notify_error("Checkout failed: " + error_msg);
        }
    }
}
```

---

## Common Pitfalls

**Pitfall 1: Branch Parsing Breaks on Long Names**
- **Problem:** Branch name with spaces or special characters
- **Solution:** Use `--no-color` and parse carefully, or use `--format` option

**Pitfall 2: Current Branch Not Highlighting**
- **Problem:** `is_current` not set correctly
- **Solution:** Check for `*` at start of line in git branch output

**Pitfall 3: Checkout Fails Silently**
- **Problem:** User doesn't know checkout failed
- **Solution:** Always show error message on failure

**Pitfall 4: Remote Branches Checkable**
- **Problem:** User tries to checkout remote branch → detached HEAD
- **Solution:** For now, make remote branches read-only (don't handle Enter key)

---

## Success Checklist

Before marking this slice complete:

- [ ] Local branches are displayed with current branch marked
- [ ] Can navigate branches with arrow keys
- [ ] Can checkout branch with Enter key
- [ ] Tracking info is shown (ahead/behind counts)
- [ ] Remote branches are displayed (read-only)
- [ ] Tags are displayed
- [ ] F5 refreshes all branch/tag lists
- [ ] Error handling works (uncommitted changes, etc.)
- [ ] Unit tests pass for BranchParser and TagParser
- [ ] Code follows naming conventions

**Once all checked:** You're ready for Slice 06 (Commit History Viewing)! 🎉

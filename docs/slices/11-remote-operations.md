# Vertical Slice 11: Remote Operations

**Status:** 🔵 Ready for Implementation  
**Priority:** P2 - Nice to Have  
**Estimated Effort:** 2-3 days  
**Prerequisites:** Slice 05

---

## User Value Statement

As a user, I want to fetch, pull, and push changes to remote repositories so that I can collaborate with others.

---

## Scope - Frontend

- Global keybindings for remote operations
- Progress indication during network operations
- Status bar shows operation progress

## Scope - Backend

**Extend GitRepository:**
```cpp
void fetch(const std::string& remote = "origin");
void pull(const std::string& remote = "origin", const std::string& branch = "");
void push(const std::string& remote = "origin", const std::string& branch = "", bool force = false);
```

**Git Commands:**
```bash
git fetch origin
git pull origin <branch>
git push origin <branch>
git push -f origin <branch>  # Force push (dangerous)
```

## Scope - Application Layer

**New Commands:**
- `FetchCommand`
- `PullCommand`
- `PushCommand`

## Acceptance Criteria

- [ ] Press `Shift+F` → fetch from origin
- [ ] Press `Shift+P` (pull) → pull current branch from tracking remote
- [ ] Press `p` (lowercase) → push current branch to tracking remote
- [ ] Progress shown in status bar during operations
- [ ] Error handling: no remote configured → show error
- [ ] Error handling: push rejected (non-fast-forward) → show error with hint
- [ ] Success messages: "Fetched from origin", "Pushed to origin/main"
- [ ] Branch list and status refresh after operations

## Implementation Notes

### Async Remote Operations

Remote operations can be slow (network latency). Always run async:

```cpp
class FetchCommand : public Command {
public:
    void execute() override {
        _app_state->notify_info("Fetching from origin...");
        
        std::async(std::launch::async, [this]() {
            try {
                _repo->fetch();
                _app_state->refresh_branches();
                _app_state->notify_success("Fetched from origin");
            } catch (const GitCommandException& e) {
                _app_state->notify_error("Fetch failed: " + e.stderr_output());
            }
        });
    }
};
```

### Progress Indication

Show progress in status bar:
```cpp
// In WindowManager or main Application
if (_app_state->is_operation_in_progress()) {
    return text("⏳ " + _app_state->get_current_operation()) | color(Color::Yellow);
}
```

### Push Rejection Handling

```cpp
void PushCommand::execute() {
    try {
        _repo->push("origin", _branch_name, false);
        _app_state->notify_success("Pushed to origin/" + _branch_name);
    } catch (const GitCommandException& e) {
        if (e.stderr_output().find("rejected") != std::string::npos) {
            _app_state->notify_error(
                "Push rejected (non-fast-forward). Pull first or use force push."
            );
        } else if (e.stderr_output().find("authentication") != std::string::npos) {
            _app_state->notify_error("Authentication failed. Check credentials.");
        } else {
            _app_state->notify_error("Push failed: " + e.stderr_output());
        }
    }
}
```

### Global Keybindings

Add to main Application event handler:

```cpp
if (event == ftxui::Event::Character('P')) { // Shift+P = pull
    auto cmd = std::make_unique<PullCommand>(
        _app_state->get_repo(),
        _app_state,
        _app_state->get_current_branch()
    );
    _dispatcher->execute_async(std::move(cmd));
    return true;
}

if (event == ftxui::Event::Character('p')) { // lowercase p = push
    auto cmd = std::make_unique<PushCommand>(
        _app_state->get_repo(),
        _app_state,
        _app_state->get_current_branch()
    );
    _dispatcher->execute_async(std::move(cmd));
    return true;
}
```

## Success Checklist

- [ ] Can fetch from remote
- [ ] Can pull current branch
- [ ] Can push current branch
- [ ] Progress shown during operations
- [ ] Error handling for network failures
- [ ] Error handling for auth failures
- [ ] Error handling for push rejections
- [ ] Success notifications shown
- [ ] Operations run asynchronously (UI stays responsive)

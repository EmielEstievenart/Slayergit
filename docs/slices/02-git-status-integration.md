# Vertical Slice 02: Git Status Integration

**Status:** 🔵 Ready for Implementation
**Priority:** P0 - Foundation
**Estimated Effort:** 2-3 days
**Prerequisites:** Slice 01 (UI Panels and Navigation)

---

## User Value Statement

As a user, I want to see my actual Git repository status (unstaged and staged files) in the UI so that I know what changes exist in my working directory.

---

## Prerequisites

- ✅ Slice 01 completed: UI panels and navigation working
- Git installed and available in PATH
- Understanding of `git status --porcelain` output format

---

## Scope - Frontend

**Update Existing Tabs:**
- `UnstagedTab` - Display real unstaged files from AppState
- `StagedTab` - Display real staged files from AppState
- Both tabs implement `AppState::Observer` interface
- Both tabs update when `on_status_changed()` is called

**Update Main Application:**
- Create `AppState` instance
- Create `GitRepository` instance
- Connect tabs as observers to AppState
- Initial refresh on startup

---

## Scope - Backend (Infrastructure Layer)

**New Files:**
- `src/lib/infra/git_process_executor.cpp/.hpp` - Execute git commands
- `src/lib/infra/parsers/status_parser.cpp/.hpp` - Parse `git status --porcelain`

**Key Components:**

**GitProcessExecutor:**
```cpp
struct ProcessResult {
    int exit_code;
    std::string stdout_output;
    std::string stderr_output;
};

class GitProcessExecutor {
public:
    explicit GitProcessExecutor(const std::string& repo_path);
    ProcessResult execute(const std::vector<std::string>& args);
    std::future<ProcessResult> execute_async(const std::vector<std::string>& args);
private:
    std::string _repo_path;
};
```

**StatusParser:**
```cpp
class StatusParser {
public:
    static std::vector<core::FileStatus> parse(const std::string& porcelain_output);
};
```

---

## Scope - Backend (Core Layer)

**New Files:**
- `src/lib/core/models/file_status.hpp` - Domain model for file status
- `src/lib/core/git_repository.cpp/.hpp` - High-level Git operations

**Domain Models:**

**FileStatus:**
```cpp
enum class FileStatusType {
    Untracked,
    Modified,
    Added,
    Deleted,
    Renamed,
    Copied,
    Unmerged
};

struct FileStatus {
    std::string path;
    FileStatusType staged_status;
    FileStatusType unstaged_status;
    std::string old_path; // For renames
};
```

**RepositoryStatus:**
```cpp
struct RepositoryStatus {
    std::string current_branch;
    std::vector<FileStatus> files;
    int ahead_count = 0;
    int behind_count = 0;
    bool is_clean = true;
};
```

**GitRepository:**
```cpp
class GitRepository {
public:
    explicit GitRepository(const std::string& repo_path);
    
    RepositoryStatus get_status();
    std::future<RepositoryStatus> get_status_async();
    
private:
    std::unique_ptr<infra::GitProcessExecutor> _executor;
    std::string _repo_path;
};
```

---

## Scope - Backend (Application Layer)

**New Files:**
- `src/lib/app/app_state.cpp/.hpp` - Central state management with observers

**AppState:**
```cpp
class AppState {
public:
    class Observer {
    public:
        virtual ~Observer() = default;
        virtual void on_status_changed(const core::RepositoryStatus& status) = 0;
        virtual void on_error(const std::string& message) = 0;
    };

    explicit AppState(std::shared_ptr<core::GitRepository> repo);

    // State access
    const core::RepositoryStatus& get_status() const;

    // Refresh operations
    void refresh_status();
    void refresh_all(); // For F5 key

    // Observer management
    void add_observer(Observer* observer);
    void remove_observer(Observer* observer);

private:
    void notify_status_changed();
    void notify_error(const std::string& message);

    std::shared_ptr<core::GitRepository> _repo;
    core::RepositoryStatus _status;
    std::vector<Observer*> _observers;
    std::mutex _mutex;
};
```

---

## Acceptance Criteria

**Functional:**
- [ ] Launch SlayerGit in a Git repository → unstaged files are displayed in Window 1, Unstaged tab
- [ ] Files staged with external `git add` command → pressing F5 → staged files appear in Staged tab
- [ ] Files unstaged with external `git reset` → pressing F5 → files move to Unstaged tab
- [ ] Empty repository (no changes) → both tabs show "No changes" message
- [ ] Untracked files are shown in Unstaged tab with "??" indicator
- [ ] Modified files show "M" indicator
- [ ] Added files show "A" indicator
- [ ] Deleted files show "D" indicator
- [ ] Renamed files show "R" with old and new paths
- [ ] Error handling: Running in non-Git directory → show error message in status bar
- [ ] F5 refresh updates both tabs with latest Git status

**Quality:**
- [ ] Git status command executes in < 500ms for typical repository
- [ ] UI remains responsive during Git command execution
- [ ] No crashes when Git command fails
- [ ] Memory is properly managed (no leaks with observers)

---

## Implementation Approach

### Tracer Bullet Phase

**Goal:** Get minimal end-to-end flow working.

**Steps:**
1. Implement `GitProcessExecutor::execute()` synchronously only
2. Hardcode parsing for basic modified files (M status only)
3. Create minimal `GitRepository::get_status()` that returns hardcoded data
4. Create minimal `AppState` with simple observer notification
5. Update `UnstagedTab` to display files from AppState
6. Wire everything together in `main.cpp`

**Success:** Pressing F5 shows at least one hardcoded file in Unstaged tab.

### Completion Phase

**Steps:**
1. Implement full `StatusParser` for all Git status codes (M, A, D, R, C, U, ?)
2. Add async execution with `execute_async()` and `std::future`
3. Implement `AppState::refresh_status()` using async Git calls
4. Update both `UnstagedTab` and `StagedTab` to show real files
5. Add proper error handling (git command failures, parse errors)
6. Add visual indicators for different file status types (colors, icons)
7. Handle edge cases (empty repo, detached HEAD, merge conflicts)
8. Add unit tests for `StatusParser`

**Success:** All acceptance criteria met.

---

## Testing Scenarios

### Happy Path
1. Open SlayerGit in repo with unstaged changes → files appear in Unstaged tab
2. Press F5 → status refreshes
3. Externally run `git add file.txt` → Press F5 → file moves to Staged tab

### Error Cases
1. Run SlayerGit in non-Git directory → error message shown
2. Git command fails (e.g., corrupted repo) → error message shown, app doesn't crash
3. Git returns unexpected output format → parse error logged, app doesn't crash

### Edge Cases
1. Empty repository (no commits yet) → show "No commits yet" message
2. Detached HEAD state → show commit hash instead of branch name
3. Merge conflict files (unmerged status) → show files with "U" indicator
4. Very large status output (1000+ files) → UI remains responsive

---

## Integration Points

### Consumes From:
- Slice 01: Uses existing `Tab` interface and `WindowManager`

### Provides For:
- Slice 03 (File Staging): `AppState` provides file status data for staging operations
- Slice 07 (Diff Viewing): `GitRepository` will be extended with diff methods
- All future slices: `AppState` observer pattern is the foundation for all data updates

---

## Files to Create

**Infrastructure Layer:**
```
src/lib/infra/
├── CMakeLists.txt
├── git_process_executor.cpp
├── git_process_executor.hpp
└── parsers/
    ├── status_parser.cpp
    └── status_parser.hpp
```

**Core Layer:**
```
src/lib/core/
├── CMakeLists.txt
├── git_repository.cpp
├── git_repository.hpp
└── models/
    ├── file_status.hpp
    └── repository_status.hpp
```

**Application Layer:**
```
src/lib/app/
├── CMakeLists.txt
├── app_state.cpp
└── app_state.hpp
```

**Tests:**
```
tests/lib/infra/parsers/
└── status_parser_test.cpp
```

---

## Files to Modify

**UI Layer:**
```
src/ui/tabs/unstaged_tab.cpp/.hpp    # Implement Observer, display real files
src/ui/tabs/staged_tab.cpp/.hpp      # Implement Observer, display real files
src/ui/main.cpp                      # Create AppState, GitRepository, wire observers
```

**Build System:**
```
CMakeLists.txt                       # Add new libraries (infra, core, app)
src/lib/infra/CMakeLists.txt         # New file
src/lib/core/CMakeLists.txt          # New file
src/lib/app/CMakeLists.txt           # New file
```

---

## Notes for Implementation Agent

### Git Status Porcelain Format

`git status --porcelain` output format:
```
 M file.txt        # Unstaged modification
M  file.txt        # Staged modification
MM file.txt        # Staged modification + unstaged modification
A  file.txt        # Staged addition (new file)
 D file.txt        # Unstaged deletion
D  file.txt        # Staged deletion
?? file.txt        # Untracked file
R  old.txt -> new.txt  # Renamed file
```

**Parsing Strategy:**
- First two characters are status codes (XY format)
- X = staged status, Y = unstaged status
- Space means "no change"
- File path starts at character 4 (skip first 3: XY + space)

### Observer Pattern Memory Management

**Critical:** 
- Tabs (observers) are owned by `Window`, which is owned by `WindowManager`
- `AppState` only stores raw pointers to observers
- **Must call `remove_observer()` in Tab destructor** to avoid dangling pointers
- Or use `std::weak_ptr` if you prefer smart pointers

**Safe Pattern:**
```cpp
class UnstagedTab : public Tab, public AppState::Observer {
public:
    UnstagedTab(AppState* app_state) : _app_state(app_state) {
        _app_state->add_observer(this);
    }

    ~UnstagedTab() override {
        _app_state->remove_observer(this);
    }

    void on_status_changed(const core::RepositoryStatus& status) override {
        _files = extract_unstaged_files(status);
        // UI will re-render automatically
    }

private:
    AppState* _app_state;
    std::vector<core::FileStatus> _files;
};
```

### Async Execution Pattern

```cpp
std::future<RepositoryStatus> GitRepository::get_status_async() {
    return std::async(std::launch::async, [this]() {
        return this->get_status(); // Call sync version on background thread
    });
}

// In AppState
void AppState::refresh_status() {
    auto future = _repo->get_status_async();
    
    // Option 1: Block and wait (simple)
    auto status = future.get();
    
    // Option 2: Poll in FTXUI event loop (better)
    // Store future, check is_ready() in update loop
    
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _status = status;
    }
    
    notify_status_changed();
}
```

### CMake Library Dependencies

```cmake
# src/lib/infra/CMakeLists.txt
add_library(slayergit_infra STATIC
    git_process_executor.cpp
    parsers/status_parser.cpp
)
target_include_directories(slayergit_infra PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

# src/lib/core/CMakeLists.txt
add_library(slayergit_core STATIC
    git_repository.cpp
)
target_link_libraries(slayergit_core PUBLIC slayergit_infra)
target_include_directories(slayergit_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

# src/lib/app/CMakeLists.txt
add_library(slayergit_app STATIC
    app_state.cpp
)
target_link_libraries(slayergit_app PUBLIC slayergit_core)
target_include_directories(slayergit_app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

# Root CMakeLists.txt
add_subdirectory(src/lib/infra)
add_subdirectory(src/lib/core)
add_subdirectory(src/lib/app)

target_link_libraries(slayergit PRIVATE
    slayergit_ui
    slayergit_app
    slayergit_core
    slayergit_infra
    ftxui::screen
    ftxui::dom
    ftxui::component
)
```

### Process Execution (Cross-Platform)

For Windows support, use:
- `CreateProcess` on Windows
- `fork` + `exec` on Unix/Linux/macOS

Or use a library like:
- Boost.Process (external dependency)
- TinyProcessLibrary (header-only, lightweight)

**Recommended:** Start with simple `std::system()` for tracer, then upgrade to proper process execution.

### Example Git Command Execution

```cpp
ProcessResult GitProcessExecutor::execute(const std::vector<std::string>& args) {
    // Build command string
    std::string cmd = "git";
    for (const auto& arg : args) {
        cmd += " " + arg;
    }

    // Execute in repository directory
    std::string old_dir = std::filesystem::current_path().string();
    std::filesystem::current_path(_repo_path);

    // Capture output (platform-specific)
    FILE* pipe = popen((cmd + " 2>&1").c_str(), "r");
    std::string output;
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe)) {
        output += buffer;
    }
    int exit_code = pclose(pipe) >> 8;

    std::filesystem::current_path(old_dir);

    return ProcessResult{exit_code, output, ""};
}
```

**Note:** This is simplified. Proper implementation should split stdout/stderr and handle errors.

---

## Common Pitfalls

**Pitfall 1: Observer Double-Free**
- **Problem:** Tab destroyed but still registered as observer → AppState tries to notify → segfault
- **Solution:** Always call `remove_observer()` in Tab destructor

**Pitfall 2: Deadlock with Mutex**
- **Problem:** AppState locks mutex, calls observer, observer calls AppState method → deadlock
- **Solution:** Release mutex before calling observers, or use recursive mutex

**Pitfall 3: Git Not in PATH**
- **Problem:** `git` command not found on some systems
- **Solution:** Check git availability on startup, show error if not found

**Pitfall 4: Parsing Edge Cases**
- **Problem:** Filenames with spaces, special characters break parsing
- **Solution:** Use `--porcelain` format and handle quoted filenames

---

## Success Checklist

Before marking this slice complete:

- [ ] Infrastructure layer compiles and executes `git status` successfully
- [ ] Core layer parses Git status output into `FileStatus` domain models
- [ ] Application layer manages state and notifies observers
- [ ] UI tabs display real unstaged and staged files
- [ ] F5 refresh updates UI with latest Git status
- [ ] Error handling works (non-Git directory, git command failures)
- [ ] Unit tests pass for `StatusParser`
- [ ] No memory leaks (run with valgrind)
- [ ] Code follows naming conventions from architecture doc

**Once all checked:** You're ready for Slice 03 (File Staging Operations)! 🎉

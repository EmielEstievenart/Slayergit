# SlayerGit - Software Architecture Document

**Version:** 1.0
**Date:** November 8, 2025
**Status:** Initial Architecture

**📚 Additional Resources:**
- **Detailed Rationale:** See `decisions_rationale.md` for comprehensive reasoning behind architectural decisions
- **Example Code:** See `examples/basic-skeleton/` for reference implementations
- **Implementation Guides:** See `slices/` for step-by-step vertical slice documents

---

## 1. Project Vision

SlayerGit is a blazing-fast, intuitive terminal UI (TUI) for Git operations, built with modern C++ and FTXUI. It provides a keyboard-driven interface with numbered windows and tabs, inspired by Lazygit's proven UX patterns. The architecture emphasizes clean separation of concerns, testability, and responsive performance.

---

## 2. System Architecture

### 2.1 High-Level Components

```
┌─────────────────────────────────────────────────────────────┐
│                      SlayerGit Application                  │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌───────────────────────────────────────────────────┐     │
│  │              UI Layer (Frontend)                  │     │
│  │  - FTXUI Components & Rendering                   │     │
│  │  - Event Handling & Key Bindings                  │     │
│  │  - Window/Tab Management                          │     │
│  │  - View Components (Status, Branches, etc.)       │     │
│  └─────────────────┬─────────────────────────────────┘     │
│                    │                                        │
│                    ▼                                        │
│  ┌───────────────────────────────────────────────────┐     │
│  │         Application Layer (Orchestration)         │     │
│  │  - State Management (AppState)                    │     │
│  │  - Command Dispatcher                             │     │
│  │  - Event Bus / Observer Pattern                   │     │
│  │  - Configuration Manager                          │     │
│  └─────────────────┬─────────────────────────────────┘     │
│                    │                                        │
│                    ▼                                        │
│  ┌───────────────────────────────────────────────────┐     │
│  │           Core Logic Layer (Backend)              │     │
│  │  - Git Operations Library (GitLib)                │     │
│  │  - Repository Manager                             │     │
│  │  - Domain Models (Commit, Branch, File, etc.)     │     │
│  │  - Business Logic Services                        │     │
│  └─────────────────┬─────────────────────────────────┘     │
│                    │                                        │
│                    ▼                                        │
│  ┌───────────────────────────────────────────────────┐     │
│  │          Infrastructure Layer                     │     │
│  │  - Git Process Executor                           │     │
│  │  - Output Parser (git command parsing)            │     │
│  │  - File System Access                             │     │
│  │  - Async Task Executor                            │     │
│  └───────────────────────────────────────────────────┘     │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 2.2 Technology Stack

- **Language:** C++17 or C++20 (modern, idiomatic)
- **Build System:** CMake 3.20+
- **UI Framework:** FTXUI (ArthurSonzogni/FTXUI)
- **Testing:** Google Test (gtest) + Google Mock (gmock)
- **Git Integration:** Process execution wrapper around `git` CLI
- **Async:** `std::async` / `std::future` or thread pool pattern
- **Dependency Management:** Git submodules

### 2.3 Key Architectural Decisions

**Decision 1: Git CLI Wrapper vs libgit2**
- **Choice:** Custom wrapper around Git CLI
- **Key Benefit:** Simpler, automatically stays current with Git features
- See `decisions_rationale.md` for detailed rationale

**Decision 2: Layered Architecture**
- **Choice:** Four-layer architecture (UI → Application → Core → Infrastructure)
- **Key Benefit:** Clear separation enables independent testing of each layer
- See `decisions_rationale.md` for detailed rationale

**Decision 3: Observer Pattern for State Changes**
- **Choice:** Event-driven architecture with observers
- **Key Benefit:** Decouples state management from UI, supports async operations
- See `decisions_rationale.md` for detailed rationale

**Decision 4: Numbered Window/Tab System**
- **Choice:** Window manager with focus tracking and tab cycling
- **Key Benefit:** Proven UX from Lazygit, keyboard-efficient
- See `decisions_rationale.md` for detailed rationale

**Decision 5: Manual Refresh (F5)**
- **Choice:** No auto-refresh; user explicitly triggers refresh with F5
- **Key Benefit:** Performance visibility, user control, battery-friendly
- See `decisions_rationale.md` for detailed rationale

---

## 3. Detailed Component Breakdown

### 3.1 Infrastructure Layer

This is the lowest layer - it interacts with the operating system and Git executable.

#### 3.1.1 Git Process Executor

**Responsibility:** Execute Git commands as child processes and capture output.

**Key Components:**
- `ProcessResult` - Struct containing exit code, stdout, stderr
- `GitProcessExecutor` - Executes git commands (sync and async)

**Key Methods:**
- `execute()` - Synchronous execution
- `execute_async()` - Returns `std::future<ProcessResult>`
- `execute_in_dir()` - Execute with custom working directory

**📁 See:** `examples/basic-skeleton/` for implementation patterns

#### 3.1.2 Output Parsers

**Responsibility:** Parse Git command output into structured data.

**Parser Classes:**
- `StatusParser` - Parses `git status --porcelain` output
- `LogParser` - Parses `git log` with custom format
- `BranchParser` - Parses local and remote branch lists
- `DiffParser` - Parses diff output into hunks
- `ReflogParser` - Parses reflog entries
- `StashParser` - Parses stash list
- `TagParser` - Parses tag list

**Design Notes:**
- All parsers are static methods
- Use porcelain formats for stable parsing
- Throw exceptions on parse failures

#### 3.1.3 Async Task Executor

**Responsibility:** Manage background tasks and thread pool.

**Key Methods:**
- `submit()` - Submit function for async execution, returns `std::future`
- `wait_all()` - Wait for all pending tasks
- `cancel_all()` - Cancel pending tasks

---

### 3.2 Core Logic Layer

This layer contains business logic and domain models - NO UI dependencies.

#### 3.2.1 Domain Models

**Responsibility:** Represent Git concepts as C++ structs.

**Key Models:**
- `FileStatus` - File path, staged/unstaged status, old path (for renames)
- `Commit` - Hash, author, date, message, parents
- `Branch` - Name, type (local/remote), current status, tracking info
- `Diff` / `DiffHunk` / `FileDiff` - Diff hunks with additions/deletions
- `ReflogEntry` - Hash, selector (HEAD@{N}), action, timestamp
- `Stash` - Index, hash, message, branch
- `Tag` - Name, commit hash, annotation
- `RepositoryStatus` - Current branch, files, ahead/behind counts

**Enums:**
- `FileStatusType` - Untracked, Modified, Added, Deleted, Renamed, Copied, Unmerged
- `BranchType` - Local, Remote

#### 3.2.2 Git Operations Library

**Responsibility:** High-level Git operations API via `GitRepository` class.

**Operation Categories:**
- **Status:** `get_status()`, `get_status_async()`
- **Staging:** `stage_file()`, `unstage_file()`, `stage_all()`, `unstage_all()`
- **Commits:** `commit()`, `amend_commit()`
- **Branches:** `get_local_branches()`, `get_remote_branches()`, `checkout_branch()`, `create_branch()`, `delete_branch()`
- **Log/History:** `get_log()`, `get_log_for_file()`
- **Diffs:** `get_diff_unstaged()`, `get_diff_staged()`, `get_diff_commit()`, `get_diff_between()`
- **Reflog:** `get_reflog()`
- **Stashes:** `get_stashes()`, `stash_save()`, `stash_apply()`, `stash_pop()`, `stash_drop()`
- **Tags:** `get_tags()`, `create_tag()`, `delete_tag()`
- **Remotes:** `fetch()`, `pull()`, `push()`

**Design:**
- Each method wraps git CLI commands
- Uses parsers from infrastructure layer
- Throws exceptions on errors
- Async versions for slow operations

---

### 3.3 Application Layer

This layer orchestrates the application - it connects UI to core logic.

#### 3.3.1 Application State

**Responsibility:** Central state management with observer pattern.

**Key Features:**
- Caches Git data (status, branches, commits, stashes, reflog, tags, diffs)
- Observer interface with callbacks for each data type change
- `refresh_all()` method refreshes all Git data (triggered by F5)
- `refresh_*()` methods fetch specific data from GitRepository
- Notifies all observers when state changes

**Observer Callbacks:**
- `on_status_changed()`, `on_branches_changed()`, `on_commits_changed()`
- `on_stashes_changed()`, `on_diff_changed()`, `on_reflog_changed()`
- `on_tags_changed()`, `on_error()`

**Pattern:** Subject-Observer (classic GoF pattern)

#### 3.3.2 Command Dispatcher

**Responsibility:** Execute user commands using Command pattern.

**Key Components:**
- `Command` interface - Abstract base with `execute()` and `undo()` methods
- Concrete commands - `StageFileCommand`, `CommitCommand`, `CheckoutBranchCommand`, etc.
- `CommandDispatcher` - Executes commands (sync or async), tracks pending tasks

**Pattern:** Command (classic GoF pattern)

**Flow:** User action → Create command → Dispatcher executes → Updates GitRepository → Refreshes AppState → Notifies observers

#### 3.3.3 Configuration Manager

**Responsibility:** Load and manage application configuration.

**Structures:**
- `Theme` - Colors for UI elements (background, foreground, borders, diffs, etc.)
- `KeyBindings` - Keyboard shortcuts (quit, refresh, window navigation, git operations)
- `Configuration` - Singleton managing theme, keybindings, and settings

**Settings:**
- Log max count, auto-refresh enabled, refresh interval
- Config file location: `~/.config/slayergit/config.toml`

**Pattern:** Singleton

---

### 3.4 UI Layer (Frontend)

This layer handles all rendering and user interaction using FTXUI.

**📁 Example Implementation:** Complete working examples available in `examples/basic-skeleton/src/ui/`

#### 3.4.1 Window/Tab System

**Responsibility:** Manage numbered windows with tab cycling.

**Key Classes:**
- `Tab` - Interface with `name()`, `render()`, `handle_event()`, `on_focus()`, `on_blur()`
- `Window` - Contains multiple tabs, handles cycling via `next_tab()`, tracks focus
- `WindowManager` - Manages all 5 windows, handles focus switching, renders layout

**Behavior:**
- Pressing window number focuses that window
- Pressing same number again cycles through tabs
- Focus tracking updates visual appearance
- Layout: 4 windows stacked (left) + details panel (right)

**📁 Full implementation:** See `examples/basic-skeleton/src/ui/window*.hpp`

#### 3.4.2 View Components (Tabs)

**Responsibility:** Render specific Git data as tabs.

**All Tabs:** Inherit from `Tab` and `AppState::Observer`

**Window 1 - Status:**
- `UnstagedTab` - Shows unstaged files, observes status changes
- `StagedTab` - Shows staged files, observes status changes

**Window 2 - Branches:**
- `LocalBranchesTab` - Shows local branches, observes branch changes
- `RemoteBranchesTab` - Shows remote branches, observes branch changes
- `TagsTab` - Shows tags, observes tag changes

**Window 3 - History:**
- `CommitsTab` - Shows commit log, observes commit changes
- `ReflogTab` - Shows reflog, observes reflog changes

**Window 4 - Stashes:**
- `StashesView` - Shows stash list, observes stash changes

**Window 5 - Details:**
- `DetailsView` - Shows diffs/details, observes diff changes

**Common Pattern:** Each tab maintains selected index, handles arrow keys for navigation, triggers commands on user actions.

**📁 Full implementations:** See `examples/basic-skeleton/src/ui/tabs/`

#### 3.4.3 Main Application Component

**Responsibility:** FTXUI main loop and application lifecycle.

**Key Class:** `Application`
- Creates GitRepository, AppState, CommandDispatcher, WindowManager
- Initializes FTXUI screen and component tree
- Runs main event loop
- Handles global keys (quit with 'q', refresh with F5)

**Flow:** `main()` → `Application::run()` → FTXUI loop → handles events → updates state → re-renders

---

## 4. Data Flow Architecture

### 4.1 User Action Flow

```
User Input (Key Press)
    ↓
WindowManager::handle_event()
    ↓
Current Window::handle_event()
    ↓
Current Tab::handle_event()
    ↓
Create Command (e.g., StageFileCommand)
    ↓
CommandDispatcher::execute()
    ↓
Command::execute() → GitRepository::stage_file()
    ↓
GitProcessExecutor::execute()
    ↓
Parse Git Output
    ↓
AppState::refresh_status()
    ↓
Notify Observers (Tabs)
    ↓
Tab Updates Internal State
    ↓
FTXUI Re-renders UI
    ↓
Screen Update
```

### 4.2 Async Operation Flow

```
User Presses F5 (Refresh All)
    ↓
Application::handle_global_event()
    ↓
AppState::refresh_all()
    ↓
Multiple std::async → GitRepository methods
    ↓
GitProcessExecutor::execute_async() (parallel)
    ↓
Background Threads: Execute Git Commands
    |
    | (UI remains responsive)
    |
    ↓
Futures Complete
    ↓
AppState updates cached data
    ↓
AppState::notify_all_observers()
    ↓
Observers Update
    ↓
UI Re-renders
```

### 4.3 State Update Flow

```
GitRepository (Core Layer)
    ↓ (returns domain models)
AppState (Application Layer)
    ↓ (caches and notifies)
Observers (UI Components/Tabs)
    ↓ (update internal state)
FTXUI Render
    ↓
Terminal Display
```

---

## 5. Design Patterns

### 5.1 Observer Pattern
- **Used For:** State change notifications
- **Components:** AppState (Subject), Tabs (Observers)
- **Benefit:** Decouples state management from UI

### 5.2 Command Pattern
- **Used For:** User actions and Git operations
- **Components:** Command interface, concrete commands, CommandDispatcher
- **Benefit:** Encapsulates operations, enables undo/redo, async execution

### 5.3 Repository Pattern
- **Used For:** Git data access abstraction
- **Components:** GitRepository
- **Benefit:** Clean API for Git operations, testable

### 5.4 Singleton Pattern
- **Used For:** Configuration management
- **Components:** Configuration
- **Benefit:** Global access to settings

### 5.5 Factory Pattern (Future)
- **Used For:** Creating tabs and windows dynamically
- **Components:** TabFactory, WindowFactory (to be added if needed)
- **Benefit:** Flexible window/tab creation

---

## 6. Module Organization

### 6.1 Directory Structure

```
SlayerGit/
├── CMakeLists.txt                 # Root CMake file
├── src/
│   ├── main.cpp                   # Entry point
│   │
│   ├── lib/                       # Backend libraries (organized by layer)
│   │   ├── infra/                 # Infrastructure layer
│   │   │   ├── CMakeLists.txt
│   │   │   ├── git_process_executor.cpp/.hpp
│   │   │   ├── parsers/
│   │   │   │   ├── status_parser.cpp/.hpp
│   │   │   │   ├── log_parser.cpp/.hpp
│   │   │   │   ├── branch_parser.cpp/.hpp
│   │   │   │   ├── diff_parser.cpp/.hpp
│   │   │   │   ├── reflog_parser.cpp/.hpp
│   │   │   │   ├── stash_parser.cpp/.hpp
│   │   │   │   └── tag_parser.cpp/.hpp
│   │   │   └── task_executor.cpp/.hpp
│   │   │
│   │   ├── core/                  # Core logic layer
│   │   │   ├── CMakeLists.txt
│   │   │   ├── models/
│   │   │   │   ├── file_status.hpp
│   │   │   │   ├── commit.hpp
│   │   │   │   ├── branch.hpp
│   │   │   │   ├── diff.hpp
│   │   │   │   ├── reflog_entry.hpp
│   │   │   │   ├── stash.hpp
│   │   │   │   ├── tag.hpp
│   │   │   │   └── repository_status.hpp
│   │   │   └── git_repository.cpp/.hpp
│   │   │
│   │   └── app/                   # Application layer
│   │       ├── CMakeLists.txt
│   │       ├── app_state.cpp/.hpp
│   │       ├── command.hpp
│   │       ├── commands/
│   │       │   ├── stage_file_command.cpp/.hpp
│   │       │   ├── unstage_file_command.cpp/.hpp
│   │       │   ├── commit_command.cpp/.hpp
│   │       │   └── ... (more commands)
│   │       ├── command_dispatcher.cpp/.hpp
│   │       └── configuration.cpp/.hpp
│   │
│   ├── ui/                        # Frontend UI components
│   │   ├── CMakeLists.txt
│   │   ├── application.cpp/.hpp
│   │   ├── window_manager.cpp/.hpp
│   │   ├── window.cpp/.hpp
│   │   ├── tab.hpp
│   │   ├── tabs/
│   │   │   ├── unstaged_tab.cpp/.hpp
│   │   │   ├── staged_tab.cpp/.hpp
│   │   │   ├── local_branches_tab.cpp/.hpp
│   │   │   ├── remote_branches_tab.cpp/.hpp
│   │   │   ├── tags_tab.cpp/.hpp
│   │   │   ├── commits_tab.cpp/.hpp
│   │   │   ├── reflog_tab.cpp/.hpp
│   │   │   ├── stashes_view.cpp/.hpp
│   │   │   └── details_view.cpp/.hpp
│   │   └── components/            # Reusable UI widgets
│   │       ├── list_view.cpp/.hpp
│   │       ├── diff_view.cpp/.hpp
│   │       ├── input_dialog.cpp/.hpp
│   │       └── confirm_dialog.cpp/.hpp
│   │
│   └── include/                   # Public headers (if needed for external use)
│
├── tests/                         # Google Test unit tests
│   ├── CMakeLists.txt
│   ├── lib/
│   │   ├── infra/
│   │   │   ├── git_process_executor_test.cpp
│   │   │   ├── parsers/
│   │   │   │   ├── status_parser_test.cpp
│   │   │   │   ├── log_parser_test.cpp
│   │   │   │   └── ... (more parser tests)
│   │   ├── core/
│   │   │   └── git_repository_test.cpp
│   │   └── app/
│   │       ├── app_state_test.cpp
│   │       └── command_dispatcher_test.cpp
│   └── ui/
│       ├── window_manager_test.cpp
│       └── tabs/
│           └── ... (tab tests if needed)
│
├── cmake/                         # CMake modules
│   ├── Findfmtlib.cmake
│   └── ... (other find modules)
│
├── submodules/                    # Git submodules
│   ├── FTXUI/
│   └── googletest/
│
└── docs/                          # Documentation
    ├── kickoff.md
    ├── 00-architecture.md         # This document
    └── ... (slice documents)
```

### 6.2 CMake Library Organization

```cmake
# Root CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(SlayerGit VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add submodules
add_subdirectory(submodules/FTXUI)
add_subdirectory(submodules/googletest)

# Add libraries (in dependency order)
add_subdirectory(src/lib/infra)   # Infrastructure layer
add_subdirectory(src/lib/core)    # Core layer (depends on infra)
add_subdirectory(src/lib/app)     # Application layer (depends on core)
add_subdirectory(src/ui)          # UI layer (depends on app)

# Main executable
add_executable(slayergit src/main.cpp)
target_link_libraries(slayergit PRIVATE
    slayergit_ui
    slayergit_app
    slayergit_core
    slayergit_infra
    ftxui::screen
    ftxui::dom
    ftxui::component
)

# Tests
enable_testing()
add_subdirectory(tests)
```

**Layer Libraries:**

```cmake
# src/lib/infra/CMakeLists.txt
add_library(slayergit_infra STATIC
    git_process_executor.cpp
    task_executor.cpp
    parsers/status_parser.cpp
    parsers/log_parser.cpp
    parsers/branch_parser.cpp
    parsers/diff_parser.cpp
    parsers/reflog_parser.cpp
    parsers/stash_parser.cpp
    parsers/tag_parser.cpp
)
target_include_directories(slayergit_infra PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# src/lib/core/CMakeLists.txt
add_library(slayergit_core STATIC
    git_repository.cpp
)
target_link_libraries(slayergit_core PUBLIC slayergit_infra)
target_include_directories(slayergit_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# src/lib/app/CMakeLists.txt
add_library(slayergit_app STATIC
    app_state.cpp
    command_dispatcher.cpp
    configuration.cpp
    commands/stage_file_command.cpp
    commands/unstage_file_command.cpp
    commands/commit_command.cpp
    # ... more commands
)
target_link_libraries(slayergit_app PUBLIC slayergit_core)
target_include_directories(slayergit_app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# src/ui/CMakeLists.txt
add_library(slayergit_ui STATIC
    application.cpp
    window_manager.cpp
    window.cpp
    tabs/unstaged_tab.cpp
    tabs/staged_tab.cpp
    tabs/local_branches_tab.cpp
    tabs/remote_branches_tab.cpp
    tabs/tags_tab.cpp
    tabs/commits_tab.cpp
    tabs/reflog_tab.cpp
    tabs/stashes_view.cpp
    tabs/details_view.cpp
    components/list_view.cpp
    components/diff_view.cpp
    components/input_dialog.cpp
    components/confirm_dialog.cpp
)
target_link_libraries(slayergit_ui PUBLIC
    slayergit_app
    ftxui::screen
    ftxui::dom
    ftxui::component
)
target_include_directories(slayergit_ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
```

---

## 7. Testing Strategy

### 7.1 Unit Testing Approach

**Infrastructure Layer:**
- Mock file system operations
- Test Git output parsing with known examples
- Test process execution with fake Git executable

**Core Layer:**
- Mock GitProcessExecutor
- Test GitRepository API with various scenarios
- Test edge cases (empty repo, detached HEAD, merge conflicts)

**Application Layer:**
- Test state updates and observer notifications
- Test command execution and rollback
- Mock GitRepository

**UI Layer:**
- Test event handling logic
- Test state synchronization
- UI rendering tests are optional (FTXUI integration)

### 7.2 Integration Testing

- Test full stack: UI → App → Core → Infra
- Use real Git repository in test directory
- Verify end-to-end workflows (stage → commit → push)

### 7.3 Test Organization

```cpp
// Example: tests/lib/infra/parsers/status_parser_test.cpp
#include <gtest/gtest.h>
#include "infra/parsers/status_parser.hpp"

TEST(StatusParserTest, ParsesUnstagedModification) {
    std::string git_output = " M src/main.cpp\n";
    auto files = slayergit::infra::StatusParser::parse(git_output);

    ASSERT_EQ(files.size(), 1);
    EXPECT_EQ(files[0].path, "src/main.cpp");
    EXPECT_EQ(files[0].unstaged_status, slayergit::core::FileStatusType::Modified);
}

TEST(StatusParserTest, ParsesStagedAddition) {
    std::string git_output = "A  README.md\n";
    auto files = slayergit::infra::StatusParser::parse(git_output);

    ASSERT_EQ(files.size(), 1);
    EXPECT_EQ(files[0].path, "README.md");
    EXPECT_EQ(files[0].staged_status, slayergit::core::FileStatusType::Added);
}

// Example: tests/lib/core/git_repository_test.cpp
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "core/git_repository.hpp"

class MockGitExecutor : public slayergit::infra::GitProcessExecutor {
public:
    MOCK_METHOD(ProcessResult, execute, (const std::vector<std::string>&), (override));
};

TEST(GitRepositoryTest, GetStatusReturnsCorrectData) {
    auto mock_executor = std::make_unique<MockGitExecutor>();

    EXPECT_CALL(*mock_executor, execute(testing::_))
        .WillOnce(testing::Return(ProcessResult{0, "M src/main.cpp\n", ""}));

    // Test implementation...
}
```

---

## 8. Async Operations & Concurrency

### 8.1 Async Strategy

**Why Async:**
- Git operations can be slow (especially on large repos)
- UI must remain responsive during operations
- Background refresh without blocking user input

**Approach:**
- Use `std::async` or thread pool for background Git operations
- Return `std::future` from async methods
- Update AppState when future completes
- Notify observers on main thread

### 8.2 Thread Safety

**AppState:**
- Protect state with `std::mutex`
- Lock when reading/writing state
- Notify observers on main thread only

**Command Execution:**
- Commands execute on background threads
- State updates marshalled to main thread
- Use thread-safe queue for state updates

### 8.3 Example Async Implementation

```cpp
// In GitRepository
std::future<RepositoryStatus> GitRepository::get_status_async() {
    return std::async(std::launch::async, [this]() {
        return this->get_status(); // Blocking call on background thread
    });
}

// In AppState
void AppState::refresh_status() {
    auto future = repo_->get_status_async();

    // Poll future and update when ready (FTXUI event loop integration)
    // Or use callback pattern
}
```

---

## 9. Error Handling

### 9.1 Error Strategy

**Git Command Failures:**
- Throw descriptive exceptions from GitRepository
- Catch in CommandDispatcher
- Notify observers via `on_error()`
- Display error message in status bar

**Parse Failures:**
- Validate Git output format
- Throw on unexpected format
- Log error details for debugging

**UI Errors:**
- Display error dialog or status bar message
- Allow user to retry or cancel

### 9.2 Exception Hierarchy

```cpp
namespace slayergit {

class SlayerGitException : public std::runtime_error {
public:
    explicit SlayerGitException(const std::string& message)
        : std::runtime_error(message) {}
};

class GitCommandException : public SlayerGitException {
public:
    GitCommandException(const std::string& command, int exit_code, const std::string& stderr_output)
        : SlayerGitException(format_message(command, exit_code, stderr_output)),
          _command(command), _exit_code(exit_code), _stderr_output(stderr_output) {}

    const std::string& command() const { return _command; }
    int exit_code() const { return _exit_code; }
    const std::string& stderr_output() const { return _stderr_output; }

private:
    std::string _command;
    int _exit_code;
    std::string _stderr_output;

    static std::string format_message(const std::string& cmd, int code, const std::string& err) {
        return "Git command failed: " + cmd + " (exit code: " + std::to_string(code) + ")\n" + err;
    }
};

class ParseException : public SlayerGitException {
public:
    explicit ParseException(const std::string& message)
        : SlayerGitException("Parse error: " + message) {}
};

} // namespace slayergit
```

---

## 10. Performance Considerations

### 10.1 Optimization Strategies

**Lazy Loading:**
- Load data only when window/tab is focused
- Don't fetch all branches/tags on startup

**Caching:**
- Cache Git command results in AppState
- Invalidate cache on user actions
- Auto-refresh with configurable interval

**Incremental Updates:**
- Only re-parse changed data
- Diff old vs new state to minimize UI updates

**Virtual Scrolling:**
- Render only visible items in lists
- Use FTXUI's virtual list component (if available)

**Background Threads:**
- Offload expensive operations (log parsing, diff generation)
- Keep UI thread free for rendering

### 10.2 Startup Performance

**Goals:**
- Launch in < 100ms on modern hardware
- Initial UI visible immediately
- Background load Git data

**Approach:**
- Minimal startup initialization
- Display empty UI skeleton first
- Async load repository status
- Progressive enhancement as data loads

---

## 11. Configuration & Theming

### 11.1 Configuration File Format

**Location:** `~/.config/slayergit/config.toml` (or Windows equivalent)

**Example:**
```toml
[theme]
background = "#1e1e1e"
foreground = "#d4d4d4"
border = "#3c3c3c"
highlight = "#569cd6"
error = "#f44747"
success = "#4ec9b0"
added = "#587c0c"
deleted = "#c74e39"
modified = "#e2c08d"

[keybindings]
quit = 'q'
refresh = 'F5'
window_1 = '1'
window_2 = '2'
window_3 = '3'
window_4 = '4'
window_5 = '5'
stage = 's'
unstage = 'u'
commit = 'c'
push = 'p'
pull = 'P'

[general]
log_max_count = 100
auto_refresh = false
refresh_interval_ms = 1000
```

### 11.2 Theme Application

- Load theme from config on startup
- Apply colors to FTXUI elements using `ftxui::Color`
- Support predefined themes (dark, light, solarized, etc.)
- Allow custom themes via config file

---

## 12. Numbered Window/Tab Implementation Details

### 12.1 Window Focus & Tab Cycling

**User Interaction:**
1. Press `1` → Focus Window 1, show first tab
2. Press `1` again → Cycle to next tab in Window 1
3. Press `1` again → Cycle back to first tab
4. Press `2` → Focus Window 2, show first tab

**Implementation:**

```cpp
bool WindowManager::handle_event(ftxui::Event event) {
    // Check for window number keys
    if (event == ftxui::Event::Character('1')) {
        handle_window_key(1);
        return true;
    }
    if (event == ftxui::Event::Character('2')) {
        handle_window_key(2);
        return true;
    }
    // ... more window keys

    // If no window key, pass to focused window
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
```

### 12.2 Layout Rendering

**Layout Structure:**

```
┌─────────────────────────────────────────────────────────────┐
│ Top Bar (Repository Info)                                   │
├─────────────────────┬───────────────────────────────────────┤
│                     │                                       │
│ Window 1            │                                       │
│ (Status)            │                                       │
│                     │                                       │
├─────────────────────┤                                       │
│                     │                                       │
│ Window 2            │   Window 5                            │
│ (Branches)          │   (Details - takes right 60%)         │
│                     │                                       │
├─────────────────────┤                                       │
│                     │                                       │
│ Window 3            │                                       │
│ (History)           │                                       │
│                     │                                       │
├─────────────────────┤                                       │
│                     │                                       │
│ Window 4            │                                       │
│ (Stashes)           │                                       │
│                     │                                       │
└─────────────────────┴───────────────────────────────────────┘
```

**FTXUI Layout Code:**

```cpp
ftxui::Element WindowManager::render_layout() {
    using namespace ftxui;

    // Get window elements
    auto window1 = _windows[0]->render(); // Status
    auto window2 = _windows[1]->render(); // Branches
    auto window3 = _windows[2]->render(); // History
    auto window4 = _windows[3]->render(); // Stashes
    auto window5 = _windows[4]->render(); // Details

    // Left column (40% width)
    auto left_column = vbox({
        window1 | flex_grow,
        separator(),
        window2 | flex_grow,
        separator(),
        window3 | flex_grow,
        separator(),
        window4 | flex_grow,
    });

    // Right column (60% width) - Details
    auto right_column = window5;

    // Horizontal split
    auto main_layout = hbox({
        left_column | size(WIDTH, EQUAL, 40),
        separator(),
        right_column | flex,
    });

    // Add top bar
    return vbox({
        render_top_bar(),
        separator(),
        main_layout | flex,
    });
}
```

---

## 13. Manual Refresh (F5) Strategy

SlayerGit uses **manual refresh** instead of automatic polling. The user presses **F5** to explicitly refresh all repository data, providing complete control over when Git operations execute.

### 13.1 Refresh Behavior

**Full Refresh (F5):** Refreshes all Git data (status, branches, commits, reflog, stashes, tags, diffs)

**Targeted Refresh:** After Git operations, only affected data refreshes:
- Stage/unstage → status only
- Commit → status + commits
- Branch operations → branches + status
- Stash operations → stashes + status

### 13.2 Key Implementation Points

**Application Layer:**
- `AppState::refresh_all()` - Full refresh triggered by F5
- `AppState::refresh_*()` - Targeted refresh methods
- All refresh operations run asynchronously to maintain UI responsiveness
- Optional performance timing display shows refresh duration

**UI Layer:**
- `Application::handle_global_event()` - Intercepts F5 key
- Global key handler checked before window-specific handlers
- Status bar can display last refresh time

**📁 Implementation Examples:** See `examples/003-manual-refresh-f5/` for:
- AppState refresh method patterns
- Application F5 event handling
- Command integration with targeted refresh
- Async refresh with thread safety
- Optional performance monitoring

**📋 Full Rationale:** See `adr/004-manual-refresh-f5.md` for complete decision context and trade-offs

### 13.3 Design Benefits

✅ Performance visibility ✅ User control ✅ Resource efficient ✅ Battery friendly ✅ Predictable UX

---

## 14. Git Wrapper Architecture Details

### 14.1 Command Execution Pipeline

```
GitRepository::stage_file("/path/to/file")
    ↓
Build Git Args: ["add", "/path/to/file"]
    ↓
GitProcessExecutor::execute(args)
    ↓
Execute: git add /path/to/file
    ↓
Capture stdout/stderr
    ↓
ProcessResult {exit_code=0, stdout="", stderr=""}
    ↓
Check exit_code == 0
    ↓
Return success or throw GitCommandException
```

### 14.2 Git Command Templates

**Status:**
```bash
git status --porcelain=v1 --untracked-files=all
```

**Log:**
```bash
git log --pretty=format:"%H%x00%an%x00%ae%x00%at%x00%s%x00%b%x00%P" --max-count=100
```

**Branches (Local):**
```bash
git branch -vv --no-color
```

**Branches (Remote):**
```bash
git branch -r -vv --no-color
```

**Diff (Unstaged):**
```bash
git diff
```

**Diff (Staged):**
```bash
git diff --cached
```

**Stash List:**
```bash
git stash list --pretty=format:"%gd%x00%H%x00%at%x00%gs"
```

**Reflog:**
```bash
git reflog --pretty=format:"%H%x00%gD%x00%gs%x00%at" --max-count=100
```

**Tags:**
```bash
git tag -l --format="%(refname:short)%x00%(objectname)%x00%(subject)"
```

---

## 15. Shared Conventions

### 15.1 Naming Conventions

**Files:**
- Snake_case for C++ source files: `git_repository.cpp`, `app_state.hpp`
- Match class name: `GitRepository` → `git_repository.cpp/.hpp`

**Classes:**
- PascalCase: `GitRepository`, `AppState`, `WindowManager`
- Suffix for interfaces: `Observer`, `Command`

**Methods:**
- Snake_case: `get_status()`, `stage_file()`, `next_tab()`

**Variables:**
- Snake_case: `current_branch`, `selected_index`
- Leading underscore for private members: `_repo_path`, `_observers`

**Constants:**
- Snake_case: `max_log_count`, `default_theme`
- ALL_CAPS is ONLY for compile-time preprocessor macros: `#define MAX_BUFFER_SIZE`

**Namespaces:**
- Lowercase: `slayergit::core`, `slayergit::ui`, `slayergit::infra`

### 15.2 Code Style

**Formatting:**
- Indentation: 4 spaces (no tabs)
- Braces: Opening brace on same line
- Line length: 100 characters max

**Comments:**
- Doxygen-style comments for public APIs
- Inline comments for complex logic
- TODO comments with issue numbers

**Headers:**
- Include guards: `SLAYERGIT_CORE_GIT_REPOSITORY_HPP`
- Order: Standard library → Third-party → Project headers
- Forward declarations where possible

### 15.3 Error Handling Conventions

- Use exceptions for error conditions
- Validate input at API boundaries
- Log errors with context
- Provide user-friendly error messages
- Never swallow exceptions silently

### 15.4 Testing Conventions

- Test file naming: `<source_file>_test.cpp`
- Test fixture naming: `<ClassName>Test`
- Test case naming: `<MethodName>_<Scenario>_<ExpectedBehavior>`
- Example: `GetStatus_EmptyRepository_ReturnsCleanStatus`

### 15.5 Comprehensive Coding Guidelines

#### Rule 1: Types are PascalCase, Everything Else is snake_case

**Correct:**
```cpp
class GitRepository {                    // Type = PascalCase
public:
    std::string get_current_branch();    // Method = snake_case
    void stage_file(const std::string& file_path) {  // Parameter = snake_case
        std::string local_variable;     // Variable = snake_case
    }
private:
    std::vector<core::Commit> _commits; // Member = leading _ + snake_case
};

// Namespace = lowercase
namespace slayergit::core {
    enum class FileStatusType {         // Enum type = PascalCase
        Modified,                       // Enum value = PascalCase
        Added,
        Deleted
    };

    struct Commit {                     // Struct = PascalCase
        std::string hash;               // Field = snake_case
        std::string author_name;
    };
}
```

**Incorrect:**
```cpp
class gitRepository {                   // ✗ Type should be PascalCase
public:
    std::string GetCurrentBranch();     // ✗ Method should be snake_case
    void StageFile(const std::string& FilePath) {  // ✗ Parameter should be snake_case
    }
};

enum class file_status_type {           // ✗ Type should be PascalCase
    MODIFIED,                           // ✗ Value should be PascalCase, not ALL_CAPS
    ADDED
};
```

#### Rule 2: ALL_CAPS is ONLY for Preprocessor Macros

**Correct:**
```cpp
#define MAX_BUFFER_SIZE 4096            // Macro = ALL_CAPS
const int max_retries = 3;              // Constant = snake_case
constexpr int default_port = 8080;      // Constexpr = snake_case
const std::string config_file_path = "/etc/app.conf";  // Constant = snake_case

struct Config {
    int max_connections = 100;          // Member constant = snake_case
    int timeout_ms = 5000;
};
```

**Incorrect:**
```cpp
const int MAX_RETRIES = 3;              // ✗ Constant should be snake_case
const std::string DEFAULT_CONFIG_PATH = "/etc/app.conf";  // ✗ Should be snake_case
constexpr int DEFAULT_PORT = 8080;      // ✗ Should be snake_case

// Only use ALL_CAPS for actual macros:
#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))  // ✓ Correct
```

#### Rule 3: Private Member Variables Start with Leading Underscore

**Correct:**
```cpp
class AppState {
public:
    const core::RepositoryStatus& get_status() const {
        return _status;
    }

    void refresh_status() {
        _status = _repo->get_status_async();
        notify_status_changed();
    }

private:
    core::RepositoryStatus _status;     // Private member = _snake_case
    std::shared_ptr<core::GitRepository> _repo;
    std::vector<Observer*> _observers;
    int _refresh_interval_ms = 1000;
};
```

**Incorrect:**
```cpp
class AppState {
private:
    core::RepositoryStatus status_;     // ✗ Should be _status (leading underscore)
    int refresh_interval_ms_;           // ✗ Should be _refresh_interval_ms
};
```

#### Additional Guidelines

**Public vs Private:**
- Public methods/properties don't have prefix/suffix
- Private members always have leading underscore
- No trailing underscore ever

**Class Members:**
```cpp
class Window {
public:
    int get_number() const { return _number; }
    std::string get_title() const;
    void next_tab();                    // Methods = snake_case, no prefix

    void on_focus();                    // Event handlers = on_<action>
    void on_blur();

private:
    int _number;                        // Private = _snake_case
    std::vector<std::unique_ptr<Tab>> _tabs;
    size_t _current_tab_index = 0;
};
```

**Static and Module-Level Constants:**
```cpp
// At namespace/file scope
namespace slayergit {
    constexpr int default_timeout_ms = 5000;        // Constant = snake_case
    const std::string config_directory = "~/.config/slayergit";

    class Config {
        static constexpr int max_history_entries = 1000;  // Static const = snake_case
        static int parse_config(const std::string& path);  // Static method = snake_case
    };
}
```

**Type Aliases:**
```cpp
using FileStatusList = std::vector<core::FileStatus>;     // Type alias = PascalCase
using StatusCallback = std::function<void(const RepositoryStatus&)>;  // PascalCase
```

---

## 16. Development Workflow

### 16.1 Build Commands

```bash
# Configure
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build build

# Run
./build/slayergit

# Test
cd build && ctest --output-on-failure

# Or run specific test
./build/tests/slayergit_tests --gtest_filter=StatusParserTest.*
```

### 16.2 Debugging

**GDB:**
```bash
gdb ./build/slayergit
```

**Valgrind:**
```bash
valgrind --leak-check=full ./build/slayergit
```

**Logging:**
- Use conditional compilation for debug logs
- Log to file in debug mode: `~/.slayergit/debug.log`

### 16.3 Git Workflow

- Feature branches: `feature/window-system`, `feature/git-wrapper`
- Commit messages: Conventional commits format
- PR workflow: Review before merge
- CI: Run tests on every PR

---

## 17. Future Enhancements

### 17.1 Features Beyond MVP

- [ ] Interactive rebase UI
- [ ] Cherry-pick support
- [ ] Merge conflict resolution UI
- [ ] Submodule management
- [ ] Customizable layouts
- [ ] Plugin system
- [ ] Git hooks visualization
- [ ] Performance profiling overlay

### 17.2 Technical Improvements

- [ ] Custom parsing library (faster than regex)
- [ ] Incremental diff rendering
- [ ] Git object database access (bypass CLI for speed)
- [ ] Undo/redo stack for Git operations
- [ ] Persistent undo history
- [ ] Multi-repository support (workspace view)

---

## 18. Non-Functional Requirements

### 18.1 Performance Targets

- **Startup Time:** < 100ms on SSD
- **UI Responsiveness:** 60 FPS rendering
- **Git Operations:** < 500ms for common operations (status, log)
- **Memory Usage:** < 50MB for typical repositories

### 18.2 Compatibility

- **Operating Systems:** Linux, macOS, Windows (via MSYS2/MinGW)
- **Git Versions:** Git 2.20+
- **Terminal Emulators:** Any with 256-color support
- **Minimum Terminal Size:** 80x24 characters

### 18.3 Accessibility

- Keyboard-only navigation
- High contrast theme support
- Screen reader compatibility (limited by terminal)

---

## 19. Architecture Decision Records (ADRs)

**Note:** Detailed rationale for all decisions can be found in `decisions_rationale.md`.

### ADR-001: Use Git CLI Instead of libgit2
- **Decision:** Use Git CLI wrapper instead of libgit2
- **Status:** Accepted
- **Key Trade-off:** Simplicity vs raw performance (chose simplicity)

### ADR-002: Four-Layer Architecture
- **Decision:** Use layered architecture (UI → Application → Core → Infrastructure)
- **Status:** Accepted
- **Key Trade-off:** Boilerplate vs maintainability (chose maintainability)

### ADR-003: Observer Pattern for State Management
- **Decision:** Use Observer pattern with AppState as subject
- **Status:** Accepted
- **Key Trade-off:** Manual management vs loose coupling (chose loose coupling)

### ADR-004: Manual Refresh Strategy (F5)
- **Decision:** Manual refresh via F5 key instead of automatic polling
- **Status:** Accepted
- **Key Trade-off:** Convenience vs performance visibility (chose visibility)
- **Rationale:** Users maintain full control over when Git operations execute, making performance characteristics visible and measurable

---

## 20. Key Interfaces Summary

### 20.1 Infrastructure Layer

- `GitProcessExecutor`: Execute Git commands
- `StatusParser`, `LogParser`, etc.: Parse Git output
- `TaskExecutor`: Background task execution

### 20.2 Core Layer

- `GitRepository`: High-level Git operations API
- Domain models: `Commit`, `Branch`, `FileStatus`, etc.

### 20.3 Application Layer

- `AppState`: Central state management with observers
- `Command`: Command pattern interface
- `CommandDispatcher`: Execute and manage commands
- `Configuration`: Application settings

### 20.4 UI Layer

- `Application`: Main application lifecycle
- `WindowManager`: Manage windows and focus
- `Window`: Container for tabs
- `Tab`: Individual view interface
- Concrete tabs: `UnstagedTab`, `LocalBranchesTab`, etc.

---

## 21. Integration Points Between Layers

### 21.1 Infrastructure ↔ Core

- Core layer calls Infrastructure to execute Git commands
- Core layer uses parsers to convert output to domain models
- Infrastructure has no knowledge of Core layer

### 21.2 Core ↔ Application

- Application layer uses Core's `GitRepository` for Git operations
- Application layer caches Core's domain models in `AppState`
- Core layer has no knowledge of Application layer

### 21.3 Application ↔ UI

- UI components observe `AppState` for changes
- UI creates `Command` objects for user actions
- Application layer has no knowledge of UI layer

---

## Conclusion

This architecture provides a solid foundation for SlayerGit with:

✅ **Clear Separation:** Four distinct layers with well-defined responsibilities
✅ **Testability:** Each layer can be tested independently
✅ **Extensibility:** Easy to add new windows, tabs, and Git operations
✅ **Performance:** Async operations keep UI responsive
✅ **Maintainability:** Clean abstractions and design patterns
✅ **User Experience:** Keyboard-driven, numbered window/tab system

The architecture balances simplicity with flexibility, providing a clear path from MVP to feature-rich Git TUI.

---

**Next Steps:**
1. Review this architecture with the team
2. Set up CMake build system with library structure
3. Implement infrastructure layer (Git wrapper + parsers)
4. Build core layer (GitRepository + domain models)
5. Create application layer (AppState + commands)
6. Implement UI layer (FTXUI components)
7. Integrate all layers and test end-to-end

**Let's build something amazing! 🎸⚡**

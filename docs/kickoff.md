# SlayerGit - Project Kickoff

**Date:** November 7, 2025  
**Status:** Initial Setup - Let's Go! 🚀

## Vision

SlayerGit is a blazing-fast, intuitive terminal UI (TUI) for Git, inspired by the awesomeness of Lazygit. We're building a modern C++ application that makes Git operations feel effortless and enjoyable.

## Tech Stack

- **Language:** C++ (modern, idiomatic)
- **Build System:** CMake
- **UI Framework:** [FTXUI](https://github.com/ArthurSonzogni/FTXUI) - Beautiful terminal graphics
- **Testing:** Google Test (gtest)
- **Git Integration:** Custom wrapper around Git executable

## Architecture

### Clean Separation of Concerns

We're maintaining a clear split between backend and frontend:

**Backend:**
- Git wrapper library for all Git operations
- Business logic and state management
- Command execution and parsing

**Frontend:**
- FTXUI-based user interface components
- Event handling and user interactions
- Visual rendering and theming

### UI Layout

Following Lazygit's proven design, SlayerGit uses **numbered windows** that can be activated by pressing the corresponding number key. Each window supports **tabbing** - press the same number multiple times to cycle through tabs:

```
┌─────────────────────────────┬─────────────────────────────────┐
│ Repository: SlayerGit                                         │
│ Branch: main | Status: Clean | Commits ahead: 2 | Behind: 0   │
├─────────────────────────────┬─────────────────────────────────┤
│ 1: Status [Unstaged]        │                                 │
├─────────────────────────────┤                                 │
│  M  src/main.cpp            │                                 │
│  M  src/ui.cpp              │                                 │
│  ?? README.md               │                                 │
│                             │                                 │
│  Tabs: Unstaged | Staged    │                                 │
│                             │                                 │
├─────────────────────────────┤   5: Details                    │
│ 2: Branches [Local]         │                                 │
├─────────────────────────────┤                                 │
│  * main                     │   Detailed view of selected     │
│    develop                  │   item from active window       │
│    feature/ui               │                                 │
│                             │   - File diffs                  │
│  Tabs: Local | Remote | Tags│   - Commit details              │
│                             │   - Branch info                 │
├─────────────────────────────┤   - Stash contents              │
│ 3: History [Commits]        │   - Reflog entries              │
├─────────────────────────────┤                                 │
│  * a1b2c3d Fix bug (2h ago) │                                 │
│    d4e5f6g Add feature (1d) │                                 │
│    h7i8j9k Initial (2d)     │                                 │
│                             │                                 │
│  Tabs: Commits | Reflog     │                                 │
│                             │                                 │
├─────────────────────────────┤                                 │
│ 4: Stashes                  │                                 │
├─────────────────────────────┤                                 │
│  stash@{0}: WIP on main     │                                 │
│  stash@{1}: feature work    │                                 │
│                             │                                 │
└─────────────────────────────┴─────────────────────────────────┘
```

**Window Navigation with Tabs:**
- **Top Bar** - Repository information (name, current branch, status)
- Press `1` repeatedly - Cycle Status tabs: **Unstaged** → **Staged** → (back to Unstaged)
- Press `2` repeatedly - Cycle Branches tabs: **Local** → **Remote** → **Tags** → (back to Local)
- Press `3` repeatedly - Cycle History tabs: **Commits** → **Reflog** → (back to Commits)
- Press `4` - Focus Stashes (no tabs)
- Press `5` - Focus Details view (right side - shows details of selected item)

## 📁 Project Structure

```
SlayerGit/
├── src/                    # Source code
│   ├── lib/                # Backend libraries (organized as needed)
│   ├── ui/                 # Frontend UI components
│   ├── include/            # Public header files
│   └── main.cpp            # Application entry point
├── tests/                  # Google Test unit tests
│   ├── lib/                # Backend tests
│   └── ui/                 # Frontend tests
├── cmake/                  # CMake modules and configs
├── submodules/             # Git submodules (FTXUI, gtest, etc.)
├── docs/                   # Documentation
└── LICENSE
```

**Note:** Backend should be organized into separate libraries where practical to maintain modularity and testability.

## 🎮 Core Features (MVP)

- [ ] Git status visualization
- [ ] Stage/unstage files interactively
- [ ] Commit with inline message editing
- [ ] Branch viewing and switching
- [ ] Log viewing with pretty graphs
- [ ] Diff visualization
- [ ] Keyboard-driven navigation

## 🧪 Development Principles

1. **Test-Driven:** Write tests first, code second
2. **Clean Code:** Readable, maintainable, documented
3. **Performance:** Fast startup, responsive UI
4. **User Experience:** Intuitive, minimal keystrokes
5. **Testability:** Design components to be testable without overdoing it - separate business logic from UI rendering where practical

## 🚀 Next Steps

1. Set up CMake build system
2. Add FTXUI as submodule
3. Add Google Test as submodule
4. Create basic project structure
5. Implement Git wrapper proof-of-concept
6. Build "Hello World" FTXUI app
7. Integrate backend + frontend

## 💡 Inspiration

Taking cues from Lazygit's brilliant UX while adding our own C++ performance and customization.

---

**Let's slay this! 🎸⚡**

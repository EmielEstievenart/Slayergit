# SlayerGit - Architecture Decision Rationale

**Version:** 1.0
**Date:** November 8, 2025

This document contains the detailed rationale for architectural decisions made in SlayerGit. For the decisions themselves, see `00-architecture.md`.

---

## ADR-001: Use Git CLI Instead of libgit2

**Rationale:**
- **Simpler Implementation**: Wrapping Git CLI commands is straightforward compared to learning and using libgit2's extensive API
- **Automatic Compatibility**: Stays current with latest Git features without code changes
- **Easier Debugging**: Can compare behavior with manual git commands in terminal
- **Lower Learning Curve**: Contributors familiar with Git CLI can contribute immediately
- **Proven Approach**: Tools like Lazygit successfully use this pattern

**Consequences:**
- Parsing overhead for command output (acceptable for TUI responsiveness)
- Slower than direct libgit2 calls (negligible for typical operations)
- Dependency on Git being installed (reasonable assumption for Git TUI users)
- Potential parsing brittleness if Git output format changes (mitigated by using porcelain formats)

**Trade-offs Accepted:**
- We chose maintainability and simplicity over raw performance
- The performance difference is not noticeable in typical Git operations
- Users of a Git TUI will always have Git installed

---

## ADR-002: Four-Layer Architecture

**Rationale:**
- **Clear Boundaries**: Each layer has well-defined responsibilities and interfaces
- **Testable Without UI**: Backend can be fully tested without FTXUI dependency
- **Business Logic Isolation**: Core Git operations separated from rendering concerns
- **Future Refactoring**: Easy to swap implementations (e.g., UI framework, Git backend)
- **Parallel Development**: Different layers can be developed independently

**Consequences:**
- More boilerplate code (layer interfaces and adapters)
- Indirection between layers (extra function calls)
- Need careful dependency management (prevent circular dependencies)
- Steeper initial setup (worth it for long-term maintainability)

**Trade-offs Accepted:**
- We chose maintainability and testability over initial simplicity
- The boilerplate is one-time cost; benefits compound over time
- Clear layers make onboarding new contributors easier

**Why Not Alternatives:**
- **Monolithic approach**: Would tightly couple UI to Git operations, making testing difficult
- **Two-layer (UI/Backend)**: Would mix business logic with Git operations
- **More layers**: Would add unnecessary complexity for this project's scope

---

## ADR-003: Observer Pattern for State Management

**Rationale:**
- **Decouples State from UI**: AppState doesn't need to know about UI components
- **Multiple Observers**: Multiple views can observe same state (e.g., status bar + main view)
- **Easy Extension**: Adding new UI components doesn't require changing AppState
- **Well-Understood Pattern**: Standard pattern with clear semantics
- **Async-Friendly**: Observers can be notified when async operations complete

**Consequences:**
- Manual observer registration/deregistration required (risk of memory leaks)
- No automatic cleanup (must remember to remove observers)
- Need thread safety when notifying from background threads
- Potential for notification storms (many state changes at once)

**Trade-offs Accepted:**
- We chose decoupling over automatic memory management
- Manual management is acceptable with clear ownership rules
- Benefits of loose coupling outweigh registration boilerplate

**Why Not Alternatives:**
- **Direct references**: Would tightly couple AppState to UI components
- **Event bus/signals-slots**: Would add third-party dependency (e.g., Boost.Signals2)
- **Reactive streams**: Overkill for our use case, complex to implement

---

## ADR-004: Numbered Window/Tab Navigation

**Rationale:**
- **Proven UX**: Lazygit users love this navigation pattern
- **Keyboard Efficiency**: Single keypress to focus any window
- **Simple Mental Model**: Number = window, repeat = cycle tabs
- **Low Cognitive Load**: Numbers are visible on screen, no memorization needed
- **Fast Navigation**: No need for arrow keys or multiple keypresses

**Consequences:**
- Limited to 9 windows maximum (acceptable for our use case)
- No vim-style hjkl navigation (can be added as alternative)
- Window positions are fixed (not customizable in MVP)

**Trade-offs Accepted:**
- We chose proven UX pattern over innovation
- 9-window limit is not a practical constraint
- Fixed layout simplifies implementation and is familiar to users

---

## ADR-005: C++ Standard Library for Async (No External Dependencies)

**Rationale:**
- **Zero Dependencies**: std::async and std::future are built-in
- **Good Enough**: Sufficient for our async needs (background Git operations)
- **Simple**: No need to learn external async library
- **Portable**: Works everywhere C++11+ is available

**Consequences:**
- Less sophisticated than Boost.Asio or similar
- Limited control over thread pool
- No cancellation support in std::future (need to implement if required)

**Trade-offs Accepted:**
- We chose simplicity over advanced async features
- Can upgrade to thread pool pattern if needed
- Most Git operations are fast enough that simple async is sufficient

---

## ADR-006: FTXUI for Terminal UI

**Rationale:**
- **Modern C++**: Uses C++17 idioms, feels natural
- **Component-Based**: Composable UI elements
- **Cross-Platform**: Works on Linux, macOS, Windows
- **Active Development**: Well-maintained project
- **Beautiful Output**: Supports colors, borders, layouts
- **Git-Friendly**: Easy to include as submodule

**Consequences:**
- Smaller community than ncurses
- Less mature than ncurses (fewer examples)
- Requires C++17 (acceptable for new project)

**Trade-offs Accepted:**
- We chose modern C++ experience over established library
- FTXUI's component model fits our architecture well
- Active development means bugs get fixed quickly

---

## ADR-007: Google Test for Testing

**Rationale:**
- **Industry Standard**: Widely used and understood
- **Good Documentation**: Many resources and examples
- **Mock Support**: Google Mock included for mocking dependencies
- **Fixtures**: Good support for test fixtures and parameterized tests
- **CMake Integration**: Easy to integrate with our build system

**Consequences:**
- Larger dependency than Catch2
- More verbose than modern alternatives
- Some consider it dated

**Trade-offs Accepted:**
- We chose familiarity and features over minimalism
- Most C++ developers know Google Test
- Mock support is essential for testing our layers

---

## ADR-008: CMake for Build System

**Rationale:**
- **Industry Standard**: De facto standard for C++ projects
- **IDE Support**: All major IDEs support CMake
- **Cross-Platform**: Works on all target platforms
- **Submodule Support**: Easy to integrate Git submodules
- **Flexible**: Can generate for any build system

**Consequences:**
- Verbose syntax
- Learning curve for CMake language
- Sometimes finicky with dependencies

**Trade-offs Accepted:**
- We chose ecosystem support over simplicity
- CMake's ubiquity makes it easy for contributors
- Other build systems (Meson, Bazel) are less universal

---

## ADR-009: Manual Refresh Strategy (F5)

**Status:** Accepted  
**Date:** November 8, 2025  
**Decision Maker:** Architecture Team

### Decision (One-Line Summary)

Use manual refresh triggered by F5 key instead of automatic background polling for repository data updates.

### Context

Git TUI applications need to keep their display synchronized with the actual state of the Git repository. There are two primary approaches:

1. **Automatic Refresh:** Background polling that periodically executes Git commands to check for changes
2. **Manual Refresh:** User explicitly triggers refresh when they want updated data

### Problem Statement

We need to decide: Should SlayerGit auto-refresh repository data in the background, or should users explicitly trigger refreshes?

### Options Considered

#### Option A: Auto-Refresh (Like Lazygit)

**Implementation:**
- Background thread polls `git status` and other commands every 500-1000ms
- UI automatically updates when changes detected

**Pros:**
- Familiar UX (Lazygit users expect this)
- "Live" feel - changes appear immediately

**Cons:**
- Constant CPU usage
- Performance characteristics hidden from users
- Hard to debug performance issues
- Polling interval configuration headache
- Can slow down large repositories
- Interferes with ongoing operations

#### Option B: Manual Refresh with F5 (Chosen)

**Implementation:**
- User presses F5 to refresh all repository data
- Commands automatically refresh only affected data
- No background polling

**Pros:**
- User has complete control over when operations execute
- Performance characteristics visible and measurable
- Easy to profile and optimize
- No configuration needed
- Predictable behavior
- Scales well to large repositories

**Cons:**
- Requires manual user action (press F5)
- External changes not immediately visible
- Different from Lazygit UX

#### Option C: Hybrid Approach

**Implementation:**
- Manual refresh by default
- Optional auto-refresh via configuration flag

**Pros:**
- Flexibility for different use cases

**Cons:**
- Two code paths to maintain
- Configuration complexity

### Rationale

We chose **Option B: Manual Refresh with F5** for the following reasons:

#### 1. Performance Visibility

The core philosophy of SlayerGit is **performance transparency**. Users should understand exactly when Git operations occur and how long they take.

With manual refresh:
- Users know: "I pressed F5, now Git commands are running"
- Performance problems are immediately obvious

With auto-refresh:
- Performance is invisible background magic
- Hard to isolate which operation is slow

#### 2. Scalability to Large Repositories

In large repositories (e.g., Linux kernel, Chromium), `git status` can take seconds. Auto-refresh at 500ms intervals would create a terrible UX.

Manual refresh: user runs refresh when ready, sees it takes 3 seconds, understands the repository is large.

#### 3. Predictable Behavior

Auto-refresh can update the UI while the user is in the middle of an action. This creates a jarring experience.

Manual refresh: UI only changes when user triggers it or performs a Git operation.

#### 4. Debugging and Development

Manual refresh makes profiling and debugging straightforward with clear operation boundaries.

#### 5. Configuration Simplicity

Auto-refresh requires configuration for refresh intervals. Manual refresh: no configuration needed.

#### 6. Proven Pattern

Many successful TUI tools use manual refresh (htop, ncdu, monitoring dashboards).

### Decision Details

1. **F5 triggers full refresh** of all repository data
2. **Git operations trigger targeted refresh** (e.g., stage refreshes status)
3. **Async execution** to keep UI responsive
4. **Optional display** of refresh time in status bar
5. **Future option** for auto-refresh if users want it

### User Experience

Most operations auto-refresh affected data. F5 is mainly for external changes.

### Alternatives Considered (Summary)

| Aspect | Auto-Refresh | Manual Refresh (Chosen) |
|--------|-------------|------------------------|
| CPU Usage (idle) | Constant polling | Zero |
| Performance Visibility | Hidden | Visible |
| Large Repos | Poor UX | Scales well |
| Configuration | Complex | Simple |
| Debugging | Difficult | Easy |

### Consequences

#### Positive

✅ **Performance transparency** - Users see when and how long operations take
✅ **Scalable** - Works well even with massive repositories
✅ **Debugging-friendly** - Clear operation boundaries
✅ **Predictable UX** - Screen only changes when user triggers it
✅ **Simple** - No refresh interval configuration needed

#### Negative

❌ **Requires user action** - Must press F5 to see external changes
❌ **Different from Lazygit** - Users may expect auto-refresh

#### Mitigation Strategies

1. **Clear UI feedback:** Status bar shows "[F5] Refresh" hint
2. **Smart targeted refresh:** Git operations auto-refresh affected data
3. **Optional auto-refresh:** Advanced users can enable it via config

### References

- SlayerGit Architecture Document Section 13
- Examples: `examples/003-manual-refresh-f5/`
- Lazygit refresh behavior: https://github.com/jesseduffield/lazygit/wiki/Config-Docs#refresher

### Revisit Criteria

This decision should be reconsidered if user feedback strongly indicates preference for auto-refresh.

### Notes

- This decision aligns with SlayerGit's core principle: **performance visibility**
- F5 is a familiar refresh key in many applications

---

## Design Pattern Rationale

### Why Observer Pattern (Not Event Bus)?

**Event Bus Pros:**
- More decoupled (components don't know about each other)
- Can filter events
- Can add middleware

**Event Bus Cons:**
- Requires additional implementation or library
- More complex debugging (harder to trace event flow)
- Potential performance overhead

**Our Choice**: Observer pattern is simpler and sufficient for our needs. We have a small number of well-defined state changes, making direct observer notification clearer than an event bus.

---

### Why Command Pattern (Not Direct Calls)?

**Direct Call Pros:**
- Simpler, less boilerplate
- No indirection

**Direct Call Cons:**
- Hard to implement undo/redo
- Difficult to make async
- No command history or logging

**Our Choice**: Command pattern enables future features (undo, macro recording, command logging) and makes async execution clean. The boilerplate is worth it.

---

### Why Repository Pattern (Not Direct Git Access)?

**Direct Access Pros:**
- One less layer
- Faster (no abstraction overhead)

**Direct Access Cons:**
- Tightly couples UI to Git implementation
- Hard to test without real Git repository
- Difficult to mock for unit tests

**Our Choice**: Repository pattern provides clean API boundary and makes testing feasible. The abstraction cost is negligible compared to Git operation time.

---

## Performance Rationale

### Why Lazy Loading?

Loading all data upfront would:
- Slow startup (fetch all branches, tags, stashes immediately)
- Waste resources (user may not view all windows)
- Block UI (waiting for all Git operations)

Lazy loading means:
- Fast startup (show empty UI immediately)
- Efficient (load only what's needed)
- Responsive (background loading doesn't block UI)

### Why Virtual Scrolling?

For repositories with:
- Thousands of commits
- Hundreds of branches
- Many files

Rendering all items would:
- Use excessive memory
- Slow down rendering
- Provide no benefit (only ~50 lines visible)

Virtual scrolling:
- Renders only visible items
- Constant memory usage
- Smooth scrolling even with huge lists

---

## Why These Tech Choices Over Alternatives?

### Why Not Rust?

**Rust Pros:**
- Memory safety
- Better async story (Tokio)
- Modern language

**Rust Cons:**
- Smaller TUI ecosystem
- Harder to find contributors
- Team is more familiar with C++

**Our Choice**: C++ with modern practices achieves similar safety. FTXUI is more mature than Rust TUI libraries (Ratatui/Cursive). C++ has larger talent pool for open source contributions.

---

### Why Not libgit2?

Already covered in ADR-001, but specifically:
- Lazygit proves CLI wrapper is viable
- Parsing is fast enough for TUI responsiveness
- Git porcelain formats are stable
- Direct CLI access easier to reason about and debug

---

### Why Not Different UI Frameworks?

**ncurses:**
- Pros: Mature, stable, widely used
- Cons: C API (not idiomatic C++), manual memory management, harder to compose UI

**Termion/Crossterm:**
- Pros: Low-level control
- Cons: Too low-level (we'd need to build component system), Rust-focused

**Final Term:**
- Pros: Modern, C++
- Cons: Less mature than FTXUI, smaller community

**Our Choice**: FTXUI's component model and modern C++ API fit our architecture best. It's the most natural for component-based UI.

---

## Future-Proofing Rationale

### Why Separate Layers Now?

"YAGNI" (You Aren't Gonna Need It) suggests not building abstractions until needed. However:

**We built layers because:**
1. **Testing**: Cannot test Git operations without abstraction
2. **Parallel Work**: Multiple developers can work on different layers
3. **Future Ports**: Might want web UI, different TUI library, or libgit2 backend
4. **Learning**: Clean architecture makes codebase accessible to contributors

**Cost is minimal:**
- Interface definitions are small
- No complex abstraction logic
- Clear benefits for testing alone justify it

### Why Plan for Async Early?

Git operations can be slow:
- Large repositories (Linux kernel, Chromium)
- Network operations (fetch, push)
- Complex diffs

Building async support later would require:
- Rewriting synchronous code
- Adding threading everywhere
- Fixing race conditions

Building it early:
- Design APIs to support async
- Use std::future from start
- Test with async in mind

**Cost is minimal:**
- std::async is simple
- async methods can call sync methods
- No complex async machinery needed

---

## Summary

All architectural decisions prioritize:
1. **Maintainability** over minimal code
2. **Testability** over simplicity
3. **Contributor Experience** over cutting-edge tech
4. **Proven Patterns** over innovation
5. **Long-term Health** over short-term speed

These trade-offs reflect that SlayerGit is an open-source project where code clarity and ease of contribution are paramount.

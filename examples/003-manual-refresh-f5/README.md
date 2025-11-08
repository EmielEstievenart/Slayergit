# Manual Refresh (F5) - Implementation Examples

**Intent:** Demonstrate the manual refresh pattern where users explicitly trigger repository data updates via F5 key.

**When to Use:** Reference these patterns when implementing AppState refresh logic and F5 event handling.

**Key Files:**
- `app_state_refresh.hpp` - AppState refresh method signatures
- `application_f5_handler.cpp` - F5 event handling in Application class
- `command_with_refresh.cpp` - Commands that trigger targeted refresh
- `async_refresh_pattern.cpp` - Async refresh with thread safety
- `performance_monitoring.cpp` - Optional refresh timing display

**Caveats:**
- These are reference patterns, not production code
- Actual implementation will integrate with FTXUI event loop
- Thread safety mechanisms shown are simplified examples
- Performance monitoring is optional and can be disabled via config

**Related:**
- See `adr/004-manual-refresh-f5.md` for full rationale
- See Section 13 of `00-architecture.md` for architectural overview

# Vertical Slice 09: Reflog Viewing

**Status:** 🔵 Ready for Implementation  
**Priority:** P2 - Nice to Have  
**Estimated Effort:** 1 day  
**Prerequisites:** Slice 02, 06

---

## User Value Statement

As a user, I want to view the reflog so that I can see all changes to HEAD and recover from mistakes.

---

## Scope - Frontend

- `ReflogTab` - Display reflog entries with selection

## Scope - Backend

**New Files:**
- `src/lib/infra/parsers/reflog_parser.cpp/.hpp`
- `src/lib/core/models/reflog_entry.hpp`

**Domain Models:**
```cpp
struct ReflogEntry {
    std::string hash;
    std::string short_hash;
    std::string selector;     // HEAD@{0}, HEAD@{1}, etc.
    std::string action;       // commit, checkout, reset, etc.
    std::time_t timestamp;
    std::string message;
};
```

**Extend GitRepository:**
```cpp
std::vector<ReflogEntry> get_reflog(int max_count = 100);
```

**Git Command:**
```bash
git reflog --pretty=format:"%H%x00%gD%x00%gs%x00%at" --max-count=100
```

## Scope - Application Layer

**Extend AppState:**
```cpp
const std::vector<core::ReflogEntry>& get_reflog() const;
void refresh_reflog();
void on_reflog_changed(const std::vector<core::ReflogEntry>& entries) = 0;
```

## Acceptance Criteria

- [ ] Reflog tab shows last 100 reflog entries
- [ ] Each entry shows: selector, action, timestamp, message
- [ ] Navigate with ↑/↓
- [ ] Selected entry highlighted
- [ ] Pressing Enter shows commit details in Details panel
- [ ] F5 refreshes reflog

## Implementation Notes

### ReflogParser

```cpp
std::vector<ReflogEntry> ReflogParser::parse(const std::string& output) {
    std::vector<ReflogEntry> entries;
    std::istringstream stream(output);
    std::string line;

    while (std::getline(stream, line)) {
        if (line.empty()) continue;

        ReflogEntry entry;
        
        auto parts = split(line, '\0');
        if (parts.size() >= 4) {
            entry.hash = parts[0];
            entry.short_hash = parts[0].substr(0, 7);
            entry.selector = parts[1];
            entry.message = parts[2];
            entry.timestamp = std::stol(parts[3]);
            
            // Extract action from message (e.g., "commit: Add feature")
            size_t colon = entry.message.find(':');
            if (colon != std::string::npos) {
                entry.action = entry.message.substr(0, colon);
            }
        }

        entries.push_back(entry);
    }

    return entries;
}
```

### ReflogTab Display

```cpp
ftxui::Element render() override {
    std::vector<Element> lines;
    for (size_t i = 0; i < _entries.size(); ++i) {
        const auto& e = _entries[i];
        
        std::string line_text = e.selector + " " + e.short_hash + " " + e.action + ": " + e.message;
        
        auto line = text(line_text);
        if (i == _selected_index) {
            line = line | bgcolor(Color::Blue) | color(Color::White);
        }
        lines.push_back(line);
    }

    return vbox({
        text("Reflog") | bold,
        separator(),
        vbox(lines) | vscroll_indicator | frame,
        filler(),
        text("[Enter] View Details | [↑↓] Navigate") | dim
    }) | border;
}
```

## Success Checklist

- [ ] Reflog entries display correctly
- [ ] Can navigate with arrow keys
- [ ] Enter shows commit details
- [ ] F5 refreshes reflog
- [ ] Unit tests for ReflogParser

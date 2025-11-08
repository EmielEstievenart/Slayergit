# Vertical Slice 06: Commit History Viewing

**Status:** 🔵 Ready for Implementation  
**Priority:** P1 - Important Functionality  
**Estimated Effort:** 1-2 days  
**Prerequisites:** Slice 02 (Git Status Integration)

---

## User Value Statement

As a user, I want to view commit history in the History window so that I can review past changes and understand project evolution.

---

## Scope - Frontend

- `CommitsTab` - Display commit log with selection, pagination

## Scope - Backend

**New Files:**
- `src/lib/infra/parsers/log_parser.cpp/.hpp`
- `src/lib/core/models/commit.hpp`

**Domain Models:**
```cpp
struct Commit {
    std::string hash;
    std::string short_hash; // First 7 chars
    std::string author_name;
    std::string author_email;
    std::time_t author_date;
    std::string subject; // First line of message
    std::string body;    // Rest of message
    std::vector<std::string> parent_hashes;
};
```

**Extend GitRepository:**
```cpp
std::vector<Commit> get_log(int max_count = 100);
std::vector<Commit> get_log_for_file(const std::string& file_path, int max_count = 100);
```

**Git Command:**
```bash
git log --pretty=format:"%H%x00%an%x00%ae%x00%at%x00%s%x00%b%x00%P" --max-count=100
```

## Scope - Application Layer

**Extend AppState:**
```cpp
const std::vector<core::Commit>& get_commits() const;
void refresh_commits();
void on_commits_changed(const std::vector<core::Commit>& commits) = 0;
```

## Acceptance Criteria

- [ ] Commits tab shows last 100 commits
- [ ] Each commit shows: short hash, author, date, subject
- [ ] Navigate commits with ↑/↓
- [ ] Selected commit highlighted
- [ ] Pressing Enter shows commit details in Details panel (Slice 07)
- [ ] F5 refreshes commit log
- [ ] Pagination: load more on Page Down (optional)

## Implementation Notes

### LogParser Pattern

```cpp
std::vector<Commit> LogParser::parse(const std::string& output) {
    std::vector<Commit> commits;
    std::istringstream stream(output);
    std::string line;

    while (std::getline(stream, line)) {
        if (line.empty()) continue;

        Commit commit;
        
        // Split by null character (0x00)
        std::vector<std::string> parts = split(line, '\0');
        if (parts.size() >= 7) {
            commit.hash = parts[0];
            commit.short_hash = parts[0].substr(0, 7);
            commit.author_name = parts[1];
            commit.author_email = parts[2];
            commit.author_date = std::stol(parts[3]);
            commit.subject = parts[4];
            commit.body = parts[5];
            
            // Parse parent hashes (space-separated)
            if (!parts[6].empty()) {
                commit.parent_hashes = split(parts[6], ' ');
            }
        }

        commits.push_back(commit);
    }

    return commits;
}
```

### CommitsTab Display

```cpp
ftxui::Element render() override {
    std::vector<Element> lines;
    for (size_t i = 0; i < _commits.size(); ++i) {
        const auto& c = _commits[i];
        
        std::string date_str = format_date(c.author_date);
        std::string line_text = c.short_hash + " " + date_str + " " + c.subject;
        
        auto line = text(line_text);
        if (i == _selected_index) {
            line = line | bgcolor(Color::Blue) | color(Color::White);
        }
        lines.push_back(line);
    }

    return vbox({
        text("Commits") | bold,
        separator(),
        vbox(lines) | vscroll_indicator | frame,
        filler(),
        text("[Enter] View Details | [↑↓] Navigate") | dim
    }) | border;
}
```

## Success Checklist

- [ ] Commit history displays correctly
- [ ] Can navigate with arrow keys
- [ ] Dates formatted properly (relative time)
- [ ] F5 refreshes log
- [ ] Unit tests for LogParser

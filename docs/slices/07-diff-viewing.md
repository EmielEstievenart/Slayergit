# Vertical Slice 07: Diff Viewing

**Status:** 🔵 Ready for Implementation  
**Priority:** P1 - Important Functionality  
**Estimated Effort:** 2-3 days  
**Prerequisites:** Slices 02, 03, 06

---

## User Value Statement

As a user, I want to view diffs in the Details panel when I select files or commits so that I can see exactly what changed.

---

## Scope - Frontend

- `DetailsView` - Display diffs with syntax highlighting

## Scope - Backend

**New Files:**
- `src/lib/infra/parsers/diff_parser.cpp/.hpp`
- `src/lib/core/models/diff.hpp`

**Domain Models:**
```cpp
struct DiffLine {
    enum class Type { Context, Addition, Deletion, Header };
    Type type;
    std::string content;
    int old_line_no = -1;
    int new_line_no = -1;
};

struct DiffHunk {
    std::string header; // @@ -10,7 +10,8 @@
    std::vector<DiffLine> lines;
};

struct FileDiff {
    std::string old_path;
    std::string new_path;
    bool is_new_file = false;
    bool is_deleted = false;
    bool is_renamed = false;
    std::vector<DiffHunk> hunks;
};

struct Diff {
    std::vector<FileDiff> files;
};
```

**Extend GitRepository:**
```cpp
Diff get_diff_unstaged();
Diff get_diff_staged();
Diff get_diff_commit(const std::string& commit_hash);
Diff get_diff_between(const std::string& from, const std::string& to);
```

**Git Commands:**
```bash
git diff                    # Unstaged changes
git diff --cached           # Staged changes
git show <hash>             # Commit diff
git diff <from>..<to>       # Between commits
```

## Scope - Application Layer

**Extend AppState:**
```cpp
const Diff& get_current_diff() const;
void set_diff_for_file(const std::string& file_path, bool staged);
void set_diff_for_commit(const std::string& commit_hash);
void on_diff_changed(const Diff& diff) = 0;
```

## Acceptance Criteria

- [ ] Select file in Unstaged tab → diff shown in Details panel
- [ ] Select file in Staged tab → diff shown in Details panel
- [ ] Select commit in Commits tab → commit diff shown in Details panel
- [ ] Additions shown in green
- [ ] Deletions shown in red
- [ ] Context lines shown in default color
- [ ] Hunk headers shown (@@ -10,7 +10,8 @@)
- [ ] Scroll through diff with ↑/↓ or Page Up/Down

## Implementation Notes

### Syntax Highlighting

```cpp
ftxui::Element render_diff_line(const DiffLine& line) {
    using namespace ftxui;
    
    switch (line.type) {
        case DiffLine::Type::Addition:
            return text("+ " + line.content) | color(Color::Green);
        case DiffLine::Type::Deletion:
            return text("- " + line.content) | color(Color::Red);
        case DiffLine::Type::Header:
            return text(line.content) | color(Color::Cyan) | bold;
        case DiffLine::Type::Context:
        default:
            return text("  " + line.content);
    }
}
```

### DiffParser Implementation

Parse unified diff format:
- Lines starting with `+` are additions
- Lines starting with `-` are deletions
- Lines starting with `@@` are hunk headers
- Other lines are context

### Integration with Tabs

When file is selected in UnstagedTab:
```cpp
if (event == ftxui::Event::Return && !_files.empty()) {
    _app_state->set_diff_for_file(_files[_selected_index].path, false);
    return true;
}
```

When commit is selected in CommitsTab:
```cpp
if (event == ftxui::Event::Return && !_commits.empty()) {
    _app_state->set_diff_for_commit(_commits[_selected_index].hash);
    return true;
}
```

## Success Checklist

- [ ] File selection shows diff in Details panel
- [ ] Commit selection shows commit diff
- [ ] Syntax highlighting works (green/red/default)
- [ ] Can scroll through large diffs
- [ ] Hunk headers are visible
- [ ] Unit tests for DiffParser

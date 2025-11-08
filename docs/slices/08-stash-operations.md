# Vertical Slice 08: Stash Operations

**Status:** 🔵 Ready for Implementation  
**Priority:** P2 - Nice to Have  
**Estimated Effort:** 1-2 days  
**Prerequisites:** Slice 02

---

## User Value Statement

As a user, I want to stash my uncommitted changes and restore them later so that I can switch contexts without committing incomplete work.

---

## Scope - Frontend

- `StashesView` - Display stash list, create/apply/pop/drop stashes

## Scope - Backend

**New Files:**
- `src/lib/infra/parsers/stash_parser.cpp/.hpp`
- `src/lib/core/models/stash.hpp`

**Domain Models:**
```cpp
struct Stash {
    int index;          // stash@{0}, stash@{1}, etc.
    std::string hash;
    std::time_t timestamp;
    std::string message;
    std::string branch; // Branch where stash was created
};
```

**Extend GitRepository:**
```cpp
std::vector<Stash> get_stashes();
void stash_save(const std::string& message = "");
void stash_apply(int index);
void stash_pop(int index);
void stash_drop(int index);
```

**Git Commands:**
```bash
git stash list --pretty=format:"%gd%x00%H%x00%at%x00%gs"
git stash push -m "message"
git stash apply stash@{N}
git stash pop stash@{N}
git stash drop stash@{N}
```

## Scope - Application Layer

**New Commands:**
- `StashSaveCommand`
- `StashApplyCommand`
- `StashPopCommand`
- `StashDropCommand`

**Extend AppState:**
```cpp
const std::vector<core::Stash>& get_stashes() const;
void refresh_stashes();
void on_stashes_changed(const std::vector<core::Stash>& stashes) = 0;
```

## Acceptance Criteria

- [ ] Stashes tab shows all stashes with index, message, timestamp
- [ ] Press `s` → input dialog → create stash with message
- [ ] Select stash, press `a` → apply stash (keep in list)
- [ ] Select stash, press `p` → pop stash (apply and remove)
- [ ] Select stash, press `d` → drop stash (remove)
- [ ] Confirmation dialog for destructive operations (drop)
- [ ] F5 refreshes stash list
- [ ] Status refreshes after apply/pop

## Implementation Notes

### StashesView

```cpp
bool handle_event(ftxui::Event event) override {
    if (event == ftxui::Event::Character('s')) {
        _input_dialog.show("Create Stash", "Enter stash message (optional)");
        return true;
    }

    if (event == ftxui::Event::Character('a') && !_stashes.empty()) {
        auto cmd = std::make_unique<StashApplyCommand>(
            _app_state->get_repo(),
            _app_state,
            _stashes[_selected_index].index
        );
        _dispatcher->execute(std::move(cmd));
        return true;
    }

    if (event == ftxui::Event::Character('p') && !_stashes.empty()) {
        auto cmd = std::make_unique<StashPopCommand>(
            _app_state->get_repo(),
            _app_state,
            _stashes[_selected_index].index
        );
        _dispatcher->execute(std::move(cmd));
        return true;
    }

    if (event == ftxui::Event::Character('d') && !_stashes.empty()) {
        _confirm_dialog.show("Drop stash?", "This cannot be undone.");
        return true;
    }

    return false;
}
```

### Confirmation Dialog

Reuse or create `ConfirmDialog` component:
```cpp
class ConfirmDialog {
public:
    void show(const std::string& title, const std::string& message);
    bool is_confirmed() const;
    ftxui::Component component();
};
```

## Success Checklist

- [ ] Can create stash with message
- [ ] Can apply stash
- [ ] Can pop stash
- [ ] Can drop stash with confirmation
- [ ] Stash list updates after operations
- [ ] Unit tests for StashParser

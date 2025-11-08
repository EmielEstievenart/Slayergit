# Vertical Slice 10: Branch Management

**Status:** 🔵 Ready for Implementation  
**Priority:** P2 - Nice to Have  
**Estimated Effort:** 1-2 days  
**Prerequisites:** Slice 05

---

## User Value Statement

As a user, I want to create, delete, and rename branches so that I can organize my work into features and experiments.

---

## Scope - Frontend

- `LocalBranchesTab` - Add keybindings for create/delete/rename

## Scope - Backend

**Extend GitRepository:**
```cpp
void create_branch(const std::string& name, const std::string& start_point = "HEAD");
void delete_branch(const std::string& name, bool force = false);
void rename_branch(const std::string& old_name, const std::string& new_name);
```

**Git Commands:**
```bash
git branch <name> [start-point]
git branch -d <name>        # Safe delete
git branch -D <name>        # Force delete
git branch -m <old> <new>   # Rename
```

## Scope - Application Layer

**New Commands:**
- `CreateBranchCommand`
- `DeleteBranchCommand`
- `RenameBranchCommand`

## Acceptance Criteria

- [ ] Press `n` in Local Branches tab → input dialog → create new branch
- [ ] Press `d` on selected branch → confirmation → delete branch
- [ ] Cannot delete current branch (show error)
- [ ] Press `r` on selected branch → input dialog → rename branch
- [ ] Confirmation dialog for destructive operations
- [ ] Branch list refreshes after operations
- [ ] Error handling: delete branch with unmerged changes → show error or force option

## Implementation Notes

### LocalBranchesTab Extensions

```cpp
bool handle_event(ftxui::Event event) override {
    // ... existing navigation and checkout

    if (event == ftxui::Event::Character('n')) {
        _input_dialog.show("Create Branch", "Enter branch name");
        return true;
    }

    if (event == ftxui::Event::Character('d') && !_branches.empty()) {
        const auto& branch = _branches[_selected_index];
        if (branch.is_current) {
            _app_state->notify_error("Cannot delete current branch");
        } else {
            _confirm_dialog.show(
                "Delete branch?",
                "Delete branch '" + branch.name + "'? This cannot be undone."
            );
        }
        return true;
    }

    if (event == ftxui::Event::Character('r') && !_branches.empty()) {
        const auto& branch = _branches[_selected_index];
        _input_dialog.show("Rename Branch", "New name for '" + branch.name + "'");
        return true;
    }

    return false;
}
```

### Confirmation Flow

```cpp
// After confirmation dialog closes
if (_confirm_dialog.is_confirmed()) {
    auto cmd = std::make_unique<DeleteBranchCommand>(
        _app_state->get_repo(),
        _app_state,
        _branches[_selected_index].name,
        false // force = false (safe delete)
    );
    _dispatcher->execute(std::move(cmd));
}
```

### Error Handling

DeleteBranchCommand should handle unmerged branches:
```cpp
void DeleteBranchCommand::execute() {
    try {
        _repo->delete_branch(_branch_name, _force);
        _app_state->refresh_branches();
        _app_state->notify_success("Deleted branch: " + _branch_name);
    } catch (const GitCommandException& e) {
        if (e.stderr_output().find("not fully merged") != std::string::npos) {
            _app_state->notify_error(
                "Branch not fully merged. Use force delete (Shift+D) to delete anyway."
            );
        } else {
            _app_state->notify_error("Delete failed: " + e.stderr_output());
        }
    }
}
```

## Success Checklist

- [ ] Can create new branch
- [ ] Can delete branch with confirmation
- [ ] Cannot delete current branch
- [ ] Can rename branch
- [ ] Error handling for unmerged branches
- [ ] Branch list refreshes after operations

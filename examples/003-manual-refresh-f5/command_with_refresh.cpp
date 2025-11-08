// command_with_refresh.cpp
// Reference: Commands that trigger targeted refresh after Git operations

#include "app/commands/commit_command.hpp"
#include "app/commands/stage_file_command.hpp"


namespace slayergit::app {

// Example 1: Staging a file - refresh only status
class StageFileCommand : public Command {
public:
  StageFileCommand(const std::string &file_path,
                   std::shared_ptr<core::GitRepository> repo,
                   std::shared_ptr<AppState> app_state)
      : _file_path(file_path), _repo(repo), _app_state(app_state) {}

  void execute() override {
    // Execute git stage operation
    _repo->stage_file(_file_path);

    // Targeted refresh - only update status (not branches, commits, etc.)
    _app_state->refresh_status();
  }

private:
  std::string _file_path;
  std::shared_ptr<core::GitRepository> _repo;
  std::shared_ptr<AppState> _app_state;
};

// Example 2: Committing - refresh status AND commits
class CommitCommand : public Command {
public:
  CommitCommand(const std::string &message,
                std::shared_ptr<core::GitRepository> repo,
                std::shared_ptr<AppState> app_state)
      : _message(message), _repo(repo), _app_state(app_state) {}

  void execute() override {
    // Execute git commit
    _repo->commit(_message);

    // Broader refresh - status changed (no staged files) and new commit added
    _app_state->refresh_status();
    _app_state->refresh_commits();
  }

private:
  std::string _message;
  std::shared_ptr<core::GitRepository> _repo;
  std::shared_ptr<AppState> _app_state;
};

// Example 3: Branch checkout - refresh status AND branches
class CheckoutBranchCommand : public Command {
public:
  void execute() override {
    _repo->checkout_branch(_branch_name);

    // Refresh both status and branches (current branch indicator changes)
    _app_state->refresh_status();
    _app_state->refresh_branches();
  }

private:
  std::string _branch_name;
  std::shared_ptr<core::GitRepository> _repo;
  std::shared_ptr<AppState> _app_state;
};

} // namespace slayergit::app

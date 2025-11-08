// app_state_refresh.hpp
// Reference implementation of AppState refresh methods

#pragma once

#include "core/git_repository.hpp"
#include "core/models/branch.hpp"
#include "core/models/commit.hpp"
#include "core/models/repository_status.hpp"
#include <future>
#include <memory>
#include <vector>


namespace slayergit::app {

class AppState {
public:
  // Full refresh triggered by F5 - refreshes ALL repository data
  void refresh_all() {
    // Launch async operations for all Git data
    auto status_future = _repo->get_status_async();
    auto local_branches_future = _repo->get_local_branches_async();
    auto remote_branches_future = _repo->get_remote_branches_async();
    auto commits_future = _repo->get_log_async();
    auto reflog_future = _repo->get_reflog_async();
    auto stashes_future = _repo->get_stashes_async();
    auto tags_future = _repo->get_tags_async();

    // Wait for all futures and update state
    // (In real implementation, integrate with FTXUI event loop)

    // Notify all observers once data is ready
    notify_all_observers();
  }

  // Targeted refresh methods - called after specific Git operations
  void refresh_status();   // After stage/unstage
  void refresh_branches(); // After branch create/delete/checkout
  void refresh_commits();  // After commit/amend
  void refresh_reflog();   // After operations that update reflog
  void refresh_stashes();  // After stash operations
  void refresh_tags();     // After tag operations
  void refresh_diff();     // When selection changes

private:
  std::shared_ptr<core::GitRepository> _repo;

  // Cached repository data
  core::RepositoryStatus _status;
  std::vector<core::Branch> _local_branches;
  std::vector<core::Branch> _remote_branches;
  std::vector<core::Commit> _commits;
  std::vector<core::ReflogEntry> _reflog;
  std::vector<core::Stash> _stashes;
  std::vector<core::Tag> _tags;
  core::Diff _current_diff;

  // Observer management
  std::vector<Observer *> _observers;
  void notify_all_observers();
};

} // namespace slayergit::app

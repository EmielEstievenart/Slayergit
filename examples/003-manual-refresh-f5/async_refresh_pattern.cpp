// async_refresh_pattern.cpp
// Reference: Async refresh implementation with thread safety

#include "app/app_state.hpp"
#include <future>
#include <mutex>
#include <vector>

namespace slayergit::app {

void AppState::refresh_all() {
  // Set loading state
  _is_loading = true;
  notify_loading_state_changed();

  // Launch all async operations in parallel
  std::vector<std::future<void>> futures;

  // Each lambda captures 'this' and updates state with proper locking
  futures.push_back(std::async(std::launch::async, [this]() {
    auto status = _repo->get_status();
    std::lock_guard<std::mutex> lock(_state_mutex);
    _status = std::move(status);
    _status_dirty = true;
  }));

  futures.push_back(std::async(std::launch::async, [this]() {
    auto branches = _repo->get_local_branches();
    std::lock_guard<std::mutex> lock(_state_mutex);
    _local_branches = std::move(branches);
    _branches_dirty = true;
  }));

  futures.push_back(std::async(std::launch::async, [this]() {
    auto commits = _repo->get_log();
    std::lock_guard<std::mutex> lock(_state_mutex);
    _commits = std::move(commits);
    _commits_dirty = true;
  }));

  // ... more futures for reflog, stashes, tags, etc.

  // Wait for all to complete (in another background thread to not block UI)
  std::async(std::launch::async,
             [this, futures = std::move(futures)]() mutable {
               // Wait for all futures
               for (auto &future : futures) {
                 future.get();
               }

               // All done - notify observers on main thread
               // (In real implementation, use FTXUI's post_to_main_thread or
               // similar)
               post_to_main_thread([this]() {
                 _is_loading = false;
                 notify_all_observers();
               });
             });
}

// Targeted refresh - simpler, just one async operation
void AppState::refresh_status() {
  auto future = _repo->get_status_async();

  // When future completes (integrated with FTXUI event loop in real
  // implementation)
  auto status = future.get();

  {
    std::lock_guard<std::mutex> lock(_state_mutex);
    _status = std::move(status);
  }

  // Notify only status observers
  for (auto *observer : _observers) {
    observer->on_status_changed(_status);
  }
}

} // namespace slayergit::app

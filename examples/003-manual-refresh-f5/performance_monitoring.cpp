// performance_monitoring.cpp
// Reference: Optional refresh timing display

#include "app/app_state.hpp"
#include <chrono>

namespace slayergit::app {

void AppState::refresh_all() {
  // Start timing
  auto start = std::chrono::steady_clock::now();

  // ... perform refresh (see async_refresh_pattern.cpp) ...

  // End timing
  auto end = std::chrono::steady_clock::now();
  auto duration_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
          .count();

  _last_refresh_duration_ms = duration_ms;
  notify_refresh_complete(duration_ms);
}

int AppState::get_last_refresh_duration_ms() const {
  return _last_refresh_duration_ms;
}

} // namespace slayergit::app

// In UI layer - display refresh time in status bar
namespace slayergit::ui {

ftxui::Element render_status_bar(std::shared_ptr<app::AppState> app_state) {
  using namespace ftxui;

  std::string refresh_info =
      "Last refresh: " +
      std::to_string(app_state->get_last_refresh_duration_ms()) + "ms";

  return hbox({
      text("SlayerGit") | bold,
      separator(),
      text(app_state->get_current_branch()),
      separator(),
      text(refresh_info) | dim, // Show refresh time
      separator(),
      text("[F5] Refresh | [q] Quit"),
  });
}

} // namespace slayergit::ui

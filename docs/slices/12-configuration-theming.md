# Vertical Slice 12: Configuration & Theming

**Status:** 🔵 Ready for Implementation  
**Priority:** P3 - Polish  
**Estimated Effort:** 2-3 days  
**Prerequisites:** None (can be done anytime)

---

## User Value Statement

As a user, I want to customize colors, themes, and key bindings so that SlayerGit matches my preferences and workflow.

---

## Scope - Frontend

- Apply theme colors to all UI elements
- Load custom key bindings from config

## Scope - Backend

**New Files:**
- `src/lib/app/configuration.cpp/.hpp`
- `src/lib/app/theme.hpp`
- `src/lib/app/key_bindings.hpp`

**Configuration Structure:**
```cpp
struct Theme {
    ftxui::Color background;
    ftxui::Color foreground;
    ftxui::Color border;
    ftxui::Color highlight;
    ftxui::Color error;
    ftxui::Color success;
    ftxui::Color added;    // Diff additions
    ftxui::Color deleted;  // Diff deletions
    ftxui::Color modified; // Modified files
};

struct KeyBindings {
    std::string quit = "q";
    std::string refresh = "F5";
    std::string window_1 = "1";
    std::string window_2 = "2";
    // ... etc
    std::string stage = "s";
    std::string unstage = "u";
    std::string commit = "c";
    std::string push = "p";
    std::string pull = "P";
};

class Configuration {
public:
    static Configuration& instance();
    
    void load(const std::string& config_path = "");
    void save(const std::string& config_path = "");
    
    const Theme& theme() const { return _theme; }
    const KeyBindings& keys() const { return _keys; }
    
private:
    Configuration();
    Theme _theme;
    KeyBindings _keys;
    
    void load_defaults();
    void parse_toml(const std::string& content);
};
```

## Scope - Configuration File

**Location:**
- Linux/macOS: `~/.config/slayergit/config.toml`
- Windows: `%APPDATA%\slayergit\config.toml`

**Example config.toml:**
```toml
[theme]
background = "#1e1e1e"
foreground = "#d4d4d4"
border = "#3c3c3c"
highlight = "#569cd6"
error = "#f44747"
success = "#4ec9b0"
added = "#587c0c"
deleted = "#c74e39"
modified = "#e2c08d"

[keybindings]
quit = 'q'
refresh = 'F5'
window_1 = '1'
window_2 = '2'
window_3 = '3'
window_4 = '4'
window_5 = '5'
stage = 's'
unstage = 'u'
commit = 'c'
push = 'p'
pull = 'P'

[general]
log_max_count = 100
auto_refresh = false
refresh_interval_ms = 1000
show_refresh_time = true
```

## Acceptance Criteria

- [ ] Config file loaded on startup
- [ ] Default config created if not exists
- [ ] Theme colors applied to all UI elements
- [ ] Custom key bindings work
- [ ] Invalid config file → fallback to defaults + show warning
- [ ] Color format supports: hex (#rrggbb), color names (red, blue, etc.)
- [ ] Can reload config without restarting (optional: Shift+R)

## Implementation Notes

### TOML Parsing

Use a lightweight TOML parser library:
- **toml11** (header-only, C++11) - Recommended
- **cpptoml** (header-only)

Or implement simple INI-style parser (no TOML library needed).

### Configuration::load()

```cpp
void Configuration::load(const std::string& config_path) {
    std::string path = config_path;
    if (path.empty()) {
        path = get_default_config_path(); // ~/.config/slayergit/config.toml
    }

    if (!std::filesystem::exists(path)) {
        load_defaults();
        save(path); // Create default config file
        return;
    }

    try {
        auto config = toml::parse(path);

        // Parse theme
        if (config.contains("theme")) {
            auto theme_table = toml::find(config, "theme");
            _theme.background = parse_color(toml::find<std::string>(theme_table, "background"));
            _theme.foreground = parse_color(toml::find<std::string>(theme_table, "foreground"));
            // ... etc
        }

        // Parse keybindings
        if (config.contains("keybindings")) {
            auto keys_table = toml::find(config, "keybindings");
            _keys.quit = toml::find<std::string>(keys_table, "quit");
            _keys.refresh = toml::find<std::string>(keys_table, "refresh");
            // ... etc
        }

    } catch (const std::exception& e) {
        // Fallback to defaults
        load_defaults();
        std::cerr << "Failed to load config: " << e.what() << std::endl;
    }
}
```

### Color Parsing

```cpp
ftxui::Color parse_color(const std::string& color_str) {
    // Hex format: #rrggbb
    if (color_str[0] == '#' && color_str.length() == 7) {
        int r = std::stoi(color_str.substr(1, 2), nullptr, 16);
        int g = std::stoi(color_str.substr(3, 2), nullptr, 16);
        int b = std::stoi(color_str.substr(5, 2), nullptr, 16);
        return ftxui::Color::RGB(r, g, b);
    }

    // Named colors
    if (color_str == "red") return ftxui::Color::Red;
    if (color_str == "green") return ftxui::Color::Green;
    if (color_str == "blue") return ftxui::Color::Blue;
    // ... etc

    return ftxui::Color::Default;
}
```

### Applying Theme

In every render function:
```cpp
auto& theme = Configuration::instance().theme();

auto line = text(_files[i].path);
if (i == _selected_index) {
    line = line | bgcolor(theme.highlight) | color(theme.foreground);
} else if (_files[i].type == FileStatusType::Modified) {
    line = line | color(theme.modified);
} else if (_files[i].type == FileStatusType::Added) {
    line = line | color(theme.added);
}
```

### Key Binding Mapping

```cpp
bool handle_event(ftxui::Event event) {
    auto& keys = Configuration::instance().keys();

    if (event == ftxui::Event::Character(keys.stage[0])) {
        // Stage file
        return true;
    }

    if (event == ftxui::Event::Character(keys.quit[0])) {
        // Quit
        return true;
    }

    // For F5, need to check special keys:
    if (keys.refresh == "F5" && event == ftxui::Event::F5) {
        // Refresh
        return true;
    }

    return false;
}
```

### Default Config Path

```cpp
std::string get_default_config_path() {
#ifdef _WIN32
    const char* appdata = std::getenv("APPDATA");
    return std::string(appdata) + "\\slayergit\\config.toml";
#else
    const char* home = std::getenv("HOME");
    return std::string(home) + "/.config/slayergit/config.toml";
#endif
}
```

## Success Checklist

- [ ] Config file loads on startup
- [ ] Default config created if missing
- [ ] Theme colors applied to UI
- [ ] Custom key bindings work
- [ ] Invalid config handled gracefully
- [ ] Config file has sensible defaults
- [ ] Documentation for config file format

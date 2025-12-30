# AppBase - CLAUDE.md

## Project Overview

AppBase is a C++14 plugin framework library for building modular, plugin-based applications. Developed for the Hive blockchain ecosystem, it provides:

- Dynamic plugin registration and lifecycle management
- Automatic dependency resolution and initialization ordering
- Command-line arguments and configuration file (.ini) support
- Graceful signal handling (SIGINT, SIGTERM) with proper shutdown sequence
- Boost ASIO-based I/O service for async operations

Used as a submodule in larger projects like Hive. Parent projects must provide `hive_utilities` and `fc` targets before including AppBase.

## Tech Stack

- **Language:** C++14 (clang or g++)
- **Build System:** CMake 2.8.12+
- **Dependencies:**
  - Boost 1.60+ (program_options, ASIO, filesystem, preprocessor)
  - `hive_utilities` (logging, notifications, data collection)
  - `fc` (libfc) - transitively required via hive_utilities

## Directory Structure

```
appbase/
├── CMakeLists.txt           # Main build configuration
├── README.md                # Project documentation
├── application.cpp          # Core application implementation
├── signals_handler.cpp      # Signal handling implementation
├── include/appbase/
│   ├── application.hpp      # Main application class
│   ├── plugin.hpp           # Plugin base class and macros
│   ├── signals_handler.hpp  # Signal handling interface
│   └── shutdown_mgr.hpp     # Shutdown state management
└── examples/
    ├── CMakeLists.txt       # Example build config
    └── main.cpp             # Two-plugin example application
```

## Development Commands

### Building (typically as submodule)

```bash
# In parent project (e.g., Hive)
mkdir build && cd build
cmake ..
make appbase
```

### Building examples

```bash
mkdir build && cd build
cmake -DBUILD_EXAMPLES=ON ..
make appbase_example
```

### CMake Options

- `BUILD_EXAMPLES` - Build example applications
- `ENABLE_COVERAGE_TESTING` - Enable coverage flags
- `FULL_STATIC_BUILD` - Static linking

## Key Files

| File | Purpose |
|------|---------|
| `application.cpp` | Core application class, plugin orchestration, config parsing |
| `signals_handler.cpp` | Async signal handling via Boost ASIO |
| `include/appbase/application.hpp` | Application class, initialization API |
| `include/appbase/plugin.hpp` | Plugin base class, CRTP template, dependency macros |
| `examples/main.cpp` | Working two-plugin example |

## Coding Conventions

### Naming

- `snake_case` for members and functions
- `PascalCase` for classes
- `SCREAMING_SNAKE_CASE` for macros
- Plugin override methods prefixed with `plugin_`

### Plugin Development

```cpp
class my_plugin : public appbase::plugin<my_plugin> {
public:
  // Required: static name function
  static const std::string& name() {
    static std::string n = "my_plugin";
    return n;
  }

  // Declare dependencies
  APPBASE_PLUGIN_REQUIRES((other_plugin))

  // Lifecycle hooks
  void plugin_initialize(const boost::program_options::variables_map& options);
  void plugin_startup();
  void plugin_pre_shutdown();  // Optional, for ordered cleanup
  void plugin_shutdown();

  // Configuration options
  void set_program_options(
    boost::program_options::options_description& cli,
    boost::program_options::options_description& cfg);
};

// Registration macro (in implementation file)
APPBASE_PLUGIN_IMPL(my_plugin)
```

### Plugin Lifecycle

1. **Registered** - Plugin created but not initialized
2. **Initialized** - Config options parsed, state prepared
3. **Started** - Plugin actively running
4. **Pre-Shutdown** - Preparation for shutdown (reverse order)
5. **Stopped** - Plugin shutdown complete

### Shutdown Ordering

Plugins can specify pre-shutdown order via `set_pre_shutdown_order()`:
- `basic_order` (0) - Default
- `p2p_order` (1)
- `webserver_order` (2)
- `colony_order` (3)

### Application Usage

```cpp
appbase::application theApp;
theApp.register_plugin<my_plugin>();

auto result = theApp.initialize(argc, argv);
if (result.should_start_loop()) {
  theApp.startup();
  theApp.wait();  // Blocks until SIGINT/SIGTERM
}
```

### Threading

- Logging runs on separate `fc::thread`
- Signals handled in dedicated thread
- Use `std::mutex` / `std::lock_guard` for state protection
- Signal masking done before logging thread creation

### Error Handling

- Use Boost exceptions with `diagnostic_information`
- FC assertions via `FC_ASSERT` macro
- Standard C++ exceptions where appropriate

## CI/CD Notes

No `.gitlab-ci.yml` in this repo - AppBase is built as part of parent projects (Hive, etc.) through their CI pipelines.

### Integration Testing

Parent projects typically test AppBase indirectly through their plugin implementations and integration tests.

## Command-Line Options

Applications built with AppBase support:

- `--help` - Show all options
- `--version` - Show version
- `--plugin <name>` - Load specific plugins
- `--data-dir` - Set data directory
- `--config` - Set config file path
- `--dump-config` - Output config as JSON
- `--list-plugins` - List available plugins
- `--generate-completions` - Bash completion script (Boost 1.68+)

## Configuration Files

- Default location: `$HOME/.<app_name>/config.ini` or `data-dir/config.ini`
- Auto-generated if missing (commented template)
- CLI flags override config file values

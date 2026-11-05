# CppDemo

A small, modern C++26 project template. The compiler, CMake and every tool run
inside a **Docker dev container**. On your own machine you need only Docker,
Git and (optionally) VS Code. You do not install a C++ compiler.

| Component | Version |
|---|---|
| Compiler | Clang 23 with libc++ 23, building with `-std=c++26` |
| Build | CMake 3.28 + Ninja, configured with `CMakePresets.json` |
| Tests | GoogleTest 1.17.0, run with CTest |
| Quality | Strict warnings, ASan + UBSan, clang-tidy, clang-format |
| Editor | VS Code + clangd, debugging with gdb |
| CI | GitHub Actions, using the same container image |

---

## 1. Prerequisites (one-time machine setup)

Install these three things:

| Tool | macOS | Windows | Linux |
|---|---|---|---|
| **Git** | `xcode-select --install` or `brew install git` | [git-scm.com](https://git-scm.com/download/win) | `sudo apt install git` (or your distro's package) |
| **Docker** | [Docker Desktop](https://www.docker.com/products/docker-desktop/) | [Docker Desktop](https://www.docker.com/products/docker-desktop/) with the WSL 2 backend | [Docker Engine](https://docs.docker.com/engine/install/), then add yourself to the `docker` group: `sudo usermod -aG docker $USER` (log out and back in) |
| **VS Code** *(recommended)* | [code.visualstudio.com](https://code.visualstudio.com/) | same | same |

Then, in VS Code, install the **Dev Containers** extension
(`ms-vscode-remote.remote-containers`). All other extensions are installed
automatically inside the container.

**Check that Docker works:**

```bash
docker run --rm hello-world
```

> **Resources:** the dev image takes about **3 GB** of disk space. On Docker
> Desktop, give Docker at least **4 GB of RAM** (Settings → Resources).
> Intel/AMD (x86-64) and Apple Silicon or other ARM64 machines are both
> supported.

> **Windows:** clone the repo **inside WSL 2** (for example `~/code/...` in
> Ubuntu), not under `C:\`. Builds on a Windows-drive mount are much slower.
> Run `code .` from the WSL terminal.

---

## 2. Get the code

```bash
git clone <[repo-url](https://github.com/mdarade/my_cpp_2026_project.git)> my_cpp_2026_project
cd my_cpp_2026_project
```

---

## 3a. Build and run with VS Code (recommended)

1. Open the folder: `code .`
2. When VS Code shows *"Folder contains a Dev Container configuration file"*,
   click **Reopen in Container**. If it doesn't, open the Command Palette
   (`Cmd/Ctrl+Shift+P`) → **Dev Containers: Reopen in Container**.
   - The **first build takes about 5–10 minutes** while it downloads LLVM.
     Later opens take seconds.
3. CMake Tools asks you to pick a configure preset. Choose **Debug**.
4. Use it:

| Action | How |
|---|---|
| Build | `Cmd/Ctrl+Shift+B` |
| Run the tests | Command Palette → **Tasks: Run Test Task** |
| Run / debug the app | `F5` → **Debug app (gdb)**. Builds first, then stops at your breakpoints |
| Debug the tests | Run and Debug panel → **Debug tests (gdb)** → `F5` |
| Run the full CI check | Command Palette → **Tasks: Run Task** → **CI: full check** |
| Format a file | Save it (format-on-save is enabled) |

clangd provides code completion and go-to-definition, and shows clang-tidy
warnings inline.

### Updating the container

If `.devcontainer/Dockerfile` or `devcontainer.json` changes (for example after
a `git pull`), run **Dev Containers: Rebuild Container**.

---

## 3b. Build and run from the terminal only (no VS Code)

Run these from the repo root on your host machine. They use the same image as
the dev container.

```bash
# 1. Build the toolchain image (first time ~5–10 min, then cached)
docker build -t cpp-dev .devcontainer

# 2. Open a shell inside the container, with the repo mounted
docker run --rm -it \
  --cap-add=SYS_PTRACE --security-opt seccomp=unconfined \
  -v "$PWD":/workspace -w /workspace \
  cpp-dev bash
```

Inside that shell:

```bash
cmake --preset debug              # configure (also downloads GoogleTest)
cmake --build --preset debug      # build
ctest --preset debug              # run unit tests
./build/debug/app                 # run the program
```

Expected output:

```
1
3.14
Hello C++26
```

One-liner without an interactive shell:

```bash
docker run --rm -v "$PWD":/workspace -w /workspace cpp-dev \
  bash -c 'cmake --preset debug && cmake --build --preset debug && ctest --preset debug && ./build/debug/app'
```

---

## 4. Build presets

All presets put their output in `build/<preset>/`.

| Preset | What it's for |
|---|---|
| `debug` | Day-to-day development and debugging (`-g`, no optimization) |
| `release` | Optimized build |
| `asan` | Debug build with AddressSanitizer + UndefinedBehaviorSanitizer. Catches memory errors and undefined behavior at runtime |
| `ci` | `asan` + warnings treated as errors (`-Werror`) + clang-tidy. What CI runs |

```bash
cmake --preset asan && cmake --build --preset asan && ctest --preset asan

# Configure + build + test in one command:
cmake --workflow --preset ci
```

> Don't run an `asan` or `ci` binary under gdb. LeakSanitizer doesn't work
> while a debugger is attached. Use the `debug` preset for debugging.

---

## 5. Before you push: run what CI runs

CI fails on formatting differences, compiler warnings, clang-tidy findings,
sanitizer errors or failing tests. To run the same checks locally, inside the
container:

```bash
# Format all sources in place
cmake --build --preset debug --target format

# Check formatting without changing files (what CI runs)
find include src tests -name '*.[ch]pp' | xargs clang-format --dry-run --Werror

# Full build + tests with sanitizers, -Werror and clang-tidy
cmake --workflow --preset ci
```

In VS Code, this is **Tasks: Run Task → CI: full check**.

The GitHub Actions workflow is in
[.github/workflows/ci.yml](.github/workflows/ci.yml). It runs on every push to
`main` and on every pull request.

---

## 6. Project layout

```
.
├── .devcontainer/
│   ├── Dockerfile           # Toolchain image: Clang 23 + libc++, CMake, Ninja, gdb, clang-tidy/format
│   └── devcontainer.json    # VS Code container config, extensions, settings
├── .github/workflows/ci.yml # CI: format check + build + tests with ASan/UBSan, -Werror and clang-tidy
├── .vscode/
│   ├── launch.json          # F5 debug configurations (app, tests)
│   └── tasks.json           # Build / test / CI tasks
├── include/cppdemo/
│   └── print_all.hpp        # Header-only library: cppdemo::print_all(), built on `template for`
├── src/
│   └── main.cpp             # Application entry point
├── tests/
│   ├── CMakeLists.txt       # Fetches GoogleTest, registers tests with CTest
│   └── print_all_test.cpp   # Unit test
├── .clang-format            # Code style (Google-based)
├── .clang-tidy              # Static-analysis checks
├── .clangd                  # Points clangd at build/debug/compile_commands.json
├── CMakeLists.txt           # Build definition and options
└── CMakePresets.json        # debug / release / asan / ci presets
```

### Adding code

- **New source file for the app:** add it to `add_executable(app ...)` in
  [CMakeLists.txt](CMakeLists.txt).
- **New test file:** add it to `add_executable(print_all_test ...)` in
  [tests/CMakeLists.txt](tests/CMakeLists.txt), or create a new test
  executable. `gtest_discover_tests` registers its tests with CTest
  automatically.

### CMake options

| Option | Default | Effect |
|---|---|---|
| `CPPDEMO_WARNINGS_AS_ERRORS` | `OFF` | Treat compiler and clang-tidy warnings as errors |
| `CPPDEMO_ENABLE_SANITIZERS` | `OFF` | Build with ASan + UBSan |
| `CPPDEMO_ENABLE_CLANG_TIDY` | `OFF` | Run clang-tidy during the build |
| `CPPDEMO_BUILD_TESTS` | `ON` | Build unit tests |
| `CPPDEMO_USE_LIBCXX` | `ON` | Build with LLVM's libc++ instead of GCC's libstdc++ |

The presets set these for you, so you rarely need to pass them by hand.

---

## 7. C++26 support

The project compiles with `-std=c++26` (`CMAKE_CXX_STANDARD 26`). Compilers
are still implementing C++26, so what you can use depends on the toolchain
in the dev container: **Clang 23 + libc++ 23**.

**Used in this repo:** `template for` (expansion statements). It loops over
the elements of a tuple, the members of a struct, or a list of constants. The
loop body is instantiated once per element, so each element keeps its own
type. See `cppdemo::print_all()` in
[print_all.hpp](include/cppdemo/print_all.hpp), which [main.cpp](src/main.cpp)
calls.

```cpp
template for (const auto& element : std::tuple{1, 2.5, "three"}) {
  std::println("{}", element);  // element is int, then double, then const char*
}
```

Clang 23 supports `template for` over tuples, structs and `{...}` lists. It
does **not** yet support it over ranges, such as `std::array` or
`std::vector` ("iterating expansion statements are not yet supported").

**Other C++26 features available in Clang 23:** pack indexing (`args...[I]`),
placeholder variables (`_`), `= delete("reason")`, saturation arithmetic
(`std::add_sat`) and `std::views::concat`.

**Not available yet:** contracts (`pre`/`post`/`contract_assert`), static
reflection (`^^T`, `std::meta`), `std::inplace_vector` and `std::hive`.

**Check whether a feature is available** with its feature-test macro:

```cpp
#include <version>
#if __cpp_lib_inplace_vector
  // use std::inplace_vector
#endif
```

**Upgrade the compiler** when a new LLVM release comes out: change
`ARG LLVM_VERSION=23` in [.devcontainer/Dockerfile](.devcontainer/Dockerfile),
then run **Dev Containers: Rebuild Container**. Nothing else references the
version number.

> **Why libc++?** Clang uses GCC's libstdc++ by default. The one in Ubuntu
> 24.04 (GCC 13) is too old for this project (it has no `<print>`, for
> example). libc++ ships with the same LLVM release as the compiler, so the
> language and the library stay in step.

---

## 8. Troubleshooting

| Problem | Fix |
|---|---|
| `Cannot connect to the Docker daemon` | Start Docker Desktop, or on Linux run `sudo systemctl start docker` |
| `permission denied ... docker.sock` (Linux) | `sudo usermod -aG docker $USER`, then log out and back in |
| `fatal error: 'print' file not found`, or `expected '<' after 'template'` on a `template for` line | You're building outside the container, or with an old image. Use the dev container, or rebuild it |
| `launch: program '.../build/debug/app' does not exist` | Build first (`Cmd/Ctrl+Shift+B`), and check the **Debug** preset is selected |
| clangd shows errors for everything / can't find headers | Run `cmake --preset debug` once. clangd needs `build/debug/compile_commands.json` |
| Configure fails downloading GoogleTest | The first configure needs internet access to github.com. After that it's cached in `build/` |
| Container doesn't have new tools after `git pull` | **Dev Containers: Rebuild Container** |
| `CMakeCache.txt directory ... is different than the directory /workspace/build/debug` | You ran `cmake` on your host machine. Run it inside the dev container (VS Code terminal) or the `docker run` shell from section 3b |
| Strange build errors after switching branches | Delete the build directory: `rm -rf build/` |
| Very slow builds on Windows | Clone the repo inside WSL 2, not on `C:\` |

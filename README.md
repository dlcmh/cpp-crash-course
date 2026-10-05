# cpp-crash-course

A hands-on study of modern C++: each listing lives in
`code-listings/NN-NN/main.cpp`, is built with Clang, and gets its own commit.

| Path | What it is |
|------|------------|
| `code-listings/NN-NN/` | One self-contained listing per folder |
| `build/` | Compiled binaries (git-ignored) |
| `AGENTS.md` | Operating rules for AI coding agents |
| `DECISIONS.md` | Dated log of repo decisions |

---

## VS Code setup and C++ workflow on macOS

This is the setup the repo assumes: **Apple Clang + LLDB** (bundled with the
free Xcode Command Line Tools — no 7 GB Xcode install needed), **clangd** for
IntelliSense and clang-tidy, **CodeLLDB** for debugging, **CMake Tools** for
multi-file projects, and **clang-format** on save. It's the stack most
professional C++ developers converge on for macOS: native, free, and fully
debuggable out of the box.

> Key symbols: ⌘ Command · ⇧ Shift · ⌥ Option · ⌃ Control.

### 1. Install the toolchain

1. Open **Terminal** (⌘Space, type `Terminal`, press Return) and run:

   ```sh
   xcode-select --install
   ```

   Click **Install** in the dialog that appears and accept the license.

2. Verify — both should print Apple versions:

   ```sh
   clang++ --version
   lldb --version
   ```

3. Install [Homebrew](https://brew.sh): paste the one-liner from the website,
   press **Return** when prompted, and enter your password. Then:

   ```sh
   brew install cmake ninja ccache
   ```

   (`brew install llvm` is optional — only needed if you want a standalone
   `clang-tidy` CLI; it's a large download, and clangd already runs clang-tidy
   checks inside the editor.)

4. Sanity check: `cmake --version`.

**Why `brew install cmake ninja ccache`?** For the single-file listings,
strictly speaking, none of the three is needed — `clang++` and `lldb` (from
the Command Line Tools) do all the work. They're installed once because each
maps to a concrete part of this workflow:

- **cmake** powers [the CMake route](#7-multi-file-projects-the-cmake-route).
  The moment you go past a single `main.cpp` or want a third-party library,
  this is the tool that generates the build.
- **ninja** is a fast build executor that CMake generates build files *for*
  once a `CMakeLists.txt` exists. Strictly optional — CMake falls back to
  Makefiles — but it's the usual default: faster incremental rebuilds, cleaner
  output.
- **ccache** is a compiler cache: on rebuild it replays the previous
  compilation from disk instead of recompiling unchanged code. Invisible with
  one-file listings, but in a real project (especially switching Debug/Release)
  it turns multi-second rebuilds into near-instant ones. Wired in via
  `-DCMAKE_CXX_COMPILER_LAUNCHER=ccache` in the CMake section.

If you prefer a minimal setup, `brew install cmake` alone is enough; treat
`ninja` and `ccache` as optional accelerators.

### 2. Install VS Code

Either `brew install --cask visual-studio-code`, or manually: download the
**Mac: Universal** zip from <https://code.visualstudio.com>, double-click it in
Finder, drag **Visual Studio Code** into **Applications**, and launch it
(click **Open** on the first-launch Gatekeeper prompt).

Then enable the `code` command: press **⌘⇧P**, type **Shell Command: Install
'code' command in PATH**, press Return, and authenticate if asked. Now `code .`
opens a folder from any terminal.

### 3. Install the extensions

Click the **Extensions** icon in the Activity Bar (or press **⇧⌘X**), search
for each, and click **Install**:

| Extension | ID | Why |
|-----------|----|-----|
| clangd | `llvm-vs-code-extensions.vscode-clangd` | IntelliSense, navigation, clang-tidy. Far ahead of Microsoft's C/C++ IntelliSense. Downloads its own clangd binary on first run. |
| CodeLLDB | `vadimcn.vscode-lldb` | Debugging with LLDB, the debugger macOS actually supports. |
| CMake Tools | `ms-vscode.cmake-tools` | Configure/build/debug CMake projects from the status bar. |

If Microsoft's **C/C++** extension is installed, uninstall it (or set
`"C_Cpp.intelliSenseEngine": "disabled"`) — running two IntelliSense engines
produces duplicate squiggles and fights over the language server.

### 4. Configure the workspace

`.vscode/` is git-ignored in this repo by design (settings are per-machine);
recreate these four files once per machine. They all live in the repo root.

Open the workspace settings file with **⇧⌘P → "Preferences: Open Workspace
Settings (JSON)"** and paste:

```json
{
  "clangd.arguments": [
    "--background-index",
    "--clang-tidy",
    "--completion-style=detailed",
    "--header-insertion=iwyu"
  ],
  "[cpp]": {
    "editor.defaultFormatter": "llvm-vs-code-extensions.vscode-clangd",
    "editor.formatOnSave": true,
    "editor.rulers": [100]
  },
  "cmake.configureOnOpen": false
}
```

Create **`.clangd`** in the repo root (**File → New Text File…**, then save as
`.clangd`). It supplies flags for files that aren't in a compile database —
i.e. every single-file listing here — and turns up clang-tidy:

```yaml
CompileFlags:
  Add: ["-std=c++20", "-Wall", "-Wextra", "-Wpedantic"]

Diagnostics:
  ClangTidy:
    Add: [bugprone-*, performance-*, modernize-*, readability-*]
```

(`-std=c++20`; use `c++17` if your book targets C++17. This file is tracked by
git, unlike `.vscode/`.)

Create **`.clang-format`** in the repo root — tweak to taste:

```yaml
BasedOnStyle: LLVM
IndentWidth: 4
ColumnLimit: 100
```

Create the build task: **⇧⌘P → "Tasks: Configure Task" → "Create tasks.json
from template" → "Others"**, then replace the file contents with:

```json
{
  "version": "2.0.0",
  "tasks": [
    {
      "label": "build active listing",
      "type": "shell",
      "command": "mkdir -p \"${workspaceFolder}/build/${relativeFileDirname}\" && clang++ -std=c++20 -Wall -Wextra -Wpedantic -g \"${file}\" -o \"${workspaceFolder}/build/${relativeFileDirname}/${fileBasenameNoExtension}\"",
      "group": { "kind": "build", "isDefault": true },
      "problemMatcher": ["$gcc"]
    }
  ]
}
```

It compiles whatever `.cpp` file is focused, with debug symbols, into `build/`
mirroring the source tree (`build/code-listings/01-09/main`).

Create the debug launch config: **⇧⌘P → "Debug: Open launch.json"** — when
asked for an environment, pick **LLDB** — then replace the contents with:

```json
{
  "version": "0.2.0",
  "configurations": [
    {
      "type": "lldb",
      "request": "launch",
      "name": "Debug active listing",
      "program": "${workspaceFolder}/build/${relativeFileDirname}/${fileBasenameNoExtension}",
      "cwd": "${workspaceFolder}",
      "console": "integratedTerminal",
      "preLaunchTask": "build active listing"
    }
  ]
}
```

`"console": "integratedTerminal"` matters: listings like `01-08` read from
stdin, and that's the console mode where typing input works while debugging.

### 5. The daily loop (edit → build → run)

1. From the repo root: `code .`
2. Open the **Explorer** (⇧⌘E). Hover `code-listings` → click **New Folder…**
   → type `01-09` → Return. Hover the new folder → click **New File…** → type
   `main.cpp` → Return.
3. Write the code. clangd completes as you type (force it with **⌃Space**),
   squiggles mark problems, and **⌘.** offers quick fixes. Save with **⌘S** —
   clang-format tidies the file automatically.
4. Build: click into `main.cpp` so it's the focused editor, then press
   **⇧⌘B**. The terminal panel shows the compile; anything the compiler flags
   also lands in the **Problems** view (⇧⌘M).
5. Run: press **⌃`** to focus the terminal, then
   `./build/code-listings/01-09/main` — or press **⌃F5** ("Run Without
   Debugging") to let the launch config build and run it.
6. Commit: click **Source Control** in the Activity Bar (⌃⇧G), hover each
   changed file → click **+** to stage, type `Listing 1-9` as the message,
   press **⌘Enter** to commit, then click **Sync Changes** to push.

The raw-terminal equivalent of steps 4–5, for when VS Code isn't open:

```sh
clang++ -std=c++20 -Wall -Wextra -Wpedantic -g code-listings/01-09/main.cpp \
  -o build/code-listings/01-09/main
./build/code-listings/01-09/main
```

### 6. Debugging

1. Click in the **gutter** to the left of a line number — a red dot marks the
   breakpoint (**F9** toggles one on the focused line).
2. Press **F5**. VS Code builds via the pre-launch task, launches under LLDB,
   and stops at the breakpoint with the line highlighted.
3. The floating **debug toolbar** controls execution: Continue (F5), Step
   Over (F10), Step Into (F11), Step Out (⇧F11), Restart (⇧⌘F5), Stop (⇧F5).
4. Hover any variable to inspect it; the **Variables** pane in the Run and
   Debug sidebar (⇧⌘D) expands everything in scope; add expressions to
   **Watch** with the **+** button.
5. The **Debug Console** tab accepts both C++ expressions and real `lldb`
   commands.
6. Right-click a breakpoint → **Edit Breakpoint…** → *Expression* to make it
   conditional.
7. First time only: macOS may ask you to authorize the debugger — enter your
   password and click **Allow**. And if F5/F10/F11 change brightness or media
   volume instead: **System Settings → Keyboard → Keyboard Shortcuts… →
   Function Keys**, enable **"Use F1, F2 as standard function keys"**.

### 7. Multi-file projects: the CMake route

For anything past a single `main.cpp`:

1. **⇧⌘P → CMake: Quick Start**, pick the folder, name the project, and
   choose **Executable** — CMake Tools writes a starter `CMakeLists.txt`.
2. When prompted, **Select a Kit** and pick the **Clang** entry (run
   **CMake: Scan for Kits** first if the list is empty).
3. The CMake Tools status-bar cluster is now the cockpit: click the **gear**
   icon to build (F7 also works), the **bug** icon to debug, the **play** icon
   to run without debugging. Click the variant shown next to them to switch
   Debug/Release.
4. CMake Tools exports `build/compile_commands.json` automatically; clangd
   finds it by itself and IntelliSense immediately becomes project-accurate.
5. Optional but pro: ccache (installed in step 1) via
   `-DCMAKE_CXX_COMPILER_LAUNCHER=ccache` when configuring, so rebuilds hit
   the cache.

### 8. The quality loop

- **Format**: ⇧⌥F, or automatically on save (already configured above).
- **clang-tidy**: squiggles with ⌘. quick fixes, already enabled through
  clangd. Standalone CLI: `clang-tidy -p build <file>` (requires
  `brew install llvm`; add `/opt/homebrew/opt/llvm/bin` to PATH — on Intel
  Macs, `/usr/local/opt/llvm/bin`).
- **Sanitizers**: build once with `-fsanitize=address,undefined` added to the
  task args (or a CMake preset). Memory errors and undefined behavior now
  abort with a full report at the exact offending line — debug that build
  with F5 as usual.
- **Leaks**: `leaks --atExit -- ./build/code-listings/01-09/main` prints every
  leaked allocation — macOS's native equivalent of LeakSanitizer.

### 9. Shortcut cheat sheet

| Action | Keys |
|--------|------|
| Command Palette | ⌘⇧P |
| Quick-open file | ⌘P |
| Toggle terminal | ⌃` |
| Explorer / Search / Source Control / Run & Debug | ⇧⌘E / ⇧⌘F / ⌃⇧G / ⇧⌘D |
| Problems | ⇧⌘M |
| Extensions | ⇧⌘X |
| Build (default task) | ⇧⌘B |
| Format document | ⇧⌥F |
| Rename symbol | F2 |
| Go to definition / references | F12 / ⇧F12 |
| Quick fix | ⌘. |
| Toggle breakpoint | F9 |
| Debug / continue | F5 |
| Step over / into / out | F10 / F11 / ⇧F11 |
| Run without debugging | ⌃F5 |
| Stop debugging | ⇧F5 |
| Commit in Source Control | ⌘Enter |

### Why this stack (30 seconds)

- **Apple Clang + LLDB**: zero setup, native on Apple Silicon, and the only
  debugger macOS supports without signing gymnastics (GDB does not play well
  here).
- **clangd over Microsoft C/C++**: faster, more accurate IntelliSense, real
  clang-tidy integration, and it's the same LSP server every other editor
  uses.
- **CMake**: the industry-standard build system — every third-party C++
  library speaks it.
- **clang-format + clang-tidy + sanitizers**: mechanical consistency and bugs
  caught before they reach runtime.

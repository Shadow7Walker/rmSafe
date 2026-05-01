# rmSafe

`rmSafe` is a safe, lightweight and fast alternative to the `rm` command. It behaves just like the standard `rm` command in terms of syntax and options, but it moves files and directories to the system's Recycle Bin or Trash instead of permanently deleting them. This provides an essential safety net, preventing accidental loss of important files.

## Features

- **Syntax Compatibility:** Uses the same options as standard `rm` (`-r`, `-f`, `-v`, `-i`, `-d`).
- **Cross-Platform:** Works seamlessly on both Windows and Linux.
- **Lightweight & Fast:** Written entirely in C++17 with zero external dependencies.
- **Native Trash Integration:** 
  - On Windows, it integrates with `IFileOperation`/`SHFileOperation` to support standard Recycle Bin behavior (including restoring).
  - On Linux, it conforms to the FreeDesktop.org (XDG) Trash specification, optionally using `gio trash` or standard tools.

## Options

```
Usage: rmSafe [OPTION]... [FILE]...
Safe rm command that moves files to the recycle bin/trash instead of deleting permanently.

Options:
  -f, --force           ignore nonexistent files and arguments, never prompt
  -i, --interactive     prompt before every removal
  -r, -R, --recursive   remove directories and their contents recursively
  -d, --dir             remove empty directories
  -v, --verbose         explain what is being done
      --help            display this help and exit
```

## How to Build & Package

`rmSafe` uses CMake and CPack to generate cross-platform packages (`.deb`, `.rpm`, `.exe`/`.zip`).

### Prerequisites

- **CMake** (v3.10+)
- **C++ Compiler** supporting C++17 (`g++`, `clang++`, or MSVC)
- On Linux, `dpkg` or `rpmbuild` tools are required to create `.deb` or `.rpm` packages.

### Windows (Generating `.exe` or `.zip`)

Using MSYS2/MinGW, MSVC, or standard Windows developer tools:

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release

# To create a ZIP package containing the executable:
cpack -G ZIP
```
Alternatively, just use the `rmSafe.exe` inside the `build/` directory directly.

### Linux (Generating `.deb` and `.rpm`)

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release

# Generate DEB and RPM packages:
cpack -G "DEB;RPM"
```
This will produce a `.deb` and an `.rpm` file in the `build/` folder which can be installed with `apt`/`dpkg` or `yum`/`rpm` respectively.

## How to use

To ensure maximum safety, you should alias your system's default `rm` command to `rmSafe`. 

We have provided automated setup scripts in the `scripts/` directory to instantly configure this for you!

### Using the Automated Setup Scripts

**For Linux / macOS (Bash, Zsh, Fish):**
Run the shell script. It will automatically detect your shell and apply the alias to your `~/.bashrc`, `~/.zshrc`, or `config.fish`.
```bash
bash ./scripts/setup_alias_linux.sh
```

**For Windows (PowerShell):**
Run the PowerShell script. It will automatically add the alias to your global `$PROFILE`.
```powershell
.\scripts\setup_alias_windows.ps1
```

*(Note: After running the scripts, please restart your terminal or source your profile to apply the changes.)*

### Manual Setup
If you prefer to set the alias manually:

**Bash / Zsh:**
Add this to your `~/.bashrc` or `~/.zshrc`:
```bash
alias rm="rmSafe"
```

**PowerShell:**
Add this to your `$PROFILE`:
```powershell
Set-Alias -Name rm -Value rmSafe
```

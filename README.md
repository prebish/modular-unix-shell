
# Custom Unix Shell

A custom Unix shell program that supports plugin-based extension for additional commands.

### Table of Contents

- [Installation & Setup](#installation--setup)
    1. [Clone the Repository](#1-clone-the-repository)
    2. [Compile the Program](#2-compile-the-program)
- [Usage](#usage)
    - [Loading Plugins](#loading-plugins)
    - [Executing Commands](#executing-commands)
    - [Exiting the Shell](#exiting-the-shell)
- [License](#license)

## Installation & Setup

Make sure you have a C compiler (e.g., GCC) installed on your system.  
You can install it through your package manager or download it [**here**](https://gcc.gnu.org/install/).

### 1. Clone the Repository
```bash
git clone https://github.com/prebish/custom-unix-shell.git
```

### 2. Compile the Program
```bash
cd ./custom-unix-shell
gcc -o csh csh.c -ldl
```

## Usage

This is the baseline of what you'll use to run the shell. Try running it yourself.

```bash
./csh
```

### Loading Plugins

You can dynamically load plugins that extend the shell's functionality.

#### Load a plugin
```bash
load plugin_name
```
This command attempts to load a plugin named `plugin_name.so` located in the current directory. A sample one is provided to you in the repository.

### Executing Commands

#### Run a loaded plugin
```bash
plugin_name [args...]
```
This will execute the `run` function of the loaded plugin with the provided arguments.

#### Run a system command
```bash
ls -l
```
You can execute standard Unix commands like `ls`, `grep`, `cat`, etc., directly from the shell.

### Exiting the Shell

#### To exit the shell and clean up resources
```bash
exit
```

## License

This project is licensed under the **MIT License** - see the `LICENSE` file for details.

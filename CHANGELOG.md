# Umoria Change log



## 5.7.1 (2017-01-27)

- Improved CLI: adds _help_ and _version_ information (`-h` and `-v`).
- Lots of spelling fixes, mostly source code comments, but a few in-game also.
- Updates to the manual, FAQ, and historical/errors documents.

- Compiles to C++, with all warnings fixed! (`-Wall`, `-Wextra`, `-Werror`, `-Wshadow`)
- Now uses CMake for Mac/Linux build - Windows/MinGW still uses a normal Makefile.



## 5.7.0 (2016-11-27)

Lots of code clean-up and standardization, along with removing support for
outdated systems. The main feature of this release is support for Windows
and macOS.

### Notable changes

- **Windows**, **macOS** and **Linux** support.
- Renaming binary from `moria` to `umoria`, save file to `game.sav`,
  and scores to `scores.dat`.
- Use `clang-format`/`-tidy` to clean up the code formatting.
- Moves all standard library header includes into one file.
- Replaces custom types (e.g. `int8u`, `int16u`, etc.) with their equivalent
  C standard types.
- Introduce the `_Bool` type.
- Converts deprecated K&R style function declarations.
- Refactor all comments.
- Reorganise all old document files, create a `historical` directory.

### Deprecated

- Remove support for discontinued computers and OS: Atari ST, Amiga, MS DOS,
  "Classic" Mac OS (pre OS X), VMS, System III, etc., etc.



## 5.6.0 (2015-02)

Umoria is released under a new GPL v2 license. More information is available
on the [free-moria](http://free-moria.sourceforge.net/) website.

All previous changes can be found in the [historical/CHANGELOG](historical/CHANGELOG).
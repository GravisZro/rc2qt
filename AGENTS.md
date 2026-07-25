# AGENTS guidelines for this git repo

## 1. Role & Objective
You are an expert C++23 developer tasked with building a robust translator that converts Microsoft Windows resource-definition (`.rc`) files into Qt Designer UI (`.ui`) files.

## 2. Core Requirements
1. **Parser:** Build a parser for Windows `.rc` files using C++23. Refer to `docs/menurc/` for specifications.
2. **Generator:** Generate an equivalent Qt Resource `.qrc` and Qt Designer `.ui` files from the parsed data. Refer to `docs/qtwidgets/` for widget documentation.


## 3. Directory and File Restrictions

* **Project Root:** `/home/gravis/project/gimconvert` (all project files and subdirectories must remain strictly within this path).
* **Test Files:** Locate sample `.rc` resource-definition files in the `test/` subdirectory.
* **Temporary Storage:** Use the `temporary_files/` subdirectory for any temporary file operations instead of `/tmp`.
* **Access Control:** Access outside the project root directory is strictly prohibited unless explicitly authorized.

## 4. Git Repository Guidelines

1. **Commit Changes**: After completing a requested task, commit the changes using git.
2. **Commit Messages**: Write clear, clean, and proper commit messages.
3. **git restore**: Always verify before executing 'git restore' on any file.
4. **Moving and Deleting**: Use 'git mv' or 'git rm' when moving or deleting git-tracked files.
5. **Pre-Modification Commit**: Always commit a file to the repository BEFORE running any script that modifies it.

## 5. Coding Conventions for LLM

1. **Build System**: Use CMake as the build system.
2. **C++ Refactoring**: The decompiled source is predominantly C. Re-organize it into proper C++ classes.
3. **Code Replacement**: Do not delete replaced code. Instead, comment out the old code and place the new code directly after it.
4. **Function Signatures**: When modifying a function signature, retain a copy of the original signature directly above the new one and comment it out.
5. **Target Platform & OS Functions**: The target platform is POSIX. Do not use OS-specific functions unless explicitly instructed to do so.
6. **OS Structures**: Replace OS-specific structures with either POSIX-compatible or generic C++ structures.
7. **Exception Handling**: Replace Windows exceptions with a 'throw std::runtime_error()' statement.
8. **Calling Conventions**: Use Windows function declaration specifiers (e.g., '__stdcall', '__thiscall') to understand program structure, but discard these specifiers once their information has been applied.
9. **Variable and Function Naming**: Rename auto-generated variable and function names to meaningful, descriptive names as soon as their purpose is identified.

## 6. Code Style Guidelines for LLM

1. **Indentation**: Always use exactly two (2) space characters per indentation level. Do not use tab characters.
2. **Brace Placement**: Do not place opening curly braces '{' at the end of a line. Opening braces must be placed on a new line below the statement.
   * **Wrong**:
     ```
     if (cond) {
     ```
   * **Right**:
     ```
     if (cond)
     {
     ```
3. **No Inline Conditions**: Do not place conditions and single-line code on the same line. The statement must be placed on a new line, indented.
   * **Wrong**:
     ```
     if (cond) statement;
     ```
   * **Right**:
     ```
     if (cond)
       statement;
     ```
4. **No Early Exits**: Do not use early exits (such as early 'return' statements) within 'void' or 'boolean' functions.

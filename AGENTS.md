# AGENTS guidelines for this git repo

## 1. Goal and documentation

* Goal: The purpose of this project is to construct a parser for Microsoft Windows resource-definition files in C++23.
* Documentation: Documentation all about how Windows resources are defined and used are located in menurc directory.

## 2. Do NOT ask for permission outside of the project directory

* The project dir is /home/gravis/project/gimconvert
* All files must be kept in project directory
* The entire project and test files are located within project dir and subdirs.
* The subdir test/ contains .rc (resource-definition) files to test with
* The subdir temporary_files/ is what you will use in place of /tmp
* Access outside of the project dir is prohibited unless you are explictly instructed to.

## 3. The project directory is a git repository, use git

* After you have completed a requested task, commit the changes using git.
* Write proper commit messages for all commits.
* Always check before using `git restore` on a file.
* Use `git mv`/`git rm` when moving/deleting git tracked files.
* Always commit a file to the repo BEFORE using a script to modify it.

## 4. Coding Conventions

* The build system used is CMake
* The target platform is POSIX, do not use OS specific functions unless explictly instructed to.
* OS specific structures should be replaced with either POSIX, Qt, or generic C++ structures.

# Contributing

## Dependencies

- CMake > 3.14
- Build generator like Make or Ninja
- C++17 compiler
- Qt6::Core, Qt6::Network, Qt6::Test
- [cmake-format](https://pypi.org/project/cmakelang/)
- [clang-format](https://llvm.org/)

## Getting Started

```shell
git clone https://github.com/le0d4v1nc1/qconsole
cd qconsole
mkdir build
cd build
cmake -DQCONSOLE_BUILD_EXAMPLES=ON
      -DQCONSOLE_BUILD_TESTS=ON
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
      -DCMAKE_BUILD_TYPE=Debug ..
make all
```

## Code Style

Just run `cmake-format` and `clang-format` after modifying the source code to be consistent.

## Pull Requests

Examples of good commit messages:

- `Add a helper command, fixes #44`
- `Correct documentation format`
- `Add tests for QConsole::addCommand, fixes #1`

## Copyright

You retain your copyright when submitting changes so please append your name to the license header.

# QConsole

QConsole is a high-level C++17 REPL library for Qt6 console applications with support for Linux, MacOS, and Windows. It features syntax-highlighting, history, unicode, default commands (help, version, exit, etc...), native keybindings, hints, password input, auto-complete, and colorization.

## Usage

Here is a simple demonstration:

```cpp
QConsole console;
console.addDefaultCommands();
console.setHistoryFilePath("history.txt");
console.setDefaultPrompt(QConsole::colorize(">> ", QConsole::Color::Red, QConsole::Style::Bold));

console.addCommand({
  "hello-world",
  "Print 'Hello, world!' and the arguments given to the command.",
  [&](const QConsole::Context& ctx) {
      console.ostream() << "Hello, World! Args: " << ctx.arguments << Qt::endl;
  },
});

console.start();
```

See the [simple example](./examples/example-simple) for the above and the [complex example](./examples/example-complex) for a more involved application. There is also the [widget-example](./examples/example-widgets) demonstrating the usage of a `QConsole` alongside a `QGuiApplication` or `QApplication`.

Do not use asynchronous code to output messages with this library. Instead, wrap your signals with `QEventLoop` (like in the complex example) or use `QFuture` to make your asynchronous methods or functions synchronous.

## Dependencies

The following libraries should be found on your system:

- [qt6-base (LGPL)](https://code.qt.io/cgit/qt/qtbase.git/)

The following libraries will be automatically downloaded by CMake:

- [replxx (BSD)](https://github.com/AmokHuginnsson/replxx)
- [hat-trie (MIT)](https://github.com/Tessil/hat-trie.git)

## Installation

You can install this library by running the following commands:

```shell
git clone https://github.com/le0d4v1nc1/qconsole
cd qconsole
cmake -S . -B build --target install --config Release
```

Or if you want to include it in your your own CMake project:

```cmake
include(FetchContent)
fetchcontent_declare(qconsole GIT_REPOSITORY https://github.com/le0d4v1nc1/qconsole/ GIT_TAG 2.0.2)
fetchcontent_makeavailable(qconsole)
```

## Contributions

This project accepts pull-requests, bug-reports, and/or feature-requests; see [CONTRIBUTING](./CONTRIBUTING.md).

## License

This project is available under the MIT license.

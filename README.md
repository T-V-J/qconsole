# QConsole

QConsole is a high-level C++17 REPL library for Qt6 console applications with support for Linux, MacOS, and Windows. It features syntax-highlighting, command history, unicode, help information, default keybindings, hints, password input, completions, and colorization.

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

See the [simple example](./examples/example-simple) for the above and the [complex example](./examples/example-complex) for a more involved application. There's also the [widget-example](./examples/example-widgets) demonstrating the usage of the console alongside a `QGuiApplication/QApplication`.

Also, note that you shouldn't be using asynchronous code to output messages with this library. Wrap your network signal connections with a `QEventLoop` or use a `QFuture` otherwise you'll end up with output that isn't synchronized with the current command.

## Dependencies

The following libraries should be found on your system:

- [qt6-base (LGPL)](https://code.qt.io/cgit/qt/qtbase.git/)

The following libraries will be automatically downloaded by CMake:

- [replxx (BSD)](https://github.com/AmokHuginnsson/replxx)
- [hat-trie (MIT)](https://github.com/Tessil/hat-trie.git)

## Installation

You can install the library by running the following commands:

```shell
git clone https://github.com/le0d4v1nc1/qconsole
cd qconsole
cmake -S . -B build --target install --config Release
```

or alternatively to include it in your own CMake project:

```cmake
include(FetchContent)
fetchcontent_declare(qconsole GIT_REPOSITORY https://github.com/le0d4v1nc1/qconsole/ GIT_TAG 2.0.1)
fetchcontent_makeavailable(qconsole)
```

## Contributions

Contributions or feature-requests are quite welcome. See the [CONTRIBUTING](./CONTRIBUTING.md) file for instructions on how to submit your changes to this project.

## License

This software is available under the MIT license.

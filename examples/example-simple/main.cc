// Copyright (c) 2021 Leonardo da Vinci
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

#include <QConsole>
#include <QtCore/QCoreApplication>

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    app.setApplicationVersion("1.0.0");
    app.setApplicationName("example");
    app.setOrganizationName("example");
    app.setOrganizationDomain("example");

    QConsole c;
    c.addDefaultConfiguration();
    c.addDefaultCallbacks();
    c.addDefaultKeybindings();
    c.addDefaultCommands();
    c.setHistoryFilePath("history.txt");
    c.setDefaultPrompt(QConsole::colorize(">> ", QConsole::Color::Red));

    c.addCommand({
      "hello-world",
      "Print 'Hello, world!' and arguments.",
      [&](const QConsole::Context& ctx) {
          c.ostream() << "Hello, World! Arguments:" << ctx.arguments.join(" ") << Qt::endl;
      },
    });

    c.start();

    return app.exec();
}

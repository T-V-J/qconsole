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
#include <QtCore/QThread>
#include <QtWidgets/QApplication>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    app.setApplicationVersion("1.0.1");
    app.setApplicationName("example");
    app.setOrganizationName("example");
    app.setOrganizationDomain("example");

    // We create a new thread for QConsole to reside in to avoid blocking
    // the main thread.
    auto thread = new QThread();

    auto c = new QConsole();
    c->start(); // Start before moving to a new thread.
    c->moveToThread(thread);
    c->addDefaultConfiguration();
    c->addDefaultCallbacks();
    c->addDefaultKeybindings();
    c->addDefaultCommands();
    c->setHistoryFilePath("history.txt");
    c->setDefaultPrompt(QConsole::colorize(">> ", QConsole::Color::Red));

    auto widget     = new QWidget();
    auto mainLayout = new QVBoxLayout;
    widget->setLayout(mainLayout);

    auto textEdit = new QTextEdit();
    textEdit->setText("You can type text in here and then switch back to the REPL!");
    mainLayout->addWidget(textEdit);

    widget->show();

    c->addCommand({
      "set-text",
      "Change the text of the QTextEdit!",
      [&](const QConsole::Context& ctx) {
          // Note that we have to invoke the method from the main thread since our current
          // context is the QConsole thread.
          QMetaObject::invokeMethod(
            textEdit, [=]() { textEdit->setText(ctx.arguments.join(" ")); }, Qt::QueuedConnection);
      },
    });

    // Start the QConsole thread.
    thread->start();

    return app.exec();
}

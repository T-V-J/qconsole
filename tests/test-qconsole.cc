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

#include "test-qconsole.h"

#include <QConsole>
#include <QtTest/QtTest>

void QConsoleTester::populateTest()
{
    QConsole console;

    for (int i = 0; i < 100; ++i) {
        console.addCommand({
          QString::number(i),
          "Random description...",
          [](const QConsole::Context& ctx) { Q_UNUSED(ctx) },
        });
    }

    QVERIFY(console.commandCount() == 100);

    for (int i = 0; i < 100; ++i) {
        console.removeCommandByName(QString::number(i));
    }

    QVERIFY(console.commandCount() == 0);
}

void QConsoleTester::populateBenchmark()
{
    QConsole console;

    QBENCHMARK
    {
        for (int i = 0; i < 10000; ++i) {
            auto name = QString::number(i);
            console.addCommand({
              name,
              "Random description...",
              [](const QConsole::Context& ctx) { Q_UNUSED(ctx) },
            });

            console.removeCommandByName(name);
        }
    }
}

void QConsoleTester::colorizeTest()
{
    QVERIFY(QConsole::colorize("Test", QConsole::Color::Cyan, QConsole::Style::Normal) == "\33[0;36mTest\33[0m");
    QVERIFY(QConsole::colorize("Test", QConsole::Color::Blue, QConsole::Style::Normal) == "\33[0;34mTest\33[0m");
    QVERIFY(QConsole::colorize("Test", QConsole::Color::Cyan, QConsole::Style::Bold) == "\33[1;36mTest\33[0m");
}

void QConsoleTester::unicodeTest()
{
    QConsole console;

    bool check = false;

    console.addCommand({
      "Ā ā Ă ă Ą ą Ć ć Ĉ ĉ Ċ ċ Č č Ď ď Đ đ Ē ē Ĕ ĕ Ė ė Ę ę Ě ě Ĝ ĝ Ğ ğ Ġ ġ Ģ ģ Ĥ ĥ Ħ ħ Ĩ ĩ Ī ī Ĭ ĭ Į "
      "į İ ı Ĳ ĳ Ĵ ĵ Ķ ķ ĸ Ĺ ĺ Ļ ļ Ľ ľ Ŀ ŀ Ł ł Ń ń Ņ ņ Ň ň ŉ Ŋ ŋ Ō ō Ŏ ŏ Ő ő Œ œ Ŕ ŕ Ŗ ŗ Ř ř Ś ś Ŝ ŝ "
      "Ş ş Š š Ţ ţ Ť ť Ŧ ŧ Ũ ũ Ū ū Ŭ ŭ Ů ů Ű ű Ų ų Ŵ ŵ Ŷ ŷ Ÿ Ź ź Ż ż Ž ž ſ",
      "أنا قادر على أكل الزجاج و هذا لا يؤلمني",
      [&check](const QConsole::Context& ctx) {
          Q_UNUSED(ctx)
          check = true;
      },
    });

    console.invokeCommandByName(
      "Ā ā Ă ă Ą ą Ć ć Ĉ ĉ Ċ ċ Č č Ď ď Đ đ Ē ē Ĕ ĕ Ė ė Ę ę Ě ě Ĝ ĝ Ğ ğ Ġ ġ Ģ ģ Ĥ ĥ Ħ ħ Ĩ ĩ Ī ī Ĭ ĭ Į "
      "į İ ı Ĳ ĳ Ĵ ĵ Ķ ķ ĸ Ĺ ĺ Ļ ļ Ľ ľ Ŀ ŀ Ł ł Ń ń Ņ ņ Ň ň ŉ Ŋ ŋ Ō ō Ŏ ŏ Ő ő Œ œ Ŕ ŕ Ŗ ŗ Ř ř Ś ś Ŝ ŝ "
      "Ş ş Š š Ţ ţ Ť ť Ŧ ŧ Ũ ũ Ū ū Ŭ ŭ Ů ů Ű ű Ų ų Ŵ ŵ Ŷ ŷ Ÿ Ź ź Ż ż Ž ž ſ");

    QVERIFY(check == true);

    console.addCommand({
      "나는 유리를 먹을 수 있어요. 그래도 아프지 않아요",
      "أنا قادر على أكل الزجاج و هذا لا يؤلمني",
      [&check](const QConsole::Context& ctx) {
          Q_UNUSED(ctx);
          check = false;
      },
    });

    console.invokeCommandByName("나는 유리를 먹을 수 있어요. 그래도 아프지 않아요");

    QVERIFY(check == false);
}

void QConsoleTester::evaluateBenchmark()
{
    QConsole console;

    QBuffer buffer;
    buffer.open(QBuffer::WriteOnly);

    console.setOutputDevice(&buffer);

    for (int i = 0; i < 1000; ++i) {
        console.addCommand({
          QString::number(i),
          "Random description...",
          [](const QConsole::Context& ctx) { Q_UNUSED(ctx) },
        });
    }

    console.addCommand({
      "long-random-value-command-name-long-random-value-command-name-long-random-value-command-name-long-random-value-"
      "command-name",
      "Random description...",
      [](const QConsole::Context& ctx) {
          Q_UNUSED(ctx)
          int a = 0;
          a += 1;
      },
    });

    QBENCHMARK
    {
        for (int i = 0; i < 10000; ++i) {
            console.invokeCommandByName("long-random-value-command-name-long-random-value-command-name-long-random-"
                                        "value-command-name-long-random-value-command-name");
        }
    }
}

void QConsoleTester::promptTest()
{
    QConsole console;

    console.setDefaultPrompt("ExamplePrompt");

    QVERIFY(console.defaultPrompt() == "ExamplePrompt");
    QVERIFY(console.prompt() == console.defaultPrompt());

    qCritical() << console.defaultPrompt();

    console.setPrompt("NewPrompt");

    qCritical() << console.defaultPrompt();

    QVERIFY(console.prompt() == "NewPrompt");
    QVERIFY(console.defaultPrompt() == "ExamplePrompt");

    console.resetPrompt();

    QVERIFY(console.prompt() == console.defaultPrompt());
}

QTEST_MAIN(QConsoleTester);

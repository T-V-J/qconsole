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

#include "qconsole.h"

#include <stdio.h>
#include <tsl/htrie_map.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>
#include <QtCore/QTimer>
#include <algorithm>
#include <regex>
#include <replxx.hxx>

#ifdef Q_OS_WIN32
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

#include <QtCore/QThread>

using namespace replxx;

class QConsole::Trie : public tsl::htrie_map<char, QConsole::Command>
{
};

class QConsole::Terminal : public replxx::Replxx
{
};

QConsole::QConsole(QObject* parent)
  : QObject(parent)
  , m_commands(new Trie())
  , m_terminal(new Terminal())
  , m_echo(true)
  , m_running(false)
  , m_ostream(stdout)
{
    m_terminal->set_max_hint_rows(0);
    m_commands->burst_threshold(0);
    m_commands->max_load_factor(1.0);
}

void QConsole::start()
{
    if (!m_running) {
        m_timerID = startTimer(0, Qt::TimerType::CoarseTimer);
        m_running = true;
        m_terminal->install_window_change_handler();
    }
}

void QConsole::stop()
{
    if (m_running) {
        killTimer(m_timerID);
        m_running = false;
    }
}

bool QConsole::running()
{
    return m_running;
}

QConsole::~QConsole()
{
    if (m_running) {
        m_terminal->invoke(Replxx::ACTION::CLEAR_SELF, 0);
    }

    if (!m_historyFilePath.empty()) {
        m_terminal->history_save(m_historyFilePath);
    }

    if (!m_echo) {
        setStdinEcho(true);
    }

    delete m_terminal;
    delete m_commands;
}

void QConsole::setOutputDevice(QIODevice* device)
{
    m_ostream.device()->close();
    m_ostream.setDevice(device);
}

bool QConsole::invokeCommandByName(const QString& name, const Context& ctx)
{
    if (const auto c = findCommandByName(name.toStdString()); c != nullptr) {
        c->invoke(ctx);
        return true;
    }

    return false;
}

void QConsole::evaluateLine(const char* line)
{
    const auto a = QString(line).trimmed();

    if (a.isEmpty()) {
        return;
    }

    const auto tokens = a.split(' ');
    const auto name   = tokens[0].toStdString();

    m_terminal->history_add(a.toStdString());

    if (const auto c = findCommandByName(name); c != nullptr) {
        return c->invoke(Context{ .arguments = tokens.mid(1) });
    }

    m_ostream << QConsole::colorize(QStringLiteral("Command not found: ").append(tokens[0]), QConsole::Color::Red,
                                    QConsole::Style::Normal)
              << Qt::endl;
}

void QConsole::timerEvent(QTimerEvent* event)
{
    Q_UNUSED(event);

    // Read user input...
    const auto input = m_terminal->input(m_prompt);

    // Handle EOF (ctrl+d)
    if (input == nullptr) {
        QCoreApplication::quit();
    }

    return evaluateLine(input);
}

void QConsole::setMaxHistorySize(int size)
{
    m_terminal->set_max_history_size(size);
}

void QConsole::setWordBreakCharacters(const char* characters)
{
    m_terminal->set_word_break_characters(characters);
}

void QConsole::setCompletionCountCutoff(int cutoff)
{
    m_terminal->set_completion_count_cutoff(cutoff);
}

void QConsole::setDoubleTabCompletion(bool complete)
{
    m_terminal->set_double_tab_completion(complete);
}

void QConsole::setCompleteOnEmpty(bool complete)
{
    m_terminal->set_complete_on_empty(complete);
}

void QConsole::setBeepOnAmbiguousCompletion(bool beep)
{
    m_terminal->set_beep_on_ambiguous_completion(beep);
}

void QConsole::setNoColor(bool color)
{
    m_terminal->set_no_color(color);
}

void QConsole::setUniqueHistory(bool unique)
{
    m_terminal->set_unique_history(unique);
}

void QConsole::addDefaultConfiguration()
{
    setMaxHistorySize(10000);
    setWordBreakCharacters(" \t,%!;:=*~^'\"/?<>|[](){}");
    setCompletionCountCutoff(256);
    setDoubleTabCompletion(false);
    setCompleteOnEmpty(true);
    setBeepOnAmbiguousCompletion(true);
    setNoColor(false);
    setUniqueHistory(true);
}

void QConsole::addDefaultKeybindings()
{
    m_terminal->bind_key_internal(Replxx::KEY::BACKSPACE, "delete_character_left_of_cursor");
    m_terminal->bind_key_internal(Replxx::KEY::DELETE, "delete_character_under_cursor");
    m_terminal->bind_key_internal(Replxx::KEY::LEFT, "move_cursor_left");
    m_terminal->bind_key_internal(Replxx::KEY::RIGHT, "move_cursor_right");
    m_terminal->bind_key_internal(Replxx::KEY::UP, "history_previous");
    m_terminal->bind_key_internal(Replxx::KEY::DOWN, "history_next");
    m_terminal->bind_key_internal(Replxx::KEY::PAGE_UP, "history_first");
    m_terminal->bind_key_internal(Replxx::KEY::PAGE_DOWN, "history_last");
    m_terminal->bind_key_internal(Replxx::KEY::HOME, "move_cursor_to_begining_of_line");
    m_terminal->bind_key_internal(Replxx::KEY::END, "move_cursor_to_end_of_line");
    m_terminal->bind_key_internal(Replxx::KEY::TAB, "complete_line");
    m_terminal->bind_key_internal(Replxx::KEY::control(Replxx::KEY::LEFT), "move_cursor_one_word_left");
    m_terminal->bind_key_internal(Replxx::KEY::control(Replxx::KEY::RIGHT), "move_cursor_one_word_right");
    m_terminal->bind_key_internal(Replxx::KEY::control(Replxx::KEY::UP), "hint_previous");
    m_terminal->bind_key_internal(Replxx::KEY::control(Replxx::KEY::DOWN), "hint_next");
    m_terminal->bind_key_internal(Replxx::KEY::control(Replxx::KEY::ENTER), "commit_line");
    m_terminal->bind_key_internal(Replxx::KEY::control('R'), "history_incremental_search");
    m_terminal->bind_key_internal(Replxx::KEY::control('W'), "kill_to_begining_of_word");
    m_terminal->bind_key_internal(Replxx::KEY::control('U'), "kill_to_begining_of_line");
    m_terminal->bind_key_internal(Replxx::KEY::control('K'), "kill_to_end_of_line");
    m_terminal->bind_key_internal(Replxx::KEY::control('Y'), "yank");
    m_terminal->bind_key_internal(Replxx::KEY::control('L'), "clear_screen");
    m_terminal->bind_key_internal(Replxx::KEY::control('D'), "send_eof");
    m_terminal->bind_key_internal(Replxx::KEY::control('C'), "abort_line");
    m_terminal->bind_key_internal(Replxx::KEY::control('T'), "transpose_characters");
    m_terminal->bind_key_internal(Replxx::KEY::control('N'), "history_next");
    m_terminal->bind_key_internal(Replxx::KEY::control('P'), "history_previous");
#ifndef Q_OS_WIN32
    m_terminal->bind_key_internal(Replxx::KEY::control('V'), "verbatim_insert");
    m_terminal->bind_key_internal(Replxx::KEY::control('Z'), "suspend");
#endif
    m_terminal->bind_key_internal(Replxx::KEY::meta(Replxx::KEY::BACKSPACE), "kill_to_whitespace_on_left");
    m_terminal->bind_key_internal(Replxx::KEY::meta('p'), "history_common_prefix_search");
    m_terminal->bind_key_internal(Replxx::KEY::meta('n'), "history_common_prefix_search");
    m_terminal->bind_key_internal(Replxx::KEY::meta('d'), "kill_to_end_of_word");
    m_terminal->bind_key_internal(Replxx::KEY::meta('y'), "yank_cycle");
    m_terminal->bind_key_internal(Replxx::KEY::meta('u'), "uppercase_word");
    m_terminal->bind_key_internal(Replxx::KEY::meta('l'), "lowercase_word");
    m_terminal->bind_key_internal(Replxx::KEY::meta('c'), "capitalize_word");
    m_terminal->bind_key_internal(Replxx::KEY::INSERT, "toggle_overwrite_mode");
}

void QConsole::addDefaultCallbacks()
{
    m_terminal->set_hint_callback([this](std::string const& input, int& input_length, Replxx::Color& color) {
        if (input_length > 0) {
            if (const auto& pr = m_commands->equal_prefix_range(input); pr.first != pr.second) {
                color = Replxx::Color::BROWN;
                return Replxx::hints_t({ pr.first.key() });
            }
        }

        return Replxx::hints_t();
    });

    m_terminal->set_completion_callback([this](const std::string& input, int& input_length) {
        Q_UNUSED(input_length);

        Replxx::completions_t completions;

        const auto& pr = m_commands->equal_prefix_range(input);
        for (auto iter = pr.first; iter != pr.second; ++iter) {
            completions.emplace_back(Replxx::Completion(iter.key(), Replxx::Color::BROWN));
        }

        return completions;
    });

    m_terminal->set_highlighter_callback([this](const std::string& input, Replxx::colors_t& colors) {
        auto prefixHighlightLength = 0;
        auto endOfWord = input.find(" "); // Check to see if we have multiple words (i.e. command + arguments)

        if (endOfWord == -1ul) {
            if (const auto c = findCommandByName(input); c != nullptr) {
                prefixHighlightLength = input.length();
            }
        } else {
            if (const auto c = findCommandByName(input.substr(0, endOfWord)); c != nullptr) {
                prefixHighlightLength = endOfWord;
            }
        }

        for (auto i = 0; i < prefixHighlightLength; i++) {
            colors.at(i) = Replxx::Color::BRIGHTGREEN;
        }
    });
}

size_t QConsole::commandCount()
{
    return m_commands->size();
}

void QConsole::addDefaultCommands()
{
    addCommand({
      "exit",
      "Exit the application.",
      [](const Context& ctx) {
          Q_UNUSED(ctx);
          QCoreApplication::quit();
      },
    });

    addCommand({
      "help",
      "Print help information.",
      [this](const Context& ctx) {
          Q_UNUSED(ctx);

          m_ostream << "\nList of commands:\n\n";

          for (auto iter = m_commands->begin(); iter != m_commands->end(); ++iter) {
              m_ostream << QConsole::colorize(iter->name, QConsole::Color::Green) << ": " << iter->description << "\n";
          }

          m_ostream << "\nUsage: <command> [arguments...]\n\n";
          m_ostream.flush();
      },
    });

    addCommand({
      "history",
      "Print command history.",
      [this](const Context& ctx) {
          Q_UNUSED(ctx);

          Replxx::HistoryScan hs(m_terminal->history_scan());

          for (auto i = 0; hs.next(); i++) {
              m_ostream << qSetFieldWidth(4) << i << qSetFieldWidth(0) << " "
                        << QConsole::colorize(QString::fromStdString(hs.get().timestamp()), QConsole::Color::Blue)
                        << " " << hs.get().text().c_str() << "\n";
          }

          m_ostream.flush();
      },
    });

    addCommand({
      "clear",
      "Clear the screen.",
      [this](const Context& ctx) {
          Q_UNUSED(ctx);
          m_terminal->clear_screen();
      },
    });

    addCommand({
      "version",
      "Print the application version.",
      [](const Context& ctx) {
          Q_UNUSED(ctx);
          QTextStream(stdout) << QCoreApplication::applicationVersion() << Qt::endl;
      },
    });
}

void QConsole::addCommand(const Command& c)
{
    m_commands->insert(c.name.toStdString(), c);
}

void QConsole::removeCommandByName(const QString& name)
{
    m_commands->erase(name.toStdString());
}

void QConsole::setPrompt(const QString& prompt)
{
    m_prompt = prompt.toStdString();
}

void QConsole::setDefaultPrompt(const QString& prompt)
{
    m_defaultPrompt = prompt.toStdString();
    m_prompt        = m_defaultPrompt;
}

void QConsole::resetPrompt()
{
    m_prompt = m_defaultPrompt;
}

const QString QConsole::historyFilePath()
{
    return QString::fromStdString(m_historyFilePath);
}

const QString QConsole::defaultPrompt()
{
    return QString::fromStdString(m_defaultPrompt);
}

const QString QConsole::prompt()
{
    return QString::fromStdString(m_prompt);
}

void QConsole::setHistoryFilePath(const QString& path)
{
    if (QFileInfo fi(path); !fi.exists()) {
        if (!fi.absoluteDir().exists()) {
            fi.absoluteDir().mkpath(QLatin1String("."));
        }

        QFile f(path);
        f.open(QIODevice::WriteOnly);
        f.close();
    }

    m_historyFilePath = path.toStdString();

    m_terminal->history_load(m_historyFilePath);
}

void QConsole::setStdinEcho(bool enable)
{
#ifdef WIN32
    HANDLE handle = GetStdHandle(STD_INPUT_HANDLE);
    DWORD  mode;

    GetConsoleMode(handle, &mode);

    if (!enable) {
        mode &= ~ENABLE_ECHO_INPUT;
    } else {
        mode |= ENABLE_ECHO_INPUT;
    }

    SetConsoleMode(handle, mode);
#else
    struct termios tty;

    tcgetattr(STDIN_FILENO, &tty);

    if (!enable) {
        tty.c_lflag &= ~ECHO;
    } else {
        tty.c_lflag |= ECHO;
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
#endif

    m_echo = enable;
}

QByteArray QConsole::readLine(const QString& prompt)
{
    QByteArray line;
    m_ostream << prompt;

    m_ostream.flush();
    QTextStream istream(stdin);

    istream >> line;
    istream.flush();

    return line;
}

QByteArray QConsole::readPass(const QString& prompt)
{
    setStdinEcho(false);
    const auto pass = readLine(prompt);
    m_ostream << Qt::endl;
    setStdinEcho(true);
    return pass;
}

QTextStream& QConsole::ostream()
{
    return m_ostream;
}

const QConsole::Command* QConsole::findCommandByName(std::string_view name)
{
    const auto& iter = m_commands->longest_prefix(name);

    if (iter != m_commands->end() && iter.key().length() == name.size()) {
        return &iter.value();
    }

    return nullptr;
}

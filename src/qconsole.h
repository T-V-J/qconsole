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

#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QTextStream>

// QConsole is the access point to our REPL session and the terminal. It provides
// the ability to add and remove invokable commands.
class QConsole : public QObject
{
public:
    // Color represents a color code.
    enum class Color
    {
        Black   = 0,
        Red     = 1,
        Green   = 2,
        Yellow  = 3,
        Blue    = 4,
        Magenta = 5,
        Cyan    = 6,
        White   = 7,
    };

    // Style represents a text style (ex. Normal, Bold).
    enum class Style
    {
        Normal = 0,
        Bold   = 1
    };

    // Context represents a command execution environment.
    struct Context
    {
        // The arguments used to invoke the command.
        const QList<QString> arguments;
    };

    // Command represents an invokable object.
    struct Command
    {
        typedef std::function<void(const Context& ctx)> Callback;

        // The name of the command.
        QString name;

        // The description of the command.
        QString description;

        // The callback to be run when the command is invoked.
        Callback invoke;
    };

    // Return a formatted string with the specified color and style.
    static inline QString colorize(const QString& str, const Color& color, const Style& style = Style::Bold)
    {
        return QStringLiteral("\33[%1;3%2m%3\33[0m").arg(static_cast<int>(style)).arg(static_cast<int>(color)).arg(str);
    }

    // Construct a new QConsole object. Note that you shouldn't create multiple instances of this
    // class since that would result in unexpected behavior.
    explicit QConsole(QObject* parent = nullptr);

    // Destroy the QConsole object.
    ~QConsole();

    // Enable reading user input. This isn't a blocking method because the console will
    // be activated after the main loop has been started.
    void start();

    // Disable reading user input after the current read is finished.
    void stop();

    // Check if the console is currently reading user input.
    bool running();

    // Add a new command to the list of available commands.
    void addCommand(const Command& command);

    // Remove a command using its name.
    void removeCommandByName(const QString& name);

    // Return the number of commands currently available.
    size_t commandCount();

    // Invoke a command using its name with the specified context. This method returns false
    // if the command wasn't found in the list of available commands.
    bool invokeCommandByName(const QString& name, const Context& ctx = Context{});

    // Reset the prompt to the default prompt value.
    void resetPrompt();

    // Set the default prompt value.
    void setDefaultPrompt(const QString& prompt);

    // Set the current prompt value.
    void setPrompt(const QString& prompt);

    // Set the path to the history file.
    void setHistoryFilePath(const QString& path);

    // Add the default commands: "help", "version", "exit", "history", and "clear".
    void addDefaultCommands();

    // Set to false to hide user input in the terminal.
    void setStdinEcho(bool enable);

    // Get the current prompt.
    const QString prompt();

    // Get the default prompt.
    const QString defaultPrompt();

    // Get the path to the history file.
    const QString historyFilePath();

    // Read a line from stdin and return it as a byte array.
    QByteArray readLine(const QString& prompt);

    // Same thing as "readLine" except the input is hidden from the user.
    QByteArray readPass(const QString& prompt);

    // The output text stream. This is a convenience object that can be used to provide
    // faster and more idiomatic access to stdout.
    QTextStream& ostream();

    // Set the maximum number of saved history items.
    void setMaxHistorySize(int size);

    // Set the word break characters.
    void setWordBreakCharacters(const char* characters);

    // Set the maximum number of completions to show before paginating.
    void setCompletionCountCutoff(int cutoff);

    // Set to true if auto-complete should require two tab presses.
    void setDoubleTabCompletion(bool complete);

    // Set to true to enable auto-complete even when user input is empty.
    void setCompleteOnEmpty(bool complete);

    // Set to true to make a "beep" sound when no command is found during auto-completion.
    void setBeepOnAmbiguousCompletion(bool beep);

    // Set to true if color should be disabled.
    void setNoColor(bool color);

    // Set the output device for the output text stream. This is useful if you want to
    // write to a file or buffer instead of stdout.
    void setOutputDevice(QIODevice* device);

    // Set to true to discard duplicate history items.
    void setUniqueHistory(bool unique);

protected:
    void timerEvent(QTimerEvent* event) override;

private:
    Q_DISABLE_COPY(QConsole)

private:
    class Terminal;
    class Trie;

    Trie*     m_commands;
    Terminal* m_terminal;

    std::string m_historyFilePath;
    std::string m_defaultPrompt;
    std::string m_prompt;

    bool m_echo;
    int  m_timerID;
    bool m_running;

    QTextStream m_ostream;

    const Command* findCommandByName(std::string_view name);
    void           evaluateLine(const char* line);
};

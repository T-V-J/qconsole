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
#include <QtCore/QDir>
#include <QtCore/QLoggingCategory>
#include <QtCore/QProcess>
#include <QtCore/QStandardPaths>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

int main(int argc, char** argv)
{
    auto serverHost = "localhost";
    auto serverPort = 4443;
    auto history    = "./history.txt";

    for (int i = 1; i < argc; ++i) {
        if (const auto& a = argv[i]; strcmp(a, "--help") == 0 || strcmp(a, "-h") == 0) {
            puts("");
            puts("Usage:");
            puts("");
            puts("  example [options]");
            puts("");
            puts("Options:");
            puts("");
            puts("  -h or --help            Print this help messsage.");
            puts("  -v or --version         Print the version string.");
            puts("");
            puts("  --history     <value>   Disable all message logging.");
            puts("  --server-host <value>   The server host. [default: localhost]");
            puts("  --server-port <value>   The server port. [default: 4443]");
            puts("");
            return EXIT_SUCCESS;
        } else if (strcmp(a, "--silent") == 0) {
            QLoggingCategory::setFilterRules("*=false");
        } else if (strcmp(a, "--verbose") == 0) {
            QLoggingCategory::setFilterRules("*=true");
        } else if (strcmp(a, "--version") == 0 || strcmp(a, "-v") == 0) {
            puts("Example 0.0.1");
            return EXIT_SUCCESS;
        } else if (strcmp(a, "--server-host") == 0 && (i + 1) <= argc) {
            serverHost = argv[++i];
        } else if (strcmp(a, "--server-port") == 0 && (i + 1) <= argc) {
            serverPort = QString::fromUtf8(argv[++i]).toUShort();
        } else {
            printf("Unknown option: %s", a);
            return EXIT_FAILURE;
        }
    }

    QCoreApplication app(argc, argv);
    app.setApplicationVersion(QStringLiteral("1.0.1"));
    app.setApplicationName(QStringLiteral("example"));
    app.setOrganizationName(QStringLiteral("example"));
    app.setOrganizationDomain(QStringLiteral("example"));

    // Create a static instance so that we can use it inside our message handler.
    static QConsole c;
    c.addDefaultCommands();

    qInstallMessageHandler([](QtMsgType type, const QMessageLogContext& context, const QString& message) {
        Q_UNUSED(context);

        switch (type) {
        case QtInfoMsg:
        case QtDebugMsg:
            c.ostream() << message << Qt::endl;
            break;
        case QtCriticalMsg:
            c.ostream() << QConsole::colorize(QStringLiteral("Error: ").append(message), QConsole::Color::Red,
                                              QConsole::Style::Normal)
                        << Qt::endl;
            break;
        case QtWarningMsg:
            c.ostream() << QConsole::colorize(QStringLiteral("Warning: ").append(message), QConsole::Color::Red,
                                              QConsole::Style::Normal)
                        << Qt::endl;
            break;
        case QtFatalMsg:
            c.ostream() << QConsole::colorize(QStringLiteral("Fatal: ").append(message), QConsole::Color::Red,
                                              QConsole::Style::Normal)
                        << Qt::endl;
        }
    });

    c.setHistoryFilePath(history);
    c.setDefaultPrompt(QStringLiteral("[?][%1]: ").arg(QConsole::colorize("#", QConsole::Color::Red)));

    c.addCommand({
      "http-get",
      "Send an http request to showcase QEventLoop capabilities.",
      [&](const QConsole::Context& ctx) {
          QNetworkAccessManager nm;

          QNetworkRequest request(ctx.arguments.join(" "));
          QNetworkReply*  reply = nm.get(request);

          QEventLoop loop;
          QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
          loop.exec();

          if (reply->error() == QNetworkReply::NoError) {
              c.ostream() << reply->readAll() << Qt::endl;
          } else {
              qWarning() << reply->errorString();
          }

          reply->deleteLater();
      },
    });

    c.addCommand({
      "shell",
      "Add executable programs found under $PATH as commands.",
      [&](const QConsole::Context& ctx) {
          Q_UNUSED(ctx);

          for (auto path : qEnvironmentVariable("PATH").split(QDir::listSeparator())) {
              QDir dir(path);
              dir.setFilter(QDir::Files);

              if (dir.count() == 0) {
                  continue;
              }

              for (auto entry : dir.entryInfoList()) {
                  auto program = entry.fileName();
                  c.addCommand({
                    program,
                    "[executable]",
                    [program](const QConsole::Context& ctx) {
                        QProcess process;

                        process.start(program, ctx.arguments);

                        if (!process.waitForStarted(1000)) {
                            qCritical() << process.error();
                            return;
                        }

                        process.closeWriteChannel();

                        if (!process.waitForFinished()) {
                            qCritical() << process.error();
                            return;
                        }

                        if (process.exitCode() == 0) {
                            c.ostream() << process.readAllStandardOutput();
                        } else {
                            c.ostream() << process.readAllStandardError();
                        }

                        c.ostream().flush();
                    },
                  });
              }
          }
      },
    });

    c.addCommand({
      "connect",
      "Connect to the server.",
      [&](const QConsole::Context& ctx) {
          Q_UNUSED(ctx);

          c.setDefaultPrompt(QStringLiteral("[?][%1]: ").arg(QConsole::colorize("#", QConsole::Color::Green)));

          c.addCommand({
            "login",
            "Login to the server.",
            [&](const QConsole::Context& ctx) {
                Q_UNUSED(ctx);

                auto username = c.readLine("Username: ");
                auto password = c.readPass("Password: ");

                if (username == "root" && password == "123") {
                    c.setPrompt(QStringLiteral("[%1@%2][%3]: ")
                                  .arg(QConsole::colorize(username.constData(), QConsole::Color::Green),
                                       QConsole::colorize(serverHost, QConsole::Color::Cyan),
                                       QConsole::colorize("#", QConsole::Color::Green)));
                } else {
                    qCritical() << "Incorrect password!";
                }
            },
          });

          c.addCommand({
            "logout",
            "Logout of the server.",
            [&](const QConsole::Context& ctx) {
                Q_UNUSED(ctx);
                c.ostream() << "Logging out!" << Qt::endl;
                c.resetPrompt();
            },
          });

          c.addCommand({
            "ping",
            "Ping the server.",
            [&](const QConsole::Context& ctx) {
                Q_UNUSED(ctx);
                c.ostream() << "Ping!" << Qt::endl;
            },
          });

          c.addCommand({
            "disconnect",
            "Disconnect from the server.",
            [&](const QConsole::Context& ctx) {
                Q_UNUSED(ctx);

                c.removeCommandByName("login");
                c.removeCommandByName("logout");
                c.removeCommandByName("ping");
                c.removeCommandByName("disconnect");

                c.setDefaultPrompt(QStringLiteral("[?][%1]: ").arg(QConsole::colorize("#", QConsole::Color::Red)));

                qInfo() << "Disconnected from server. Online commands have been removed.";
            },
          });

          qInfo() << "Connected to server. See 'help' for online commands.";
      },
    });

    c.start();

    return app.exec();
}

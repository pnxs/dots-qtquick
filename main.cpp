#include <QGuiApplication>

#include <QQmlApplicationEngine>
#include <dots/Application.h>
#include <dots/io/Io.h>
#include <QTimer>
#include "DotsSubscriptionManager.h"

static int sigintFd[2];
static int sigtermFd[2];

static void handleSigIng(int)
{
    ::write(sigintFd[0], "a", 1);
}

static void handleSigTerm(int)
{
    ::write(sigtermFd[0], "a", 1);
}

static int setup_unix_signal_handlers()
{
    struct sigaction hup, term;

    hup.sa_handler = &handleSigIng;
    sigemptyset(&hup.sa_mask);
    hup.sa_flags = 0;
    hup.sa_flags |= SA_RESTART;

    if (sigaction(SIGINT, &hup, 0))
        return 1;

    term.sa_handler = &handleSigTerm;
    sigemptyset(&term.sa_mask);
    term.sa_flags = 0;
    term.sa_flags |= SA_RESTART;

    if (sigaction(SIGTERM, &term, 0))
        return 2;

    return 0;
}

int main(int argc, char *argv[])
{
    dots::Application dotsApp("dots-qtquick", argc, argv);

    auto& ioCtx = dots::global_transceiver().ioContext();

    QGuiApplication app(argc, argv);

    setup_unix_signal_handlers();

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("dots-qtquick-qml", "Main");

    dots::io::global_service<DotsSubscriptionManager>();

    QTimer timer;
    timer.connect(&timer, &QTimer::timeout, [&](){ ioCtx.run_one_for(std::chrono::milliseconds{ 10 }); });
    timer.start();

    return app.exec();
}

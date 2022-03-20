#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtDebug>
#include <QThread>
#include "serialworker.h"
#include "frameprocessor.h"
#include <QQueue>
#include "frame.h"

int main(int argc, char *argv[])
{
    int rv;
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQueue<Frame*> outFrameQueue;

    QThread *threadSerial = new QThread();
    SerialWorker *serialWorker = new SerialWorker(&outFrameQueue);
    FrameProcessor *frameProcessor = new FrameProcessor(&outFrameQueue);


    QQmlApplicationEngine engine;
    //QQmlContext* ctx = engine.rootContext();
    //ctx->setContextProperty("serial", frameProcessor);

    // Singleton
    //
    qmlRegisterSingletonType<FrameProcessor>("com.raspberrypi.SerialCom", 1, 0, "FrameProcessorSingleton",
                                         [&](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
            Q_UNUSED(engine)
            Q_UNUSED(scriptEngine)

            return frameProcessor;
        });

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    serialWorker->moveToThread(threadSerial);
    QObject::connect(serialWorker, SIGNAL(frameReceived(Frame*)), frameProcessor, SLOT(FrameIncoming(Frame*)));
    QObject::connect(serialWorker, SIGNAL(workRequested()), threadSerial, SLOT(start()));
    QObject::connect(threadSerial, SIGNAL(started()), serialWorker, SLOT(doWork()));
    QObject::connect(serialWorker, SIGNAL(finished()), threadSerial, SLOT(quit()), Qt::DirectConnection);

    serialWorker->abort();
    threadSerial->wait(); // If the thread is not running, this will immediately return.
    serialWorker->requestWork();

    rv = app.exec();
    serialWorker->abort();
    threadSerial->wait();
    delete threadSerial;
    qDebug() << "Delete Serial Thread";
    delete serialWorker;
    qDebug() << "Delete Serial SerialWorker";

    qDebug() << "End Application";

    return rv;
}

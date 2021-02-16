#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "game.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    auto game = std::make_unique<Game>();

    qmlRegisterSingletonInstance("game", 1, 0, "Game", game.get());
    qmlRegisterUncreatableMetaObject(
      qtns::staticMetaObject,   // static meta object
      "enums",                  // import statement (can be any string)
      1, 0,                     // major and minor version of the import
      "Enums",                  // name in QML (does not have to match C++ name)
      "Error: only enums"       // error in case someone tries to create a MyNamespace object
    );

    QQmlApplicationEngine engine;

    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    QObject *root = engine.rootObjects().at(0);
    QQuickItem *board = root->findChild<QQuickItem*>("board");

    game->setParentItem(board);

    return app.exec();
}

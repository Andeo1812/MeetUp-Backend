#include "mainwindow.h"
#include "screensfactory.h"
#include <common/navigator.h>

#include <QTranslator>
#include <QLibraryInfo>

//#include <QDesktopWidget>
#include <QScreen>

#include <QException>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{   


    try {
        qDebug("create main window");
        container = new QStackedWidget;
        container->setObjectName("ui");

        this->factory = new ScreensFactory;
        this->navigator = new Navigator(
                    this->container,
                    this->factory
                    );

        QString mainStyle = "QWidget#window {"
                            "background-color:#EFF5FC;"
                            "}";
        container->setStyleSheet(mainStyle);
        container->setObjectName("window");

        this->setWindowTitle("MeetUp");
        this->setWindowIcon(QIcon(":/resc/resc/logo.png"));
        //this->resize(QDesktopWidget().availableGeometry(this).size() * 0.7);
        this->setCentralWidget(container);
    } catch (std::exception& e) {
        qDebug("%s", e.what());
    }
}

MainWindow::~MainWindow() {
    delete container;
    delete navigator;
    delete factory;
}

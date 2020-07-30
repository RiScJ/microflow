#include "main_window.h"

#include "entity_list.h"
#include "env3d.h"

#include <QMenuBar>
#include <QToolBar>
#include <QDockWidget>
#include <QListWidgetItem>

MainWindow::MainWindow(Env3D &view, QWidget *parent) : QMainWindow(parent) {
    MainWindow::create_menubar();
    MainWindow::create_toolbar(view);
    MainWindow::create_dockwidget();
};


void MainWindow::create_menubar(void) {
    QMenuBar *bar = menuBar();
    QMenu *file = bar->addMenu("&File");
    file->addAction("Open");
    file->addAction("New");
};


void MainWindow::create_toolbar(Env3D &view) {
    QToolBar *bar = addToolBar("");
    bar->setIconSize(QSize(40, 40));
    QAction *new2D = new QAction(QIcon(":/icon/pencil.svg"), "New 2D", this);
    new2D->setCheckable(true);
    connect(new2D, &QAction::toggled, &view, &Env3D::new_2D);
    bar->addAction(new2D);
};


void MainWindow::create_dockwidget(void) {
    dockWidget = new QDockWidget(tr("Entities"), this);
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea);
    entitylist = new EntityList(dockWidget);
    dockWidget->setWidget(entitylist);
    addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
};


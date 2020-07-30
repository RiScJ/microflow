#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

#include "entity_list.h"
#include "env3d.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(Env3D &view, QWidget *parent = nullptr);
    EntityList *entitylist;
private:
    void create_menubar(void);
    void create_toolbar(Env3D &view);
    void populate_central(void);
    void create_dockwidget(void);
    QDockWidget *dockWidget;
};

#endif // MAIN_WINDOW_H

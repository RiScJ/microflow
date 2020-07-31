#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <Qt3DCore>

#include "env3d.h"

using namespace Qt3DCore;

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(Env3D &view, QWidget *parent = nullptr);
//	EntityList *entitylist;
	QListWidget *list;
	void add_entity(const QString& text, QEntity* entity);
	void handle_entity_selection(QListWidgetItem* item);
	void handle_entity_unselection();
signals:
	void select_entity(QListWidgetItem* item);
private:
	void create_menubar(void);
	void create_toolbar(void);
	void populate_central(void);
	void create_dockwidget(void);
	QDockWidget *dockWidget;
	int entc;
	static Env3D* _view;
};

#endif // MAIN_WINDOW_H

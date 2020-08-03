#include "main_window.h"

#include "env3d.h"

#include <QMenuBar>
#include <QToolBar>
#include <QDockWidget>
#include <QListWidgetItem>
#include <Qt3DCore>

using namespace Qt3DCore;

Env3D* MainWindow::_view = nullptr;

MainWindow::MainWindow(Env3D &view, QWidget *parent) : QMainWindow(parent) {
	_view = &view;
	MainWindow::create_menubar();
	MainWindow::create_toolbar();
	MainWindow::create_dockwidget();
	connect(_view, &Env3D::entity_unselected, this, &MainWindow::handle_entity_unselection);
	connect(this, &MainWindow::select_entity, _view, &Env3D::select_entity);
	connect(_view, &Env3D::destroy_grid, this, &MainWindow::destroy_grid);
};


void MainWindow::create_menubar(void) {
	QMenuBar *bar = menuBar();
	QMenu *file = bar->addMenu("&File");
	file->addAction("Open");
	file->addAction("New");
};


void MainWindow::create_toolbar() {
	QToolBar *bar = addToolBar("");
	bar->setIconSize(QSize(40, 40));
	QAction *new2D = new QAction(QIcon(":/icon/pencil.svg"), "New 2D", this);
	new2D->setCheckable(true);
	connect(new2D, &QAction::toggled, _view, &Env3D::new_2D);
	bar->addAction(new2D);
};


void MainWindow::create_dockwidget() {
	dockWidget = new QDockWidget(tr("Entities"), this);
	dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea);
	QListWidget* mylist = new QListWidget(dockWidget);
	list = mylist;
	dockWidget->setWidget(list);
	addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
	connect(_view, &Env3D::entity_created, this, &MainWindow::add_entity);
	connect(list, &QListWidget::itemClicked, this, &MainWindow::handle_entity_selection);
};


void MainWindow::add_entity(const QString &text, QEntity* entity, Mode drawMode) {
	QListWidgetItem* newItem = new QListWidgetItem();
	newItem->setData(EntityTypeRole, drawMode);
	newItem->setData(NodePtrRole, QVariant::fromValue(reinterpret_cast<void*>(entity)));
	switch (drawMode) {
	case GRID:
		newItem->setHidden(true);
		break;
	default: {
		newItem->setText(text);
		break;
	}
	}
	list->addItem(newItem);
};


void MainWindow::destroy_grid(void) {
	for (int i = list->count() - 1; i >= 0; i--) {
		if (list->item(i)->data(EntityTypeRole) == GRID) {
			QEntity* entity = reinterpret_cast<QEntity*>(list->item(i)->data(NodePtrRole).value<void*>());
			entity->~QEntity();
			list->item(i)->~QListWidgetItem();
		}
	}
};


void MainWindow::handle_entity_selection(QListWidgetItem *item) {
	connect(list, &QListWidget::itemSelectionChanged, _view, &Env3D::unselect_entity);
	emit select_entity(item);
};


void MainWindow::handle_entity_unselection(void) {
	disconnect(list, &QListWidget::itemSelectionChanged, _view, &Env3D::unselect_entity);
};

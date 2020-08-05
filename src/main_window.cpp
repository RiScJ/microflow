#include "main_window.h"

#include "env3d.h"

#include <QMenuBar>
#include <QToolBar>
#include <QDockWidget>
#include <QListWidgetItem>
#include <Qt3DCore>
#include <QLayout>


Env3D* MainWindow::_view = nullptr;
QToolBar* MainWindow::_bar = nullptr;
QAction* MainWindow::new_2D = nullptr;
QAction* MainWindow::new_line = nullptr;
QAction* MainWindow::end_2D = nullptr;
QAction* MainWindow::_extrude = nullptr;

//QListWidget* MainWindow::_list_face = new QListWidget();// This will cause crash at runtime -- lesson learned, leave time for QApplication init
QListWidget* MainWindow::_list_face = nullptr;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	Env3D *view = new Env3D();
	_view = view;

	MainWindow::create_menubar();
	MainWindow::create_actions();
	MainWindow::create_toolbar();
	MainWindow::create_dockwidget();

	connect(_view, &Env3D::entity_unselected, this, &MainWindow::handle_entity_unselection);
	connect(this, &MainWindow::select_entity, _view, &Env3D::select_entity);
	connect(_view, &Env3D::destroy_entities_of_type, this, &MainWindow::destroy_entities_of_type);
	connect(_view, &Env3D::changed_dimension, this, &MainWindow::handle_change_dimension);
	connect(_view, &Env3D::register_face, this, &MainWindow::add_face);
	connect(_view, &Env3D::face_to_pointv, this, &MainWindow::face_to_pointv);
	connect(this, &MainWindow::do_extrude, _view, &Env3D::do_extrude);

	setCentralWidget(view);
};


void MainWindow::create_actions(void) {
	QAction *new2D = new QAction(QIcon(":/icon/pencil.svg"), "New 2D", this);
	new_2D = new2D;
	new_2D->setCheckable(true);
	connect(new_2D, &QAction::toggled, _view, &Env3D::new_2D);

	QAction *extrude = new QAction(QIcon(":/icon/export.svg"), "Extrude", this);
	_extrude = extrude;
	_extrude->setCheckable(true);
	connect(_extrude, &QAction::toggled, _view, &Env3D::sig_extrude);

	QAction *drawLine = new QAction(QIcon(":/icon/line.svg"), "Line", this);
	drawLine->setCheckable(true);
	new_line = drawLine;

	QAction *end2D = new QAction(QIcon(":/icon/3d-cube.svg"), "Finish 2D", this);
	end_2D = end2D;

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
	_bar = bar;
	_bar->addAction(new_2D);
	_bar->addAction(_extrude);
};


void MainWindow::handle_change_dimension(int dim) {
	switch (dim) {
	case 2: {
		disconnect(new_2D, &QAction::toggled, _view, &Env3D::new_2D);
		new_2D->setChecked(false);
		_bar->removeAction(new_2D);

		disconnect(_extrude, &QAction::toggled, _view, &Env3D::sig_extrude);
		_extrude->setChecked(false);
		_bar->removeAction(_extrude);

		_bar->addAction(end_2D);
		connect(end_2D, &QAction::triggered, _view, &Env3D::end_2D);

		_bar->addAction(new_line);
		connect(new_line, &QAction::toggled, _view, &Env3D::new_line);
		break;
	}
	case 3: {
		disconnect(end_2D, &QAction::triggered, _view, &Env3D::end_2D);
		_bar->removeAction(end_2D);

		disconnect(new_line, &QAction::toggled, _view, &Env3D::new_line);
		new_line->setChecked(false);
		_bar->removeAction(new_line);

		_bar->addAction(new_2D);
		connect(new_2D, &QAction::toggled, _view, &Env3D::new_2D);

		_bar->addAction(_extrude);
		connect(_extrude, &QAction::toggled, _view, &Env3D::sig_extrude);

		for (int i = list->count() - 1; i >= 0; i--) {
			if (list->item(i)->data(EntityTypeRole) == CARTESIAN) {
				QEntity* entity = reinterpret_cast<QEntity*>(list->item(i)->data(NodePtrRole).value<void*>());
				entity->~QEntity();
				list->item(i)->~QListWidgetItem();
			}
		}

		break;
	}
	default: break;
	}
};


void MainWindow::create_dockwidget() {
	_list_face = new QListWidget;

	dockWidget = new QDockWidget(tr("Entities"), this);
	dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea);
	QListWidget* mylist = new QListWidget(dockWidget);
	list = mylist;
	dockWidget->setWidget(list);
	addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
	connect(_view, &Env3D::entity_created, this, &MainWindow::add_entity);
	connect(list, &QListWidget::itemClicked, this, &MainWindow::handle_entity_selection);
};


void MainWindow::add_face(QEntity *face, QVector<QVector3D>* pointv) {
	QListWidgetItem* face_item = new QListWidgetItem();
	face_item->setData(NodePtrRole, QVariant::fromValue(reinterpret_cast<void*>(face)));
	face_item->setData(PointvPtrRole, QVariant::fromValue(reinterpret_cast<void*>(pointv)));
	_list_face->addItem(face_item);
};


void MainWindow::face_to_pointv(Qt3DCore::QEntity *face) {
	for (int i = 0; i < _list_face->count(); i++) {
		QEntity* test_face = reinterpret_cast<QEntity*>(_list_face->item(i)->data(NodePtrRole).value<void*>());
		if (test_face == face) {
			QVector<QVector3D>* pointv = reinterpret_cast<QVector<QVector3D>*>(_list_face->item(i)->data(PointvPtrRole).value<void*>());
			qDebug() << pointv->count();
//			emit do_extrude(pointv);
		}
	}
};


void MainWindow::add_entity(const QString &text, QEntity* entity, Mode drawMode) {
	QListWidgetItem* newItem = new QListWidgetItem();
	newItem->setData(EntityTypeRole, drawMode);
	newItem->setData(NodePtrRole, QVariant::fromValue(reinterpret_cast<void*>(entity)));
	switch (drawMode) {
	case GRID: {
		newItem->setHidden(true);
		break;
	}
	case CARTESIAN: {
		newItem->setHidden(true);
		break;
	}
	default: {
		newItem->setText(text);
		break;
	}
	}
	list->addItem(newItem);
};


void MainWindow::destroy_entities_of_type(Mode mode) {
	for (int i = 0; i < list->count();) {
		if (list->item(i)->data(EntityTypeRole) == mode) {
			QEntity* entity = reinterpret_cast<QEntity*>(list->item(i)->data(NodePtrRole).value<void*>());
			entity->~QEntity();
			list->item(i)->~QListWidgetItem();
		} else {
			i++;
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

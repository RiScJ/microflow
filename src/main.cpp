#include "main_window.h"
#include "env3d.h"

#include <QApplication>
#include <QSurfaceFormat>

#include <QMainWindow>
#include <QMenuBar>
#include <QGridLayout>
#include <QLabel>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSizePolicy>
#include <QObject>

#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QAspectEngine>

#include <Qt3DInput/QInputAspect>

#include <Qt3DRender/QRenderAspect>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QTorusMesh>

#include <QPropertyAnimation>


int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	Env3D *view = new Env3D();
	MainWindow mainWindow(*view);



	mainWindow.setCentralWidget(view);
	mainWindow.showMaximized();
	mainWindow.show();
	return app.exec();
}

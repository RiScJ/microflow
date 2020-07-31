#include "env3d.h"

#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QForwardRenderer>
#include <QQuaternion>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QCamera>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DRender/QCameraLens>
#include <Qt3DCore/QAspectEngine>

#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QGeometry>

#include <Qt3DInput/QInputAspect>

#include <Qt3DRender/QRenderAspect>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QTorusMesh>

#include <QPropertyAnimation>

#include <QOrbitCameraController>
#include <QObjectPicker>

#include <QDebug>
#include <QMouseEvent>

#include "qt3dwindow.h"

using namespace Qt3DCore;

QEntity *Env3D::_rootEntity = nullptr;
Qt3DRender::QCamera *Env3D::_camera = nullptr;

QEntity *Env3D::_origin = nullptr;

QEntity* Env3D::selection_buffer = nullptr;
QEntity* Env3D::selected_buffer = nullptr;

Env3D::Env3D(QWidget *parent) : QWidget(parent) {
	mode = NONE;
	dim = THREE_D;

	init_helpers();

	auto view = new Qt3DExtras::Qt3DWindow();
	view->installEventFilter(this);
	view->defaultFrameGraph()->setClearColor(QColor(QRgb(0xeeeeff)));

	container = createWindowContainer(view, this);
	container->setMinimumSize(1920,1080);

	createScene();
//	QObjectPicker gridPicker = QObjectPicker();

	_camera = view->camera();
	_camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 10000.0f);
	_camera->setPosition(QVector3D(0, 0, 4.0f));
	_camera->setViewCenter(QVector3D(0, 0, 0));

	Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(_rootEntity);
	camController->setLinearSpeed(50.0f);
	camController->setLookSpeed(360.0f);
	camController->setCamera(_camera);

	view->setRootEntity(_rootEntity);
}


void Env3D::start_2D(Axis axis, float da) {
	dim = TWO_D;
	_camera->lens()->setOrthographicProjection(10.0f, 10.0f, 10.0f, 10.0f, 0.1f, 10000.0f);
	_camera->setPosition(QVector3D(0, 0, 10.0f));
	_camera->setViewCenter(QVector3D(0, 0, 0));
	_camera->setUpVector(QVector3D(0,1,0));
	_camera->viewAll();
}


void Env3D::end_2D(void) {

}


void Env3D::new_2D(bool checked) {
	if (checked) {
		/* User wants to start a new 2D drawing, so we prepare the scene for
		 * them to do so. First, we draw small planes (as squares) representing
		 * the coordinate planes (XY, XZ, YZ), with mild highlighting to make
		 * their presence obvious. If user mouses over the planes, they will be
		 * replaced by grids. User can click to select a plane. Doing so will
		 * switch to orthographic projection, move the camera above the plane,
		 * face it, and rotate away any skew. */

		start_2D(X);

	} else {
		// User cancelled
		dim = THREE_D;
		_camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 10000.0f);
		_camera->setPosition(QVector3D(0, 0, 4.0f));
		_camera->setViewCenter(QVector3D(0, 0, 0));
		_camera->viewAll();
	}
}


void Env3D::createScene(void) {
	Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity;
	_rootEntity = rootEntity;

	create_origin();
}


void Env3D::init_helpers(void) {
	line_start = QPoint(0, 0);
	line_hasStart = false;
};


QEntity* Env3D::draw_square(const QVector3D &start, const QVector3D &end, const QColor &color) {
//	QEntity* square = new QEntity(_rootEntity);
//	QEntity* segment = new QEntity(square);

	QVector3D other1;
	QVector3D other2;

	QVector3D axisX(1,0,0);

	QVector3D start_to_end(end-start);

	double m_start_to_other1 = sqrt(start_to_end.lengthSquared() / 2);
	double d_start_to_other1 = acos(QVector3D::dotProduct(axisX, start_to_end) / start_to_end.length()) + PI/4;

	QVector3D start_to_other1;
	start_to_other1.setX(cos(d_start_to_other1));
	start_to_other1.setY(sin(d_start_to_other1));
	start_to_other1.setZ(0);
	start_to_other1 *= m_start_to_other1;

	other1 = start + start_to_other1;



	double m_start_to_other2 = sqrt(start_to_end.lengthSquared() / 2);
	double d_start_to_other2 = acos(QVector3D::dotProduct(axisX, start_to_end) / start_to_end.length()) - PI/4;

	QVector3D start_to_other2;
	start_to_other2.setX(cos(d_start_to_other2));
	start_to_other2.setY(sin(d_start_to_other2));
	start_to_other2.setZ(0);
	start_to_other2 *= m_start_to_other2;

	other2 = start + start_to_other2;



	draw_line(start, other1, color);
	draw_line(other1, end, color);
	draw_line(end, other2, color);
	draw_line(other2, start, color);
};


QEntity* Env3D::draw_line(const QVector3D& start, const QVector3D& end, const QColor& color) {
	auto *geometry = new Qt3DRender::QGeometry(_rootEntity);

	// Extract line start and endpoint coordinates
	QByteArray bufferBytes;
	bufferBytes.resize(3 * 2 * sizeof(float)); // start.x, start.y, start.end + end.x, end.y, end.z
	float *positions = reinterpret_cast<float*>(bufferBytes.data());
	*positions++ = start.x();
	*positions++ = start.y();
	*positions++ = start.z();
	*positions++ = end.x();
	*positions++ = end.y();
	*positions++ = end.z();

	auto *buf = new Qt3DRender::QBuffer(geometry);
	buf->setData(bufferBytes);

	// Add the vertices to the geometry
	auto *positionAttribute = new Qt3DRender::QAttribute(geometry);
	positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
	positionAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
	positionAttribute->setVertexSize(3);
	positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
	positionAttribute->setBuffer(buf);
	positionAttribute->setByteStride(3 * sizeof(float));
	positionAttribute->setCount(2);
	geometry->addAttribute(positionAttribute);

	// Specify connection between vertices
	QByteArray indexBytes;
	indexBytes.resize(2 * sizeof(unsigned int));
	unsigned int *indices = reinterpret_cast<unsigned int*>(indexBytes.data());
	*indices++ = 0;
	*indices++ = 1;
	auto *indexBuffer = new Qt3DRender::QBuffer(geometry);
	indexBuffer->setData(indexBytes);
	auto *indexAttribute = new Qt3DRender::QAttribute(geometry);
	indexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedInt);
	indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
	indexAttribute->setBuffer(indexBuffer);
	indexAttribute->setCount(2);
	geometry->addAttribute(indexAttribute);

	// Mesh the line
	auto *line = new Qt3DRender::QGeometryRenderer(_rootEntity);
	line->setGeometry(geometry);
	line->setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);
	auto *material = new Qt3DExtras::QPhongMaterial(_rootEntity);
	material->setAmbient(color);

	// Entity from line, add entity to root
	auto *lineEntity = new QEntity(_rootEntity);
	lineEntity->addComponent(line);
	lineEntity->addComponent(material);

	emit entity_created("line", lineEntity);
	return lineEntity;
}


void Env3D::create_origin(void) {
	draw_line(QVector3D(0, 0, 0), { AXIS_LENGTH, 0, 0 }, X_AXIS_COLOR);     // X
	draw_line(QVector3D(0, 0, 0), { 0, AXIS_LENGTH, 0 }, Y_AXIS_COLOR);     // Y
	draw_line(QVector3D(0, 0, 0), { 0, 0, AXIS_LENGTH }, Z_AXIS_COLOR);     // Z
}


void Env3D::create_grid(Axis axis, float dx, float dy, float dz) {
	for (int i = 0; i <= GRID_LENGTH; i += GRID_SPACING) {
		draw_line(QVector3D(dx + i, dy, dz), GRID_LENGTH * QVector3D(0, 1, 0) + QVector3D(dx + i, dy, dz), GRID_COLOR);
		draw_line(QVector3D(dx, dy + i, dz), GRID_LENGTH * QVector3D(1, 0, 0) + QVector3D(dx, dy + i, dz), GRID_COLOR);
	}
}


void Env3D::select_entity(QListWidgetItem *item) {
	QEntity* selected_entity = reinterpret_cast<QEntity*>(item->data(NodePtrRole).value<void*>());

	selected_entity->setEnabled(false);
	selected_buffer = selected_entity;


	QEntity* selection_entity = new QEntity(_rootEntity);
	selection_buffer = selection_entity;

	// Remove old material...
	QVector<Qt3DExtras::QPhongMaterial*> materialv = selected_entity->componentsOfType<Qt3DExtras::QPhongMaterial>();
//	int materialc = materialv.count();
//	for (int i = 0; i < materialc; i++) {
//		selected_entity->removeComponent(materialv.at(i));
//	}
	// ...and replace it with a new one
	auto *material = new Qt3DExtras::QPhongMaterial(_rootEntity);
	material->setAmbient(Qt::red);
	selection_entity->addComponent(material);

	QVector<Qt3DRender::QGeometryRenderer*> meshv = selected_entity->componentsOfType<Qt3DRender::QGeometryRenderer>();
	int meshc = meshv.count();
	for (int i = 0; i < meshc; i++) {
		QVector<Qt3DRender::QAttribute*> attrv = meshv.at(i)->geometry()->attributes();
		int attrc = attrv.count();
		for (int j = 0; j < attrc; j++) {
			if (attrv.at(j)->attributeType() == Qt3DRender::QAttribute::VertexAttribute) {
				QByteArray copy = attrv.at(j)->buffer()->data();
				copy.resize(2 * 3 * sizeof(float));
				float *positions = reinterpret_cast<float*>(copy.data());


				QVector3D start;
				QVector3D end;

				start.setX(*positions++);
				start.setY(*positions++);
				start.setZ(*positions++);
				end.setX(*positions++);
				end.setY(*positions++);
				end.setZ(*positions++);


				Qt3DExtras::QCylinderMesh *cylinderMesh = new Qt3DExtras::QCylinderMesh;
				cylinderMesh->setRings(10);
				cylinderMesh->setSlices(10);
				cylinderMesh->setRadius(1);
				cylinderMesh->setLength(start.distanceToPoint(end));

				Qt3DCore::QTransform *cylinderTransform = new Qt3DCore::QTransform;
				//cylinderTransform->setScale3D(QVector3D(1.5, 1, 0.5));
				cylinderTransform->setRotation(QQuaternion::rotationTo(QVector3D(0,1,0), start - end));
				cylinderTransform->setTranslation((start + end) / 2);

				selection_entity->addComponent(cylinderMesh);
				selection_entity->addComponent(cylinderTransform);

				selection_entity->setEnabled(true);
			}
		}
	}


};


void Env3D::unselect_entity(void) {
	selected_buffer->setEnabled(true);
	selection_buffer->setEnabled(false);
	emit entity_unselected();
};


bool Env3D::eventFilter(QObject *obj, QEvent *e) {
	switch (e->type()) {
	case QEvent::MouseButtonPress: {
		switch (mode) {
		case LINE: {
			QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(e);
			if (!line_hasStart) {
				line_start = QPoint(mouseEvent->pos());
			} else {
				QPoint end(mouseEvent->pos());
				draw_line(QVector3D(line_start), QVector3D(end), Qt::black);
			}
			line_hasStart ^= 1;
			return true;
		}
		case SQUARE: {
			QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(e);
			if (!line_hasStart) {
				line_start = QPoint(mouseEvent->pos());
			} else {
				QPoint end(mouseEvent->pos());
				draw_square(QVector3D(line_start), QVector3D(end), Qt::black);
			}
			line_hasStart ^= 1;
			return true;
		}
		default: {
			break;
		}
		}
		break;
	}
	default:  {
		break;
	}
	}
	return false;
}


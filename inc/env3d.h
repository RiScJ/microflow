#ifndef ENV3D_H
#define ENV3D_H

#include <QObject>
#include <QWidget>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DRender>
#include <Qt3DCore>
#include <QListWidgetItem>
#include <QPickEvent>
#include <Qt3DInput/QMouseHandler>

#define PI 3.14159265358979323846264


enum ItemDataRole {
	NodePtrRole = 0x0101,
	EntityTypeRole = 0x0102
};

enum Dim {
	RESERVED_1,
	RESERVED_2,
	TWO_D,
	THREE_D
};

enum Mode {
	NONE,
	POINT,
	LINE,
	SQUARE,
	GRID = 1000,
	CARTESIAN,
	GRIDXY,
	GRIDXZ,
	GRIDYZ
};

enum Axis {
	T,
	X,
	Y,
	Z
};

enum Plane {
	XY,
	XZ,
	YZ
};

class Env3D : public QWidget
{	Q_OBJECT
public:
	explicit Env3D(QWidget *parent = nullptr);
	void createScene(void);
	Qt3DCore::QEntity* draw_line(const QVector3D &start, const QVector3D &end, const QColor &color);
	void new_2D(bool checked);
	QWidget *container;
	void select_entity(QListWidgetItem* item);
	void unselect_entity(void);
	Qt3DCore::QEntity* draw_square(const Plane& plane, const QVector3D& start, const QVector3D& end, const QColor& color, const float& alpha = 0);
	void entered_XY(void);
	void exited_XY(void);
	void entered_XZ(void);
	void exited_XZ(void);
	void entered_YZ(void);
	void exited_YZ(void);
	void selected_sketchplane(Qt3DRender::QPickEvent* pick);
	void end_2D(void);
	Qt3DCore::QEntity* draw_sphere(const QVector3D& center, const float radius);

	void new_line(void);
	void start_line(Qt3DRender::QPickEvent *pick);
	void mid_line(Qt3DRender::QPickEvent *pick);
	void end_line(Qt3DRender::QPickEvent *pick);
	void rerender_as_patch(void);
	void stop_line(void);

signals:
	void entity_created(const QString& text, Qt3DCore::QEntity* entity, Mode drawMode);
	void entity_unselected(void);
	void destroy_entities_of_type(Mode type);
	void changed_dimension(int dim);
	void disconnect_line(Qt3DRender::QObjectPicker* picker);

protected:

private:
	void init_helpers(void);
	bool eventFilter(QObject *obj, QEvent *e) override;

	static Qt3DCore::QEntity *_rootEntity;
	static Qt3DRender::QCamera *_camera;

	static Qt3DCore::QEntity *_origin;
	static Qt3DCore::QEntity *_XY;
	static Qt3DCore::QEntity *_XZ;
	static Qt3DCore::QEntity *_YZ;
	static const int AXIS_LENGTH = 10;
	static const Qt::GlobalColor X_AXIS_COLOR = Qt::red;
	static const Qt::GlobalColor Y_AXIS_COLOR = Qt::green;
	static const Qt::GlobalColor Z_AXIS_COLOR = Qt::blue;
	void create_origin(void);

	static const int GRID_LENGTH = 10;
	static const int GRID_SPACING = 1;
	static const Qt::GlobalColor GRID_COLOR = Qt::gray;
	void create_grid(Axis axis, int length = GRID_LENGTH, int spacing = GRID_SPACING, float dx = 0, float dy = 0, float dz = 0);
	void start_2D(Axis axis, float da = 0);

	Qt3DCore::QEntity* draw_patch(const QVector<QVector3D> pointv, const int pointc, const QColor& color, const float& alpha = 0);


	static Qt3DCore::QEntity* selection_buffer;
	static Qt3DCore::QEntity* selected_buffer;

	static QVector<QVector3D> line_pointv;
	static Qt3DRender::QObjectPicker* _lineHandler;
	static Qt3DCore::QEntity* line_buffer;
	static Qt3DCore::QEntity* _square;
	static Qt3DCore::QEntity* _pointSphere;

	// Helpers
	Mode mode;
	Dim dim;
	QVector3D line_start;
	QVector3D last_point;
	bool line_hasStart;
};

#endif // ENV3D_H

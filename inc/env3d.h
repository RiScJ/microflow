#ifndef ENV3D_H
#define ENV3D_H

#include <QObject>
#include <QWidget>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DRender>
#include <Qt3DCore>
#include <QListWidgetItem>

using namespace Qt3DCore;

enum ItemDataRole {
	NodePtrRole = 0x0101
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
	LINE
};

enum Axis {
	T,
	X,
	Y,
	Z
};

class Env3D : public QWidget
{	Q_OBJECT
public:
	explicit Env3D(QWidget *parent = nullptr);
	void createScene(void);
	Qt3DCore::QEntity *draw_line(const QVector3D &start, const QVector3D &end, const QColor &color);
	void new_2D(bool checked);
	QWidget *container;
	void select_entity(QListWidgetItem* item);
	void unselect_entity(void);

signals:
	void entity_created(const QString& text, QEntity* entity);
	void entity_unselected(void);

protected:

private:
	void init_helpers(void);
	bool eventFilter(QObject *obj, QEvent *e) override;

	static Qt3DCore::QEntity *_rootEntity;
	static Qt3DRender::QCamera *_camera;

	static Qt3DCore::QEntity *_origin;
	static const int AXIS_LENGTH = 10;
	static const Qt::GlobalColor X_AXIS_COLOR = Qt::red;
	static const Qt::GlobalColor Y_AXIS_COLOR = Qt::green;
	static const Qt::GlobalColor Z_AXIS_COLOR = Qt::blue;
	void create_origin(void);

	static const int GRID_LENGTH = 10;
	static const int GRID_SPACING = 1;
	static const Qt::GlobalColor GRID_COLOR = Qt::gray;
	void create_grid(Axis axis, float dx = 0, float dy = 0, float dz = 0);

	void start_2D(Axis axis, float da = 0);


	static QEntity* selection_buffer;
	static QEntity* selected_buffer;

	// Helpers
	Mode mode;
	Dim dim;
	QPoint line_start;
	bool line_hasStart;
};

#endif // ENV3D_H

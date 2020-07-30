#ifndef ENTITY_LIST_H
#define ENTITY_LIST_H

#include <QListWidget>

class EntityList : public QListWidget
{
    Q_OBJECT
public:
    explicit EntityList(QWidget *parent = nullptr);

signals:

};

#endif // ENTITY_LIST_H

#ifndef UUID_130B5D86_2621_4BFD_97EC_3A5F7FD0E6C5
#define UUID_130B5D86_2621_4BFD_97EC_3A5F7FD0E6C5

#include <QtWidgets/QGraphicsScene>

class BlockScene : public QGraphicsScene
{
    Q_OBJECT
public:
    using QGraphicsScene::QGraphicsScene;
protected:
    void dragEnterEvent(QGraphicsSceneDragDropEvent* event) override;
    void dragMoveEvent(QGraphicsSceneDragDropEvent* event) override;
    void dropEvent(QGraphicsSceneDragDropEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
private:
    QGraphicsItem* (*factory_)();
};

#endif

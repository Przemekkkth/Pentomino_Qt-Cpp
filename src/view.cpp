#include "view.h"

View::View()
    : m_gameScene(new GameScene())
{
    setScene(m_gameScene);
    resize(m_gameScene->sceneRect().width(), m_gameScene->sceneRect().height());

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

View::~View()
{
    delete m_gameScene;
}

void View::wheelEvent(QWheelEvent *event)
{
    Q_UNUSED(event)
}

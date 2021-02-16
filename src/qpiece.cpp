#include "qpiece.h"
#include <QSGImageNode>

QPiece::QPiece(Square sq_, Type type_, Color color_, QQuickItem *parent)
    : QQuickItem(parent), sq(sq_), type(type_), color(color_)
{
    setWidth(SQ_SIZE);
    setHeight(SQ_SIZE);

    setY((7 - (sq / 8)) * SQ_SIZE);
    setX((sq & 7) * SQ_SIZE);

    setEnabled(false);
    setFlag(ItemHasContents, true);
}

QSGNode *QPiece::updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *)
{
    QSGImageNode *node = static_cast<QSGImageNode *>(oldNode);
    if (!node)
        node = window()->createImageNode();
    QString str(":/img/");

    switch (type) {
        case DEAD:	str += "dead";  break;
        case MAN:	str += "man";	break;
        case KING:	str += "king";	break;
    }
    if (color == BLACK)
        str += "_2";
    str += ".png";

    QSGTexture *texture = window()->createTextureFromImage(QImage(str));

    node->setTexture(texture);
    node->setOwnsTexture(true);
    node->setRect(boundingRect());

    window()->update();

    return node;
}

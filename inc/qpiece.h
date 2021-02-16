#ifndef QPIECE_H
#define QPIECE_H

#include <QQuickItem>
#include "misc.h"

class QPiece : public QQuickItem {
    Q_OBJECT
public:
    QPiece(Square sq_, Type type_, Color color_, QQuickItem *parent = nullptr);

    Square  sq;
    Type    type;
    Color   color;
protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data) override;
};

#endif // QPIECE_H

#pragma once

#include <QBrush>
#include <QCursor>
#include <QGraphicsRectItem>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QPen>

class CropBoxItem: public QGraphicsRectItem
{
public:
  CropBoxItem(const QRectF& rect, QGraphicsItem* parent = nullptr);

  enum ResizeHandle
  {
    None,
    TopLeft,
    Top,
    TopRight,
    Right,
    BottomRight,
    Bottom,
    BottomLeft,
    Left,
    Center
  };

protected:
  void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
  ResizeHandle getHandleAt(const QPointF& pos);
  void updateCursor();

  ResizeHandle currentHandle;
  QPointF      lastMousePos;
  bool         isResizing;
  qreal        handleSize;
};

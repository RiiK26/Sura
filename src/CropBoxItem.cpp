#include "CropBoxItem.h"

#include <QPen>

CropBoxItem::CropBoxItem(const QRectF& rect, QGraphicsItem* parent) :
    QGraphicsRectItem(rect, parent),
    currentHandle(None),
    isResizing(false),
    handleSize(10.0)
{
  setAcceptHoverEvents(true);
  setFlags(ItemIsSelectable | ItemSendsGeometryChanges);
  QPen pen(Qt::DashLine);
  pen.setColor(Qt::red);
  pen.setWidth(2);
  setPen(pen);
  setBrush(QColor(255, 0, 0, 50));
}

CropBoxItem::ResizeHandle CropBoxItem::getHandleAt(const QPointF& pos)
{
  QRectF r    = rect();
  qreal  x    = pos.x();
  qreal  y    = pos.y();

  bool left   = x >= r.left() && x <= r.left() + handleSize;
  bool right  = x <= r.right() && x >= r.right() - handleSize;
  bool top    = y >= r.top() && y <= r.top() + handleSize;
  bool bottom = y <= r.bottom() && y >= r.bottom() - handleSize;

  if (left && top)
    return TopLeft;
  if (right && top)
    return TopRight;
  if (left && bottom)
    return BottomLeft;
  if (right && bottom)
    return BottomRight;
  if (left)
    return Left;
  if (right)
    return Right;
  if (top)
    return Top;
  if (bottom)
    return Bottom;

  if (r.contains(pos))
    return Center;

  return None;
}

void CropBoxItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
  currentHandle = getHandleAt(event->pos());
  updateCursor();
  QGraphicsRectItem::hoverMoveEvent(event);
}

void CropBoxItem::updateCursor()
{
  switch (currentHandle) {
  case TopLeft :
  case BottomRight :
    setCursor(Qt::SizeFDiagCursor);
    break;
  case TopRight :
  case BottomLeft :
    setCursor(Qt::SizeBDiagCursor);
    break;
  case Left :
  case Right :
    setCursor(Qt::SizeHorCursor);
    break;
  case Top :
  case Bottom :
    setCursor(Qt::SizeVerCursor);
    break;
  default :
    setCursor(Qt::SizeAllCursor);  // For moving
    break;
  }
}

void CropBoxItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  currentHandle = getHandleAt(event->pos());
  if (currentHandle != None) {
    isResizing   = true;
    lastMousePos = event->pos();
  }
  else {
    QGraphicsRectItem::mousePressEvent(event);
  }
}

void CropBoxItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  if (isResizing) {
    QPointF delta = event->pos() - lastMousePos;
    QRectF  r     = rect();

    if (currentHandle == Center) {
      r.translate(delta);
      if (parentItem()) {
        QRectF parentBounds = parentItem()->boundingRect();
        if (r.left() < parentBounds.left())
          r.moveLeft(parentBounds.left());
        if (r.right() > parentBounds.right())
          r.moveRight(parentBounds.right());
        if (r.top() < parentBounds.top())
          r.moveTop(parentBounds.top());
        if (r.bottom() > parentBounds.bottom())
          r.moveBottom(parentBounds.bottom());
      }
    }
    else {
      switch (currentHandle) {
      case TopLeft :
        r.setTopLeft(r.topLeft() + delta);
        break;
      case Top :
        r.setTop(r.top() + delta.y());
        break;
      case TopRight :
        r.setTopRight(r.topRight() + delta);
        break;
      case Right :
        r.setRight(r.right() + delta.x());
        break;
      case BottomRight :
        r.setBottomRight(r.bottomRight() + delta);
        break;
      case Bottom :
        r.setBottom(r.bottom() + delta.y());
        break;
      case BottomLeft :
        r.setBottomLeft(r.bottomLeft() + delta);
        break;
      case Left :
        r.setLeft(r.left() + delta.x());
        break;
      default :
        break;
      }

      if (parentItem()) {
        QRectF parentBounds = parentItem()->boundingRect();
        if (r.left() < parentBounds.left())
          r.setLeft(parentBounds.left());
        if (r.right() > parentBounds.right())
          r.setRight(parentBounds.right());
        if (r.top() < parentBounds.top())
          r.setTop(parentBounds.top());
        if (r.bottom() > parentBounds.bottom())
          r.setBottom(parentBounds.bottom());
      }
    }

    // Prevent negative width/height
    if (r.width() > handleSize && r.height() > handleSize) {
      setRect(r);
      lastMousePos = event->pos();
    }
  }
  else {
    QGraphicsRectItem::mouseMoveEvent(event);
  }
}

void CropBoxItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  if (isResizing) {
    isResizing = false;
  }
  else {
    QGraphicsRectItem::mouseReleaseEvent(event);
  }
}

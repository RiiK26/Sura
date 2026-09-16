#pragma once

#include "CropBoxItem.h"

#include <QAction>
#include <QBrush>
#include <QDockWidget>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QMainWindow>
#include <QMenu>
#include <QPen>
#include <QStringList>
#include <QTextEdit>

class Sura: public QMainWindow
{
  Q_OBJECT

public:
  Sura(QWidget* parent = nullptr);
  void loadInitialFile(const QString& fileName);

protected:
  bool eventFilter(QObject* obj, QEvent* event) override;
  void closeEvent(QCloseEvent* event) override;

private slots:
  void openFile();
  bool saveImage();
  void nextImage();
  void previousImage();
  void enterCropMode();
  void applyCrop();
  void cancelCropMode();
  void zoomIn();
  void zoomOut();
  void rotateLeft();
  void rotateRight();
  void toggleExif();

private:
  void loadImage(const QString& fileName);
  void updateDirectoryList(const QString& fileName);
  void loadExif(const QString& fileName);

  QGraphicsView*       view;
  QGraphicsScene*      scene;
  QGraphicsPixmapItem* pixmapItem;
  QImage               currentImage;

  QString     currentFile;
  QStringList currentDirectoryFiles;
  int         currentIndex;

  QDockWidget* exifDock;
  QTextEdit*   exifTextEdit;

  CropBoxItem* cropBox;
  QAction*     applyCropAct;
  QAction*     saveAct;

  bool isUnsaved;
  bool showRawExif;
  bool maybeSave();
};

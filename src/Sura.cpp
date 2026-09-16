#include "Sura.h"

#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWheelEvent>
#include <QWidget>
#include <exiv2/exiv2.hpp>
#include <sstream>

Sura::Sura(QWidget* parent) :
    QMainWindow(parent),
    currentIndex(-1),
    pixmapItem(nullptr),
    cropBox(nullptr),
    isUnsaved(false),
    showRawExif(false)
{
  // Setup Graphics View & Scene
  scene = new QGraphicsScene(this);
  view  = new QGraphicsView(scene, this);
  view->setDragMode(QGraphicsView::ScrollHandDrag);
  view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
  view->viewport()->installEventFilter(this);
  setCentralWidget(view);

  // Setup Exif Dock
  exifDock = new QDockWidget("EXIF Data", this);
  exifDock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);

  QWidget*     dockContents = new QWidget();
  QVBoxLayout* dockLayout   = new QVBoxLayout(dockContents);
  dockLayout->setContentsMargins(0, 0, 0, 0);

  QPushButton* toggleRawBtn = new QPushButton("Show Raw/Advanced Data", this);
  toggleRawBtn->setCheckable(true);
  toggleRawBtn->setChecked(showRawExif);
  connect(toggleRawBtn, &QPushButton::toggled, this, [this](bool checked) {
    this->showRawExif = checked;
    if (!this->currentFile.isEmpty()) {
      this->loadExif(this->currentFile);
    }
  });

  exifTextEdit = new QTextEdit(dockContents);
  exifTextEdit->setReadOnly(true);

  dockLayout->addWidget(toggleRawBtn);
  dockLayout->addWidget(exifTextEdit);

  exifDock->setWidget(dockContents);
  addDockWidget(Qt::RightDockWidgetArea, exifDock);
  exifDock->hide();

  // Create Actions
  QAction* openAct = new QAction(QIcon::fromTheme("document-open"), "&Open...", this);
  openAct->setShortcut(QKeySequence::Open);
  connect(openAct, &QAction::triggered, this, &Sura::openFile);

  QAction* nextAct = new QAction(QIcon::fromTheme("go-next"), "&Next Image", this);
  nextAct->setShortcut(QKeySequence::MoveToNextChar);  // Right arrow
  connect(nextAct, &QAction::triggered, this, &Sura::nextImage);

  QAction* prevAct = new QAction(QIcon::fromTheme("go-previous"), "&Previous Image", this);
  prevAct->setShortcut(QKeySequence::MoveToPreviousChar);  // Left arrow
  connect(prevAct, &QAction::triggered, this, &Sura::previousImage);

  saveAct = new QAction(QIcon::fromTheme("document-save"), "&Save Image As...", this);
  saveAct->setShortcut(QKeySequence::Save);
  connect(saveAct, &QAction::triggered, this, &Sura::saveImage);

  QAction* cropAct = new QAction(QIcon::fromTheme("transform-crop"), "&Crop", this);
  cropAct->setShortcut(tr("Ctrl+K"));
  connect(cropAct, &QAction::triggered, this, &Sura::enterCropMode);

  applyCropAct = new QAction(QIcon::fromTheme("dialog-ok-apply"), "&Apply Crop", this);
  applyCropAct->setShortcut(Qt::Key_Return);
  connect(applyCropAct, &QAction::triggered, this, &Sura::applyCrop);
  applyCropAct->setEnabled(false);

  QAction* cancelCropAct = new QAction(this);
  cancelCropAct->setShortcut(Qt::Key_Escape);
  connect(cancelCropAct, &QAction::triggered, this, &Sura::cancelCropMode);
  addAction(cancelCropAct);

  QAction* exifAct = new QAction(QIcon::fromTheme("dialog-information"), "EXIF &DATA", this);
  exifAct->setShortcut(tr("Ctrl+E"));
  connect(exifAct, &QAction::triggered, this, &Sura::toggleExif);

  QAction* quitAct = new QAction(QIcon::fromTheme("application-exit"), "&Quit", this);
  quitAct->setShortcut(QKeySequence::Quit);
  connect(quitAct, &QAction::triggered, this, &QWidget::close);

  QAction* zoomInAct = new QAction(QIcon::fromTheme("zoom-in"), "Zoom &In", this);
  zoomInAct->setShortcut(QKeySequence::ZoomIn);
  connect(zoomInAct, &QAction::triggered, this, &Sura::zoomIn);

  QAction* zoomOutAct = new QAction(QIcon::fromTheme("zoom-out"), "Zoom &Out", this);
  zoomOutAct->setShortcut(QKeySequence::ZoomOut);
  connect(zoomOutAct, &QAction::triggered, this, &Sura::zoomOut);

  QAction* rotateLeftAct = new QAction(QIcon::fromTheme("object-rotate-left"), "Rotate &Left", this);
  rotateLeftAct->setShortcut(tr("Ctrl+L"));
  connect(rotateLeftAct, &QAction::triggered, this, &Sura::rotateLeft);

  QAction* rotateRightAct = new QAction(QIcon::fromTheme("object-rotate-right"), "Rotate &Right", this);
  rotateRightAct->setShortcut(tr("Ctrl+R"));
  connect(rotateRightAct, &QAction::triggered, this, &Sura::rotateRight);

  // Setup Menus
  QMenu* fileMenu = menuBar()->addMenu("&File");
  fileMenu->addAction(openAct);
  fileMenu->addAction(saveAct);
  fileMenu->addSeparator();
  fileMenu->addAction(quitAct);

  QMenu* viewMenu = menuBar()->addMenu("&View");
  viewMenu->addAction(nextAct);
  viewMenu->addAction(prevAct);
  viewMenu->addSeparator();
  viewMenu->addAction(zoomInAct);
  viewMenu->addAction(zoomOutAct);
  viewMenu->addSeparator();
  viewMenu->addAction(rotateLeftAct);
  viewMenu->addAction(rotateRightAct);
  viewMenu->addSeparator();
  viewMenu->addAction(cropAct);
  viewMenu->addAction(applyCropAct);
  viewMenu->addAction(exifAct);

  // Setup Toolbar
  QToolBar* toolBar = addToolBar("Main Toolbar");
  toolBar->addAction(openAct);
  toolBar->addAction(saveAct);
  toolBar->addAction(prevAct);
  toolBar->addAction(nextAct);
  toolBar->addAction(zoomInAct);
  toolBar->addAction(zoomOutAct);
  toolBar->addAction(rotateLeftAct);
  toolBar->addAction(rotateRightAct);
  toolBar->addAction(cropAct);
  toolBar->addAction(applyCropAct);
  toolBar->addAction(exifAct);
}

void Sura::openFile()
{
  if (!maybeSave())
    return;
  QString fileName =
    QFileDialog::getOpenFileName(this, "Open Image", QDir::homePath(), "Images (*.png *.xpm *.jpg *.jpeg *.bmp *.svg)");
  if (!fileName.isEmpty()) {
    loadImage(fileName);
    updateDirectoryList(fileName);
  }
}

void Sura::loadInitialFile(const QString& fileName)
{
  if (!fileName.isEmpty()) {
    loadImage(fileName);
    updateDirectoryList(fileName);
  }
}

#ifndef SURA_VERSION
  #define SURA_VERSION "vUnknown"
#endif

bool Sura::saveImage()
{
  if (currentImage.isNull())
    return true;
  QString fileName =
    QFileDialog::getSaveFileName(this, "Save Image As", currentFile, "Images (*.png *.xpm *.jpg *.jpeg *.bmp *.svg)");
  if (!fileName.isEmpty()) {
    if (currentImage.save(fileName)) {
      currentFile = fileName;
      setWindowTitle(QFileInfo(fileName).fileName() + QString(" - Sura %1").arg(SURA_VERSION));
      isUnsaved = false;
      updateDirectoryList(currentFile);
      return true;
    }
    else {
      QMessageBox::information(this, "Error", "Cannot save image.");
    }
  }
  return false;
}

void Sura::updateDirectoryList(const QString& fileName)
{
  QFileInfo fileInfo(fileName);
  QDir      dir = fileInfo.dir();

  QStringList filters;
  filters << "*.png" << "*.xpm" << "*.jpg" << "*.jpeg" << "*.bmp" << "*.svg";

  QFileInfoList fileList = dir.entryInfoList(filters, QDir::Files, QDir::Name);
  currentDirectoryFiles.clear();
  currentIndex = -1;

  for (int i = 0; i < fileList.size(); ++i) {
    currentDirectoryFiles.append(fileList.at(i).absoluteFilePath());
    if (fileList.at(i).absoluteFilePath() == fileName) {
      currentIndex = i;
    }
  }
}

void Sura::loadImage(const QString& fileName)
{
  QFileInfo fileInfo(fileName);
  if (!fileInfo.isFile()) {
    QMessageBox::warning(this, "Security Warning", "The selected path is not a regular file.");
    return;
  }

  if (currentImage.load(fileName)) {
    if (pixmapItem) {
      scene->removeItem(pixmapItem);
      delete pixmapItem;
    }
    pixmapItem = scene->addPixmap(QPixmap::fromImage(currentImage));
    scene->setSceneRect(pixmapItem->boundingRect());
    view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
    currentFile = fileName;
    setWindowTitle(QFileInfo(fileName).fileName() + QString(" - Sura %1").arg(SURA_VERSION));

    loadExif(fileName);
  }
  else {
    QMessageBox::information(this, "Error", "Cannot load image.");
  }
}

void Sura::nextImage()
{
  if (!maybeSave())
    return;
  if (currentDirectoryFiles.isEmpty() || currentIndex == -1)
    return;
  currentIndex = (currentIndex + 1) % currentDirectoryFiles.size();
  loadImage(currentDirectoryFiles.at(currentIndex));
}

void Sura::previousImage()
{
  if (!maybeSave())
    return;
  if (currentDirectoryFiles.isEmpty() || currentIndex == -1)
    return;
  currentIndex = (currentIndex - 1 + currentDirectoryFiles.size()) % currentDirectoryFiles.size();
  loadImage(currentDirectoryFiles.at(currentIndex));
}

void Sura::enterCropMode()
{
  if (!pixmapItem)
    return;

  if (cropBox) {
    scene->removeItem(cropBox);
    delete cropBox;
    cropBox = nullptr;
  }

  // Create crop box in the center, 50% of the image size
  QRectF imgRect = pixmapItem->boundingRect();
  QRectF cropRect(imgRect.width() * 0.25, imgRect.height() * 0.25, imgRect.width() * 0.5, imgRect.height() * 0.5);

  cropBox = new CropBoxItem(cropRect, pixmapItem);
  applyCropAct->setEnabled(true);
}

void Sura::applyCrop()
{
  if (!pixmapItem || !cropBox)
    return;

  QRectF actualCropRect = cropBox->mapRectToParent(cropBox->rect());
  QRect  intersection   = currentImage.rect().intersected(actualCropRect.toRect());

  if (intersection.isValid() && !intersection.isEmpty()) {
    currentImage = currentImage.copy(intersection);
    pixmapItem->setPixmap(QPixmap::fromImage(currentImage));
    scene->setSceneRect(pixmapItem->boundingRect());
    view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);

    cancelCropMode();
    isUnsaved = true;
    exifTextEdit->setText("Cropped image (Unsaved).");
  }
}

bool Sura::maybeSave()
{
  if (!isUnsaved)
    return true;
  QMessageBox::StandardButton ret = QMessageBox::warning(
    this, "Unsaved Changes", "The image has been cropped but not saved.\nDo you want to save your changes?",
    QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel
  );
  if (ret == QMessageBox::Save) {
    return saveImage();
  }
  else if (ret == QMessageBox::Cancel) {
    return false;
  }
  return true;
}

void Sura::closeEvent(QCloseEvent* event)
{
  if (maybeSave()) {
    event->accept();
  }
  else {
    event->ignore();
  }
}

void Sura::cancelCropMode()
{
  if (cropBox) {
    scene->removeItem(cropBox);
    delete cropBox;
    cropBox = nullptr;
    applyCropAct->setEnabled(false);
  }
}

bool Sura::eventFilter(QObject* obj, QEvent* event)
{
  if (obj == view->viewport()) {
    if (event->type() == QEvent::MouseButtonPress) {
      QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
      if (mouseEvent->button() == Qt::RightButton && cropBox != nullptr) {
        cancelCropMode();
        return true;
      }
    }
    else if (event->type() == QEvent::Wheel) {
      QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
      if (wheelEvent->angleDelta().y() > 0) {
        zoomIn();
      }
      else if (wheelEvent->angleDelta().y() < 0) {
        zoomOut();
      }
      return true;
    }
  }
  return QMainWindow::eventFilter(obj, event);
}

void Sura::zoomIn()
{
  if (view->transform().m11() < 20.0) {
    view->scale(1.2, 1.2);
  }
}

void Sura::zoomOut()
{
  if (view->transform().m11() > 0.05) {
    view->scale(1 / 1.2, 1 / 1.2);
  }
}

void Sura::rotateLeft()
{
  if (!pixmapItem || currentImage.isNull())
    return;

  QTransform transform;
  transform.rotate(-90);
  currentImage = currentImage.transformed(transform, Qt::SmoothTransformation);

  pixmapItem->setPixmap(QPixmap::fromImage(currentImage));
  scene->setSceneRect(pixmapItem->boundingRect());
  view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);

  cancelCropMode();
  isUnsaved = true;
  exifTextEdit->setText("Rotated image (Unsaved).");
}

void Sura::rotateRight()
{
  if (!pixmapItem || currentImage.isNull())
    return;

  QTransform transform;
  transform.rotate(90);
  currentImage = currentImage.transformed(transform, Qt::SmoothTransformation);

  pixmapItem->setPixmap(QPixmap::fromImage(currentImage));
  scene->setSceneRect(pixmapItem->boundingRect());
  view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);

  cancelCropMode();
  isUnsaved = true;
  exifTextEdit->setText("Rotated image (Unsaved).");
}

void Sura::toggleExif() { exifDock->setVisible(!exifDock->isVisible()); }

void Sura::loadExif(const QString& fileName)
{
  try {
    auto image = Exiv2::ImageFactory::open(fileName.toStdString());
    image->readMetadata();
    Exiv2::ExifData& exifData = image->exifData();

    if (exifData.empty()) {
      exifTextEdit->setText("No EXIF data found.");
      return;
    }

    QString html = "<h3>Image Metadata</h3><table border='0' cellspacing='4'>";
    for (auto i = exifData.begin(); i != exifData.end(); ++i) {
      std::string keyName = i->tagName();

      std::ostringstream os;
      os << i->value();
      QString value = QString::fromStdString(os.str());

      // Skip binary, extremely long, or noisy data
      if (!showRawExif) {
        if (
          keyName == "MakerNote" || keyName == "UserComment" || value.length() > 60
          || keyName.find("Offset") != std::string::npos || keyName.find("ByteCount") != std::string::npos
        )
          continue;
      }

      html += QString("<tr><td style='padding-right: 15px;'><b>%1</b></td><td>%2</td></tr>")
                .arg(QString::fromStdString(keyName).toHtmlEscaped())
                .arg(value.toHtmlEscaped());
    }
    html += "</table>";
    exifTextEdit->setHtml(html);
  } catch (const Exiv2::Error& e) {
    exifTextEdit->setText(QString("Error reading EXIF: %1").arg(e.what()));
  } catch (const std::exception& e) {
    exifTextEdit->setText(QString("Exception parsing EXIF metadata: %1").arg(e.what()));
  } catch (...) {
    exifTextEdit->setText("Unknown error occurred while parsing EXIF metadata.");
  }
}

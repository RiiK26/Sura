#include "Cli.h"
#include "Sura.h"
#include "Theme.h"

#include <QApplication>
#include <QIcon>

#ifndef SURA_VERSION
  #define SURA_VERSION "vUnknown"
#endif

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);
  app.setApplicationName("sura");
  app.setApplicationVersion(SURA_VERSION);
  app.setDesktopFileName("sura");
  app.setWindowIcon(QIcon(":/images/icon.png"));

  QString fileToLoad;
  if (!Cli::handleArguments(app, fileToLoad)) {
    return 0;
  }

  Theme::setup(app);

  Sura viewer;

  viewer.resize(1024, 768);
  viewer.setWindowTitle(QString("Sura %1").arg(SURA_VERSION));
  viewer.show();

  if (!fileToLoad.isEmpty()) {
    viewer.loadInitialFile(fileToLoad);
  }

  return app.exec();
}

#include "Cli.h"

#include <QCommandLineParser>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <iostream>

namespace Cli
{
  bool handleArguments(QApplication& app, QString& fileToLoad)
  {
    QCommandLineParser parser;
    parser.setApplicationDescription("Sura - A lightweight, Qt6 based image viewer");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption updateOption(QStringList() << "update", "Update Sura to the latest version.");
    parser.addOption(updateOption);

    QCommandLineOption uninstallOption(QStringList() << "uninstall", "Uninstall Sura from your system.");
    parser.addOption(uninstallOption);

    parser.addPositionalArgument("file", "Image file to open.");

    parser.process(app);

    if (parser.isSet(updateOption)) {
      QString updaterPath = QDir::homePath() + "/.local/bin/sura-update";
      if (QFile::exists(updaterPath)) {
        std::cout << "Launching Sura Updater..." << std::endl;
        QProcess::startDetached("/bin/sh", QStringList() << "-c" << QString("%1 >/dev/null 2>&1").arg(updaterPath));
      }
      else {
        std::cerr << "Error: Updater script not found at " << updaterPath.toStdString() << std::endl;
      }
      return false;
    }

    if (parser.isSet(uninstallOption)) {
      QString uninstallerPath = QDir::homePath() + "/.local/bin/sura-uninstall";
      if (QFile::exists(uninstallerPath)) {
        std::cout << "Launching Sura Uninstaller..." << std::endl;
        QProcess::startDetached("/bin/sh", QStringList() << "-c" << QString("%1 >/dev/null 2>&1").arg(uninstallerPath));
      }
      else {
        std::cerr << "Error: Uninstaller script not found at " << uninstallerPath.toStdString() << std::endl;
      }
      return false;
    }

    const QStringList positionalArguments = parser.positionalArguments();
    if (!positionalArguments.isEmpty()) {
      fileToLoad = positionalArguments.first();
    }

    return true;
  }

}  // namespace Cli

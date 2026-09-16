#include "Cli.h"

#include <QDir>
#include <QFile>
#include <QProcess>
#include <iostream>

namespace Cli
{
  bool handleArguments(const QStringList& args)
  {
    if (args.contains("--help") || args.contains("-h")) {
      std::cout << "Sura - A lightweight, Qt6 based image viewer\n\n"
                << "Usage: sura [FILE] [OPTIONS]\n\n"
                << "Options:\n"
                << "  --update      Update Sura to the latest version\n"
                << "  --uninstall   Uninstall Sura from your system\n"
                << "  --help, -h    Show this help message\n"
                << std::endl;
      return true;
    }

    if (args.contains("--update")) {
      QString updaterPath = QDir::homePath() + "/.local/bin/sura-update";
      if (QFile::exists(updaterPath)) {
        std::cout << "Launching Sura Updater..." << std::endl;
        QProcess::startDetached(updaterPath, QStringList());
      }
      else {
        std::cerr << "Error: Updater script not found at " << updaterPath.toStdString() << std::endl;
      }
      return true;
    }

    if (args.contains("--uninstall")) {
      QString uninstallerPath = QDir::homePath() + "/.local/bin/sura-uninstall";
      if (QFile::exists(uninstallerPath)) {
        std::cout << "Launching Sura Uninstaller..." << std::endl;
        QProcess::startDetached(uninstallerPath, QStringList());
      }
      else {
        std::cerr << "Error: Uninstaller script not found at " << uninstallerPath.toStdString() << std::endl;
      }
      return true;
    }

    return false;
  }

}  // namespace Cli

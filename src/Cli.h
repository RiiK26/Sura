#pragma once

#include <QApplication>
#include <QString>

namespace Cli
{
  /**
     * @brief Process command line arguments.
     * @param app The application instance.
     * @param fileToLoad Output parameter for the image file path to open.
     * @return true if the application should continue to the GUI, false if it should exit.
     */
  bool handleArguments(QApplication& app, QString& fileToLoad);
}  // namespace Cli

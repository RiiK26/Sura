#pragma once

#include <QStringList>

namespace Cli
{
  /**
     * @brief Process command line arguments.
     * @param args The application arguments.
     * @return true if a CLI command was handled and the app should exit.
     */
  bool handleArguments(const QStringList& args);
}  // namespace Cli

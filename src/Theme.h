#pragma once

#include <QApplication>

namespace Theme
{
  /**
     * @brief Setup and apply the dynamic Dark/Light theme.
     *        It also listens for system theme changes and updates automatically.
     * @param app The main QApplication instance.
     */
  void setup(QApplication& app);
}  // namespace Theme

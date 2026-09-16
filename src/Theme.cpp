#include "Theme.h"

#include <QColor>
#include <QPalette>
#include <QProcess>
#include <QStyle>
#include <QStyleFactory>
#include <QStyleHints>

namespace Theme
{
  bool isSystemDarkMode()
  {
    // 1. Standard Qt method (works if platform theme plugin is available)
    if (QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark) {
      return true;
    }

    // 2. Linux-specific fallback (especially for AppImages lacking the portal plugin)
#ifdef Q_OS_LINUX
    QProcess process;
    process.start(
      "dbus-send", QStringList() << "--print-reply=literal"
                                 << "--dest=org.freedesktop.portal.Desktop"
                                 << "/org/freedesktop/portal/desktop"
                                 << "org.freedesktop.portal.Settings.Read"
                                 << "string:org.freedesktop.appearance"
                                 << "string:color-scheme"
    );

    if (process.waitForFinished(500)) {
      QString output = process.readAllStandardOutput();
      // Freedesktop spec: 1 means prefer dark appearance
      if (output.contains("uint32 1")) {
        return true;
      }
    }
#endif

    return false;
  }

  void apply(QApplication& app)
  {
    app.setStyle(QStyleFactory::create("Fusion"));

    if (isSystemDarkMode()) {
      QPalette darkPalette;
      darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
      darkPalette.setColor(QPalette::WindowText, Qt::white);
      darkPalette.setColor(QPalette::Base, QColor(35, 35, 35));
      darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
      darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
      darkPalette.setColor(QPalette::ToolTipText, Qt::white);
      darkPalette.setColor(QPalette::Text, Qt::white);
      darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
      darkPalette.setColor(QPalette::ButtonText, Qt::white);
      darkPalette.setColor(QPalette::BrightText, Qt::red);
      darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
      darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
      darkPalette.setColor(QPalette::HighlightedText, Qt::black);
      app.setPalette(darkPalette);
    }
    else {
      app.setPalette(app.style()->standardPalette());
    }
  }

  void setup(QApplication& app)
  {
    apply(app);

    // Auto-update theme if user changes system setting while app is running
    QObject::connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged, &app, [&app](Qt::ColorScheme) {
      apply(app);
    });
  }

}  // namespace Theme

//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <memory>
#include <exception>
#include <qglobal.h>
#include <QLocale>
#include <QTranslator>

#if defined(QT_STATIC) || !(defined(QT_DLL) || defined(QT_SHARED))
#include <QtPlugin>
#endif

#include <QApplication>
#include <QDesktopWidget>
#include <ma/chrono/databasemodel_fwd.h>
#include <ma/chrono/navigationwindow.h>
#include <ma/chrono/excelexporter.h>

#if defined(QT_STATIC) || !(defined(QT_DLL) || defined(QT_SHARED))

#if QT_VERSION >= 0x050000
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
Q_IMPORT_PLUGIN(QSvgPlugin)
Q_IMPORT_PLUGIN(QSvgIconPlugin)
Q_IMPORT_PLUGIN(QIBaseDriverPlugin)
#else
Q_IMPORT_PLUGIN(qsvg)
Q_IMPORT_PLUGIN(qsvgicon)
Q_IMPORT_PLUGIN(qsqlibase)
#endif

#endif

namespace {

void installTranslation(QCoreApplication& application, const QString& path,
    const QString& localeName, const QString& baseFilename)
{
  std::auto_ptr<QTranslator> translator(new QTranslator(&application));
  if (translator->load(baseFilename + localeName, path))
  {
    application.installTranslator(translator.get());
    translator.release();
  }
}

void setApplicationDescription(QCoreApplication& /*application*/)
{
  QCoreApplication::setOrganizationName(
      QString::fromLatin1("Social Technologies"));
  QCoreApplication::setApplicationName(QString::fromLatin1("Chrono"));
}

void installSystemLocaleTranslation(QCoreApplication& application)
{
  QString translationPath(QString::fromLatin1(":/ma/chrono/translation/"));
  QString sysLocaleName(QLocale::system().name());
  installTranslation(application, translationPath, sysLocaleName, 
      QString::fromLatin1("qt_"));
  installTranslation(application, translationPath, sysLocaleName, 
      QString::fromLatin1("qtbase_"));
  installTranslation(application, translationPath, sysLocaleName, 
      QString::fromLatin1("chrono_"));
}

void adjustDesktopAwareGeometry(QWidget& mainWindow)
{
  QRect screenRect = 
      QApplication::desktop()->screenGeometry(&mainWindow);
  mainWindow.resize(
      screenRect.width() * 3 / 4, screenRect.height() * 6 / 7);
  QSize offsetSize = (screenRect.size() - mainWindow.frameSize()) / 2;
  QPoint startupPos = QPoint(
      screenRect.x() + offsetSize.width(), 
      screenRect.y() + offsetSize.height());

  mainWindow.move(startupPos);
}

} // anonymous namespace

int main(int argc, char* argv[])
{
  try
  {
    ma::chrono::ExcelExporter::ThreadInitializer excelExporterInitializer;
    QApplication application(argc, argv);

#if defined(QT_STATIC) || !(defined(QT_DLL) || defined(QT_SHARED))

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4127)
#endif
    Q_INIT_RESOURCE(chrono);
#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif

    setApplicationDescription(application);
    installSystemLocaleTranslation(application);

    ma::chrono::registerDatabaseMetaTypes();

    ma::chrono::NavigationWindow navigationWindow;  
    adjustDesktopAwareGeometry(navigationWindow);
    navigationWindow.show();

    return application.exec();
  }
  catch (const std::exception& e)
  {
    qFatal("Unexpected error: %s", e.what());
  }
  catch (...)
  {
    qFatal("Unknown exception");
  }
}

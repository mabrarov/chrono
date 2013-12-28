//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#include <memory>
#include <qtglobal>
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

} // anonymous namespace

int main(int argc, char* argv[])
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

  QCoreApplication::setOrganizationName(
      QString::fromWCharArray(L"Social Technologies"));
  QCoreApplication::setApplicationName(QString::fromWCharArray(L"Chrono"));

  QString translationPath(
      QString::fromWCharArray(L":/ma/chrono/translation/"));
  QString sysLocaleName(QLocale::system().name());
  installTranslation(application, translationPath, sysLocaleName, 
      QString::fromWCharArray(L"qt_"));
  installTranslation(application, translationPath, sysLocaleName, 
      QString::fromWCharArray(L"qtbase_"));
  installTranslation(application, translationPath, sysLocaleName, 
      QString::fromWCharArray(L"chrono_"));

  ma::chrono::registerDatabaseMetaTypes();      
  ma::chrono::NavigationWindow navigationWindow;  

  // Resize main window and center it in screen containing main window
  QRect screenRect = QApplication::desktop()->screenGeometry(&navigationWindow);
  navigationWindow.resize(
      screenRect.width() * 3 / 4, screenRect.height() * 6 / 7);
  QSize offsetSize = (screenRect.size() - navigationWindow.frameSize()) / 2;
  QPoint startupPos = QPoint(
      screenRect.x() + offsetSize.width(), 
      screenRect.y() + offsetSize.height());
  navigationWindow.move(startupPos);

  navigationWindow.show();
  return application.exec();
}

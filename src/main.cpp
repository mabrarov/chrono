//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#include <memory>
#include <QLocale>
#include <QTranslator>

#ifndef QT_DLL
#include <QtPlugin>
#endif

#include <QApplication>
#include <QDesktopWidget>
#include <ma/chrono/databasemodel_fwd.h>
#include <ma/chrono/navigationwindow.h>
#include <ma/chrono/excelexporter.h>

#ifndef QT_DLL
Q_IMPORT_PLUGIN(qsvg)
Q_IMPORT_PLUGIN(qsvgicon)
Q_IMPORT_PLUGIN(qsqlibase)
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
  QApplication application(argc, argv);

#pragma warning(push)
#pragma warning(disable: 4127)
  Q_INIT_RESOURCE(chrono);
#pragma warning(pop)

  QCoreApplication::setOrganizationName(
      QString::fromWCharArray(L"Social Technologies"));
  QCoreApplication::setApplicationName(QString::fromWCharArray(L"Chrono"));

  QString translationPath(
      QString::fromWCharArray(L":/ma/chrono/translation/"));
  //QString sysLocaleName(QLocale::system().name());  
  QString sysLocaleName(QString::fromWCharArray(L"ru_RU"));
  installTranslation(application, translationPath, sysLocaleName, 
      QString::fromWCharArray(L"qt_"));
  installTranslation(application, translationPath, sysLocaleName, 
      QString::fromWCharArray(L"chrono_"));

  ma::chrono::registerDatabaseMetaTypes();
  ma::chrono::ExcelExporter::ThreadInitializer excelExporterInitializer;
    
  ma::chrono::NavigationWindow navigationWindow;
  QSize desktopSize = QApplication::desktop()->size();
  navigationWindow.resize(desktopSize * 3 / 4);
  QSize startupPos = (desktopSize - navigationWindow.size()) / 2;  
  navigationWindow.move(startupPos.width(), startupPos.height());    
  navigationWindow.show();

  return application.exec();
}

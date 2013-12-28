//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#ifndef MA_CHRONO_EXCELEXPORTER_H
#define MA_CHRONO_EXCELEXPORTER_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/utility.hpp>
#include <QtGlobal>
#include <ma/chrono/exportdatalist.h>
#include <ma/chrono/databasemodel_fwd.h>

QT_BEGIN_NAMESPACE
class QTableView;
QT_END_NAMESPACE

namespace ma {
namespace chrono {

class ExcelExporter : private boost::noncopyable
{
private:
  ExcelExporter();

public:
  class ThreadInitializer : private boost::noncopyable 
  {
  public:
    explicit ThreadInitializer();
    ~ThreadInitializer();
  }; // ThreadInitializer

  static bool exportTable(const QTableView& table);
  static bool exportIcdoc(qint64 icdocId, const QString& templateFilename, 
      const DatabaseModel& databaseModel);
  static bool exportIct(qint64 ictId, const QString& templateFilename, 
      const DatabaseModel& databaseModel);
  static bool exportDataList(const ExportDataList& exportDataList);
}; // class ExcelExporter

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_EXCELEXPORTER_H

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#ifndef MA_CHRONO_BASICFILTERCONDITION_H
#define MA_CHRONO_BASICFILTERCONDITION_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cstddef>
#include <boost/utility.hpp>
#include <QString>
#include <ma/chrono/databasemodel_fwd.h>

QT_BEGIN_NAMESPACE
class QStringList;
class QSqlQuery;  
QT_END_NAMESPACE

namespace ma {
namespace chrono {

class BasicFilterCondition : private boost::noncopyable
{
public:
  BasicFilterCondition()
  {
  }

  virtual ~BasicFilterCondition()
  {
  }      

  QString ñonditionText() const
  {
    return conditionText_;
  }

  QString sql() const
  {
    return sql_;
  }

  virtual std::size_t fieldCount() const = 0;
  virtual std::size_t paramCount() const = 0;
  virtual void bindQueryParams(QSqlQuery& query, std::size_t baseParamNo, 
      const DatabaseModel& databaseModel) const = 0;      

protected:
  void setConditionText(const QString& conditionText)
  {
    conditionText_ = conditionText;
  }

  void setSql(const QString& sql)
  {
    sql_ = sql;
  }

private:
  QString conditionText_;
  QString sql_;
}; // class BasicFilterCondition    

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_BASICFILTERCONDITION_H

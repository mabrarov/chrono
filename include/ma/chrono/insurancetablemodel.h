//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#ifndef MA_CHRONO_INSURANCETABLEMODEL_H
#define MA_CHRONO_INSURANCETABLEMODEL_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cstddef>
#include <boost/optional.hpp>
#include <ma/chrono/tabledescription.h>
#include <ma/chrono/basictablemodel.h>
#include <ma/chrono/insurancetablemodel_fwd.h>

namespace ma {
namespace chrono {

class InsuranceTableModel : public BasicTableModel
{
  Q_OBJECT

public:                  
  enum FilterType
  {
    expiredFilterId = 0,
    expireTodayFilterId,
    expireTomorrowFilterId,
    expireThisWeekFilterId,
    expireThisMonthFilterId,
    expireIn10DaysFilterId,
    filterIdCount
  }; // enum FilterType
      
  InsuranceTableModel(const DatabaseModelPtr& databaseModel, 
      QObject* parent, const OptionalInt& filterId, 
      const FilterExpression& filterExpression, 
      const OrderExpression& orderExpression);
  ~InsuranceTableModel();            
  int columnCount(const QModelIndex& parent = QModelIndex()) const;      
  QVariant headerData(int section, Qt::Orientation orientation, 
      int role = Qt::DisplayRole) const;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
  static TableDescription tableDescription();

protected:
  QString internalFilterSql() const;
  int internalFilterParamCount() const;
  void bindInternalFilterQueryParams(QSqlQuery& query, 
      std::size_t baseParamNo) const;
  QString selectSql() const;            

private:
  Q_DISABLE_COPY(InsuranceTableModel)
      
  OptionalInt filterId_;
}; // class CcdTableModel

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_INSURANCETABLEMODEL_H

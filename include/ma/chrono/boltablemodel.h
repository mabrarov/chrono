//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_BOLTABLEMODEL_H
#define MA_CHRONO_BOLTABLEMODEL_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cstddef>
#include <boost/optional.hpp>
#include <ma/chrono/types.h>
#include <ma/chrono/tabledescription.h>
#include <ma/chrono/basictablemodel.h>
#include <ma/chrono/boltablemodel_fwd.h>

namespace ma {
namespace chrono {

class BolTableModel : public BasicTableModel
{
  Q_OBJECT

public:
  enum FilterType
  {
    todayFilterId = 0,
    yesterdayFilterId,
    thisWeekFilterId,
    prevWeekFilterId,
    thisMonthFilterId,
    prevMonthFilterId,
    filterIdCount
  }; // enum FilterType

  BolTableModel(const DatabaseModelPtr& databaseModel, 
      QObject* parent, const OptionalInt& filterId, 
      const FilterExpression& filterExpression, 
      const OrderExpression& orderExpression);
  ~BolTableModel();

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
  Q_DISABLE_COPY(BolTableModel) 

  OptionalInt filterId_;
}; // class BolTableModel

} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_BOLTABLEMODEL_H

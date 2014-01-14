//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_ICTTABLEMODEL_H
#define MA_CHRONO_ICTTABLEMODEL_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cstddef>
#include <boost/optional.hpp>
#include <ma/chrono/tabledescription.h>
#include <ma/chrono/basictablemodel.h>
#include <ma/chrono/icttablemodel_fwd.h>

namespace ma {
namespace chrono {

class IctTableModel : public BasicTableModel
{
  Q_OBJECT

public:
  enum FilterType
  {
    notConfirmedFilterId = 0,
    confirmedFilterId,
    filterIdCount
  }; // enum FilterType

  IctTableModel(const DatabaseModelPtr& databaseModel, 
      QObject* parent, const OptionalInt& filterId, 
      const FilterExpression& filterExpression, 
      const OrderExpression& orderExpression);
  ~IctTableModel();            
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
  Q_DISABLE_COPY(IctTableModel)
      
  OptionalInt filterId_;
}; // class IctTableModel

} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_ICTTABLEMODEL_H

//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_BASICTABLEMODEL_H
#define MA_CHRONO_BASICTABLEMODEL_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cstddef>
#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>
#include <QStringList>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <ma/chrono/types.h>
#include <ma/chrono/databasemodel_fwd.h>
#include <ma/chrono/tabledescription.h>
#include <ma/chrono/filterexpression.h>
#include <ma/chrono/orderexpression.h>
#include <ma/chrono/basictablemodel_fwd.h>

namespace ma {
namespace chrono {

class BasicTableModel : public QSqlQueryModel
{
  Q_OBJECT

public:
  BasicTableModel(const DatabaseModelPtr& databaseModel,
      QObject* parent, const FilterExpression& filterExpression, 
      const OrderExpression& orderExpression);
  ~BasicTableModel();

  DatabaseModelPtr databaseModel() const;      

  void open();
  void close();
  void refresh();      

  void setFilterExpression(const FilterExpression& filterExpression);
  FilterExpression filterExpression() const;
  void setOrderExpression(const OrderExpression& orderExpression);
  OrderExpression orderExpression() const;

protected:
  virtual QString internalFilterSql() const;
  virtual int internalFilterParamCount() const;
  virtual void bindInternalFilterQueryParams(QSqlQuery& query, 
      std::size_t baseParamNo) const;
  virtual QString selectSql() const = 0;     

private:
  Q_DISABLE_COPY(BasicTableModel)

  OptionalQString retriveSelectSql();
  OptionalQString retriveInternalFilterSql();
  OptionalQString retriveUserFilterSql();
  OptionalQString retriveUserOrderSql();
  OptionalQString retriveQuerySql();
  boost::optional<QSqlQuery> retriveQuery();

  DatabaseModelPtr databaseModel_;      
  FilterExpression filterExpression_;
  OrderExpression  orderExpression_;
  OptionalQString  selectSql_;
  OptionalQString  internalFilterSql_;
  OptionalQString  userFilterSql_;
  OptionalQString  userOrderSql_;
  OptionalQString  querySql_;
  boost::optional<QSqlQuery> query_;
}; // class BasicTableModel

} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_BASICTABLEMODEL_H

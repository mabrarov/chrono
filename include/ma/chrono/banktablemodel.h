//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_BANKTABLEMODEL_H
#define MA_CHRONO_BANKTABLEMODEL_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ma/chrono/tabledescription.h>
#include <ma/chrono/basictablemodel.h>
#include <ma/chrono/banktablemodel_fwd.h>

namespace ma {
namespace chrono {

class BankTableModel : public BasicTableModel
{
  Q_OBJECT

public:
  BankTableModel(const DatabaseModelPtr& databaseModel,
      QObject* parent, const FilterExpression& filterExpression, 
      const OrderExpression& orderExpression);
  ~BankTableModel();

  int columnCount(const QModelIndex& parent = QModelIndex()) const;      
  QVariant headerData(int section, Qt::Orientation orientation, 
      int role = Qt::DisplayRole) const;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
  static TableDescription tableDescription();      

protected:
  QString selectSql() const;            

private:
  Q_DISABLE_COPY(BankTableModel)
}; // class BankTableModel

} // namespace chrono
} // namespace ma

#endif // MA_CHRONO_BANKTABLEMODEL_H

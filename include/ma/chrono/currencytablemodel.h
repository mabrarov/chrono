//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#ifndef MA_CHRONO_CURRENCYTABLEMODEL_H
#define MA_CHRONO_CURRENCYTABLEMODEL_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <ma/chrono/tabledescription.h>
#include <ma/chrono/basictablemodel.h>
#include <ma/chrono/currencytablemodel_fwd.h>

namespace ma {
namespace chrono {

class CurrencyTableModel : public BasicTableModel
{
  Q_OBJECT

public:
  CurrencyTableModel(const DatabaseModelPtr& databaseModel,
      QObject* parent, const FilterExpression& filterExpression, 
      const OrderExpression& orderExpression);
  ~CurrencyTableModel();

  int columnCount(const QModelIndex& parent = QModelIndex()) const;
  QVariant headerData(int section, Qt::Orientation orientation, 
      int role = Qt::DisplayRole) const;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
  static TableDescription tableDescription();

protected:
  QString selectSql() const;

private:
  Q_DISABLE_COPY(CurrencyTableModel)
}; // class CurrencyTableModel

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_CURRENCYTABLEMODEL_H

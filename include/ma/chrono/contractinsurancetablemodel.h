//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#ifndef MA_CHRONO_CONTRACTCCDTABLEMODEL_H
#define MA_CHRONO_CONTRACTCCDTABLEMODEL_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cstddef>
#include <boost/optional.hpp>
#include <ma/chrono/tabledescription.h>
#include <ma/chrono/basictablemodel.h>
#include <ma/chrono/contractinsurancetablemodel_fwd.h>

namespace ma {
namespace chrono {

class ContractInsuranceTableModel : public BasicTableModel
{
  Q_OBJECT

public:
  ContractInsuranceTableModel(
      const DatabaseModelPtr& databaseModel, QObject* parent,
      const OptionalQInt64& contractId, 
      const FilterExpression& filterExpression, 
      const OrderExpression& orderExpression);
  ~ContractInsuranceTableModel();

  int columnCount(const QModelIndex& parent = QModelIndex()) const;            
  QVariant headerData(int section, Qt::Orientation orientation, 
      int role = Qt::DisplayRole) const;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
  static TableDescription tableDescription();            

  void setContractId(qint64 contractId);

protected:
  QString internalFilterSql() const;
  int internalFilterParamCount() const;
  void bindInternalFilterQueryParams(QSqlQuery& query, 
      std::size_t baseParamNo) const;
  QString selectSql() const;            

private:
  Q_DISABLE_COPY(ContractInsuranceTableModel)      

  OptionalQInt64 contractId_;
}; // class ContractInsuranceTableModel

} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_CONTRACTCCDTABLEMODEL_H

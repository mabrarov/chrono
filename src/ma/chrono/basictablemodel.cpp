/*
TRANSLATOR ma::chrono::BasicTableModel
*/

//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@mail.ru)
//

#include <QSqlDatabase>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/model/currencyamount.h>
#include <ma/chrono/model/rateamount.h>
#include <ma/chrono/basictablemodel.h>

namespace ma {
namespace chrono {

BasicTableModel::BasicTableModel(const DatabaseModelPtr& databaseModel,
    QObject* parent, const FilterExpression& filterExpression, 
    const OrderExpression& orderExpression)
  : QSqlQueryModel(parent)
  , databaseModel_(databaseModel)
  , filterExpression_(filterExpression)
  , orderExpression_(orderExpression)
{      
}

BasicTableModel::~BasicTableModel()
{
}    

DatabaseModelPtr BasicTableModel::databaseModel() const
{
  return databaseModel_;
}

void BasicTableModel::open()
{
  if (boost::optional<QSqlQuery> query = retriveQuery())
  {
    setQuery(query.get());
  }
}

void BasicTableModel::close()
{      
  beginResetModel();
  clear();      
  endResetModel();      
  query_.reset();
}

void BasicTableModel::refresh()
{
  QSqlDatabase database = databaseModel_->database();
  if (database.isValid())
  {
    if (database.isOpen())
    {          
      query_.reset();
      if (boost::optional<QSqlQuery> query = retriveQuery())
      {            
        setQuery(QSqlQuery());
        setQuery(query.get());
      }                                
    }
  }      
}
    
void BasicTableModel::setFilterExpression(const FilterExpression& filterExpression)      
{
  filterExpression_ = filterExpression;
  userFilterSql_.reset();
  querySql_.reset();
  query_.reset();
  if (boost::optional<QSqlQuery> query = retriveQuery())
  {            
    setQuery(QSqlQuery());
    setQuery(query.get());
  }
}

FilterExpression BasicTableModel::filterExpression() const    
{
  return filterExpression_;
}

void BasicTableModel::setOrderExpression(const OrderExpression& orderExpression)
{
  orderExpression_ = orderExpression;
  userOrderSql_.reset();
  querySql_.reset();
  query_.reset();
  if (boost::optional<QSqlQuery> query = retriveQuery())
  {            
    setQuery(QSqlQuery());
    setQuery(query.get());
  }
}

OrderExpression BasicTableModel::orderExpression() const
{
  return orderExpression_;
}

QString BasicTableModel::internalFilterSql() const
{
  return QString();
}

int BasicTableModel::internalFilterParamCount() const
{
  return 0;
}

void BasicTableModel::bindInternalFilterQueryParams(QSqlQuery& /*query*/, 
    std::size_t /*baseParamNo*/) const
{
}    

OptionalQString BasicTableModel::retriveSelectSql()
{
  if (!selectSql_)
  {
    selectSql_.reset(selectSql());
  }
  return selectSql_;
}

OptionalQString BasicTableModel::retriveInternalFilterSql()
{
  if (!internalFilterSql_)
  {
    internalFilterSql_.reset(internalFilterSql());
  }
  return internalFilterSql_;
}

OptionalQString BasicTableModel::retriveUserFilterSql()
{
  static const QString implicationSql(" and ");
  if (!userFilterSql_ && !filterExpression_.isEmpty())
  {
    QString result = filterExpression_.sql();
    if (!result.isEmpty())
    {
      userFilterSql_.reset(result);
    }
  }  
  return userFilterSql_;
}

OptionalQString BasicTableModel::retriveUserOrderSql()
{      
  if (!userOrderSql_ && !orderExpression_.isEmpty())
  {
    typedef OrderExpression::ItemList list_type;
    typedef list_type::const_iterator iterator;
    QString result = orderExpression_.sql();        
    if (!result.isEmpty())
    {
      userOrderSql_.reset(result);
    }        
  }  
  return userOrderSql_;
}

OptionalQString BasicTableModel::retriveQuerySql()
{
  if (!querySql_)
  {                
    if (OptionalQString selectSql = retriveSelectSql())
    {
      QString completeQuerySql(selectSql.get());
      OptionalQString internalFilterSql = retriveInternalFilterSql();
      OptionalQString userFilterSql = retriveUserFilterSql();
      OptionalQString userOrderSql = retriveUserOrderSql();
      QString fullFilterSql;
      if (internalFilterSql)
      {
        fullFilterSql = internalFilterSql.get();
      }
      if (userFilterSql)
      {
        if (fullFilterSql.isEmpty())
        {
          fullFilterSql = userFilterSql.get();
        }
        else
        {
          static const QString implicationSql(" and ");
          fullFilterSql = QString("(") + fullFilterSql + QString(")") 
            + implicationSql + QString("(") + userFilterSql.get() + QString(")");
        }
      }          
      if (!fullFilterSql.isEmpty())
      {
        static const QString whereClause(" where ");                              
        completeQuerySql += whereClause + fullFilterSql;          
      }
      if (userOrderSql)
      {
        static const QString orderClause(" order by ");
        completeQuerySql += orderClause + userOrderSql.get();
      }
      querySql_.reset(completeQuerySql);
    }                
  }
  return querySql_;
}

boost::optional<QSqlQuery> BasicTableModel::retriveQuery()
{
  if (!query_)
  {        
    QSqlQuery newQuery(databaseModel_->database());        
    if (OptionalQString querySql = retriveQuerySql())
    {        
      if (newQuery.prepare(querySql.get()))
      {
        std::size_t baseParamNo = 0;
        bindInternalFilterQueryParams(newQuery, baseParamNo);
        if (!filterExpression_.isEmpty())
        {                        
          typedef FilterExpression::ItemList list_type;
          typedef list_type::const_iterator iterator;
          baseParamNo += internalFilterParamCount();
          list_type filterItems = filterExpression_.items();
          for (iterator i = filterItems.begin(), end = filterItems.end(); i != end; ++i)
          {
            std::size_t paramCount = (*i)->paramCount();
            (*i)->bindQueryParams(newQuery, baseParamNo, *databaseModel_);
            baseParamNo += paramCount;
          }
        }            
        if (newQuery.exec())
        {
          query_.reset(newQuery);          
        }
      }
    }
  }
  return query_;
}    

} // namespace chrono
} //namespace ma

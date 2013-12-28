//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#ifndef MA_CHRONO_FIELDDESCRIPTION_H
#define MA_CHRONO_FIELDDESCRIPTION_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <stdexcept>
#include <boost/throw_exception.hpp>
#include <boost/shared_ptr.hpp>
#include <QString>
#include <ma/chrono/dbconstants.h>

namespace ma {
namespace chrono {    

class FieldDescription;
typedef boost::shared_ptr<FieldDescription> FieldDescriptionPtr;    

class FieldDescription
{
public:
  enum FieldType 
  {
    dateField,
    dateTimeField,
    currencyAmountField,
    rateAmountField,
    entityField,
    entitySetField,
    stringField,
    boolField,
    intField        
  }; // enum FieldType 

  FieldDescription(const QString& title, const QString& fieldName, 
      bool nullable, FieldType fieldType, Entity entitySubType)
    : title_(title)
    , fieldName_(fieldName)
    , nullable_(nullable)
    , fieldType_(fieldType)
    , entitySubType_(entitySubType)
    , relationFieldName_()
    , entitySetTableName_()        
    , entitySetRelationFieldName_()
  {
    if (entitySetField == fieldType)
    {          
      boost::throw_exception(
          std::invalid_argument("invalid entity set description"));
    }
  }

  FieldDescription(const QString& title, const QString& fieldName, 
      bool nullable, FieldType fieldType, Entity entitySubType, 
      const QString& entitySetTableName, const QString& relationFieldName,
      const QString& entitySetRelationFieldName)
    : title_(title)
    , fieldName_(fieldName)
    , nullable_(nullable)
    , fieldType_(fieldType)
    , entitySubType_(entitySubType)
    , relationFieldName_(relationFieldName)
    , entitySetTableName_(entitySetTableName)        
    , entitySetRelationFieldName_(entitySetRelationFieldName)
  {
    if (entitySetField == fieldType)
    {
      bool isUnknownEntity = unknownEntity == entitySubType;
      if (isUnknownEntity 
          || relationFieldName.isEmpty() 
          || entitySetTableName.isEmpty() 
          || entitySetRelationFieldName.isEmpty())
      {
        boost::throw_exception(
            std::invalid_argument("invalid entity set description"));
      }
    }
  }

  QString title() const
  {
    return title_;
  }

  QString fieldName() const
  {
    return fieldName_;
  }

  bool nullable() const
  {
    return nullable_;
  }

  FieldType fieldType() const
  {
    return fieldType_;
  }

  Entity entitySubType() const
  {
    return entitySubType_;
  }

  QString relationFieldName() const
  {
    return relationFieldName_;
  }

  QString entitySetTableName() const
  {
    return entitySetTableName_;
  }      

  QString entitySetRelationFieldName() const
  {
    return entitySetRelationFieldName_;
  }

private:
  QString   title_;      
  QString   fieldName_;
  bool      nullable_;
  FieldType fieldType_;
  Entity    entitySubType_;      
  QString   relationFieldName_;
  QString   entitySetTableName_;      
  QString   entitySetRelationFieldName_;
}; // class FieldDescription    
    
} // namespace chrono
} //namespace ma

#endif // MA_CHRONO_FIELDDESCRIPTION_H

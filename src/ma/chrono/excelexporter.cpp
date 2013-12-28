//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#include <ma/chrono/buildenvironment.h>

#if (MA_CHRONO_BUILD_ENVIRONMENT_WINDOWS_VERSION_BIT == 64)

  #if (MA_CHRONO_BUILD_ENVIRONMENT_OFFICE_VERSION == 2007)

// Microsoft Office Objects
#import "C:\Program Files (x86)\Common Files\Microsoft Shared\OFFICE12\MSO.DLL" \
  rename("DocumentProperties","DocumentPropertiesXL") \
  rename("RGB", "RGBXL") \
  rename("SearchPath", "SearchPathXL")

// Microsoft VBA Objects
#import "C:\Program Files (x86)\Common Files\Microsoft Shared\VBA\VBA6\VBE6EXT.OLB"

// Excel Application Objects
#import "C:\Program Files (x86)\Microsoft Office\OFFICE12\EXCEL.EXE" \
  rename("DialogBox", "DialogBoxXL") \
  rename("RGB", "RGBXL") \
  rename("DocumentProperties", "DocumentPropertiesXL") \
  rename("ReplaceText","ReplaceTextXL") \
  rename("CopyFile","CopyFileXL") \
  exclude("IFont","IPicture") no_dual_interfaces

  #elif (MA_CHRONO_BUILD_ENVIRONMENT_OFFICE_VERSION == 2010) && (MA_CHRONO_BUILD_ENVIRONMENT_OFFICE_VERSION_BIT == 64)

// Microsoft Office Objects
#import "C:\Program Files\Common Files\Microsoft Shared\OFFICE14\MSO.DLL" \
  rename("DocumentProperties","DocumentPropertiesXL") \
  rename("RGB", "RGBXL") \
  rename("SearchPath", "SearchPathXL") 

// Microsoft VBA Objects
#import "C:\Program Files (x86)\Common Files\Microsoft Shared\VBA\VBA6\VBE6EXT.OLB"  

// Excel Application Objects
#import "C:\Program Files\Microsoft Office\OFFICE14\EXCEL.EXE" \
  rename("DialogBox", "DialogBoxXL") \
  rename("RGB", "RGBXL") \
  rename("DocumentProperties", "DocumentPropertiesXL") \
  rename("ReplaceText","ReplaceTextXL") \
  rename("CopyFile","CopyFileXL") \
  exclude("IFont","IPicture") no_dual_interfaces

  #elif (MA_CHRONO_BUILD_ENVIRONMENT_OFFICE_VERSION == 2013) && (MA_CHRONO_BUILD_ENVIRONMENT_OFFICE_VERSION_BIT == 64)

// Microsoft Office Objects
#import "C:\Program Files\Common Files\Microsoft Shared\OFFICE15\MSO.DLL" \
  rename("DocumentProperties","DocumentPropertiesXL") \
  rename("RGB", "RGBXL") \
  rename("SearchPath", "SearchPathXL") 

// Microsoft VBA Objects
#import "C:\Program Files (x86)\Common Files\Microsoft Shared\VBA\VBA6\VBE6EXT.OLB"  

// Excel Application Objects
#import "C:\Program Files\Microsoft Office\OFFICE15\EXCEL.EXE" \
  rename("DialogBox", "DialogBoxXL") \
  rename("RGB", "RGBXL") \
  rename("DocumentProperties", "DocumentPropertiesXL") \
  rename("ReplaceText","ReplaceTextXL") \
  rename("CopyFile","CopyFileXL") \
  exclude("IFont","IPicture") no_dual_interfaces

  #endif

#elif (MA_CHRONO_BUILD_ENVIRONMENT_WINDOWS_VERSION_BIT == 32)

  #if (MA_CHRONO_BUILD_ENVIRONMENT_OFFICE_VERSION == 2007)

// Microsoft Office Objects
#import "C:\Program Files\Common Files\Microsoft Shared\OFFICE12\MSO.DLL" \
  rename("DocumentProperties","DocumentPropertiesXL") \
  rename("RGB", "RGBXL") \
  rename("SearchPath", "SearchPathXL")

// Microsoft VBA Objects
#import "C:\Program Files\Common Files\Microsoft Shared\VBA\VBA6\VBE6EXT.OLB"

// Excel Application Objects
#import "C:\Program Files\Microsoft Office\OFFICE12\EXCEL.EXE" \
  rename("DialogBox", "DialogBoxXL") \
  rename("RGB", "RGBXL") \
  rename("DocumentProperties", "DocumentPropertiesXL") \
  rename("ReplaceText","ReplaceTextXL") \
  rename("CopyFile","CopyFileXL") \
  exclude("IFont","IPicture") no_dual_interfaces

  #endif

#endif

#include <ole2.h>
#include <boost/utility.hpp>
#include <boost/optional.hpp>
#include <QtGlobal>
#include <QVector>
#include <QDate>
#include <QDateTime>
#include <QAbstractItemModel>
#include <QTableView>
#include <QHeaderView>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <ma/chrono/model/currencyamount.h>
#include <ma/chrono/model/rateamount.h>
#include <ma/chrono/model/ict.h>
#include <ma/chrono/model/icdoc.h>
#include <ma/chrono/model/ccd.h>
#include <ma/chrono/model/paysheet.h>
#include <ma/chrono/databasemodel.h>
#include <ma/chrono/excelexporter.h>

namespace ma
{
  namespace chrono
  {    
    namespace
    {  
      void setValue(const Excel::RangePtr& range, int value)
      {        
        range->FormulaR1C1 = value;
      }

      void setValue(const Excel::RangePtr& range, const QString& value)
      {        
        range->FormulaR1C1 = reinterpret_cast<const wchar_t*>(value.utf16());
      }

      void setValue(const Excel::RangePtr& range, const QDateTime& value)
      {
        setValue(range, value.toString(Qt::DefaultLocaleShortDate));
      }

      void setValue(const Excel::RangePtr& range, const QDate& value)
      {
        setValue(range, value.toString(Qt::DefaultLocaleShortDate));
      }

      void setValue(const Excel::RangePtr& range, double value, const QString& numberFormat)
      {        
        if (!numberFormat.isEmpty())
        {
          range->NumberFormat = reinterpret_cast<const wchar_t*>(numberFormat.utf16());
        }        
        range->FormulaR1C1 = value;
      }

      void setValue(const Excel::RangePtr& range, const model::DecimalAmount& value, const QString& numberFormat)
      {        
        setValue(range, value.toDouble(), numberFormat);        
      }

      void setValue(const Excel::RangePtr& range, const model::CurrencyAmount& value)
      {
        QString numberFormat("0,00");
        setValue(range, value, numberFormat);
      }

      void setValue(const Excel::RangePtr& range, const model::RateAmount& value)
      {
        QString numberFormat("0,0000");        
        setValue(range, value, numberFormat);
      }

      void setValue(const Excel::RangePtr& range, bool value)
      {
        if (value)
        {
          range->FormulaR1C1 = 1;
        }
      }

      void setValue(const Excel::RangePtr& range, const QVariant& data)
      {                
        switch (data.type())
        {
        case QVariant::UserType:
          {            
            int userType = data.userType();
            if (currencyAmountTypeId == userType)
            {
              if (data.canConvert<model::CurrencyAmount>())
              {                
                setValue(range, data.value<model::CurrencyAmount>());                
              }
            }
            else if (rateAmountTypeId == userType)
            {
              if (data.canConvert<model::RateAmount>())
              {            
                setValue(range, data.value<model::RateAmount>());
              }
            }
          }
          break;
        case QVariant::Date:
          if (!data.isNull())
          {
            setValue(range, data.value<QDate>());              
          }
          break;
        case QVariant::DateTime:
          if (!data.isNull())
          {
            setValue(range, data.value<QDateTime>());
          }          
          break;
        case QVariant::Double:
          if (!data.isNull())
          {
            static const QString doubleFormat;
            setValue(range, data.value<double>(), doubleFormat);
          }          
          break;
        case QVariant::Int:
          if (!data.isNull())
          {            
            setValue(range, data.value<int>());
          }          
          break;
        default:          
          if (data.canConvert<QString>())
          {
            setValue(range, data.toString());
          }          
          break;
        }
      } 

      void exportItem(const Excel::RangePtr& range, const QAbstractItemModel& model, const QModelIndex& index)
      {
        QVariant data = model.data(index);
        if (!data.isNull())
        {
          setValue(range, data);
        }
        else
        {          
          QVariant data = model.data(index, Qt::CheckStateRole);
          if (!data.isNull() && data.canConvert<int>())
          {
            setValue(range, Qt::Checked == data.value<int>());
          }
        }        
      } 

      class ApplicationWrapper : private boost::noncopyable
      {              
      public:
        typedef Excel::_ApplicationPtr ptr_type;

        explicit ApplicationWrapper(const ptr_type application)
          : application_(application)
        {
        }

        ~ApplicationWrapper()
        {
          if (application_)
          {
            application_->Quit();
          }
        }

        ptr_type get() const
        {
          return application_;
        }

        ptr_type release() 
        {
          ptr_type temp(application_);
          application_ = ptr_type();
          return temp;
        }

      private:
        ptr_type application_;
      }; // class ApplicationWrapper

      class WorkbookWrapper : private boost::noncopyable
      {              
      public:
        typedef Excel::_WorkbookPtr ptr_type;

        explicit WorkbookWrapper(const ptr_type workbook)
          : workbook_(workbook)
        {
        }

        ~WorkbookWrapper()
        {
          if (workbook_)
          {
            workbook_->Close(false);
          }
        }

        ptr_type get() const
        {
          return workbook_;
        }

        ptr_type release() 
        {
          ptr_type temp(workbook_);
          workbook_ = ptr_type();
          return temp;
        }

      private:
        ptr_type workbook_;
      }; // class WorkbookWrapper

      struct IctPaysheetData
      {
        model::Paysheet paysheet;        
        QString currencyAlphabeticCode;
        QString contractNumber;        
        OptionalQString passportNumber;
        OptionalQString ccdNumber;
        QString residentFullName;      
        int curencyNumericCode;
      }; // IctPaysheetData

      typedef QVector<IctPaysheetData> IctPaysheetDataVector;

      struct IctData
      {
        model::Ict ict;
        int bankCode;
        QString bankShortName;            
        QString bankFullName;            
        QString createUserName;
        QString updateUserName;      
        IctPaysheetDataVector paysheetData;
      }; // struct IctData      

      IctPaysheetDataVector readIctPaysheetData(QSqlQuery& query, const DatabaseModel& databaseModel)
      {
        IctPaysheetDataVector vector;
        while (query.next())
        {
          IctPaysheetData data;
          data.paysheet.paysheetId = query.value(0).toLongLong();
          data.paysheet.contractId = query.value(1).toLongLong();
          data.contractNumber      = query.value(2).toString();
          data.paysheet.ccdId      = query.value(3).isNull() ? OptionalQInt64() : query.value(3).toLongLong();
          data.ccdNumber           = query.value(4).isNull() ? OptionalQString() : query.value(4).toString();
          data.paysheet.docNumber  = query.value(5).toString();
          data.paysheet.payDate    = databaseModel.convertFromServer(query.value(6).toDate());
          data.paysheet.amount     = query.value(7).toLongLong();
          data.currencyAlphabeticCode  = query.value(8).toString();
          data.paysheet.accountNumber  = query.value(9).isNull() ? OptionalQString() : query.value(9).toString();              
          data.paysheet.remark         = query.isNull(10) ? OptionalQString() : query.value(10).toString();
          data.paysheet.createUserId   = query.value(11).toLongLong();
          data.paysheet.updateUserId   = query.value(12).toLongLong();        
          data.paysheet.createTime = databaseModel.convertFromServer(query.value(15).toDateTime()); 
          data.paysheet.updateTime = databaseModel.convertFromServer(query.value(16).toDateTime());              
          data.paysheet.currencyId = query.value(17).toLongLong();
          data.passportNumber = query.value(18).isNull() ? OptionalQString() : query.value(18).toString();          
          data.residentFullName = query.value(19).toString();
          data.curencyNumericCode = query.value(20).toInt();
          vector.append(data);
        }      
        return vector;
      }

      boost::optional<IctData> readIctFromDatabase(qint64 ictId, const DatabaseModel& databaseModel)
      {
        static const char* querySql = "select " \
          "t.\"BANK_ID\", b.\"SHORT_NAME\", " \
          "t.\"ICT_DATE\", t.\"STATE\", t.\"RECV_DATE\", " \
          "t.\"REMARK\", " \
          "cu.\"LOGIN\", uu.\"LOGIN\", " \
          "t.\"CREATE_TIME\", t.\"UPDATE_TIME\", " \
          "t.\"CREATE_USER_ID\", t.\"UPDATE_USER_ID\", b.\"BANK_CODE\", b.\"FULL_NAME\" " \
          "from \"ICT\" t " \
          "join \"BANK\" b on t.\"BANK_ID\" = b.\"BANK_ID\" " \
          "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
          "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\" " \
          "where t.\"ICT_ID\" = ?";

        static const char* paysheetQuerySql = "select " \
          "rel.\"PAY_SHEET_ID\", " \
          "t.\"CONTRACT_ID\", cr.\"NUMBER\", " \
          "t.\"CCD_ID\", c.\"NUMBER\", " \
          "t.\"DOC_NUMBER\", t.\"PAY_DATE\", t.\"AMOUNT\", " \
          "cur.\"ALPHABETIC_CODE\", t.\"ACCOUNT_NUMBER\", t.\"REMARK\", " \
          "t.\"CREATE_USER_ID\", t.\"UPDATE_USER_ID\", " \
          "cu.\"LOGIN\", uu.\"LOGIN\", " \
          "t.\"CREATE_TIME\", t.\"UPDATE_TIME\", t.\"CURRENCY_ID\", " \
          "cr.\"PASSPORT_NUMBER\", r.\"FULL_NAME\", cur.\"NUMERIC_CODE\" " \
          "from \"PAYSHEET_ICT_RELATION\" rel " \
          "join \"PAY_SHEET\" t on t.\"PAY_SHEET_ID\" = rel.\"PAY_SHEET_ID\" " \
          "join \"CONTRACT\" cr on t.\"CONTRACT_ID\" = cr.\"CONTRACT_ID\" " \
          "join \"RESIDENT\" r on cr.\"RESIDENT_ID\" = r.\"RESIDENT_ID\" " \
          "left join \"CCD\" c on t.\"CCD_ID\" = c.\"CCD_ID\" " \
          "join \"CURRENCY\" cur on t.\"CURRENCY_ID\" = cur.\"CURRENCY_ID\" " \
          "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
          "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\" " \
          "where rel.\"ICT_ID\" = ? order by rel.\"TAB_ORDER\"";

        QSqlDatabase database(databaseModel.database());        
        if (database.transaction())
        {        
          QSqlQuery query(database);
          QSqlQuery paysheetQuery(database);
          if (query.prepare(querySql) && paysheetQuery.prepare(paysheetQuerySql))
          {
            query.bindValue(0, ictId);
            paysheetQuery.bindValue(0, ictId);
            if (query.exec() && paysheetQuery.exec())
            {
              if (query.next())
              { 
                IctData ictData;                
                ictData.ict.ictId     = ictId;
                ictData.ict.bankId    = query.value(0).toLongLong();
                ictData.bankShortName = query.value(1).toString();
                ictData.ict.ictDate   = databaseModel.convertFromServer(query.value(2).toDate());
                ictData.ict.state     = query.value(3).toInt();
                ictData.ict.recvDate  = query.isNull(4) ? OptionalQDate() : databaseModel.convertFromServer(query.value(4).toDate());
                ictData.ict.remark    = query.isNull(5) ? OptionalQString() : query.value(5).toString();              
                ictData.createUserName = query.value(6).toString(); 
                ictData.updateUserName = query.value(7).toString(); 
                ictData.ict.createTime = databaseModel.convertFromServer(query.value(8).toDateTime()); 
                ictData.ict.updateTime = databaseModel.convertFromServer(query.value(9).toDateTime());
                ictData.ict.createUserId  = query.value(10).toLongLong();
                ictData.ict.updateUserId  = query.value(11).toLongLong();
                ictData.bankCode = query.value(12).toInt();
                ictData.bankFullName = query.value(13).toString();
                ictData.paysheetData = readIctPaysheetData(paysheetQuery, databaseModel);
                if (database.commit())
                {
                  return ictData;
                }
                else
                {
                  database.rollback();
                  return boost::optional<IctData>();
                } // if (database.commit())
              } // if (query.next())
            } // if (query.exec() && paysheetQuery.exec())
          } // if (query.prepare(querySql) && paysheetQuery.prepare(paysheetQuerySql))
          database.rollback();
        } // if (database.transaction())
        return boost::optional<IctData>();
      }

      struct IcdocCcdData
      {
        model::Ccd ccd;
        OptionalQDate paymentLimitDate;
        QDate zeroTaxLimitDate;
        QString contractNumber;        
        OptionalQString passportNumber;
        qint64 currencyId;
        QString currencyAlphabeticCode;  
        QString residentFullName;
        int curencyNumericCode;
      }; // IcdocCcdData

      typedef QVector<IcdocCcdData> IcdocCcdDataVector;

      struct IcdocData
      {
        model::Icdoc icdoc;
        int bankCode;
        QString bankShortName;            
        QString bankFullName;            
        QString createUserName;
        QString updateUserName;      
        IcdocCcdDataVector ccdData;
      }; // struct IcdocData

      IcdocCcdDataVector readIcdocCcdData(QSqlQuery& query, const DatabaseModel& databaseModel)
      {
        IcdocCcdDataVector vector;
        while (query.next())
        {
          IcdocCcdData data;
          data.ccd.ccdId  = query.value(0).toLongLong();
          data.ccd.number = query.value(1).toString();
          data.ccd.contractId = query.value(2).toLongLong();
          data.ccd.deppointId = query.value(3).toLongLong();
          data.ccd.exportPermDate = databaseModel.convertFromServer(query.value(4).toDate());
          data.ccd.exportDate  = query.value(5).isNull() ? OptionalQDate() : databaseModel.convertFromServer(query.value(5).toDate());
          data.ccd.productCode = query.value(6).isNull() ? OptionalQString() : query.value(6).toString();
          data.ccd.series      = query.value(7).isNull() ? OptionalQString() : query.value(7).toString();
          data.ccd.ttermId = query.value(8).toLongLong();
          data.ccd.amount  = query.value(9).toLongLong();
          data.ccd.duty    = query.value(10).isNull() ? OptionalQInt64() : query.value(10).toLongLong();
          data.ccd.rate    = query.value(11).isNull() ? OptionalQInt64() : query.value(11).toLongLong();
          data.ccd.zeroTaxPeriod   = query.value(12).toInt();
          data.ccd.paymentTermType = query.value(13).toInt();
          data.ccd.paymentTermDate = query.value(14).isNull() ? OptionalQDate() : databaseModel.convertFromServer(query.value(14).toDate());
          data.ccd.paymentTermPeriod = query.value(15).isNull() ? OptionalInt() : query.value(15).toInt();
          data.paymentLimitDate = query.value(16).isNull() ? OptionalQDate() : databaseModel.convertFromServer(query.value(16).toDate());
          data.zeroTaxLimitDate = databaseModel.convertFromServer(query.value(17).toDate());
          data.ccd.createUserId = query.value(18).toLongLong();
          data.ccd.updateUserId = query.value(19).toLongLong();
          data.ccd.createTime   = databaseModel.convertFromServer(query.value(20).toDateTime());
          data.ccd.updateTime   = databaseModel.convertFromServer(query.value(21).toDateTime());
          data.contractNumber   = query.value(22).toString();
          data.passportNumber   = query.value(23).isNull() ? OptionalQString() : query.value(23).toString();
          data.currencyId       = query.value(24).toLongLong();
          data.currencyAlphabeticCode = query.value(25).toString();
          data.curencyNumericCode = query.value(26).toInt();
          data.residentFullName = query.value(27).toString();
          vector.append(data);
        }      
        return vector;
      }

      boost::optional<IcdocData> readIcdocFromDatabase(qint64 icdocId, const DatabaseModel& databaseModel)
      {
        static const char* querySql = "select " \
          "t.\"BANK_ID\", b.\"SHORT_NAME\", " \
          "t.\"ICDOC_DATE\", t.\"STATE\", t.\"RECV_DATE\", " \
          "t.\"REMARK\", " \
          "cu.\"LOGIN\", uu.\"LOGIN\", " \
          "t.\"CREATE_TIME\", t.\"UPDATE_TIME\", " \
          "t.\"CREATE_USER_ID\", t.\"UPDATE_USER_ID\", b.\"BANK_CODE\", b.\"FULL_NAME\" " \
          "from \"ICDOC\" t " \
          "join \"BANK\" b on t.\"BANK_ID\" = b.\"BANK_ID\" " \
          "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
          "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\" " \
          "where t.\"ICDOC_ID\" = ?";

        static const char* ccdQuerySql = "select " \
          "rel.\"CCD_ID\", " \
          "t.\"NUMBER\", t.\"CONTRACT_ID\", t.\"DEPPOINT_ID\", t.\"EXPORT_PERM_DATE\", " \
          "t.\"EXPORT_DATE\", t.\"PRODUCT_CODE\", t.\"SERIES\", t.\"TTERM_ID\", t.\"AMOUNT\", " \
          "t.\"DUTY\", t.\"RATE\", t.\"ZERO_TAX_PERIOD\", t.\"PAYMENT_TERM_TYPE\", " \
          "t.\"PAYMENT_TERM_DATE\", t.\"PAYMENT_TERM_PERIOD\", t.\"PAYMENT_LIMIT_DATE\", t.\"ZERO_TAX_LIMIT_DATE\", " \
          "t.\"CREATE_USER_ID\", t.\"UPDATE_USER_ID\", t.\"CREATE_TIME\", t.\"UPDATE_TIME\", " \
          "c.\"NUMBER\", c.\"PASSPORT_NUMBER\", c.\"CURRENCY_ID\", cur.\"ALPHABETIC_CODE\", " \
          "cur.\"NUMERIC_CODE\", r.\"FULL_NAME\" " \
          "from \"CCD_ICDOC_RELATION\" rel " \
          "join \"CCD_SEL\" t on t.\"CCD_ID\" = rel.\"CCD_ID\" " \
          "join \"CONTRACT\" c on t.\"CONTRACT_ID\" = c.\"CONTRACT_ID\" " \
          "join \"CURRENCY\" cur on c.\"CURRENCY_ID\" = cur.\"CURRENCY_ID\" " \
          "join \"USER\" cu on t.\"CREATE_USER_ID\" = cu.\"USER_ID\" " \
          "join \"USER\" uu on t.\"UPDATE_USER_ID\" = uu.\"USER_ID\" " \
          "join \"RESIDENT\" r on c.\"RESIDENT_ID\" = r.\"RESIDENT_ID\" " \
          "where rel.\"ICDOC_ID\" = ? order by rel.\"TAB_ORDER\"";

        QSqlDatabase database(databaseModel.database());        
        if (database.transaction())
        {        
          QSqlQuery query(database);
          QSqlQuery ccdQuery(database);
          if (query.prepare(querySql) && ccdQuery.prepare(ccdQuerySql))
          {
            query.bindValue(0, icdocId);
            ccdQuery.bindValue(0, icdocId);
            if (query.exec() && ccdQuery.exec())
            {
              if (query.next())
              { 
                IcdocData icdocData;                
                icdocData.icdoc.icdocId     = icdocId;
                icdocData.icdoc.bankId    = query.value(0).toLongLong();
                icdocData.bankShortName = query.value(1).toString();
                icdocData.icdoc.icdocDate   = databaseModel.convertFromServer(query.value(2).toDate());
                icdocData.icdoc.state     = query.value(3).toInt();
                icdocData.icdoc.recvDate  = query.isNull(4) ? OptionalQDate() : databaseModel.convertFromServer(query.value(4).toDate());
                icdocData.icdoc.remark    = query.isNull(5) ? OptionalQString() : query.value(5).toString();              
                icdocData.createUserName = query.value(6).toString(); 
                icdocData.updateUserName = query.value(7).toString(); 
                icdocData.icdoc.createTime = databaseModel.convertFromServer(query.value(8).toDateTime()); 
                icdocData.icdoc.updateTime = databaseModel.convertFromServer(query.value(9).toDateTime());
                icdocData.icdoc.createUserId  = query.value(10).toLongLong();
                icdocData.icdoc.updateUserId  = query.value(11).toLongLong();
                icdocData.bankCode = query.value(12).toInt();
                icdocData.bankFullName = query.value(13).toString();
                icdocData.ccdData = readIcdocCcdData(ccdQuery, databaseModel);
                if (database.commit())
                {
                  return icdocData;
                }
                else
                {
                  database.rollback();
                  return boost::optional<IcdocData>();
                } // if (database.commit())
              } // if (query.next())
            } // if (query.exec() && ccdQuery.exec())
          } // if (query.prepare(querySql) && ccdQuery.prepare(ccdQuerySql))
          database.rollback();
        } // if (database.transaction())
        return boost::optional<IcdocData>();
      }

      void replace(const Excel::RangePtr& range, const QString& what, const QString& value)
      {
        range->Replace(reinterpret_cast<const wchar_t*>(what.utf16()), 
          reinterpret_cast<const wchar_t*>(value.utf16()),
          Excel::xlPart, Excel::xlByRows, false, false, false, false);
      }

      void replace(const Excel::RangePtr& range, const QString& what, int value)
      {
        range->Replace(reinterpret_cast<const wchar_t*>(what.utf16()), 
          value, Excel::xlPart, Excel::xlByRows, false, false, false, false);
      }

      void exportIctData(const Excel::RangePtr& range, const IctPaysheetData& paysheetData)
      {
        replace(range, "{paysheet.date}", paysheetData.paysheet.payDate.toString(Qt::DefaultLocaleShortDate));
        replace(range, "{paysheet.ccd.number}", paysheetData.ccdNumber ? paysheetData.ccdNumber.get() : QString());
        replace(range, "{paysheet.currecy.code}", paysheetData.curencyNumericCode);
        replace(range, "{paysheet.amount}", model::CurrencyAmount(paysheetData.paysheet.amount).toString());
      }

      void insertRowAt(const Excel::_WorksheetPtr& worksheet, long row)
      {
        static const QString rowAddressFormat("%1:%1");
        QString rowAddress = rowAddressFormat.arg(row);
        Excel::RangePtr rowRange = worksheet->Range[reinterpret_cast<const wchar_t*>(rowAddress.utf16())];
        rowRange = rowRange->EntireRow;        
        rowRange->Insert(Excel::xlDown, Excel::xlFormatFromLeftOrAbove);
      }      

      void copyRange(const Excel::RangePtr& srcRange, const Excel::RangePtr dstRange)
      {
        srcRange->Copy(dstRange.GetInterfacePtr());
      } 

      void exportIctData(const ApplicationWrapper& application, const IctPaysheetDataVector& paysheetData)
      {
        Excel::RangePtr detailRange;
        try
        {
          detailRange = application.get()->Range[L"detail"];
        }
        catch (...)
        {
        	return;
        }        
        int rowCount = paysheetData.size();
        Excel::RangePtr range = detailRange;            
        for (IctPaysheetDataVector::const_iterator i = paysheetData.begin(), end = paysheetData.end(); i != end; ++i)
        {
          if (1 != rowCount)
          {              
            long nextRow = range->Row + 1;
            insertRowAt(range->Worksheet, nextRow);
            Excel::RangePtr nextRange = range->Offset[1][0];
            copyRange(range, nextRange);            
          }            
          exportIctData(range, *i);
          --rowCount;
          range = range->Offset[1][0];
        }
      }

      void exportIctData(const ApplicationWrapper& application, const IctData& ictData)
      {
        Excel::RangePtr allCells = application.get()->Cells;
        replace(allCells, "{ict.date}", ictData.ict.ictDate.toString(Qt::DefaultLocaleShortDate));
        replace(allCells, "{ict.state}", QString("%1").arg(ictData.ict.state));
        replace(allCells, "{ict.recvDate}", ictData.ict.recvDate ? ictData.ict.recvDate->toString(Qt::DefaultLocaleShortDate) : QString()); //todo
        replace(allCells, "{ict.remark}", ictData.ict.remark ? ictData.ict.remark.get() : QString());
        replace(allCells, "{ict.createTime}", ictData.ict.createTime.toString(Qt::DefaultLocaleShortDate));
        replace(allCells, "{ict.updateTime}", ictData.ict.updateTime.toString(Qt::DefaultLocaleShortDate));
        replace(allCells, "{ict.createUser.userName}", ictData.createUserName);
        replace(allCells, "{ict.updateUser.userName}", ictData.updateUserName);
        replace(allCells, "{ict.bank.bankCode}", QString("%1").arg(ictData.bankCode));
        replace(allCells, "{ict.bank.shortName}", ictData.bankShortName);
        replace(allCells, "{ict.bank.fullName}", ictData.bankFullName);   

        QString firstResidentFullName;
        QString firstPassportNumber;
        if (!ictData.paysheetData.isEmpty())
        {
          firstResidentFullName = ictData.paysheetData.begin()->residentFullName;
        }
        replace(allCells, "{ict.resident.fullName}", firstResidentFullName);        
        for (IctPaysheetDataVector::const_iterator i = ictData.paysheetData.begin(), end = ictData.paysheetData.end(); i != end; ++i)
        {
          if (i->passportNumber)
          {
            firstPassportNumber = i->passportNumber.get();
            break;
          }
        }
        replace(allCells, "{ict.contract.passportNumber}", firstPassportNumber);        

        exportIctData(application, ictData.paysheetData);
      }

      void exportIcdocData(const Excel::RangePtr& range, const IcdocCcdData& ccdData)
      {
        replace(range, "{ccd.date}", ccdData.ccd.createTime.date().toString(Qt::DefaultLocaleShortDate));
        replace(range, "{ccd.exportPermDate}", ccdData.ccd.exportPermDate.toString(Qt::DefaultLocaleShortDate));
        replace(range, "{ccd.number}", ccdData.ccd.number);
        replace(range, "{ccd.currecy.code}", ccdData.curencyNumericCode);
        replace(range, "{ccd.amount}", model::CurrencyAmount(ccdData.ccd.amount).toString());
      }

      void exportIcdocData(const ApplicationWrapper& application, const IcdocCcdDataVector& ccdData)
      {
        Excel::RangePtr detailRange;
        try
        {
          detailRange = application.get()->Range[L"detail"];
        }
        catch (...)
        {
        	return;
        }        
        int rowCount = ccdData.size();
        Excel::RangePtr range = detailRange;            
        for (IcdocCcdDataVector::const_iterator i = ccdData.begin(), end = ccdData.end(); i != end; ++i)
        {
          if (1 != rowCount)
          {              
            long nextRow = range->Row + 1;
            insertRowAt(range->Worksheet, nextRow);
            Excel::RangePtr nextRange = range->Offset[1][0];
            copyRange(range, nextRange);            
          }            
          exportIcdocData(range, *i);
          --rowCount;
          range = range->Offset[1][0];
        }
      }

      void exportIcdocData(const ApplicationWrapper& application, const IcdocData& icdocData)
      {
        Excel::RangePtr allCells = application.get()->Cells;
        replace(allCells, "{icdoc.date}", icdocData.icdoc.icdocDate.toString(Qt::DefaultLocaleShortDate));
        replace(allCells, "{icdoc.state}", QString("%1").arg(icdocData.icdoc.state));
        replace(allCells, "{icdoc.recvDate}", icdocData.icdoc.recvDate ? icdocData.icdoc.recvDate->toString(Qt::DefaultLocaleShortDate) : QString()); //todo
        replace(allCells, "{icdoc.remark}", icdocData.icdoc.remark ? icdocData.icdoc.remark.get() : QString());
        replace(allCells, "{icdoc.createTime}", icdocData.icdoc.createTime.toString(Qt::DefaultLocaleShortDate));
        replace(allCells, "{icdoc.updateTime}", icdocData.icdoc.updateTime.toString(Qt::DefaultLocaleShortDate));
        replace(allCells, "{icdoc.createUser.userName}", icdocData.createUserName);
        replace(allCells, "{icdoc.updateUser.userName}", icdocData.updateUserName);
        replace(allCells, "{icdoc.bank.bankCode}", QString("%1").arg(icdocData.bankCode));
        replace(allCells, "{icdoc.bank.shortName}", icdocData.bankShortName);
        replace(allCells, "{icdoc.bank.fullName}", icdocData.bankFullName);   

        QString firstResidentFullName;
        QString firstPassportNumber;
        if (!icdocData.ccdData.isEmpty())
        {
          firstResidentFullName = icdocData.ccdData.begin()->residentFullName;
        }
        replace(allCells, "{icdoc.resident.fullName}", firstResidentFullName);        
        for (IcdocCcdDataVector::const_iterator i = icdocData.ccdData.begin(), end = icdocData.ccdData.end(); i != end; ++i)
        {
          if (i->passportNumber)
          {
            firstPassportNumber = i->passportNumber.get();
            break;
          }
        }
        replace(allCells, "{icdoc.contract.passportNumber}", firstPassportNumber);        

        exportIcdocData(application, icdocData.ccdData);
      }

    } // namespace

    ExcelExporter::ThreadInitializer::ThreadInitializer()
    {
      ::CoInitialize(NULL);      
    }

    ExcelExporter::ThreadInitializer::~ThreadInitializer()
    {
      ::CoUninitialize();
    }
      
    bool ExcelExporter::exportTable(const QTableView& table)
    {      
      try 
      {
        ApplicationWrapper excelApplication(Excel::_ApplicationPtr(L"Excel.Application"));        
        WorkbookWrapper workbook(excelApplication.get()->Workbooks->Add());
        Excel::_WorksheetPtr worksheet = workbook.get()->Worksheets->Item[1L];
        const Excel::RangePtr topLeftCell = worksheet->Range[L"A1"];
               
        const QHeaderView* header = table.horizontalHeader();
        QAbstractItemModel* model = table.model();
        const int columnCount = header->count();

        // Export column headers
        Excel::RangePtr range = topLeftCell;
        for (int column = 0; column != columnCount; ++column)
        {
          int logicalIndex = header->logicalIndex(column);
          if (!header->isSectionHidden(logicalIndex))
          {
            QString columnHeader = model->headerData(logicalIndex, Qt::Horizontal).toString();
            setValue(range, columnHeader);
            range = range->Offset[0][1];
          }          
        }

        // Export data
        range = topLeftCell->Offset[1][0];
        int row = 0;
        int rowCount = model->rowCount();
        bool done = false;
        while (!done)
        {          
          if (row >= rowCount)
          {
            static QModelIndex rootIndex;
            if (!model->canFetchMore(rootIndex))
            {
              done = true;
              continue;
            }
            else
            {
              model->fetchMore(rootIndex);
              rowCount = model->rowCount();
              if (row >= rowCount)
              {
                done = true;
                continue;
              }
            }
          }          
          Excel::RangePtr columnRange = range;
          for (int column = 0; column != columnCount; ++column)
          {
            int logicalIndex = header->logicalIndex(column);
            if (!header->isSectionHidden(logicalIndex))
            {
              QModelIndex index = model->index(row, logicalIndex);
              exportItem(columnRange, *model, index);            
              columnRange = columnRange->Offset[0][1];
            }
          }
          ++row;
          range = range->Offset[1][0];          
        }

        // AutoFit created columns
        const int visibleColumnCount = columnCount - header->hiddenSectionCount();
        range = worksheet->Range[L"A:A"];
        for (int column = 0; column != visibleColumnCount; ++column)
        {      
          range->EntireColumn->AutoFit();
          range = range->Offset[0][1];
        }        
        excelApplication.get()->Visible = true;
        workbook.release();
        excelApplication.release();
        return true;
      }
      catch (...)
      {        	
        return false;
      }      
    } 

    bool ExcelExporter::exportIcdoc(qint64 icdocId, const QString& templateFilename, const DatabaseModel& databaseModel)
    { 
      if (boost::optional<IcdocData> icdocData = readIcdocFromDatabase(icdocId, databaseModel))
      {      
        try 
        {
          ApplicationWrapper excelApplication(Excel::_ApplicationPtr(L"Excel.Application"));        
          WorkbookWrapper workbook(excelApplication.get()->Workbooks->Add(reinterpret_cast<const wchar_t*>(templateFilename.utf16())));
          bool displayAlerts = excelApplication.get()->DisplayAlerts;
          excelApplication.get()->DisplayAlerts = false;
          exportIcdocData(excelApplication, icdocData.get());
          excelApplication.get()->DisplayAlerts = displayAlerts;
          excelApplication.get()->Visible = true;
          workbook.release();
          excelApplication.release();
          return true;
        }
        catch (...)
        {        	
          return false;
        }
      }
      return false;
    }

    bool ExcelExporter::exportIct(qint64 ictId, const QString& templateFilename, const DatabaseModel& databaseModel)
    { 
      if (boost::optional<IctData> ictData = readIctFromDatabase(ictId, databaseModel))
      {      
        try 
        {
          ApplicationWrapper excelApplication(Excel::_ApplicationPtr(L"Excel.Application"));        
          WorkbookWrapper workbook(excelApplication.get()->Workbooks->Add(reinterpret_cast<const wchar_t*>(templateFilename.utf16())));
          bool displayAlerts = excelApplication.get()->DisplayAlerts;
          excelApplication.get()->DisplayAlerts = false;
          exportIctData(excelApplication, ictData.get());
          excelApplication.get()->DisplayAlerts = displayAlerts;
          excelApplication.get()->Visible = true;
          workbook.release();
          excelApplication.release();
          return true;
        }
        catch (...)
        {        	
          return false;
        }
      }
      return false;
    }

    bool ExcelExporter::exportDataList(const ExportDataList& exportDataList)
    {
      try 
      {
        ApplicationWrapper excelApplication(Excel::_ApplicationPtr(L"Excel.Application"));        
        WorkbookWrapper workbook(excelApplication.get()->Workbooks->Add());
        //todo
        Excel::_WorksheetPtr worksheet = workbook.get()->Worksheets->Item[1L];
        const Excel::RangePtr topLeftCell = worksheet->Range[L"A1"];

        // Export data
        Excel::RangePtr captionRange = topLeftCell;
        Excel::RangePtr valueRange = captionRange->Offset[0][1];
        for (ExportDataList::const_iterator i = exportDataList.begin(), end = exportDataList.end(); i != end; ++i)
        {
          setValue(captionRange, i->first);
          setValue(valueRange, i->second);
          captionRange = captionRange->Offset[1][0];
          valueRange = valueRange->Offset[1][0];
        }

        // AutoFit created columns
        Excel::RangePtr range = worksheet->Range[L"A:A"];        
        range->EntireColumn->AutoFit();
        range = range->Offset[0][1];
        range->EntireColumn->AutoFit();        

        excelApplication.get()->Visible = true;
        workbook.release();
        excelApplication.release();
        return true;
      }
      catch (...)
      {        	
        return false;
      }
    }

  } // namespace chrono
} //namespace ma
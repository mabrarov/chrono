//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <stdexcept>
#include <boost/thread.hpp>
#include <boost/exception/all.hpp>
#include <QVariant>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <ma/chrono/generator.h>

namespace ma
{
  namespace chrono
  {    
    Generator::QueryOption::QueryOption(const QString& table, const QString& generatorIdColumn, const QString& lastValueColumn)
      : insertGenerator_(QString::fromWCharArray(L"insert into %1 (%2, %3) values (?, ?)").arg(table).arg(generatorIdColumn).arg(lastValueColumn))
      , loadLastValue_(QString::fromWCharArray(L"select %3 from %1 where %2 = ?").arg(table).arg(generatorIdColumn).arg(lastValueColumn))
      , updateLastValue_(QString::fromWCharArray(L"update %1 set %3 = ? where %2 = ? and %3 = ?").arg(table).arg(generatorIdColumn).arg(lastValueColumn))
    {
    }

    Generator::QueryOption::~QueryOption()
    {
    }

    QString Generator::QueryOption::insertGenerator() const
    {
      return insertGenerator_;
    }

    QString Generator::QueryOption::loadLastValue() const
    {
      return loadLastValue_;
    }

    QString Generator::QueryOption::updateLastValue() const
    {
      return updateLastValue_;
    } 

    Generator::ConcurrentOption::ConcurrentOption(int blockSize, int maxAttempts, 
      const boost::posix_time::time_duration& pauseDuration)
      : blockSize_(blockSize)
      , maxAttempts_(maxAttempts)
      , pauseDuration_(pauseDuration)
    {
    }

    Generator::ConcurrentOption::~ConcurrentOption()
    {
    }

    int Generator::ConcurrentOption::blockSize() const
    {
      return blockSize_;
    }

    int Generator::ConcurrentOption::maxAttempts() const
    {
      return maxAttempts_;
    }

    boost::posix_time::time_duration Generator::ConcurrentOption::pauseDuration() const
    {
      return pauseDuration_;
    }
              
    Generator::Generator(const QueryOption& queryOption, const ConcurrentOption& concurrentOption, qint64 generatorId)
      : queryOption_(queryOption)
      , concurrentOption_(concurrentOption)
      , generatorId_(generatorId)
      , lastValue_(0)
      , acquiredValue_(0)
    {
    }

    Generator::~Generator()
    {
    }

    qint64 Generator::generateNext(const QSqlDatabase& connection)
    {
      if (lastValue_ >= acquiredValue_) 
      {
        getNextBlock(connection);
      }        
      return ++lastValue_;
    }

    void Generator::getNextBlock(const QSqlDatabase& connection)
    {
      static const qint64 endValue = (std::numeric_limits<qint64>::max)();

      bool done = false;
      int attemp = 0;
      while (!done)
      {
        OptionalQInt64 lastValue = loadLastValue(connection);
        if (!lastValue)
        {
          lastValue = createNewGenerator(connection);
        }
        if (lastValue)
        {
          qint64 value = lastValue.get();
          if (value < startValue)
          {
            value = startValue;
          }
          if (value >= endValue)
          {
            boost::throw_exception(std::overflow_error("Generator: maximum value reached"));
          }
          else
          {
            qint64 newValue;
            if (value > endValue - concurrentOption_.blockSize())
            {
              newValue = endValue;
            }
            else
            {
              newValue = value + concurrentOption_.blockSize();
            }
            done = updateLastValue(connection, value, newValue);
            if (done)
            {
              lastValue_ = value;
              acquiredValue_ = newValue;
              return;
            }
          }
        }          
        ++attemp;
        if (attemp == concurrentOption_.maxAttempts())
        {
          boost::throw_exception(std::overflow_error("Generator: maximum attemp count reached"));
        }
        else
        { 
          boost::this_thread::sleep(concurrentOption_.pauseDuration());
        }
      }
    }

    OptionalQInt64 Generator::loadLastValue(const QSqlDatabase& connection)
    {
      OptionalQInt64 lastValue;
      QSqlQuery statement(connection);           
      if (statement.prepare(queryOption_.loadLastValue()))
      {
        statement.bindValue(0, QVariant(generatorId_));
        if (statement.exec())
        {
          if (statement.next())
          {
            lastValue.reset(statement.value(0).toLongLong());
          }
        }
      }
      return lastValue;
    }

    OptionalQInt64 Generator::createNewGenerator(const QSqlDatabase& connection)
    {
      OptionalQInt64 lastValue;
      QSqlQuery statement(connection);           
      if (statement.prepare(queryOption_.insertGenerator()))
      {
        statement.bindValue(0, generatorId_);
        statement.bindValue(1, startValue);
        if (statement.exec())
        {
          if (statement.numRowsAffected() > 0)
          {
            lastValue.reset(startValue);
          }          
        }
      }
      return lastValue;
    }

    bool Generator::updateLastValue(const QSqlDatabase& connection, qint64 currentValue, qint64 newValue)
    {
      bool done = false;
      QSqlQuery statement(connection);           
      if (statement.prepare(queryOption_.updateLastValue()))
      {
        statement.bindValue(0, newValue);
        statement.bindValue(1, generatorId_);
        statement.bindValue(2, currentValue);
        if (statement.exec())
        {
          done = 0 != statement.numRowsAffected();
        }
      }      
      return done;
    }

  } // namespace chrono
} // namespace ma

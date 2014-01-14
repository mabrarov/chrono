//
// Copyright (c) 2010-2014 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CHRONO_GENERATOR_H
#define MA_CHRONO_GENERATOR_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/utility.hpp>
#include <boost/optional.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <qglobal.h>
#include <ma/chrono/types.h>
#include <ma/chrono/generator_fwd.h>

QT_BEGIN_NAMESPACE  
class QSqlDatabase;
QT_END_NAMESPACE

namespace ma {
namespace chrono {    

class Generator : private boost::noncopyable
{
public:
  class QueryOption
  {
  public:
    QueryOption(const QString& table, const QString& generatorIdColumn, 
        const QString& lastValueColumn);
    ~QueryOption();
    QString insertGenerator() const;
    QString loadLastValue() const;
    QString updateLastValue() const;

  private:
    QString insertGenerator_;
    QString loadLastValue_;
    QString updateLastValue_;
  }; // class QueryOption

  class ConcurrentOption
  {
  public:
    ConcurrentOption(int blockSize, int maxAttempts, 
        const boost::posix_time::time_duration& pauseDuration);
    ~ConcurrentOption();
    int blockSize() const;
    int maxAttempts() const;         
    boost::posix_time::time_duration pauseDuration() const;
        
  private:
    int blockSize_;
    int maxAttempts_; 
    boost::posix_time::time_duration pauseDuration_;
  }; // class ConcurrentOption

  Generator(const QueryOption& queryOption, 
      const ConcurrentOption& concurrentOption, qint64 generatorId);
  ~Generator();
  qint64 generateNext(const QSqlDatabase& connection);

private:
  static const qint64 startValue = static_cast<qint64>(1);        

  OptionalQInt64 loadLastValue(const QSqlDatabase& connection);
  OptionalQInt64 createNewGenerator(const QSqlDatabase& connection);
  bool updateLastValue(const QSqlDatabase& connection, qint64 currentValue, 
      qint64 newValue);
  void getNextBlock(const QSqlDatabase& connection);

  QueryOption queryOption_;
  ConcurrentOption concurrentOption_;
  qint64 generatorId_;
  qint64 lastValue_;
  qint64 acquiredValue_;
}; // class Generator

} // namespace chrono

} // namespace ma

#endif // MA_CHRONO_GENERATOR_H

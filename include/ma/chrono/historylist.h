//
// Copyright (c) 2009-2013 Marat Abrarov (abrarov@gmail.com)
//

#ifndef MA_CHRONO_HISTORYLIST_H
#define MA_CHRONO_HISTORYLIST_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/utility.hpp>
#include <boost/call_traits.hpp>
#include <boost/optional.hpp>
#include <boost/circular_buffer.hpp>

namespace ma {
namespace chrono {  

template <typename T>
class HistoryList : private boost::noncopyable
{      
public:
  typedef typename boost::call_traits<T>::value_type value_type;
  typedef typename boost::call_traits<T>::param_type param_type;
  typedef boost::circular_buffer<value_type> list_type;

  explicit HistoryList(std::size_t capacity)
    : history_(capacity)
    , offset_(0)  
    , offset_iterator_()
  {
  }

  ~HistoryList()
  {
  }

  std::size_t offset() const
  {
    return offset_;
  }

  bool hasPrevious() const
  {
    if (history_.empty())
    {
      return false;
    }
    return offset_ < history_.size() - 1;
  }

  bool hasNext() const
  {
    return offset_ > 0;
  }

  void add(param_type value)
  {  
    if (!history_.empty())
    {
      if (*offset_iterator_ == value)
      {
        return;
      }
    }
    if (offset_ > 0)
    {              
      if (*boost::prior(offset_iterator_) == value)
      {
        --offset_;
        --offset_iterator_;
        return;
      }          
      else
      {
        history_.erase(history_.begin(), offset_iterator_);                    
        history_.push_front(value);   
        offset_ = 0;
        offset_iterator_ = history_.begin();
        return;
      }          
    }        
    history_.push_front(value);   
    offset_ = 0;
    offset_iterator_ = history_.begin();                
  }

  boost::optional<value_type> previous() const
  {
    if (history_.empty())
    {
      return boost::optional<value_type>();
    }
    if (offset_ >= history_.size() - 1)
    {
      return boost::optional<value_type>();
    }
    return *boost::next(offset_iterator_);
  }

  boost::optional<value_type> next() const
  {
    if (history_.empty())
    {
      return boost::optional<value_type>();
    }
    if (offset_ > 0)
    {
      return *boost::prior(offset_iterator_);          
    }
    return boost::optional<value_type>();
  }

  list_type list() const
  {
    return history_;
  }

  boost::optional<value_type> goPrevious()
  {
    if (history_.empty())
    {
      return boost::optional<value_type>();
    }
    if (offset_ >= history_.size() - 1)
    {
      return boost::optional<value_type>();
    }
    ++offset_;
    ++offset_iterator_;        
    return *offset_iterator_;
  }

  boost::optional<value_type> goNext()
  {
    if (history_.empty())
    {
      return boost::optional<value_type>();
    }
    if (offset_ > 0)
    {
      --offset_;
      --offset_iterator_;
      return *offset_iterator_;          
    }
    return boost::optional<value_type>();
  }

private:      
  list_type history_;
  std::size_t offset_;
  typename list_type::iterator offset_iterator_;
}; // class HistoryList<T>

} // namespace chrono

} //namespace ma

#endif // MA_CHRONO_HISTORYLIST_H

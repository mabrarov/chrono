/*
TRANSLATOR ma::chrono::BreadCrumbBar
*/

//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/utility.hpp>
#include <ma/chrono/breadcrumbbar.h>
#include <ma/chrono/modelutility.h>

namespace ma
{
  namespace chrono
  {    
    class BreadCrumbBar::Item : private boost::noncopyable
    {
    public:
      explicit Item()
      {
      }

      virtual ~Item()
      {
      }

    private:      
    }; // class BreadCrumbBar::Item

    class BreadCrumbBar::ElideIndicatorItem : public BreadCrumbBar::Item
    {
    public:
      explicit ElideIndicatorItem()
        : Item()
      {
      }

      ~ElideIndicatorItem()
      {
      }

      virtual QRect rect() const = 0;

    private:      
    }; // class BreadCrumbBar::ElideIndicatorItem

    class BreadCrumbBar::LabelItem : public BreadCrumbBar::Item
    {
    public:
      explicit LabelItem(const QModelIndex& index)
        : Item()
        , index_(index)
      {        
      }

      ~LabelItem()
      {
      }

      QModelIndex index()
      {        
        return index_;
      }      

    private:      
      QModelIndex index_;
    }; // class BreadCrumbBar::LabelItem

    BreadCrumbBar::BreadCrumbBar(QWidget* parent)
      : QLineEdit(parent)      
      , model_(0)
      , rootIndex_()
      , currentIndex_()
      , items_()
    {     
      setReadOnly(true);
    }

    BreadCrumbBar::~BreadCrumbBar()
    {
    }    

    void BreadCrumbBar::setModel(QAbstractItemModel* model)
    {
      //todo
      model_ = model;
    }

    QAbstractItemModel* BreadCrumbBar::model() const
    {      
      return model_;
    }

    void BreadCrumbBar::setRootIndex(const QModelIndex& index)
    {
      //todo
      rootIndex_ = index;
    }

    QModelIndex BreadCrumbBar::rootIndex() const
    {
      return rootIndex_;
    }

    QModelIndex BreadCrumbBar::currentIndex() const
    {
      return currentIndex_;
    }

    void BreadCrumbBar::setCurrentIndex(const QModelIndex& index)
    {      
      if (index != currentIndex_)
      {        
        currentIndex_ = index;
        //todo
        setText(ModelUtility::indexToPath(index)); //temporary
      }      
    }

    void BreadCrumbBar::mousePressEvent(QMouseEvent* event)
    {
      //todo
      QLineEdit::mousePressEvent(event);
    }

    void BreadCrumbBar::mouseMoveEvent(QMouseEvent* event)
    {
      //todo
      QLineEdit::mouseMoveEvent(event);
    }

    void BreadCrumbBar::mouseReleaseEvent(QMouseEvent* event)
    {
      //todo
      QLineEdit::mouseReleaseEvent(event);
    }

    void BreadCrumbBar::paintEvent(QPaintEvent* event)
    {
      //todo
      QLineEdit::paintEvent(event);
    }

  } // namespace chrono
} // namespace ma
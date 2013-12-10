/*
TRANSLATOR ma::chrono::SummaryListPage
*/

//
// Copyright (c) 2009-2010 Social Technologies (abrarov@mail.ru)
//

#include <QStandardItemModel>
#include <QTableView>
#include <QHeaderView>
#include <ma/chrono/constants.h>
#include <ma/chrono/resourcemanager.h>
#include <ma/chrono/summarylistpage.h>

namespace ma
{
  namespace chrono
  {    
    SummaryListPage::SummaryListPage(const boost::shared_ptr<ResourceManager>& resourceManager,
      const boost::weak_ptr<TaskWindowManager>& taskWindowManager,
      const boost::shared_ptr<DatabaseModel>& databaseModel,
      const boost::shared_ptr<QAction>& helpAction,
      QWidget* parent)
      : ListPage(resourceManager, taskWindowManager, databaseModel, helpAction, contractEntity, parent)
    {    
      setWindowIcon(resourceManager->getIcon(chronoIconName));
      setWindowTitle(tr("Summary"));

      //todo add model      
      QStandardItemModel* model = new QStandardItemModel(0, 4, this);      
      model->setHeaderData(0, Qt::Horizontal, tr("Date"));
      model->setHeaderData(1, Qt::Horizontal, tr("Event content"));      
      model->setHeaderData(2, Qt::Horizontal, tr("Related data"));
      model->setHeaderData(3, Qt::Horizontal, tr("Related user"));      
    
      QTableView* pageGrid = this->grid();
      pageGrid->setModel(model);
      pageGrid->resizeColumnsToContents();
    }

    SummaryListPage::~SummaryListPage()
    {
    }
        
    std::auto_ptr<ListPage> SummaryListPage::clone(QWidget* parent)
    {
      std::auto_ptr<ListPage> clonedPage(new SummaryListPage(resourceManager(), taskWindowManager(), databaseModel(), helpAction(), parent));      
      clonedPage->setWindowTitle(windowTitle());            
      return clonedPage;
    }

  } // namespace chrono
} //namespace ma
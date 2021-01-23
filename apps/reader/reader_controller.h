#ifndef APPS_reader_controller_H
#define APPS_reader_controller_H

#include <escher.h>
#include <apps/external/archive.h>
#include "reader_view.h"

namespace Reader {

class ReaderController : public ViewController, public SimpleListViewDataSource, public ScrollViewDataSource{
public:
  ReaderController(Responder * parentResponder);
  View * view() override;

  int numberOfRows() const override;
  KDCoordinate cellHeight() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() const override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;

private:
  TableView m_tableView;

  static const int NB_FILES = 20;
  External::Archive::File m_files[NB_FILES];
  int m_nbFiles = 0;

  static const int NB_CELLS = 6;
  MessageTableCell m_cells[NB_CELLS];
};

}
#endif

/** 
 * This is the controller. It allows you to add some logic when there are events like keys pressed... 
 * It follow an MVC pattern -> Model-View-Controller. The controller allows you to show some changes on the view like colors, results...
 * This file is linked to the C controller (sample_controler.cpp here). All files are linked -> app files.
**/
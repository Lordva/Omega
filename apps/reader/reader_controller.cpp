#include "reader_controller.h"
#include "utility.h"

namespace Reader {

View* ReaderController::view() {
  return &m_tableView;
}

ReaderController::ReaderController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_tableView(this, this)
{
  m_nbFiles = filesWithExtention(".txt", m_files, NB_FILES);
}

int ReaderController::numberOfRows() const{
  return m_nbFiles;
}

KDCoordinate ReaderController::cellHeight(){
  return 50;
}

HighlightCell * ReaderController::reusableCell(int index){
  return &m_cells[index];
}

int ReaderController::reusableCellCount() const{
  return NB_CELLS;
}

void ReaderController::willDisplayCellForIndex(HighlightCell * cell, int index){
  MessageTableCell* myTextCell = static_cast<MessageTableCell*>(cell);
  MessageTextView* textView = static_cast<MessageTextView*>(myTextCell->labelView());
  textView->setText(m_files[index].name);
  myTextCell->setMessageFont(KDFont::LargeFont);
}

}
/** 
 * This is the controller. It allows you to add some logic when there are events like keys pressed... 
 * It follow an MVC pattern -> Model-View-Controller. The controller allows you to show some changes on the view like colors, results...
 * This file is linked to the C controller (sample_controler.h here). All files are linked -> app files.
**/
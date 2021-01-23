#ifndef APPS_READER_VIEW_H
#define APPS_READER_VIEW_H

#include <escher.h>
#include <ion.h>

namespace Reader {

class ReaderView : public View {
public:
  ReaderView();
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void reload();
  void changeColor();
  char bat_voltage = Ion::Battery::voltage();
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
private:
  void layoutSubviews(bool force = false) override;
  BufferTextView m_bufferTextView;
  int m_color;
  KDColor m_kdcolor;
};

}

#endif

/** 
 * This is the view, it allows you to show an interface on your calc like rect, string... 
 * It follows an MVC pattern -> Model-View-Controller. The view allows you to show some changes on event
 * This file is linked to the C view (sample_view.cpp here). All files are linked -> app files.
**/
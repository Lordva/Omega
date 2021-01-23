#include "app.h"
#include "apps/apps_container.h"
#include "reader_icon.h"
#include "apps/i18n.h"
#include <assert.h>

namespace Reader {

I18n::Message App::Descriptor::name() {
  return I18n::Message::ReaderApp;
}

I18n::Message App::Descriptor::upperName() {
  return I18n::Message::ReaderAppCapital;
}

const Image * App::Descriptor::icon() {
  return ImageStore::ReaderIcon;
}

App::Snapshot::Snapshot()
{
}

App * App::Snapshot::unpack(Container * container) {
  return new (container->currentAppBuffer()) App(this);
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
}

void App::Snapshot::reset() {
}

App::App(Snapshot * snapshot) :
  ::App(snapshot, &m_readerController),
  m_readerController(this)
{
}

}

/** 
 * This is the app file, it allows you to to add a name, some description, an icon for your app, but also your logic (controller) and your view 
 * It follows an MVC pattern -> Model-View-Controller. The app file allows you to link all the MVC files
 * This file is linked to the C view (app.h here). All files are linked -> app files.
**/

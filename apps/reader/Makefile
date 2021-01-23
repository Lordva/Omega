apps += Reader::App
app_headers += apps/reader/app.h

app_reader_src = $(addprefix apps/reader/,\
  app.cpp \
  reader_controller.cpp \
  reader_view.cpp \
  utility.cpp \
)

apps_src += $(app_reader_src)

app_images += apps/reader/reader_icon.png

i18n_files += $(call i18n_without_universal_for,reader/base)

$(eval $(call depends_on_image,apps/reader/app.cpp,apps/reader/reader_icon.png))

### The Makefile, is the file who allows you to compile successfully your app.
### If you add files or rename some files, don't forget to change the Makefile!

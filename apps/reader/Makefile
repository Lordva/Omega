apps += reader::app
app_header += apps/reader/app.h

app_sreader_src = $(addprefix apps/reader/,\
	app.cpp \	
)

apps_src += $(app_sreader_src)

app_image += apps/reader/reader_icon.png

i18n_files += $(call i18n_without_universal_for,reader/base)

$(eval $(call depends_on_image,apps/reader/app.cpp,apps/reader/reader_icon.png))
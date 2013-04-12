# Automake file for Feed a Puffle

gamedatadir = $(pkgdatadir)/data

bin_PROGRAMS = feed-a-puffle
feed_a_puffle_SOURCES = feed.c


feed_a_puffle_CPPFLAGS = -DGAMEDATA_DIR=\"$(gamedatadir)/\" -DLOCALEDIR=\"$(localedir)\" $(AM_CPPFLAGS)
feed_a_puffle_CFLAGS = $(SDL_CFLAGS) $(SDL_image_CFLAGS) $(AM_CFLAGS)
feed_a_puffle_LDADD = $(SDL_LIBS) $(SDL_image_LIBS)


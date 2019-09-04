EXTRA_DIST = tf2hex.cc
CLEANFILES = tf2hex

all: tf2hex

tf2hex: tf2hex.cc
	$(CXX_FOR_BUILD) $(CXXFLAGS_FOR_BUILD) $(CPPFLAGS) $(LDFLAGS) $(srcdir)/tf2hex.cc -o tf2hex

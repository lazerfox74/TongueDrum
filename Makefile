# Project Name
TARGET = minDrum

# Sources
CPP_SOURCES = main.cpp
#CPP_SOURCES += note.cpp
CPP_SOURCES += Segment.cpp
CPP_SOURCES += env.cpp
CPP_SOURCES += Envelope.cpp
CPP_SOURCES += drumPad.cpp
CPP_SOURCES += table.cpp


# Library Locations
LIBDAISY_DIR = ../../libDaisy/
DAISYSP_DIR = ../../DaisySP/

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile

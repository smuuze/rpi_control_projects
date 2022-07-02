#-----------------------------------------------------------------------------
#       Makefile for SHC Perl projects
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------

# same name as script file (e.g. build_script.pl)
PROJECT = build_script

#-----------------------------------------------------------------------------

VERSION_MAJOR	:= 1
VERSION_MINOR	:= 1

#-----------------------------------------------------------------------------

BASE_PATH   = ../..
FRMWRK_PATH = $(BASE_PATH)/rpi_control_frmwrk
MAKE_PATH   = $(FRMWRK_PATH)/make

#-----------------------------------------------------------------------------

include $(MAKE_PATH)/make_toolchain.mk
include $(MAKE_PATH)/make_git.mk

#-----------------------------------------------------------------------------

VERSION				:= $(VERSION_MAJOR).$(VERSION_MINOR)
RELEASE_DIRECTORY	:= release/$(VERSION)
MSG_PROG_LOCATION	:= Your programm can be found at

#-----------------------------------------------------------------------------

all: release

clean:

release_dir:
	$(VERBOSE) $(ECHO) "- Creating Release directory: $(RELEASE_DIRECTORY)"
	$(VERBOSE) $(MK) $(RELEASE_DIRECTORY)

release: release_dir 
	$(VERBOSE) $(CP) $(PROJECT).pl $(RELEASE_DIRECTORY)/$(PROJECT).pl
	$(VERBOSE) $(ECHO) "$(MSG_PROG_LOCATION) $(RELEASE_DIRECTORY)/$(PROJECT).pl"
	$(VERBOSE) $(ECHO) $(MSG_FINISH)

show_version:
	$(VERBOSE) $(ECHO) "$(VERSION_MAJOR).$(VERSION_MINOR)"
	
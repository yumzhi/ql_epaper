
#-------------------------------------------------------------------------------
# Configure variable
#-------------------------------------------------------------------------------
TOP_DIR:=$(TOP_DIR)
ROOT_DIR:=$(TOP_DIR)/../..
MAKEFILE_PATH :=$(subst $(strip \),/,$(abspath $(lastword $(MAKEFILE_LIST))))


#-------------------------------------------------------------------------------
# Configure source code files
#-------------------------------------------------------------------------------
SRC_FILES+= \
  epaper_boot.c \
  epaper_main.c \
  gh_epaper.c \

#-------------------------------------------------------------------------------
# the include directories
#-------------------------------------------------------------------------------
INC_DIRS+= \
  -I${TOP_DIR}/ \
  -I${TOP_DIR}/interface/ql_epaper/inc \
  -I${TOP_DIR}/common/include \
  -I${TOP_DIR}/interface/os/inc \
  -I${TOP_DIR}/interface/driver/inc \
  -I${TOP_DIR}/interface/network/data_call/inc \
  -I${TOP_DIR}/interface/network/sockets/inc \
  -I${TOP_DIR}/interface/sim/inc \
  -I${TOP_DIR}/interface/network/sockets/inc \
  -I${TOP_DIR}/interface/dev/inc \
  -I${TOP_DIR}/interface/network/nw/inc \
#-------------------------------------------------------------------------------
# Configure compile flag for C
#-------------------------------------------------------------------------------
U_FLAGS:=

#-------------------------------------------------------------------------------
# Configure compile flag for C++
#-------------------------------------------------------------------------------
U_CPPFLAGS:=

#-------------------------------------------------------------------------------
# Configure default macro define
#-------------------------------------------------------------------------------
DFLAGS+= \
  -D__TM_ZONE=tm_zone \
  -D__TM_GMTOFF=tm_gmtoff \


#-------------------------------------------------------------------------------
# Configure link library
#-------------------------------------------------------------------------------
U_LIBS:=

#-------------------------------------------------------------------------------
# include mk file
#-------------------------------------------------------------------------------
include ${TOP_DIR}/config/common/makefile.mk

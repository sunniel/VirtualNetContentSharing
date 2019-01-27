#
# OMNeT++/OMNEST Makefile for VirtualNetContentSharing
#
# This file was generated with the command:
#  opp_makemake -f --deep -IC:/Workspaces/JSON_for_Modern_C++ -IC:/Workspaces/OMNeT++/omnetpp-5.4/include/omnetpp -IC:/Workspaces/boost_1_66_0
#

# Name of target to be created (-o option)
TARGET = VirtualNetContentSharing$(D)$(EXE_SUFFIX)
TARGET_DIR = .

# User interface (uncomment one) (-u option)
USERIF_LIBS = $(ALL_ENV_LIBS) # that is, $(TKENV_LIBS) $(QTENV_LIBS) $(CMDENV_LIBS)
#USERIF_LIBS = $(CMDENV_LIBS)
#USERIF_LIBS = $(TKENV_LIBS)
#USERIF_LIBS = $(QTENV_LIBS)

# C++ include paths (with -I)
INCLUDE_PATH = -IC:/Workspaces/JSON_for_Modern_C++ -IC:/Workspaces/OMNeT++/omnetpp-5.4/include/omnetpp -IC:/Workspaces/boost_1_66_0

# Additional object and library files to link with
EXTRA_OBJS =

# Additional libraries (-L, -l options)
LIBS =

# Output directory
PROJECT_OUTPUT_DIR = ../out
PROJECTRELATIVE_PATH = src
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Object files for local .cc, .msg and .sm files
OBJS = \
    $O/app/CANApp.o \
    $O/app/ChordApp.o \
    $O/common/Constants.o \
    $O/common/HostBase.o \
    $O/common/sdbm.o \
    $O/common/Util.o \
    $O/crypto/sha1.o \
    $O/ctrl/BasicClient.o \
    $O/ctrl/BasicClientDelayTest.o \
    $O/ctrl/BasicClientSortedDelayTest.o \
    $O/ctrl/CANCtrl.o \
    $O/ctrl/ChordCtrl.o \
    $O/ctrl/Client.o \
    $O/ctrl/ClientDelayTest.o \
    $O/ctrl/ClientLoadTest.o \
    $O/ctrl/ClientLoadTestNoCache.o \
    $O/ctrl/TrailCreator.o \
    $O/global/ContentDistributor.o \
    $O/global/Coordinator.o \
    $O/global/GlobalNodeList.o \
    $O/global/GlobalParameters.o \
    $O/global/GlobalStatistics.o \
    $O/global/ManualValueQuery.o \
    $O/global/UnderlayConfigurator.o \
    $O/global/UniformChurnGenerator.o \
    $O/objects/CANInfo.o \
    $O/objects/CANProfile.o \
    $O/objects/ChordInfo.o \
    $O/objects/HostInfo.o \
    $O/objects/IPAddress.o \
    $O/objects/IPv4InterfaceData.o \
    $O/objects/IPvXAddress.o \
    $O/objects/PeerInfo.o \
    $O/objects/PeerStorage.o \
    $O/objects/SimpleInfo.o \
    $O/objects/SimpleNodeEntry.o \
    $O/objects/TransportAddress.o \
    $O/others/InterfaceEntry.o \
    $O/others/InterfaceTable.o \
    $O/transport/ReliableUDP.o \
    $O/transport/SimpleUDP.o \
    $O/messages/CANMessage_m.o \
    $O/messages/ChordMessage_m.o \
    $O/messages/UDPControlInfo_m.o

# Message files
MSGFILES = \
    messages/CANMessage.msg \
    messages/ChordMessage.msg \
    messages/UDPControlInfo.msg

# SM files
SMFILES =

#------------------------------------------------------------------------------

# Pull in OMNeT++ configuration (Makefile.inc)

ifneq ("$(OMNETPP_CONFIGFILE)","")
CONFIGFILE = $(OMNETPP_CONFIGFILE)
else
ifneq ("$(OMNETPP_ROOT)","")
CONFIGFILE = $(OMNETPP_ROOT)/Makefile.inc
else
CONFIGFILE = $(shell opp_configfilepath)
endif
endif

ifeq ("$(wildcard $(CONFIGFILE))","")
$(error Config file '$(CONFIGFILE)' does not exist -- add the OMNeT++ bin directory to the path so that opp_configfilepath can be found, or set the OMNETPP_CONFIGFILE variable to point to Makefile.inc)
endif

include $(CONFIGFILE)

# Simulation kernel and user interface libraries
OMNETPP_LIBS = $(OPPMAIN_LIB) $(USERIF_LIBS) $(KERNEL_LIBS) $(SYS_LIBS)

COPTS = $(CFLAGS) $(IMPORT_DEFINES)  $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)
MSGCOPTS = $(INCLUDE_PATH)
SMCOPTS =

# we want to recompile everything if COPTS changes,
# so we store COPTS into $COPTS_FILE and have object
# files depend on it (except when "make depend" was called)
COPTS_FILE = $O/.last-copts
ifneq ("$(COPTS)","$(shell cat $(COPTS_FILE) 2>/dev/null || echo '')")
$(shell $(MKPATH) "$O" && echo "$(COPTS)" >$(COPTS_FILE))
endif

#------------------------------------------------------------------------------
# User-supplied makefile fragment(s)
# >>>
# <<<
#------------------------------------------------------------------------------

# Main target
all: $(TARGET_DIR)/$(TARGET)

$(TARGET_DIR)/% :: $O/%
	@mkdir -p $(TARGET_DIR)
	$(Q)$(LN) $< $@
ifeq ($(TOOLCHAIN_NAME),clangc2)
	$(Q)-$(LN) $(<:%.dll=%.lib) $(@:%.dll=%.lib)
endif

$O/$(TARGET): $(OBJS)  $(wildcard $(EXTRA_OBJS)) Makefile $(CONFIGFILE)
	@$(MKPATH) $O
	@echo Creating executable: $@
	$(Q)$(CXX) $(LDFLAGS) -o $O/$(TARGET) $(OBJS) $(EXTRA_OBJS) $(AS_NEEDED_OFF) $(WHOLE_ARCHIVE_ON) $(LIBS) $(WHOLE_ARCHIVE_OFF) $(OMNETPP_LIBS)

.PHONY: all clean cleanall depend msgheaders smheaders

.SUFFIXES: .cc

$O/%.o: %.cc $(COPTS_FILE) | msgheaders smheaders
	@$(MKPATH) $(dir $@)
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) -o $@ $<

%_m.cc %_m.h: %.msg
	$(qecho) MSGC: $<
	$(Q)$(MSGC) -s _m.cc -MD -MP -MF $O/$(basename $@).d $(MSGCOPTS) $?

%_sm.cc %_sm.h: %.sm
	$(qecho) SMC: $<
	$(Q)$(SMC) -c++ -suffix cc $(SMCOPTS) $?

msgheaders: $(MSGFILES:.msg=_m.h)

smheaders: $(SMFILES:.sm=_sm.h)

clean:
	$(qecho) Cleaning $(TARGET)
	$(Q)-rm -rf $O
	$(Q)-rm -f $(TARGET_DIR)/$(TARGET)
	$(Q)-rm -f $(TARGET_DIR)/$(TARGET:%.dll=%.lib)
	$(Q)-rm -f $(call opp_rwildcard, . , *_m.cc *_m.h *_sm.cc *_sm.h)

cleanall:
	$(Q)$(MAKE) -s clean MODE=release
	$(Q)$(MAKE) -s clean MODE=debug
	$(Q)-rm -rf $(PROJECT_OUTPUT_DIR)

# include all dependencies
-include $(OBJS:%.o=%.d)

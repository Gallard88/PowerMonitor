# what flags you want to pass to the C compiler & linker
AM_CFLAGS = --pedantic -Wall  -std=c99
#LDFLAGS =
# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
PowerMonitordir=/usr/sbin/
PowerMonitor_PROGRAMS= PowerMonitor PM_Test

lib_LIBRARIES = libPowerMonitor.a
libPowerMonitor_a_SOURCES = \
	ShdMem.h \
	PowerMonitor.h \
	PMon_Lib.c 

include_HEADERS = PowerMonitor.h

# Install that start up script.
PowerMonitorScdir = /etc/init.d/
PowerMonitorSc_Data=PowerMonitor.sh

PowerMonitor_SOURCES= \
	CmdParse.c \
	CmdParse.h \
	Cmds_PMon.c \
	Cmds_PMon.h \
	Serial.c \
	Serial.h \
	PowerMonitor.h \
	PowerMon_Main.c

PM_Test_SOURCES = \
	PMon_Lib.c \
	ShdMem.h \
	PowerMonitor.h \
	PM_Test.c



TARGET = common-fomc
TEMPLATE = lib

QT += core gui network

NVER1=0
NVER2=8
NVER3=1

SOURCES += plugins/fomc/fomcplugin.cpp
HEADERS += plugins/fomc/fomcplugin.h

THT_PLUGIN_NAME="FOMC"
THT_PLUGIN_AUTHOR="Dmitry Baryshev"
THT_PLUGIN_UUID="B5FD1969-B414-472D-B5E6-8900F0C086FD"
THT_PLUGIN_COPYRIGHT="(C) 2013 $$THT_PLUGIN_AUTHOR"
THT_PLUGIN_URL="https://github.com/smoked-herring/tht"
THT_PLUGIN_LICENSE_TEXT="GNU GPLv3+. See http://www.gnu.org/licenses/gpl-3.0.txt"

include(THT-common.pri)
include(THT-plugins.pri)

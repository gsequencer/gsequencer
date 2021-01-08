#!/bin/bash
# 
# Copyright (C) 2005-2021 Joël Krähemann
# 
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.

# ==============================================================================
# title:           bootstrap build
# description:     This script bootstraps the build directory. The purpose is
#                  to copy, strip or substitute files.
# author:          Joël Krähemann
# date:            Fri Jan  8 08:02:47 UTC 2021
# version:         v0.1.0
# usage:           bash bootstrap.sh
# dependencies:    tail, sed, gzip
# ==============================================================================

PACKAGE="gsequencer"
PACKAGE_VERSION="3.7.22"
PACKAGE_BUGREPORT="jkraehemann@gmail.com"
PACKAGE_NAME="Advanced Gtk+ Sequencer"
PACKAGE_STRING="${PACKAGE_NAME} version ${PACKAGE_VERSION}"
PACKAGE_TARNAME="${PACKAGE}-${PACKAGE_VERSION}.tar.gz"
PACKAGE_URL="http://nongnu.org/gsequencer"

prefix="/usr/local"
datadir="${prefix}/share"

srcdir="./"
builddir="./"

if [ ! -z "$SRCDIR" ]
then
    srcdir=$SRCDIR
else
    srcdir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
fi

if [ ! -z "$BUILDDIR" ]
then
    builddir=$BUILDDIR
else
    builddir="$( pwd )"
fi

while [[ "$#" -gt 0 ]]; do
    case $1 in
        --prefix) prefix="$2"; shift ;;
        --datadir) datadir="$2"; shift ;;
        *) echo "Unknown parameter passed: $1"; exit 1 ;;
    esac
    shift
done

# generate symbols files
echo "generating libags.sym"
tail -n +19 $srcdir/libags.sym.in > libags.sym

echo "generating libags_thread.sym"
tail -n +19 $srcdir/libags_thread.sym.in > libags_thread.sym

echo "generating libags_server.sym"
tail -n +19 $srcdir/libags_server.sym.in > libags_server.sym

echo "generating libags_audio.sym"
tail -n +19 $srcdir/libags_audio.sym.in > libags_audio.sym

echo "generating libags_gui.sym"
tail -n +19 $srcdir/libags_gui.sym.in > libags_gui.sym

# generate marshallers list
echo "generating marshallers list"

mkdir -p ags/object/
tail -n +19 $srcdir/ags/object/ags_marshallers.list.in > ags/object/ags_marshallers.list

mkdir -p ags/widget/
tail -n +19 $srcdir/ags/widget/ags_widget_marshallers.list.in > ags/widget/ags_widget_marshallers.list

# generate marshallers list
echo "generating po/POTFILES.in"

mkdir -p po
tail -n +19 $srcdir/po/POTFILES.in.in > po/POTFILES.in

# copy documentation listings
echo "generating resources for API Reference Manual"

mkdir -p docs/reference/libags/{xml,libags-html}/
mkdir -p docs/reference/libags-audio/{xml,libags-audio-html}/
mkdir -p docs/reference/libags-gui/{xml,libags-gui-html}/
mkdir -p docs/reference/libgsequencer/{xml,libgsequencer-html}/

tail -n +19 $srcdir/docs/reference/libags/libags-sections.txt.in > docs/reference/libags/libags-sections.txt
tail -n +19 $srcdir/docs/reference/libags/libags.types.in > docs/reference/libags/libags.types
tail -n +19 $srcdir/docs/reference/libags/libags.interfaces.in > docs/reference/libags/libags.interfaces

tail -n +19 $srcdir/docs/reference/libags-audio/libags_audio-sections.txt.in > docs/reference/libags-audio/libags_audio-sections.txt
tail -n +19 $srcdir/docs/reference/libags-audio/libags_audio.types.in > docs/reference/libags-audio/libags_audio.types
tail -n +19 $srcdir/docs/reference/libags-audio/libags_audio.interfaces.in > docs/reference/libags-audio/libags_audio.interfaces

tail -n +19 $srcdir/docs/reference/libags-gui/libags_gui-sections.txt.in > docs/reference/libags-gui/libags_gui-sections.txt
tail -n +19 $srcdir/docs/reference/libags-gui/libags_gui.types.in > docs/reference/libags-gui/libags_gui.types
tail -n +19 $srcdir/docs/reference/libags-gui/libags_gui.interfaces.in > docs/reference/libags-gui/libags_gui.interfaces

tail -n +19 $srcdir/docs/reference/libgsequencer/libgsequencer-sections.txt.in > docs/reference/libgsequencer/libgsequencer-sections.txt
tail -n +19 $srcdir/docs/reference/libgsequencer/libgsequencer.types.in > docs/reference/libgsequencer/libgsequencer.types
tail -n +19 $srcdir/docs/reference/libgsequencer/libgsequencer.interfaces.in > docs/reference/libgsequencer/libgsequencer.interfaces

# copy documentation listings and create directories
echo "generating code listings"

mkdir -p docs/listings/

tail -n +10 $srcdir/docs/listings/add_pattern.c.in > docs/listings/add_pattern.c
tail -n +10 $srcdir/docs/listings/application_mutex.c.in > docs/listings/application_mutex.c
tail -n +10 $srcdir/docs/listings/audio.c.in > docs/listings/audio.c
tail -n +10 $srcdir/docs/listings/audio_application_context.c.in > docs/listings/audio_application_context.c
tail -n +10 $srcdir/docs/listings/complete_example.c.in > docs/listings/complete_example.c
tail -n +10 $srcdir/docs/listings/config.c.in > docs/listings/config.c
tail -n +10 $srcdir/docs/listings/effects_echo_audio.c.in > docs/listings/effects_echo_audio.c
tail -n +10 $srcdir/docs/listings/effects_echo_audio_run.c.in > docs/listings/effects_echo_audio_run.c
tail -n +10 $srcdir/docs/listings/effects_echo_channel.c.in > docs/listings/effects_echo_channel.c
tail -n +10 $srcdir/docs/listings/effects_echo_channel_run.c.in > docs/listings/effects_echo_channel_run.c
tail -n +10 $srcdir/docs/listings/effects_recall_container.c.in > docs/listings/effects_recall_container.c
tail -n +10 $srcdir/docs/listings/file_property.c.in > docs/listings/file_property.c
tail -n +10 $srcdir/docs/listings/file_read.c.in > docs/listings/file_read.c
tail -n +10 $srcdir/docs/listings/file_write.c.in > docs/listings/file_write.c
tail -n +10 $srcdir/docs/listings/find_port.c.in > docs/listings/find_port.c
tail -n +10 $srcdir/docs/listings/fx_factory.c.in > docs/listings/fx_factory.c
tail -n +10 $srcdir/docs/listings/linking_prerequisites.c.in > docs/listings/linking_prerequisites.c
tail -n +10 $srcdir/docs/listings/linking_safe.c.in > docs/listings/linking_safe.c
tail -n +10 $srcdir/docs/listings/linking_unsafe.c.in > docs/listings/linking_unsafe.c
tail -n +10 $srcdir/docs/listings/notation_clipboard.c.in > docs/listings/notation_clipboard.c
tail -n +10 $srcdir/docs/listings/pcm_info.c.in > docs/listings/pcm_info.c
tail -n +10 $srcdir/docs/listings/port.c.in > docs/listings/port.c
tail -n +10 $srcdir/docs/listings/pull_thread.c.in > docs/listings/pull_thread.c
tail -n +10 $srcdir/docs/listings/recycling.c.in > docs/listings/recycling.c
tail -n +10 $srcdir/docs/listings/start_thread.c.in > docs/listings/start_thread.c
tail -n +10 $srcdir/docs/listings/thread_application_context.c.in > docs/listings/thread_application_context.c
tail -n +10 $srcdir/docs/listings/thread_obj_mutex.c.in > docs/listings/thread_obj_mutex.c

echo "create html directories"

mkdir -p html/
mkdir -p html/developer-docs
mkdir -p html/user-docs
mkdir -p html/osc-docs

echo "create pdf directories"

mkdir -p pdf/

# generate gsequencer.desktop
echo "generating gsequencer.desktop"

sed -e "s,\@datadir\@,${datadir},g" < "${srcdir}/gsequencer.desktop.in" > "gsequencer.desktop"

# compress changelog
echo "compress changelog"

gzip -9 -c $srcdir/ChangeLog > changelog.gz

# generate entities
echo "ags documentation entities"

mkdir -p docs/usersBook/xml/
mkdir -p docs/developersBook/xml/
mkdir -p docs/oscBook/xml/

# users book
echo "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" > docs/usersBook/xml/agsdocentities.ent
echo -e -n "\n" >> docs/usersBook/xml/agsdocentities.ent

echo "<!ENTITY package \"${PACKAGE}\">" >> docs/usersBook/xml/agsdocentities.ent
echo "<!ENTITY package_version \"${PACKAGE_VERSION}\">" >> docs/usersBook/xml/agsdocentities.ent
echo "<!ENTITY package_bugreport \"${PACKAGE_BUGREPORT}\">" >> docs/usersBook/xml/agsdocentities.ent
echo "<!ENTITY package_name \"${PACKAGE_NAME}\">" >> docs/usersBook/xml/agsdocentities.ent
echo "<!ENTITY package_string \"${PACKAGE_STRING}\">" >> docs/usersBook/xml/agsdocentities.ent
echo "<!ENTITY package_tarname \"${PACKAGE_TARNAME}\">" >> docs/usersBook/xml/agsdocentities.ent
echo "<!ENTITY package_url \"${PACKAGE_URL}\">" >> docs/usersBook/xml/agsdocentities.ent
echo "<!ENTITY package_buildddir \"${builddir}\">" >> docs/usersBook/xml/agsdocentities.ent

# developers book
echo "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" > docs/developersBook/xml/agsdocentities.ent
echo -e -n "\n" >> docs/developersBook/xml/agsdocentities.ent

echo "<!ENTITY package \"${PACKAGE}\">" >> docs/developersBook/xml/agsdocentities.ent
echo "<!ENTITY package_version \"${PACKAGE_VERSION}\">" >> docs/developersBook/xml/agsdocentities.ent
echo "<!ENTITY package_bugreport \"${PACKAGE_BUGREPORT}\">" >> docs/developersBook/xml/agsdocentities.ent
echo "<!ENTITY package_name \"${PACKAGE_NAME}\">" >> docs/developersBook/xml/agsdocentities.ent
echo "<!ENTITY package_string \"${PACKAGE_STRING}\">" >> docs/developersBook/xml/agsdocentities.ent
echo "<!ENTITY package_tarname \"${PACKAGE_TARNAME}\">" >> docs/developersBook/xml/agsdocentities.ent
echo "<!ENTITY package_url \"${PACKAGE_URL}\">" >> docs/developersBook/xml/agsdocentities.ent
echo "<!ENTITY package_buildddir \"${builddir}\">" >> docs/developersBook/xml/agsdocentities.ent

# osc book
echo "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" > docs/oscBook/xml/agsdocentities.ent
echo -e -n "\n" >> docs/oscBook/xml/agsdocentities.ent

echo "<!ENTITY package \"${PACKAGE}\">" >> docs/oscBook/xml/agsdocentities.ent
echo "<!ENTITY package_version \"${PACKAGE_VERSION}\">" >> docs/oscBook/xml/agsdocentities.ent
echo "<!ENTITY package_bugreport \"${PACKAGE_BUGREPORT}\">" >> docs/oscBook/xml/agsdocentities.ent
echo "<!ENTITY package_name \"${PACKAGE_NAME}\">" >> docs/oscBook/xml/agsdocentities.ent
echo "<!ENTITY package_string \"${PACKAGE_STRING}\">" >> docs/oscBook/xml/agsdocentities.ent
echo "<!ENTITY package_tarname \"${PACKAGE_TARNAME}\">" >> docs/oscBook/xml/agsdocentities.ent
echo "<!ENTITY package_url \"${PACKAGE_URL}\">" >> docs/oscBook/xml/agsdocentities.ent
echo "<!ENTITY package_buildddir \"${builddir}\">" >> docs/oscBook/xml/agsdocentities.ent

if [ $(readlink -f ${builddir}) != "${srcdir}" ]
then
    echo "copy XML files"

    cp $srcdir/docs/{usersBook,developersBook,oscBook,appa}.xml docs/
    
    cp $srcdir/docs/usersBook/*.xml docs/usersBook/
    cp $srcdir/docs/developersBook/*.xml docs/developersBook/
    cp $srcdir/docs/oscBook/*.xml docs/oscBook/

    cp $srcdir/docs/reference/libags/libags.xml docs/reference/libags/
    
    cp $srcdir/docs/reference/libags-audio/libags_audio.xml docs/reference/libags-audio/

    cp $srcdir/docs/reference/libags-gui/libags_gui.xml docs/reference/libags-gui/

    cp $srcdir/docs/reference/libgsequencer/libgsequencer.xml docs/reference/libgsequencer/
fi

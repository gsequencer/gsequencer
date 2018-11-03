#!/bin/bash
# 
# Copyright (C) 2005-2018 Jo\u00EBl Kr\u00E4hemann
# 
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.

# binary
files=`otool -L /Users/joelkraehemann/GSequencer.app/Contents/MacOS/GSequencer-bin | grep /Users/joelkraehemann/gtk/inst/lib | awk '{ print $1 }' `

for f in $files
do
    echo $f

    install_name_tool -change $f @executable_path/../Resources/lib/$(basename $f) Contents/MacOS/GSequencer-bin
done

install_name_tool -change libinstpatch-1.0.0.dylib @executable_path/../Resources/lib/libinstpatch-1.0.0.dylib Contents/MacOS/GSequencer-bin

files=(libgsequencer.dylib libags_gui.dylib libags_server.dylib libags_thread.dylib libags.dylib)

for f in ${files[*]}
do
    echo $f

    install_name_tool -change $f @executable_path/../Resources/lib/$(basename $f) Contents/MacOS/GSequencer-bin
done

files=`otool -L /Users/joelkraehemann/GSequencer.app/Contents/Resources/lib/libags.dylib | grep /Users/joelkraehemann/gtk/inst/lib | awk '{ print $1 }' `

for f in $files
do
    echo $f

    install_name_tool -change $f @executable_path/../Resources/lib/$(basename $f) Contents/Resources/lib/libags.dylib
    install_name_tool -change $f @executable_path/../Resources/lib/$(basename $f) Contents/Resources/lib/libags.2.dylib
done

files=`otool -L /Users/joelkraehemann/GSequencer.app/Contents/Resources/lib/libags_thread.dylib | grep /Users/joelkraehemann/gtk/inst/lib | awk '{ print $1 }' `

for f in $files
do
    echo $f

    install_name_tool -change $f @executable_path/../Resources/lib/$(basename $f) Contents/Resources/lib/libags_thread.dylib
    install_name_tool -change $f @executable_path/../Resources/lib/$(basename $f) Contents/Resources/lib/libags_thread.2.dylib
done

files=`otool -L /Users/joelkraehemann/GSequencer.app/Contents/Resources/lib/libags_server.dylib | grep /Users/joelkraehemann/gtk/inst/lib | awk '{ print $1 }' `

for f in $files
do
    echo $f

    install_name_tool -change $f @executable_path/../Resources/lib/$(basename $f) Contents/Resources/lib/libags_server.dylib
    install_name_tool -change $f @executable_path/../Resources/lib/$(basename $f) Contents/Resources/lib/libags_server.2.dylib
done

files=`otool -L /Users/joelkraehemann/GSequencer.app/Contents/Resources/lib/libags_audio.dylib | grep /Users/joelkraehemann/gtk/inst/lib | awk '{ print $1 }' `

for f in $files
do
    echo $f

    install_name_tool -change $f @executable_path/../Resources/lib/$(basename $f) Contents/Resources/lib/libags_audio.dylib
    install_name_tool -change $f @executable_path/../Resources/lib/$(basename $f) Contents/Resources/lib/libags_audio.2.dylib
done

install_name_tool -change libinstpatch-1.0.0.dylib @executable_path/../Resources/lib/libinstpatch-1.0.0.dylib Contents/Resources/lib/libags_audio.dylib
install_name_tool -change libinstpatch-1.0.0.dylib @executable_path/../Resources/lib/libinstpatch-1.0.0.dylib Contents/Resources/lib/libags_audio.2.dylib

files=`otool -L /Users/joelkraehemann/GSequencer.app/Contents/Resources/lib/libags_gui.dylib | grep /Users/joelkraehemann/gtk/inst/lib | awk '{ print $1 }' `

for f in $files
do
    echo $f

    install_name_tool -change $f @executable_path/../Resources/lib/$(basename $f) Contents/Resources/lib/libags_gui.dylib
    install_name_tool -change $f @executable_path/../Resources/lib/$(basename $f) Contents/Resources/lib/libags_gui.2.dylib
done

files=`otool -L /Users/joelkraehemann/GSequencer.app/Contents/Resources/lib/libgsequencer.dylib | grep /Users/joelkraehemann/gtk/inst/lib | awk '{ print $1 }' `

for f in $files
do
    echo $f

    install_name_tool -change $f @executable_path/../Resources/lib/$(basename $f) Contents/Resources/lib/libgsequencer.dylib
    install_name_tool -change $f @executable_path/../Resources/lib/$(basename $f) Contents/Resources/lib/libgsequencer.0.dylib
done

install_name_tool -change libinstpatch-1.0.0.dylib @executable_path/../Resources/lib/libinstpatch-1.0.0.dylib Contents/Resources/lib/libgsequencer.dylib
install_name_tool -change libinstpatch-1.0.0.dylib @executable_path/../Resources/lib/libinstpatch-1.0.0.dylib Contents/Resources/lib/libgsequencer.0.dylib

files=(libgsequencer.dylib libags_gui.dylib libags_server.dylib libags_thread.dylib libags.dylib)

for f in ${files[*]}
do
    echo $f

    install_name_tool -change $f @executable_path/../Resources/lib/$(basename $f) Contents/MacOS/GSequencer-bin
done

# libgsequencer
files=(libags_gui.dylib libags_audio.dylib libags_server.dylib libags_thread.dylib libags.dylib)

for f in ${files[*]}
do
    echo $f

    install_name_tool -change $f @executable_path/../Resources/lib/$(basename $f) Contents/Resources/lib/libgsequencer.dylib
    install_name_tool -change $f @executable_path/../Resources/lib/$(basename $f) Contents/Resources/lib/libgsequencer.0.dylib
done

# libags_gui
files=(libags_server.dylib libags_audio.dylib libags_thread.dylib libags.dylib)

for f in ${files[*]}
do
    echo $f

    install_name_tool -change $f @executable_path/../Resources/lib/$(basename $f) Contents/Resources/lib/libags_gui.dylib
    install_name_tool -change $f @executable_path/../Resources/lib/$(basename $f) Contents/Resources/lib/libags_gui.2.dylib
done

# libags_audio
files=(libags_server.dylib libags_thread.dylib libags.dylib)

for f in ${files[*]}
do
    echo $f

    install_name_tool -change $f @executable_path/../Resources/lib/$(basename $f) Contents/Resources/lib/libags_audio.dylib
    install_name_tool -change $f @executable_path/../Resources/lib/$(basename $f) Contents/Resources/lib/libags_audio.2.dylib
done

# libags_server
files=(libags_server.dylib libags_thread.dylib libags.dylib)

for f in ${files[*]}
do
    echo $f

    install_name_tool -change $f @executable_path/../Resources/lib/$(basename $f) Contents/Resources/lib/libags_server.dylib
    install_name_tool -change $f @executable_path/../Resources/lib/$(basename $f) Contents/Resources/lib/libags_server.2.dylib
done

# libags_thread
files=(libags_server.dylib libags.dylib)

for f in ${files[*]}
do
    echo $f

    install_name_tool -change $f @executable_path/../Resources/lib/$(basename $f) Contents/Resources/lib/libags_thread.dylib
    install_name_tool -change $f @executable_path/../Resources/lib/$(basename $f) Contents/Resources/lib/libags_thread.2.dylib
done

# libags
files=(libags_server.dylib libags_thread.dylib)

for f in ${files[*]}
do
    echo $f

    install_name_tool -change $f @executable_path/../Resources/lib/$(basename $f) Contents/Resources/lib/libags.dylib
    install_name_tool -change $f @executable_path/../Resources/lib/$(basename $f) Contents/Resources/lib/libags.2.dylib
done

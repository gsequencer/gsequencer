#!/bin/bash

# GSequencer - Advanced GTK Sequencer
# Copyright (C) 2005-2018 Joël Krähemann
#
# This file is part of GSequencer.
#
# GSequencer is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# GSequencer is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.

# binary
files=`otool -L /Users/joelkraehemann/GSequencer.app/Contents/MacOS/GSequencer | grep /opt/gsequencer/usr/lib | awk '{ print $1 }' `

for f in $files
do
    echo $f

    install_name_tool -change $f @executable_path/../Frameworks/$(basename $f) Contents/MacOS/gsequencer
done

files=(libgsequencer.dylib libags_gui.dylib libags_server.dylib libags_thread.dylib libags.dylib)

for f in ${files[*]}
do
    echo $f

    install_name_tool -change $f @executable_path/../Frameworks/$(basename $f) Contents/MacOS/gsequencer
done

# libgsequencer
files=(libags_gui.dylib libags_audio.dylib libags_server.dylib libags_thread.dylib libags.dylib)

for f in ${files[*]}
do
    echo $f

    install_name_tool -change $f @executable_path/../Frameworks/$(basename $f) Contents/Frameworks/libgsequencer.dylib
done

# libags_gui
files=(libags_server.dylib libags_audio.dylib libags_thread.dylib libags.dylib)

for f in ${files[*]}
do
    echo $f

    install_name_tool -change $f @executable_path/../Frameworks/$(basename $f) Contents/Frameworks/libags_gui.dylib
done

# libags_audio
files=(libags_server.dylib libags_thread.dylib libags.dylib)

for f in ${files[*]}
do
    echo $f

    install_name_tool -change $f @executable_path/../Frameworks/$(basename $f) Contents/Frameworks/libags_audio.dylib
    install_name_tool -change $f @executable_path/../Frameworks/$(basename $f) Contents/Frameworks/libags_audio.0.dylib
done

# libags_server
files=(libags_server.dylib libags_thread.dylib libags.dylib)

for f in ${files[*]}
do
    echo $f

    install_name_tool -change $f @executable_path/../Frameworks/$(basename $f) Contents/Frameworks/libags_server.dylib
    install_name_tool -change $f @executable_path/../Frameworks/$(basename $f) Contents/Frameworks/libags_server.0.dylib
done

# libags_thread
files=(libags_server.dylib libags.dylib)

for f in ${files[*]}
do
    echo $f

    install_name_tool -change $f @executable_path/../Frameworks/$(basename $f) Contents/Frameworks/libags_thread.dylib
    install_name_tool -change $f @executable_path/../Frameworks/$(basename $f) Contents/Frameworks/libags_thread.0.dylib
done

# libags
files=(libags_server.dylib libags_thread.dylib)

for f in ${files[*]}
do
    echo $f

    install_name_tool -change $f @executable_path/../Frameworks/$(basename $f) Contents/Frameworks/libags.dylib
    install_name_tool -change $f @executable_path/../Frameworks/$(basename $f) Contents/Frameworks/libags.0.dylib
done

# dylib
libs=`ls Contents/Frameworks/*.dylib`

for l in $libs
do
    echo $l
    files=`otool -L $l | grep /opt/gsequencer/usr/lib | awk '{ print $1 }' `
    
    for f in $files
    do
	echo $f

	install_name_tool -change $f @executable_path/../Frameworks/$(basename $f) $l
    done
done

# ladspa
plugins=`ls Contents/Plugins/ladspa/*.dylib`

for p in $plugins
do
    echo $p
    files=`otool -L $p | grep /opt/gsequencer/usr/lib | awk '{ print $1 }' `
    
    for f in $files
    do
	echo $f

	install_name_tool -change $f @executable_path/../Frameworks/$(basename $f) $p
    done
done

# dssi
plugins=`ls Contents/Plugins/dssi/*.dylib`

for p in $plugins
do
    echo $p
    files=`otool -L $p | grep /opt/gsequencer/usr/lib | awk '{ print $1 }' `
    
    for f in $files
    do
	echo $f

	install_name_tool -change $f @executable_path/../Frameworks/$(basename $f) $p
    done
done

# lv2
plugins=`ls Contents/Plugins/lv2/*/*.dylib`

for p in $plugins
do
    echo $p
    files=`otool -L $p | grep /opt/gsequencer/usr/lib | awk '{ print $1 }' `
    
    for f in $files
    do
	echo $f

	install_name_tool -change $f @executable_path/../Frameworks/$(basename $f) $p
    done
done

# gdk-pixbuf
loaders=`ls Contents/Frameworks/gdk-pixbuf-2.0/2.10.0/loaders/*.so`

for m in $loaders
do
    echo $m
    files=`otool -L $m | grep /opt/gsequencer/usr/lib | awk '{ print $1 }' `
    
    for f in $files
    do
	echo $f

	install_name_tool -change $f @executable_path/../Frameworks/$(basename $f) $m
    done
done

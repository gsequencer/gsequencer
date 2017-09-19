#!/bin/bash

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

#!/bin/sh

clear

mkdir -p $HOME/Music/GSequencer/free-sounds
mkdir -p $HOME/Music/GSequencer/free-sounds/sf2
cd $HOME/Music/GSequencer/free-sounds/sf2 && curl -L https://gsequencer.com/download/FluidR3_GM.sf2 -O
cd $HOME/Music/GSequencer/free-sounds/sf2 && curl -L https://gsequencer.com/download/FluidR3_GS.sf2 -O
cd $HOME/Music/GSequencer/free-sounds/sf2 && curl -L https://gsequencer.com/download/FluidR3Mono_GM2-315.sf2 -O
cd $HOME/Music/GSequencer/free-sounds/sf2 && curl -L https://gsequencer.com/download/TimGM6mb.sf2 -O

mkdir -p $HOME/Music/GSequencer/free-sounds/drumkits
curl -L https://gsequencer.com/download/audiophob.zip | bsdtar -xf- -C $HOME/Music/GSequencer
curl -L https://gsequencer.com/download/bja-pacific.zip | bsdtar -xf- -C $HOME/Music/GSequencer
curl -L https://gsequencer.com/download/circ-afrique-v4.zip | bsdtar -xf- -C $HOME/Music/GSequencer
curl -L https://gsequencer.com/download/colombo-acoustic-drumkit.zip | bsdtar -xf- -C $HOME/Music/GSequencer
curl -L https://gsequencer.com/download/electric-empire-kit.zip | bsdtar -xf- -C $HOME/Music/GSequencer
curl -L https://gsequencer.com/download/forzee-stereo.zip | bsdtar -xf- -C $HOME/Music/GSequencer
curl -L https://gsequencer.com/download/gimme-a-hand-1_0.zip | bsdtar -xf- -C $HOME/Music/GSequencer
curl -L https://gsequencer.com/download/gm-kit.zip | bsdtar -xf- -C $HOME/Music/GSequencer
curl -L https://gsequencer.com/download/hard-electro.zip | bsdtar -xf- -C $HOME/Music/GSequencer
curl -L https://gsequencer.com/download/millo-drums-v-1.zip | bsdtar -xf- -C $HOME/Music/GSequencer
curl -L https://gsequencer.com/download/millo-multi-layered-2.zip | bsdtar -xf- -C $HOME/Music/GSequencer
curl -L https://gsequencer.com/download/millo-multi-layered-3.zip | bsdtar -xf- -C $HOME/Music/GSequencer
curl -L https://gsequencer.com/download/the-black-pearl-1_0.zip | bsdtar -xf- -C $HOME/Music/GSequencer
curl -L https://gsequencer.com/download/tr808-emulation-kit.zip | bsdtar -xf- -C $HOME/Music/GSequencer
curl -L https://gsequencer.com/download/vari-breaks.zip | bsdtar -xf- -C $HOME/Music/GSequencer

#!/bin/bash
# 
# Copyright (C) 2022 Joël Krähemann
# 
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.

export AGS_TEST_CONFIG="[generic]
autosave-thread=false
simple-file=true
disable-feature=experimental
segmentation=4/4

[thread]
model=super-threaded
super-threaded-scope=audio
lock-global=ags-thread
lock-parent=ags-recycling-thread
thread-pool-max-unused-threads=4
max-precision=250

[soundcard-0]
backend=pulse
device=ags-pulse-devout-0
samplerate=44100
buffer-size=512
pcm-channels=2
dsp-channels=2
format=16
use-cache=true
cache-buffer-size=4096

[recall]
auto-sense=true
"

#!/bin/bash
# 
# Copyright (C) 2005-2019 Jo\u00EBl Kr\u00E4hemann
# 
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.

# ==============================================================================
# title:           Prepare distribution
# description:     This script prepares various files for distribution. The
#                  purpose is to add missing copyright and license information
#                  to generated files.
# author:          Joël Krähemann
# date:            Mon Jun 24 15:42:52 UTC 2019
# version:         v0.1.0
# usage:           bash prepare-distribution.sh
# ==============================================================================

gpl3_license_note="$PWD/license-notice-gnu-gpl-3-0+-sym.txt";

# API Reference Manual libags
docs_libags_gpl3_files=("libags-overrides.txt" "libags-sections.txt" "libags.interfaces" "libags.types")

for f in "${docs_libags_gpl3_files[@]}"; do
    cat $gpl3_license_note > docs/reference/libags/$f.in;
    echo -en "\n" >> docs/reference/libags/$f.in;
    cat docs/reference/libags/$f >> docs/reference/libags/$f.in;
done

# API Reference Manual libags-audio
docs_libags_audio_gpl3_files=("libags_audio-overrides.txt" "libags_audio-sections.txt" "libags_audio.interfaces" "libags_audio.types")

for f in "${docs_libags_audio_gpl3_files[@]}"; do
    cat $gpl3_license_note > docs/reference/libags-audio/$f.in;
    echo -en "\n" >> docs/reference/libags-audio/$f.in;
    cat docs/reference/libags-audio/$f >> docs/reference/libags-audio/$f.in;
done

# API Reference Manual libags-gui
docs_libags_gui_gpl3_files=("libags_gui-overrides.txt" "libags_gui-sections.txt" "libags_gui.interfaces" "libags_gui.types")

for f in "${docs_libags_gui_gpl3_files[@]}"; do
    cat $gpl3_license_note > docs/reference/libags-gui/$f.in;
    echo -en "\n" >> docs/reference/libags-gui/$f.in;
    cat docs/reference/libags-gui/$f >> docs/reference/libags-gui/$f.in;
done

# API Reference Manual libgsequencer
docs_libgsequencer_gpl3_files=("libgsequencer-overrides.txt" "libgsequencer-sections.txt" "libgsequencer.interfaces" "libgsequencer.types")

for f in "${docs_libgsequencer_gpl3_files[@]}"; do
    cat $gpl3_license_note > docs/reference/libgsequencer/$f.in;
    echo -en "\n" >> docs/reference/libgsequencer/$f.in;
    cat docs/reference/libgsequencer/$f >> docs/reference/libgsequencer/$f.in;
done

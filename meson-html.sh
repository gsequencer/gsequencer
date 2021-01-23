#!/bin/sh
# 
# Copyright (C) 2005-2021 Joël Krähemann
# 
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.

# ==============================================================================
# title:           generate HTMLHELP
# description:     This script runs XSL Transformation of HTML docs. The purpose
#                  is to generate HTML files.
# author:          Joël Krähemann
# date:            Sat Jan  9 01:12:16 UTC 2021
# version:         v0.1.0
# usage:           bash meson-html.sh
# dependencies:    xsltproc
# ==============================================================================

xsltproc --nonet --xinclude --stringparam html.stylesheet "custom.css" --stringparam htmlhelp.show.menu "1" --stringparam suppress.navigation "0" -o ${MESON_BUILD_ROOT}/html/user-manual/ http://docbook.sourceforge.net/release/xsl/current/htmlhelp/htmlhelp.xsl ${MESON_BUILD_ROOT}/docs/usersBook.xml
xsltproc --nonet --xinclude --stringparam html.stylesheet "custom.css" --stringparam htmlhelp.show.menu "1" --stringparam suppress.navigation "0" -o ${MESON_BUILD_ROOT}/html/developer-manual/ http://docbook.sourceforge.net/release/xsl/current/htmlhelp/htmlhelp.xsl ${MESON_BUILD_ROOT}/docs/developersBook.xml
xsltproc --nonet --xinclude --stringparam html.stylesheet "custom.css" --stringparam htmlhelp.show.menu "1" --stringparam suppress.navigation "0" -o ${MESON_BUILD_ROOT}/html/osc-manual/ http://docbook.sourceforge.net/release/xsl/current/htmlhelp/htmlhelp.xsl ${MESON_BUILD_ROOT}/docs/oscBook.xml

exit 0

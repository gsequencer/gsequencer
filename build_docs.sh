#!/bin/sh

xsltproc --nonet --xinclude -o ${MESON_BUILD_ROOT}/html/user-docs/ http://docbook.sourceforge.net/release/xsl/current/htmlhelp/htmlhelp.xsl docs/usersBook.xml
xsltproc --nonet --xinclude -o ${MESON_BUILD_ROOT}/html/developer-docs/ http://docbook.sourceforge.net/release/xsl/current/htmlhelp/htmlhelp.xsl docs/developersBook.xml
xsltproc --nonet --xinclude -o ${MESON_BUILD_ROOT}/html/osc-docs/ http://docbook.sourceforge.net/release/xsl/current/htmlhelp/htmlhelp.xsl docs/oscBook.xml

/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __AGS_REGEX_H__
#define __AGS_REGEX_H__

#include <glib.h>
#include <glib-object.h>

#include <sys/types.h>
#include <regex.h>

G_BEGIN_DECLS

G_DEPRECATED_FOR(ags_regex_util_compile)
int ags_regcomp(regex_t *preg, const char *regex, int cflags);
G_DEPRECATED_FOR(ags_regex_util_execute)
int ags_regexec(const regex_t *preg, const char *string, size_t nmatch,
		regmatch_t pmatch[], int eflags);

G_END_DECLS

#endif /*__AGS_REGEX_H__*/

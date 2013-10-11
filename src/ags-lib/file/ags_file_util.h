/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __AGS_FILE_UTIL_H__
#define __AGS_FILE_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <libxml/tree.h>

/* clipboard */
void ags_file_util_read_clipboard(AgsFile *file, xmlNode *node, AgsClipboard **clipboard);
xmlNode* ags_file_util_write_clipboard(AgsFile *file, xmlNode *parent, AgsClipboard *clipboard);

/* property */
void ags_file_util_read_property(AgsFile *file, xmlNode *node, AgsProperty **property);
xmlNode* ags_file_util_write_property(AgsFile *file, xmlNode *parent, AgsProperty *property);

void ags_file_util_read_property_list(AgsFile *file, xmlNode *node, GList **property);
xmlNode* ags_file_util_write_property_list(AgsFile *file, xmlNode *parent, GList *property);

/* history */
void ags_file_util_read_history(AgsFile *file, xmlNode *node, AgsHistory **history);
xmlNode* ags_file_util_write_history(AgsFile *file, xmlNode *parent, AgsHistory *history);

#endif /*__AGS_FILE_UTIL_H__*/

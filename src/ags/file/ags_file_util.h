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

#include <ags-lib/object/ags_serializeable.h>

#include <ags-lib/object/ags_serializeable.h>

#include <ags/thread/ags_history.h>

#include <ags/file/ags_file.h>
#include <ags/file/ags_embedded_audio.h>
#include <ags/file/ags_file_link.h>

/* gvalue */
void ags_file_util_read_value(xmlNode *node,
			      gchar **id,
			      GValue *value, xmlChar **xpath);
xmlNode* ags_file_util_write_value(xmlNode *parent,
				   gchar *id,
				   GValue *value, AgsSerializeable *serializeable);

/* parameter */
void ags_file_util_read_parameter(xmlNode *node,
				  gchar **id,
				  GParameter **parameter, gint *n_params, xmlChar ***xpath);
xmlNode* ags_file_util_write_parameter(xmlNode *parent,
				       gchar *id,
				       GParameter *parameter, gint n_params, AgsSerializeable **serializeable);

/* dependency */
void ags_file_util_read_dependency(xmlNode *node,
				   gchar **id,
				   gchar **name, xmlChar **xpath);
xmlNode* ags_file_util_write_dependency(xmlNode *parent,
					gchar *id,
					gchar *name, xmlChar *xpath);

/* history */
void ags_file_util_read_history(AgsFile *file, xmlNode *node, AgsHistory **history);
xmlNode* ags_file_util_write_history(AgsFile *file, xmlNode *parent, AgsHistory *history);

/* embedded audio */
void ags_file_read_embedded_audio(AgsFile *file, xmlNode *node, AgsEmbeddedAudio **embedded_audio);
xmlNode* ags_file_write_embedded_audio(AgsFile *file, xmlNode *parent, AgsEmbeddedAudio *embedded_audio);

void ags_file_read_embedded_audio_list(AgsFile *file, xmlNode *node, GList **embedded_audio);
xmlNode* ags_file_write_embedded_audio_list(AgsFile *file, xmlNode *parent, GList *embedded_audio);

/* file link */
void ags_file_read_file_link(AgsFile *file, xmlNode *node, AgsFileLink **file_link);
xmlNode* ags_file_write_file_link(AgsFile *file, xmlNode *parent, AgsFileLink *file_link);

void ags_file_read_file_link_list(AgsFile *file, xmlNode *node, GList **file_link);
xmlNode* ags_file_write_file_link_list(AgsFile *file, xmlNode *parent, GList *file_link);

#endif /*__AGS_FILE_UTIL_H__*/

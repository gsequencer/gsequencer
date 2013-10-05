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

#include <gtk/gtk.h>

#include <libxml/tree.h>

void ags_file_util_read_gvalue(xmlNode *node, gchar **id,
			       GType *type, GValue **value, xmlXPathContext **context);
void ags_file_util_write_gvalue(xmlNode *parent, gchar *id,
				GValue *value, xmlXPathContext *context);

void ags_file_util_read_parameter(xmlNode *node, gchar **id,
				  GParameter **parameter, gint *n_params, xmlXPathContext **context);
void ags_file_util_write_parameter(xmlNode *parent, gchar *id,
				   GParameter *parameter, gint n_params, xmlXPathContext *context);

void ags_file_util_read_callback(xmlNode *node, gchar **id,
				 gchar **signal_name, gchar **callback_name, xmlXPathContext **context);
void ags_file_util_write_callback(xmlNode *parent, gchar *id,
				  gchar *signal_name, gchar *callback_name, xmlXPathContext *context);

void ags_file_util_read_handler(xmlNode *node, gchar **id,
				gchar **name, AgsRecallHandler **handler, GList **parameter, xmlXPathContext **context);
void ags_file_util_read_handler_list(xmlNode *node, gchar **id,
				     GList **handler, xmlXPathContext *context);
void ags_file_util_write_handler(xmlNode *parent, gchar *id,
				 gchar *name, GList **context);
void ags_file_util_write_handler_list(xmlNode *parent, gchar *id,
				      GList *handler, AgsRecallHandler *handler, GList *parameter, GList *context);

void ags_file_util_read_dependency(xmlNode *node, gchar **id,
				   gchar **name, GList **values, xmlXPathContext **context);
void ags_file_util_read_dependency_list(xmlNode *node, gchar **id,
					GList **dependency, GList **context);
void ags_file_util_write_dependency(xmlNode *parent, gchar *id,
				    gchar *name, GList *values, GList *context);
void ags_file_util_write_dependency_list(xmlNode *parent, gchar *id,
					 GList *dependency, GList *context);

#endif /*__AGS_FILE_UTIL_H__*/

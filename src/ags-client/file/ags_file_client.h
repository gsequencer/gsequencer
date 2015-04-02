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

#ifndef __AGS_FILE_CLIENT_H__
#define __AGS_FILE_CLIENT_H__

#include <glib.h>
#include <glib-object.h>

#include <libxml/tree.h>

/* script */
void ags_file_client_read_script(AgsFile *file, xmlNode *node, AgsScript **script);
xmlNode* ags_file_client_write_script(AgsFile *file, xmlNode *node, AgsScript *script);

void ags_file_client_read_script_list(AgsFile *file, xmlNode *node, GList **script);
xmlNode* ags_file_client_write_script_list(AgsFile *file, xmlNode *node, GList *script);

/* client */
void ags_file_client_read_client(AgsFile *file, xmlNode *node, AgsClient **client);
xmlNode* ags_file_client_write_client(AgsFile *file, xmlNode *parent, AgsClient *client);


#endif /*__AGS_FILE_CLIENT_H__*/


/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#ifndef __AGS_FILE_LOOKUP_H__
#define __AGS_FILE_LOOKUP_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/file/ags_file.h>

#include <libxml/tree.h>

#define AGS_TYPE_FILE_LOOKUP                (ags_file_lookup_get_type())
#define AGS_FILE_LOOKUP(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FILE_LOOKUP, AgsFileLookup))
#define AGS_FILE_LOOKUP_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FILE_LOOKUP, AgsFileLookupClass))
#define AGS_IS_FILE_LOOKUP(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FILE_LOOKUP))
#define AGS_IS_FILE_LOOKUP_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FILE_LOOKUP))
#define AGS_FILE_LOOKUP_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FILE_LOOKUP, AgsFileLookupClass))

typedef struct _AgsFileLookup AgsFileLookup;
typedef struct _AgsFileLookupClass AgsFileLookupClass;

struct _AgsFileLookup
{
  GObject object;

  AgsFile *file;

  xmlNode *node;
  gpointer ref;
};

struct _AgsFileLookupClass
{
  GObjectClass object;

  void (*resolve)(AgsFileLookup *lookup);
};

GType ags_file_lookup_get_type(void);

GList* ags_file_lookup_find_by_node(GList *file_lookup,
				    xmlNode *node);

void ags_file_lookup_resolve(AgsFileLookup *lookup);

/* */
AgsFileLookup* ags_file_lookup_new();

#endif /*__AGS_FILE_LOOKUP_H__*/

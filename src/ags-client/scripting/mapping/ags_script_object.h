/* AGS Client - Advanced GTK Sequencer Client
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

#ifndef __AGS_SCRIPT_OBJECT_H__
#define __AGS_SCRIPT_OBJECT_H__

#include <glib.h>
#include <glib-object.h>

#include <libxml/tree.h>

#define AGS_TYPE_SCRIPT_OBJECT                (ags_script_object_get_type())
#define AGS_SCRIPT_OBJECT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SCRIPT_OBJECT, AgsScriptObject))
#define AGS_SCRIPT_OBJECT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_SCRIPT_OBJECT, AgsScriptObjectClass))
#define AGS_IS_SCRIPT_OBJECT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SCRIPT_OBJECT))
#define AGS_IS_SCRIPT_OBJECT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SCRIPT_OBJECT))
#define AGS_SCRIPT_OBJECT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_SCRIPT_OBJECT, AgsScriptObjectClass))

typedef struct _AgsScriptObject AgsScriptObject;
typedef struct _AgsScriptObjectClass AgsScriptObjectClass;

typedef enum{
  AGS_SCRIPT_OBJECT_LAUNCHED        = 1,
  AGS_SCRIPT_OBJECT_RETVAL_VOID     = 1 << 1,
}AgsScriptObjectFlags;

#define AGS_SCRIPT_OBJECT_ERROR (ags_script_object_error_quark())

typedef enum{
  AGS_SCRIPT_OBJECT_INDEX_EXCEEDED,
}AgsError;

struct _AgsScriptObject
{
  GObject object;
  
  guint flags;

  GObject *script;
  
  xmlNode *node;
  gchar *id;

  AgsScriptObject *retval;
};

struct _AgsScriptObjectClass
{
  GObjectClass object;

  void (*map_xml)(AgsScriptObject *script_object);

  AgsScriptObject* (*launch)(AgsScriptObject *script_object);

  AgsScriptObject* (*tostring)(AgsScriptObject *script_object);
  AgsScriptObject* (*valueof)(AgsScriptObject *script_object,
			      GError **error);
};

GType ags_script_object_get_type();

void ags_script_object_map_xml(AgsScriptObject *script_object);
AgsScriptObject* ags_script_object_launch(AgsScriptObject *script_object);

AgsScriptObject* ags_script_object_tostring(AgsScriptObject *script_object);
AgsScriptObject* ags_script_object_valueof(AgsScriptObject *script_object);

AgsScriptObject* ags_script_object_new(GObject *script);

#endif /*__AGS_SCRIPT_OBJECT_H__*/

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

#ifndef __AGS_SCRIPT_ARRAY_H__
#define __AGS_SCRIPT_ARRAY_H__

#include <glib.h>
#include <glib-object.h>

#include <ags-client/scripting/mapping/ags_script_object.h>

#define AGS_TYPE_SCRIPT_ARRAY                (ags_script_array_get_type())
#define AGS_SCRIPT_ARRAY(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SCRIPT_ARRAY, AgsScriptArray))
#define AGS_SCRIPT_ARRAY_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_SCRIPT_ARRAY, AgsScriptArrayClass))
#define AGS_IS_SCRIPT_ARRAY(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SCRIPT_ARRAY))
#define AGS_IS_SCRIPT_ARRAY_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SCRIPT_ARRAY))
#define AGS_SCRIPT_ARRAY_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_SCRIPT_ARRAY, AgsScriptArrayClass))

typedef struct _AgsScriptArray AgsScriptArray;
typedef struct _AgsScriptArrayClass AgsScriptArrayClass;

typedef enum{
  AGS_SCRIPT_ARRAY_UTF8_ENCODED     = 1,
  AGS_SCRIPT_ARRAY_BASE64_ENCODED   = 1 << 1,
}AgsScriptArrayFlags;

typedef enum{
  AGS_SCRIPT_ARRAY_INT16,
  AGS_SCRIPT_ARRAY_UINT16,
  AGS_SCRIPT_ARRAY_INT32,
  AGS_SCRIPT_ARRAY_UINT32,
  AGS_SCRIPT_ARRAY_INT64,
  AGS_SCRIPT_ARRAY_UINT64,
  AGS_SCRIPT_ARRAY_DOUBLE,
  AGS_SCRIPT_ARRAY_CHAR,
  AGS_SCRIPT_ARRAY_POINTER,
}AgsScriptArrayMode;

struct _AgsScriptArray
{
  AgsScriptObject object;

  guint flags;
  guint mode;

  gpointer array;
  guint dimension;

  gpointer data;
  xmlNode **node;
  guint *length;
};

struct _AgsScriptArrayClass
{
  AgsScriptObjectClass object;
};

GType ags_script_array_get_type();

AgsScriptArray* ags_script_array_new();

#endif /*__AGS_SCRIPT_ARRAY_H__*/

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

#ifndef __AGS_SCRIPT_H__
#define __AGS_SCRIPT_H__

#include <glib.h>
#include <glib-object.h>

#include <stdio.h>

#include <ags-client/scripting/ags_xml_script_factory.h>

#define AGS_TYPE_SCRIPT                (ags_script_get_type())
#define AGS_SCRIPT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SCRIPT, AgsScript))
#define AGS_SCRIPT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_SCRIPT, AgsScriptClass))
#define AGS_IS_SCRIPT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SCRIPT))
#define AGS_IS_SCRIPT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SCRIPT))
#define AGS_SCRIPT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_SCRIPT, AgsScriptClass))

#define AGS_SCRIPT_DEFAULT_ENCODING "UTF-8\0"

typedef struct _AgsScript AgsScript;
typedef struct _AgsScriptClass AgsScriptClass;

struct _AgsScript
{
  GObject object;

  GObject *xml_interpreter;

  AgsXmlScriptFactory *xml_script_factory;
  
  gchar *filename;

  xmlDoc *script;
};

struct _AgsScriptClass
{
  GObjectClass object;

  void (*read)(AgsScript *script);
  void (*write)(AgsScript *script);
  void (*launch)(AgsScript *script);
};

GType ags_script_get_type();

void ags_script_read(AgsScript *script);
void ags_script_write(AgsScript *script);
void ags_script_launch(AgsScript *script);

AgsScript* ags_script_new();

#endif /*__AGS_SCRIPT_H__*/

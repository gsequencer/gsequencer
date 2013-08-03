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

#ifndef __AGS_XML_INTERPRETER_H__
#define __AGS_XML_INTERPRETER_H__

#include <glib.h>
#include <glib-object.h>

#include <libxml/tree.h>

#include <ags-client/scripting/mapping/ags_script_stack.h>

#define AGS_TYPE_XML_INTERPRETER                (ags_xml_interpreter_get_type())
#define AGS_XML_INTERPRETER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_XML_INTERPRETER, AgsXmlInterpreter))
#define AGS_XML_INTERPRETER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_XML_INTERPRETER, AgsXmlInterpreterClass))
#define AGS_IS_XML_INTERPRETER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_XML_INTERPRETER))
#define AGS_IS_XML_INTERPRETER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_XML_INTERPRETER))
#define AGS_XML_INTERPRETER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_XML_INTERPRETER, AgsXmlInterpreterClass))

#define AGS_XML_INTERPRETER_DEFAULT_STACK_SIZE (G_MAXUINT)

typedef struct _AgsXmlInterpreter AgsXmlInterpreter;
typedef struct _AgsXmlInterpreterClass AgsXmlInterpreterClass;

typedef enum{
  AGS_XML_INTERPRETER_RUNNING     = 1,
}AgsXmlInterpreterFlags;

#define AGS_XML_INTERPRETER_ERROR (ags_xml_interpreter_error_quark())

typedef enum{
  AGS_XML_INTERPRETER_STACK_OVERFLOW,
}AgsError;

struct _AgsXmlInterpreter
{
  GObject object;
  
  guint flags;

  AgsScriptStack *default_stack;
  guint64 stack_size;
};

struct _AgsXmlInterpreterClass
{
  GObjectClass object;

  void (*start)(AgsXmlInterpreter *xml_interpreter);
};

GType ags_xml_interpreter_get_type();

GQuark ags_xml_interpreter_error_quark();

void ags_xml_interpreter_start(AgsXmlInterpreter *xml_interpreter);
void ags_xml_interpreter_run_snipped(AgsXmlInterpreter *xml_interpreter,
				     xmlNode *snipped);

AgsXmlInterpreter* ags_xml_interpreter_new(GObject *script);

#endif /*__AGS_XML_INTERPRETER_H__*/

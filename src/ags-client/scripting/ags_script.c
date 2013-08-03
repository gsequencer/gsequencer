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

#include <ags-client/scripting/ags_script.h>

#include <ags-lib/object/ags_connectable.h>

void ags_script_class_init(AgsScriptClass *script);
void ags_script_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_script_init(AgsScript *script);
void ags_script_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec);
void ags_script_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec);
void ags_script_connect(AgsConnectable *connectable);
void ags_script_disconnect(AgsConnectable *connectable);
void ags_script_finalize(GObject *gobject);

void ags_script_real_read(AgsScript *script);
void ags_script_real_write(AgsScript *script);
void ags_script_real_launch(AgsScript *script);

enum{
  READ,
  WRITE,
  LAUNCH,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_FILENAME,
  PROP_SCRIPT,
};

static gpointer ags_script_parent_class = NULL;
static guint script_signals[LAST_SIGNAL];

GType
ags_script_get_type()
{
  static GType ags_type_script = 0;

  if(!ags_type_script){
    static const GTypeInfo ags_script_info = {
      sizeof (AgsScriptClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_script_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsScript),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_script_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_script_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_script = g_type_register_static(G_TYPE_OBJECT,
					     "AgsScript\0",
					     &ags_script_info,
					     0);

    g_type_add_interface_static(ags_type_script,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_script);
}

void
ags_script_class_init(AgsScriptClass *script)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_script_parent_class = g_type_class_peek_parent(script);

  /* GObjectClass */
  gobject = (GObjectClass *) script;

  gobject->set_property = ags_script_set_property;
  gobject->get_property = ags_script_get_property;

  gobject->finalize = ags_script_finalize;

  /* properties */
  param_spec = g_param_spec_pointer("filename\0",
				    "filename of script\0",
				    "The filename of the script\0",
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SCRIPT,
				  param_spec);

  param_spec = g_param_spec_pointer("script\0",
				    "xml script document\0",
				    "The xml script document\0",
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SCRIPT,
				  param_spec);

  /* AgsScriptClass */
  script->write = ags_script_real_write;
  script->read = ags_script_real_read;
  script->launch = ags_script_real_launch;

  /* signals */
  script_signals[WRITE] =
    g_signal_new("write\0",
		 G_TYPE_FROM_CLASS(script),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsScriptObjectClass, write),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  script_signals[READ] =
    g_signal_new("read\0",
		 G_TYPE_FROM_CLASS(script),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsScriptObjectClass, read),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  script_signals[LAUNCH] =
    g_signal_new("launch\0",
		 G_TYPE_FROM_CLASS(script),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsScriptObjectClass, launch),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_script_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_script_connect;
  connectable->disconnect = ags_script_disconnect;
}

void
ags_script_init(AgsScript *script)
{
  script->xml_script_factory = ags_xml_script_factory_new();
  ags_xml_script_factory_create_prototype(script->xml_script_factory);

  script->filename = NULL;
  script->file = NULL;

  script->script = NULL;
}

void
ags_script_set_property(GObject *gobject,
			guint prop_id,
			const GValue *value,
			GParamSpec *param_spec)
{
  AgsScript *script;

  script = AGS_SCRIPT(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      gchar *filename;

      filename = (gchar *) g_value_get_pointer(value);

      script->filename = filename;
    }
    break;
  case PROP_SCRIPT:
    {
      xmlDoc *doc;

      doc = (xmlDoc *) g_value_get_pointer(value);

      script->script = doc;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_script_get_property(GObject *gobject,
			guint prop_id,
			GValue *value,
			GParamSpec *param_spec)
{
  AgsScript *script;

  script = AGS_SCRIPT(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      g_value_set_pointer(value, script->filename);
    }
    break;
  case PROP_SCRIPT:
    {
      g_value_set_pointer(value, script->filename);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_script_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_script_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_script_finalize(GObject *gobject)
{
  AgsScript *script;

  script = AGS_SCRIPT(gobject);

  G_OBJECT_CLASS(ags_script_parent_class)->finalize(gobject);
}

void
ags_script_real_read(AgsScript *script)
{
  xmlDoc *doc;

  //TODO:JK: implement me
  g_object_set(G_OBJECT(script),
	       "script\0", doc,
	       NULL);
}

void
ags_script_read(AgsScript *script)
{
  g_return_if_fail(AGS_IS_SCRIPT(script));

  g_object_ref(G_OBJECT(script));
  g_signal_emit(G_OBJECT(script),
		script_signals[READ], 0);
  g_object_unref(G_OBJECT(script));
}

void
ags_script_real_write(AgsScript *script)
{
  //TODO:JK: implement me
}

void
ags_script_write(AgsScript *script)
{
  g_return_if_fail(AGS_IS_SCRIPT(script));

  g_object_ref(G_OBJECT(script));
  g_signal_emit(G_OBJECT(script),
		script_signals[WRITE], 0);
  g_object_unref(G_OBJECT(script));
}

void
ags_script_real_launch(AgsScript *script)
{
  //TODO:JK: implement me
}

void
ags_script_launch(AgsScript *script)
{
  g_return_if_fail(AGS_IS_SCRIPT(script));

  g_object_ref(G_OBJECT(script));
  g_signal_emit(G_OBJECT(script),
		script_signals[LAUNCH], 0);
  g_object_unref(G_OBJECT(script));
}

AgsScript*
ags_script_new()
{
  AgsScript *script;

  script = (AgsScript *) g_object_new(AGS_TYPE_SCRIPT,
				      NULL);

  return(script);
}

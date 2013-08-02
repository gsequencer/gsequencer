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

#include <ags-client/scripting/mapping/ags_script_object.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags-client/object/ags_marshal.h>

#include <ags-client/scripting/ags_script.h>

void ags_script_object_class_init(AgsScriptObjectClass *script_object);
void ags_script_object_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_script_object_init(AgsScriptObject *script_object);
void ags_script_object_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec);
void ags_script_object_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec);
void ags_script_object_connect(AgsConnectable *connectable);
void ags_script_object_disconnect(AgsConnectable *connectable);
void ags_script_object_finalize(GObject *gobject);

void ags_script_object_real_map_xml(AgsScriptObject *script_object);
AgsScriptObject* ags_script_object_real_launch(AgsScriptObject *script_object);
AgsScriptObject* ags_script_object_real_valueof(AgsScriptObject *script_object);

AgsScriptObject* ags_script_object_find_flags_descending_first_match(AgsScriptObject *script_object,
								     guint flags);
AgsScriptObject* ags_script_object_find_flags_descending_last_match(AgsScriptObject *script_object,
								    guint flags);

enum{
  MAP_XML,
  LAUNCH,
  TOSTRING,
  VALUEOF,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_SCRIPT,
};

static gpointer ags_script_object_parent_class = NULL;
static guint script_object_signals[LAST_SIGNAL];

GType
ags_script_object_get_type()
{
  static GType ags_type_script_object = 0;

  if(!ags_type_script_object){
    static const GTypeInfo ags_script_object_info = {
      sizeof (AgsScriptObjectClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_script_object_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsScriptObject),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_script_object_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_script_object_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_script_object = g_type_register_static(G_TYPE_OBJECT,
						    "AgsScriptObject\0",
						    &ags_script_object_info,
						    0);
    
    g_type_add_interface_static(ags_type_script_object,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_script_object);
}

void
ags_script_object_class_init(AgsScriptObjectClass *script_object)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_script_object_parent_class = g_type_class_peek_parent(script_object);

  /* GObjectClass */
  gobject = (GObjectClass *) script_object;

  gobject->set_property = ags_script_object_set_property;
  gobject->get_property = ags_script_object_get_property;

  gobject->finalize = ags_script_object_finalize;

  /* properties */
  param_spec = g_param_spec_object("script\0",
				   "script running in\0",
				   "The script where this script object belongs to\0",
				   AGS_TYPE_SCRIPT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SCRIPT,
				  param_spec);

  /* AgsScriptObjectClass */
  script_object->map_xml = ags_script_object_real_map_xml;
  script_object->launch = ags_script_object_real_launch;
  script_object->tostring = NULL;
  script_object->valueof = NULL;

  /* signals */
  script_object_signals[MAP_XML] =
    g_signal_new("map_xml\0",
		 G_TYPE_FROM_CLASS(script_object),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsScriptObjectClass, map_xml),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  script_object_signals[LAUNCH] =
    g_signal_new("launch\0",
		 G_TYPE_FROM_CLASS(script_object),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsScriptObjectClass, launch),
		 NULL, NULL,
		 g_cclosure_user_marshal_OBJECT__VOID,
		 G_TYPE_OBJECT, 0);

  script_object_signals[TOSTRING] =
    g_signal_new("tostring\0",
		 G_TYPE_FROM_CLASS(script_object),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsScriptObjectClass, tostring),
		 NULL, NULL,
		 g_cclosure_user_marshal_OBJECT__VOID,
		 G_TYPE_OBJECT, 0);

  script_object_signals[VALUEOF] =
    g_signal_new("valueof\0",
		 G_TYPE_FROM_CLASS(script_object),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsScriptObjectClass, valueof),
		 NULL, NULL,
		 g_cclosure_user_marshal_OBJECT__VOID,
		 G_TYPE_OBJECT, 0);
}

void
ags_script_object_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_script_object_connect;
  connectable->disconnect = ags_script_object_disconnect;
}

void
ags_script_object_init(AgsScriptObject *script_object)
{
  script_object->node = NULL;
  script_object->id = NULL;

  script_object->retval = NULL;
}


void
ags_script_object_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsScriptObject *script_object;

  script_object = AGS_SCRIPT_OBJECT(gobject);

  switch(prop_id){
  case PROP_SCRIPT:
    {
      AgsScript *script;

      script = (AgsScript *) g_value_get_object(value);

      if(script == ((AgsScript *) script_object->script))
	return;

      if(script_object->script != NULL)
	g_object_unref(script_object->script);

      if(script != NULL)
	g_object_ref(G_OBJECT(script));

      script_object->script = (GObject *) script;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_script_object_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec)
{
  AgsScriptObject *script_object;

  script_object = AGS_SCRIPT_OBJECT(gobject);

  switch(prop_id){
  case PROP_SCRIPT:
    {
      g_value_set_object(value, script_object->script);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_script_object_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_script_object_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_script_object_finalize(GObject *gobject)
{
  AgsScriptObject *script_object;

  script_object = AGS_SCRIPT_OBJECT(gobject);

  G_OBJECT_CLASS(ags_script_object_parent_class)->finalize(gobject);
}

void
ags_script_object_real_map_xml(AgsScriptObject *script_object)
{
  AgsScript *script;
  GError *error;

  script = AGS_SCRIPT(script_object->script);

  error = NULL;
  script_object->node = ags_xml_script_factory_map(script->xml_script_factory,
						   script_object,
						   &error);

  if(error != NULL){
    g_warning(error->message);
  }
}

void
ags_script_object_map_xml(AgsScriptObject *script_object)
{
  g_return_if_fail(AGS_IS_SCRIPT_OBJECT(script_object));

  g_object_ref(G_OBJECT(script_object));
  g_signal_emit(G_OBJECT(script_object),
		script_object_signals[MAP_XML], 0);
  g_object_unref(G_OBJECT(script_object));
}

AgsScriptObject*
ags_script_object_real_launch(AgsScriptObject *script_object)
{
}

AgsScriptObject*
ags_script_object_launch(AgsScriptObject *script_object)
{
  AgsScriptObject *retval;

  g_return_val_if_fail(AGS_IS_SCRIPT_OBJECT(script_object), NULL);

  g_object_ref(G_OBJECT(script_object));
  g_signal_emit(G_OBJECT(script_object),
		script_object_signals[LAUNCH], 0,
		&retval);
  g_object_unref(G_OBJECT(script_object));

  return(retval);
}

AgsScriptObject*
ags_script_object_tostring(AgsScriptObject *script_object)
{
  AgsScriptObject *retval;

  g_return_val_if_fail(AGS_IS_SCRIPT_OBJECT(script_object), NULL);

  g_object_ref(G_OBJECT(script_object));
  g_signal_emit(G_OBJECT(script_object),
		script_object_signals[TOSTRING], 0,
		&retval);
  g_object_unref(G_OBJECT(script_object));

  return(retval);
}

AgsScriptObject*
ags_script_object_real_valueof(AgsScriptObject *script_object)
{
  AgsScriptObject *first_match, *last_match;
  xmlNode *node;

  if((first_match = ags_script_object_find_flags_descending_first_match(script_object,
									AGS_SCRIPT_OBJECT_LAUNCHED)) == NULL){
    return(NULL);
  }

  last_match = ags_script_object_find_flags_descending_first_match(script_object,
								   AGS_SCRIPT_OBJECT_LAUNCHED);

  //TODO:JK: implement me

  return(NULL);
}

AgsScriptObject*
ags_script_object_valueof(AgsScriptObject *script_object)
{
  AgsScriptObject *retval;

  g_return_val_if_fail(AGS_IS_SCRIPT_OBJECT(script_object), NULL);

  g_object_ref(G_OBJECT(script_object));
  g_signal_emit(G_OBJECT(script_object),
		script_object_signals[VALUEOF], 0,
		&retval);
  g_object_unref(G_OBJECT(script_object));

  return(retval);
}

AgsScriptObject*
ags_script_object_find_flags_descending_first_match(AgsScriptObject *script_object,
						    guint flags)
{
  AgsScriptObject *current, *value;

  if((flags & (script_object->flags)) == flags){
    return(current);
  }

  current = script_object->retval;

  while(current != NULL){
    value = ags_script_object_find_flags_descending(current,
						    flags);
    
    if(value != NULL){
      return(current);
    }

    current = current->next;
  }

  return(NULL);
}

AgsScriptObject*
ags_script_object_find_flags_descending_last_match(AgsScriptObject *script_object,
						   guint flags)
{
  AgsScriptObject *current, *value;

  current = script_object->retval;

  while(current != NULL){
    value = ags_script_object_find_flags_descending(current,
						    flags);
    
    if(value != NULL){
      return(current);
    }

    current = current->next;
  }

  if((flags & (script_object->flags)) == flags){
    return(current);
  }

  return(NULL);
}

AgsScriptObject*
ags_script_object_new(GObject *script)
{
  AgsScriptObject *script_object;

  script_object = (AgsScriptObject *) g_object_new(AGS_TYPE_SCRIPT_OBJECT,
						   "script\0", script,
						   NULL);
  
  return(script_object);
}

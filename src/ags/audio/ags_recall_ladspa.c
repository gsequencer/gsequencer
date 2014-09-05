/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
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

#include <ags/audio/ags_recall_ladspa.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/object/ags_plugin.h>

void ags_recall_ladspa_class_init(AgsRecallLadspaClass *recall_ladspa_class);
void ags_recall_ladspa_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_ladspa_plugin_interface_init(AgsPluginInterface *plugin);
void ags_recall_ladspa_init(AgsRecallLadspa *recall_ladspa);
void ags_recall_ladspa_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec);
void ags_recall_ladspa_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec);
void ags_recall_ladspa_connect(AgsConnectable *connectable);
void ags_recall_ladspa_disconnect(AgsConnectable *connectable);
void ags_recall_ladspa_set_ports(AgsPlugin *plugin, GList *port);
void ags_recall_ladspa_finalize(GObject *gobject);

enum{
  PROP_0,
  PROP_FILENAME,
  PROP_EFFECT,
  PROP_INDEX,
};

static gpointer ags_recall_ladspa_parent_class = NULL;
static AgsConnectableInterface* ags_recall_ladspa_parent_connectable_interface;

GType
ags_recall_ladspa_get_type (void)
{
  static GType ags_type_recall_ladspa = 0;

  if(!ags_type_recall_ladspa){
    static const GTypeInfo ags_recall_ladspa_info = {
      sizeof (AgsRecallLadspaClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_ladspa_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallLadspa),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_ladspa_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_ladspa_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_recall_ladspa_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recall_ladspa = g_type_register_static(AGS_TYPE_RECALL,
						    "AgsRecallLadspa\0",
						    &ags_recall_ladspa_info,
						    0);

    g_type_add_interface_static(ags_type_recall_ladspa,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_recall_ladspa,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_recall_ladspa);
}

void
ags_recall_ladspa_class_init(AgsRecallLadspaClass *recall_ladspa)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_recall_ladspa_parent_class = g_type_class_peek_parent(recall_ladspa);

  /* GObjectClass */
  gobject = (GObjectClass *) recall_ladspa;

  gobject->set_property = ags_recall_ladspa_set_property;
  gobject->get_property = ags_recall_ladspa_get_property;

  gobject->finalize = ags_recall_ladspa_finalize;

  /* properties */
  param_spec =  g_param_spec_string("filename\0",
				    "the dll's file\0",
				    "The filename as string of dll\0",
				    NULL,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);

  param_spec =  g_param_spec_string("effect\0",
				    "the effect\0",
				    "The effect's string representation\0",
				    NULL,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_EFFECT,
				  param_spec);

  param_spec =  g_param_spec_uint("index\0",
				  "index of effect\0",
				  "The numerical index of effect\0",
				  0,
				  65535,
				  0,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_INDEX,
				  param_spec);
}


void
ags_recall_ladspa_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_recall_ladspa_connect;
  connectable->disconnect = ags_recall_ladspa_disconnect;
}

void
ags_recall_ladspa_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->set_ports = ags_recall_ladspa_set_ports;
}

void
ags_recall_ladspa_init(AgsRecallLadspa *recall_ladspa)
{
  recall_ladspa->filename = NULL;
  recall_ladspa->effect = NULL;
  recall_ladspa->index = 0;
}

void
ags_recall_ladspa_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsRecallLadspa *recall_ladspa;

  recall_ladspa = AGS_RECALL_LADSPA(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      gchar *filename;
      
      filename = g_value_get_string(value);

      if(filename == recall_ladspa->filename){
	return;
      }

      recall_ladspa->filename = filename;
    }
    break;
  case PROP_EFFECT:
    {
      gchar *effect;
      
      effect = g_value_get_string(value);

      if(effect == recall_ladspa->effect){
	return;
      }

      recall_ladspa->effect = effect;
    }
    break;
  case PROP_INDEX:
    {
      guint *index;
      
      index = g_value_get_uint(value);

      recall_ladspa->index = index;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_recall_ladspa_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec)
{
  AgsRecallLadspa *recall_ladspa;

  recall_ladspa = AGS_RECALL_LADSPA(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      g_value_set_string(value, recall_ladspa->filename);
    }
    break;
  case PROP_EFFECT:
    {
      g_value_set_string(value, recall_ladspa->effect);
    }
    break;
  case PROP_INDEX:
    {
      g_value_set_string(value, recall_ladspa->index);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recall_ladspa_connect(AgsConnectable *connectable)
{
  ags_recall_ladspa_parent_connectable_interface->connect(connectable);
}

void
ags_recall_ladspa_disconnect(AgsConnectable *connectable)
{
  ags_recall_ladspa_parent_connectable_interface->disconnect(connectable);
}

void
ags_recall_ladspa_set_ports(AgsPlugin *plugin, GList *port)
{
  AgsRecallLadspa *recall_ladspa;

  recall_ladspa = AGS_RECALL_LADSPA(plugin);

  //TODO:JK: implement me
}

void
ags_recall_ladspa_finalize(GObject *gobject)
{
  //TODO:JK: implement me

  /* call parent */
  G_OBJECT_CLASS(ags_recall_ladspa_parent_class)->finalize(gobject);
}

AgsRecallLadspa*
ags_recall_ladspa_new(gchar *filename,
		      gchar *effect,
		      guint index)
{
  AgsRecallLadspa *recall_ladspa;

  recall_ladspa = (AgsRecallLadspa *) g_object_new(AGS_TYPE_RECALL_LADSPA,
						   "filename\0", filename,
						   "effect\0", effect,
						   "index\0", index,
						   NULL);

  return(recall_ladspa);
}

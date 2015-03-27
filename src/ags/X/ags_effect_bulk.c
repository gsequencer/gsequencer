/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
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

#include <ags/X/ags_effect_bulk.h>
#include <ags/X/ags_effect_bulk_callbacks.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/object/ags_marshal.h>
#include <ags/object/ags_plugin.h>

void ags_effect_bulk_class_init(AgsEffectBulkClass *effect_bulk);
void ags_effect_bulk_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_effect_bulk_plugin_interface_init(AgsPluginInterface *plugin);
void ags_effect_bulk_init(AgsEffectBulk *effect_bulk);
void ags_effect_bulk_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_effect_bulk_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_effect_bulk_connect(AgsConnectable *connectable);
void ags_effect_bulk_disconnect(AgsConnectable *connectable);
gchar* ags_effect_bulk_get_name(AgsPlugin *plugin);
void ags_effect_bulk_set_name(AgsPlugin *plugin, gchar *name);
gchar* ags_effect_bulk_get_version(AgsPlugin *plugin);
void ags_effect_bulk_set_version(AgsPlugin *plugin, gchar *version);
gchar* ags_effect_bulk_get_build_id(AgsPlugin *plugin);
void ags_effect_bulk_set_build_id(AgsPlugin *plugin, gchar *build_id);

/**
 * SECTION:ags_effect_bulk
 * @short_description: A composite widget to visualize a bunch of #AgsChannel
 * @title: AgsEffectBulk
 * @section_id:
 * @include: ags/X/ags_effect_bulk.h
 *
 * #AgsEffectBulk is a composite widget to visualize one #AgsChannel. It should be
 * packed by an #AgsEffectBulk.
 */

enum{
  ADD_EFFECT,
  REMOVE_EFFECT,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_CHANNEL,
};

static gpointer ags_effect_bulk_parent_class = NULL;
static guint effect_bulk_signals[LAST_SIGNAL];

GType
ags_effect_bulk_get_type(void)
{
  static GType ags_type_effect_bulk = 0;

  if(!ags_type_effect_bulk){
    static const GTypeInfo ags_effect_bulk_info = {
      sizeof(AgsEffectBulkClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_effect_bulk_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsEffectBulk),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_effect_bulk_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_effect_bulk_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_effect_bulk_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_effect_bulk = g_type_register_static(GTK_TYPE_VBOX,
						  "AgsEffectBulk\0", &ags_effect_bulk_info,
						  0);

    g_type_add_interface_static(ags_type_effect_bulk,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_effect_bulk,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_effect_bulk);
}

void
ags_effect_bulk_class_init(AgsEffectBulkClass *effect_bulk)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_effect_bulk_parent_class = g_type_class_peek_parent(effect_bulk);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(effect_bulk);

  gobject->set_property = ags_effect_bulk_set_property;
  gobject->get_property = ags_effect_bulk_get_property;

  /* properties */
  /**
   * AgsEffectBulk:channel:
   *
   * The start of a bunch of #AgsChannel to visualize.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_object("channel\0",
				   "assigned channel\0",
				   "The channel it is assigned with\0",
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL,
				  param_spec);
}

void
ags_effect_bulk_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_effect_bulk_connect;
  connectable->disconnect = ags_effect_bulk_disconnect;
}

void
ags_effect_bulk_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = NULL;
  plugin->set_name = NULL;
  plugin->get_version = ags_effect_bulk_get_version;
  plugin->set_version = ags_effect_bulk_set_version;
  plugin->get_build_id = ags_effect_bulk_get_build_id;
  plugin->set_build_id = ags_effect_bulk_set_build_id;
  plugin->get_xml_type = NULL;
  plugin->set_xml_type = NULL;
  plugin->get_ports = NULL;
  plugin->read = NULL;
  plugin->write = NULL;
  plugin->set_ports = NULL;
}

void
ags_effect_bulk_init(AgsEffectBulk *effect_bulk)
{
  GtkAlignment *alignment;
  GtkHBox *hbox;
  
  effect_bulk->flags = 0;

  effect_bulk->name = "ags-default-effect-bulk\0";
  
  effect_bulk->version = AGS_EFFECT_BULK_DEFAULT_VERSION;
  effect_bulk->build_id = AGS_EFFECT_BULK_DEFAULT_BUILD_ID;
  
  effect_bulk->channel = NULL;

  alignment = (GtkAlignment *) g_object_new(GTK_TYPE_ALIGNMENT,
					    "xalign\0", 1.0,
					    NULL);
  gtk_box_pack_start(effect_bulk,
		     alignment,
		     FALSE, FALSE,
		     0);

  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_container_add(alignment,
		    hbox);

  effect_bulk->add = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_ADD);
  gtk_box_pack_start(hbox,
		     effect_bulk->add,
		     FALSE, FALSE,
		     0);

  effect_bulk->remove = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_REMOVE);
  gtk_box_pack_start(hbox,
		     effect_bulk->remove,
		     FALSE, FALSE,
		     0);

  effect_bulk->table = (GtkTable *) gtk_table_new(1, 2, FALSE);
  gtk_box_pack_start(effect_bulk,
		     effect_bulk->table,
		     FALSE, FALSE,
		     0);
}

void
ags_effect_bulk_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsEffectBulk *effect_bulk;

  effect_bulk = AGS_EFFECT_BULK(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      AgsChannel *channel;

      channel = (AgsChannel *) g_value_get_object(value);

      if(effect_bulk->channel == channel){
	return;
      }

      if(effect_bulk->channel != NULL){
	g_object_unref(effect_bulk->channel);
      }

      if(channel != NULL){
	g_object_ref(channel);
      }

      effect_bulk->channel = channel;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_effect_bulk_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsEffectBulk *effect_bulk;

  effect_bulk = AGS_EFFECT_BULK(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_effect_bulk_connect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_effect_bulk_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

gchar*
ags_effect_bulk_get_name(AgsPlugin *plugin)
{
  return(AGS_EFFECT_BULK(plugin)->name);
}

void
ags_effect_bulk_set_name(AgsPlugin *plugin, gchar *name)
{
  AgsEffectBulk *effect_bulk;

  effect_bulk = AGS_EFFECT_BULK(plugin);

  effect_bulk->name = name;
}

gchar*
ags_effect_bulk_get_version(AgsPlugin *plugin)
{
  return(AGS_EFFECT_BULK(plugin)->version);
}

void
ags_effect_bulk_set_version(AgsPlugin *plugin, gchar *version)
{
  AgsEffectBulk *effect_bulk;

  effect_bulk = AGS_EFFECT_BULK(plugin);

  effect_bulk->version = version;
}

gchar*
ags_effect_bulk_get_build_id(AgsPlugin *plugin)
{
  return(AGS_EFFECT_BULK(plugin)->build_id);
}

void
ags_effect_bulk_set_build_id(AgsPlugin *plugin, gchar *build_id)
{
  AgsEffectBulk *effect_bulk;

  effect_bulk = AGS_EFFECT_BULK(plugin);

  effect_bulk->build_id = build_id;
}

/**
 * ags_effect_bulk_new:
 * @effect_bulk: the parent effect_bulk
 * @channel: the #AgsChannel to visualize
 *
 * Creates an #AgsEffectBulk
 *
 * Returns: a new #AgsEffectBulk
 *
 * Since: 0.4
 */
AgsEffectBulk*
ags_effect_bulk_new(AgsChannel *channel)
{
  AgsEffectBulk *effect_bulk;

  effect_bulk = (AgsEffectBulk *) g_object_new(AGS_TYPE_EFFECT_BULK,
					       "channel\0", channel,
					       NULL);

  return(effect_bulk);
}

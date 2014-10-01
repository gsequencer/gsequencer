/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
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

#include <ags/X/ags_line.h>
#include <ags/X/ags_line_callbacks.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/object/ags_plugin.h>

#include <ags/X/ags_pad.h>

#include <ags/audio/ags_channel.h>

#include <ags/X/ags_line_member.h>

void ags_line_class_init(AgsLineClass *line);
void ags_line_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_line_plugin_interface_init(AgsPluginInterface *plugin);
void ags_line_init(AgsLine *line);
void ags_line_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec);
void ags_line_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec);
void ags_line_connect(AgsConnectable *connectable);
void ags_line_disconnect(AgsConnectable *connectable);
gchar* ags_line_get_version(AgsPlugin *plugin);
void ags_line_set_version(AgsPlugin *plugin, gchar *version);
gchar* ags_line_get_build_id(AgsPlugin *plugin);
void ags_line_set_build_id(AgsPlugin *plugin, gchar *build_id);

void ags_line_real_set_channel(AgsLine *line, AgsChannel *channel);

enum{
  SET_CHANNEL,
  GROUP_CHANGED,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_PAD,
  PROP_CHANNEL,
};

static gpointer ags_line_parent_class = NULL;
static guint line_signals[LAST_SIGNAL];

GType
ags_line_get_type(void)
{
  static GType ags_type_line = 0;

  if(!ags_type_line){
    static const GTypeInfo ags_line_info = {
      sizeof(AgsLineClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_line_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsLine),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_line_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_line_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_line_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_line = g_type_register_static(GTK_TYPE_VBOX,
					   "AgsLine\0", &ags_line_info,
					   0);

    g_type_add_interface_static(ags_type_line,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_line,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_line);
}

void
ags_line_class_init(AgsLineClass *line)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_line_parent_class = g_type_class_peek_parent(line);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(line);

  gobject->set_property = ags_line_set_property;
  gobject->get_property = ags_line_get_property;

  /* properties */
  param_spec = g_param_spec_object("pad\0",
				   "parent pad\0",
				   "The pad which is its parent\0",
				   AGS_TYPE_PAD,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PAD,
				  param_spec);

  param_spec = g_param_spec_object("channel\0",
				   "assigned channel\0",
				   "The channel it is assigned with\0",
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL,
				  param_spec);

  /* AgsLineClass */
  line->set_channel = ags_line_real_set_channel;

  line->group_changed = NULL;

  line_signals[SET_CHANNEL] =
    g_signal_new("set_channel\0",
		 G_TYPE_FROM_CLASS(line),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLineClass, set_channel),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);

  line_signals[GROUP_CHANGED] =
    g_signal_new("group_changed\0",
		 G_TYPE_FROM_CLASS(line),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLineClass, group_changed),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_line_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_line_connect;
  connectable->disconnect = ags_line_disconnect;
}

void
ags_line_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = NULL;
  plugin->set_name = NULL;
  plugin->get_version = ags_line_get_version;
  plugin->set_version = ags_line_set_version;
  plugin->get_build_id = ags_line_get_build_id;
  plugin->set_build_id = ags_line_set_build_id;
  plugin->get_xml_type = NULL;
  plugin->set_xml_type = NULL;
  plugin->get_ports = NULL;
  plugin->read = NULL;
  plugin->write = NULL;
  plugin->set_ports = NULL;
}

void
ags_line_init(AgsLine *line)
{
  g_signal_connect_after((GObject *) line, "parent_set\0",
			 G_CALLBACK(ags_line_parent_set_callback), (gpointer) line);

  line->flags = 0;

  line->version = AGS_VERSION;
  line->build_id = AGS_BUILD_ID;

  line->channel = NULL;

  line->pad = NULL;

  line->label = (GtkLabel *) gtk_label_new(NULL);
  gtk_box_pack_start(GTK_BOX(line),
		     GTK_WIDGET(line->label),
		     FALSE, FALSE,
		     0);

  line->group = (GtkToggleButton *) gtk_toggle_button_new_with_label("group\0");
  gtk_toggle_button_set_active(line->group, TRUE);
  gtk_box_pack_start(GTK_BOX(line),
		     GTK_WIDGET(line->group),
		     FALSE, FALSE,
		     0);

  line->expander = ags_expander_new(1, 1);
  gtk_table_set_row_spacings(line->expander->table,
			     2);
  gtk_table_set_col_spacings(line->expander->table,
			     2);
  gtk_box_pack_start(GTK_BOX(line),
		     GTK_WIDGET(line->expander),
		     TRUE, TRUE,
		     0);
}

void
ags_line_set_property(GObject *gobject,
		      guint prop_id,
		      const GValue *value,
		      GParamSpec *param_spec)
{
  AgsLine *line;

  line = AGS_LINE(gobject);

  switch(prop_id){
  case PROP_PAD:
    {
      GtkWidget *pad;

      pad = (GtkWidget *) g_value_get_object(value);

      if(line->pad == pad){
	return;
      }

      if(line->pad != NULL){
	g_object_unref(G_OBJECT(line->pad));
      }

      if(pad != NULL){
	g_object_ref(G_OBJECT(pad));
      }
      
      line->pad = pad;
    }
    break;
  case PROP_CHANNEL:
    {
      AgsChannel *channel;

      channel = (AgsChannel *) g_value_get_object(value);

      ags_line_set_channel(line, channel);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_line_get_property(GObject *gobject,
		      guint prop_id,
		      GValue *value,
		      GParamSpec *param_spec)
{
  AgsLine *line;

  line = AGS_LINE(gobject);

  switch(prop_id){
  case PROP_PAD:
    g_value_set_object(value, line->pad);
    break;
  case PROP_CHANNEL:
    g_value_set_object(value, line->channel);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_line_connect(AgsConnectable *connectable)
{
  AgsLine *line;
  GList *list, *list_start;

  line = AGS_LINE(connectable);

  if((AGS_LINE_CONNECTED & (line->flags)) != 0){
    return;
  }

  g_signal_connect_after((GObject *) line->group, "clicked\0",
			 G_CALLBACK(ags_line_group_clicked_callback), (gpointer) line);

  /* connect line members */
  list_start = 
    list = gtk_container_get_children(GTK_CONTAINER(line->expander->table));
  
  while(list != NULL){
    if(AGS_IS_CONNECTABLE(list->data)){
      ags_connectable_connect(AGS_CONNECTABLE(list->data));
    }

    list = list->next;
  }

  g_list_free(list_start);
  
  /*  */
  line->flags |= AGS_LINE_CONNECTED;
}

void
ags_line_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

gchar*
ags_line_get_version(AgsPlugin *plugin)
{
  return(AGS_LINE(plugin)->version);
}

void
ags_line_set_version(AgsPlugin *plugin, gchar *version)
{
  AGS_LINE(plugin)->version = version;
}

gchar*
ags_line_get_build_id(AgsPlugin *plugin)
{
  return(AGS_LINE(plugin)->build_id);
}

void
ags_line_set_build_id(AgsPlugin *plugin, gchar *build_id)
{
  AGS_LINE(plugin)->build_id = build_id;
}

void
ags_line_real_set_channel(AgsLine *line, AgsChannel *channel)
{
  if(line->channel == channel){
    return;
  }

  if(line->channel != NULL){
    g_object_unref(G_OBJECT(line->channel));
  }

  if(channel != NULL){
    g_object_ref(G_OBJECT(channel));
  }

  line->channel = channel;

  /* set label */
  gtk_label_set_label(line->label, g_strdup_printf("line %d\0", channel->audio_channel));
}

void
ags_line_find_port(AgsLine *line)
{
  GList *line_member, *line_member_start;

  if(line == NULL || line->expander == NULL){
    return;
  }

  line_member_start = 
    line_member = gtk_container_get_children(GTK_CONTAINER(line->expander->table));

  g_message("find");

  while(line_member != NULL){
    if(AGS_IS_LINE_MEMBER(line_member->data)){
      ags_line_member_find_port(AGS_LINE_MEMBER(line_member->data));
    }

    line_member = line_member->next;
  }

  g_list_free(line_member_start);
}

void
ags_line_set_channel(AgsLine *line, AgsChannel *channel)
{
  g_return_if_fail(AGS_IS_LINE(line));

  g_object_ref((GObject *) line);
  g_signal_emit(G_OBJECT(line),
		line_signals[SET_CHANNEL], 0,
		channel);
  g_object_unref((GObject *) line);
}

void
ags_line_group_changed(AgsLine *line)
{
  g_return_if_fail(AGS_IS_LINE(line));

  g_object_ref((GObject *) line);
  g_signal_emit(G_OBJECT(line),
		line_signals[GROUP_CHANGED], 0);
  g_object_unref((GObject *) line);
}

GList*
ags_line_find_next_grouped(GList *line)
{
  while(line != NULL && !gtk_toggle_button_get_active(AGS_LINE(line->data)->group)){
    line = line->next;
  }

  return(line);
}

AgsLine*
ags_line_new(GtkWidget *pad, AgsChannel *channel)
{
  AgsLine *line;

  line = (AgsLine *) g_object_new(AGS_TYPE_LINE,
				  "pad\0", pad,
				  "channel\0", channel,
				  NULL);

  return(line);
}

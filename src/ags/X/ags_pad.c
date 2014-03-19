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

#include <ags/X/ags_pad.h>
#include <ags/X/ags_pad_callbacks.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/object/ags_marshal.h>
#include <ags/object/ags_plugin.h>

#include <ags/X/ags_machine.h>

void ags_pad_class_init(AgsPadClass *pad);
void ags_pad_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_pad_plugin_interface_init(AgsPluginInterface *plugin);
void ags_pad_init(AgsPad *pad);
void ags_pad_set_property(GObject *gobject,
			  guint prop_id,
			  const GValue *value,
			  GParamSpec *param_spec);
void ags_pad_get_property(GObject *gobject,
			  guint prop_id,
			  GValue *value,
			  GParamSpec *param_spec);
void ags_pad_connect(AgsConnectable *connectable);
void ags_pad_disconnect(AgsConnectable *connectable);
gchar* ags_pad_get_name(AgsPlugin *plugin);
void ags_pad_set_name(AgsPlugin *plugin, gchar *name);
gchar* ags_pad_get_version(AgsPlugin *plugin);
void ags_pad_set_version(AgsPlugin *plugin, gchar *version);
gchar* ags_pad_get_build_id(AgsPlugin *plugin);
void ags_pad_set_build_id(AgsPlugin *plugin, gchar *build_id);
gchar* ags_pad_get_xml_type(AgsPlugin *plugin);
void ags_pad_set_xml_type(AgsPlugin *plugin, gchar *xml_type);
GList* ags_pad_get_ports(AgsPlugin *plugin);
void ags_pad_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_pad_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);
void ags_pad_destroy(GtkObject *object);
void ags_pad_show(GtkWidget *widget);

void ags_pad_real_set_channel(AgsPad *pad, AgsChannel *channel);
void ags_pad_real_resize_lines(AgsPad *pad, GType line_type,
			       guint audio_channels, guint audio_channels_old);

enum{
  SET_CHANNEL,
  RESIZE_LINES,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_CHANNEL,
};

static gpointer ags_pad_parent_class = NULL;
static guint pad_signals[LAST_SIGNAL];

GType
ags_pad_get_type(void)
{
  static GType ags_type_pad = 0;

  if(!ags_type_pad){
    static const GTypeInfo ags_pad_info = {
      sizeof(AgsPadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_pad_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsPad),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_pad_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_pad_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_pad_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_pad = g_type_register_static(GTK_TYPE_VBOX,
					  "AgsPad\0", &ags_pad_info,
					  0);

    g_type_add_interface_static(ags_type_pad,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_pad,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_pad);
}

void
ags_pad_class_init(AgsPadClass *pad)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_pad_parent_class = g_type_class_peek_parent(pad);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(pad);

  gobject->set_property = ags_pad_set_property;
  gobject->get_property = ags_pad_get_property;

  //TODO:JK: add finalize

  param_spec = g_param_spec_object("channel\0",
				   "assigned channel\0",
				   "The channel it is assigned with\0",
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL,
				  param_spec);

  /* AgsPadClass */
  pad->set_channel = ags_pad_real_set_channel;
  pad->resize_lines = ags_pad_real_resize_lines;

  pad_signals[SET_CHANNEL] =
    g_signal_new("set_channel\0",
		 G_TYPE_FROM_CLASS(pad),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsPadClass, set_channel),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);

  pad_signals[RESIZE_LINES] =
    g_signal_new("resize_lines\0",
		 G_TYPE_FROM_CLASS(pad),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsPadClass, resize_lines),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__ULONG_UINT_UINT,
		 G_TYPE_NONE, 3,
		 G_TYPE_ULONG, G_TYPE_UINT, G_TYPE_UINT);
}

void
ags_pad_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_pad_connect;
  connectable->disconnect = ags_pad_disconnect;
}

void
ags_pad_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = ags_pad_get_name;
  plugin->set_name = ags_pad_set_name;
  plugin->get_version = ags_pad_get_version;
  plugin->set_version = ags_pad_set_version;
  plugin->get_build_id = ags_pad_get_build_id;
  plugin->set_build_id = ags_pad_set_build_id;
  plugin->get_xml_type = ags_pad_get_xml_type;
  plugin->set_xml_type = ags_pad_set_xml_type;
  plugin->get_ports = ags_pad_get_ports;
  plugin->read = ags_pad_read;
  plugin->write = ags_pad_write;
  plugin->set_ports = NULL;
}

void
ags_pad_init(AgsPad *pad)
{
  GtkMenu *menu;
  GtkHBox *hbox;

  g_signal_connect((GObject *) pad, "parent_set\0",
		   G_CALLBACK(ags_pad_parent_set_callback), (gpointer) pad);

  pad->flags = 0;

  pad->version = AGS_VERSION;
  pad->build_id = AGS_BUILD_ID;

  pad->cols = 2;
  pad->expander_set = ags_expander_set_new(1, 1);
  gtk_box_pack_start((GtkBox *) pad, (GtkWidget *) pad->expander_set, TRUE, TRUE, 0);

  hbox = (GtkHBox *) gtk_hbox_new(TRUE, 0);
  gtk_box_pack_start((GtkBox *) pad, (GtkWidget *) hbox, FALSE, FALSE, 0);

  pad->group = (GtkToggleButton *) gtk_toggle_button_new_with_label(g_strdup("G\0"));
  gtk_toggle_button_set_active(pad->group, TRUE);
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) pad->group, FALSE, FALSE, 0);

  pad->mute = (GtkToggleButton *) gtk_toggle_button_new_with_label(g_strdup("M\0"));
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) pad->mute, FALSE, FALSE, 0);

  pad->solo = (GtkToggleButton *) gtk_toggle_button_new_with_label(g_strdup("S\0"));
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) pad->solo, FALSE, FALSE, 0);
}

void
ags_pad_set_property(GObject *gobject,
		     guint prop_id,
		     const GValue *value,
		     GParamSpec *param_spec)
{
  AgsPad *pad;

  pad = AGS_PAD(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      AgsChannel *channel;

      channel = (AgsChannel *) g_value_get_object(value);

      ags_pad_set_channel(pad, channel);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_pad_get_property(GObject *gobject,
		     guint prop_id,
		     GValue *value,
		     GParamSpec *param_spec)
{
  AgsPad *pad;

  pad = AGS_PAD(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    g_value_set_object(value, pad->channel);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_pad_connect(AgsConnectable *connectable)
{
  AgsPad *pad;
  GList *line_list;

  /* AgsPad */
  pad = AGS_PAD(connectable);

  if((AGS_PAD_CONNECTED & (pad->flags)) != 0){
    return;
  }

  pad->flags |= AGS_PAD_CONNECTED;

  /* GtkObject */
  g_signal_connect((GObject *) pad, "destroy\0",
		   G_CALLBACK(ags_pad_destroy_callback), (gpointer) pad);

  /* GtkWidget */
  g_signal_connect((GObject *) pad, "show\0",
		   G_CALLBACK(ags_pad_show_callback), (gpointer) pad);

  /* GtkButton */
  g_signal_connect_after((GObject *) pad->group, "clicked\0",
			 G_CALLBACK(ags_pad_group_clicked_callback), (gpointer) pad);

  g_signal_connect_after((GObject *) pad->mute, "clicked\0",
			 G_CALLBACK(ags_pad_mute_clicked_callback), (gpointer) pad);

  g_signal_connect_after((GObject *) pad->solo, "clicked\0",
			 G_CALLBACK(ags_pad_solo_clicked_callback), (gpointer) pad);

  /* AgsLine */
  line_list = gtk_container_get_children(GTK_CONTAINER(pad->expander_set));

  while(line_list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(line_list->data));

    line_list = line_list->next;
  }
}

void
ags_pad_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

gchar*
ags_pad_get_name(AgsPlugin *plugin)
{
  //TODO:JK: implement me

  return(NULL);
}

void
ags_pad_set_name(AgsPlugin *plugin, gchar *name)
{
  //TODO:JK: implement me
}

gchar*
ags_pad_get_version(AgsPlugin *plugin)
{
  return(AGS_PAD(plugin)->version);
}

void
ags_pad_set_version(AgsPlugin *plugin, gchar *version)
{
  //TODO:JK: implement me
}

gchar*
ags_pad_get_build_id(AgsPlugin *plugin)
{
  return(AGS_PAD(plugin)->build_id);
}

void
ags_pad_set_build_id(AgsPlugin *plugin, gchar *build_id)
{
  //TODO:JK: implement me
}

gchar*
ags_pad_get_xml_type(AgsPlugin *plugin)
{
  //TODO:JK: implement me

  return(NULL);
}

void
ags_pad_set_xml_type(AgsPlugin *plugin, gchar *xml_type)
{
  //TODO:JK: implement me
}

GList*
ags_pad_get_ports(AgsPlugin *plugin)
{
  //TODO:JK: implement me

  return(NULL);
}

void
ags_pad_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  //TODO:JK: implement me
}

xmlNode*
ags_pad_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  //TODO:JK: implement me

  return(NULL);
}


void
ags_pad_destroy(GtkObject *object)
{
  //TODO:JK: implement me
}

void
ags_pad_show(GtkWidget *widget)
{
  AgsPad *pad;

  fprintf(stdout, "ags_pad_show\n\0");

  pad = AGS_PAD(widget);
}

void
ags_pad_real_set_channel(AgsPad *pad, AgsChannel *channel)
{
  AgsChannel *current;
  GList *line;

  if(pad->channel == channel){
    return;
  }

  if(pad->channel != NULL){
    g_object_unref(G_OBJECT(pad->channel));
  }

  if(channel != NULL){
    g_object_ref(G_OBJECT(channel));
  }

  pad->channel = channel;

  line = gtk_container_get_children(GTK_CONTAINER(AGS_PAD(pad)->expander_set));
  current = pad->channel;
  
  while(line != NULL){
    g_object_set(G_OBJECT(line->data),
		 "channel\0", current,
		 NULL);

    current = current->next;
    line = line->next;
  }
}

void
ags_pad_set_channel(AgsPad *pad, AgsChannel *channel)
{
  g_return_if_fail(AGS_IS_PAD(pad));

  g_object_ref((GObject *) pad);
  g_signal_emit(G_OBJECT(pad),
		pad_signals[SET_CHANNEL], 0,
		channel);
  g_object_unref((GObject *) pad);
}

void
ags_pad_real_resize_lines(AgsPad *pad, GType line_type,
			  guint audio_channels, guint audio_channels_old)
{
  AgsMachine *machine;
  AgsLine *line;
  AgsChannel *channel;
  guint i, j;

  //  fprintf(stdout, "ags_pad_real_resize_lines: audio_channels = %u ; audio_channels_old = %u\n\0", audio_channels, audio_channels_old);

  if(audio_channels > audio_channels_old){
    machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) pad, AGS_TYPE_MACHINE);
    channel = ags_channel_nth(pad->channel, audio_channels_old);

    /* create AgsLine */
    for(i = audio_channels_old / pad->cols; i < audio_channels / pad->cols; i++){
      for(j = 0; j < pad->cols; j++){
	line = (AgsLine *) g_object_new(line_type,
					"pad\0", pad,
					"channel\0", channel,
					NULL);
	channel->line_widget = (GtkWidget *) line;
	ags_expander_set_add(pad->expander_set,
			     (GtkWidget *) line,
			     j, i,
			     1, 1);
	
	channel = channel->next;
      }
    }

    /* check if we should show and connect the AgsLine */
    if(machine != NULL && GTK_WIDGET_VISIBLE((GtkWidget *) machine)){
      GList *list;

      list = g_list_nth(gtk_container_get_children(GTK_CONTAINER(pad->expander_set)),
			audio_channels_old);

      /* show and connect AgsLine */
      while(list != NULL){
	line = AGS_LINE(list->data);

	gtk_widget_show_all((GtkWidget *) line);
	ags_connectable_connect(AGS_CONNECTABLE(line));

	list = list->next;
      }
    }
  }else if(audio_channels < audio_channels_old){
    GList *list, *list_start;

    list_start =
      list = g_list_nth(gtk_container_get_children(GTK_CONTAINER(pad->expander_set)),
			audio_channels);
    
    while(list != NULL){
      ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

      list = list->next;
    }

    list = list_start;

    while(list != NULL){
      gtk_widget_destroy(GTK_WIDGET(list->data));

      list = list->next;
    }
  }
}

void ags_pad_resize_lines(AgsPad *pad, GType line_type,
			  guint audio_channels, guint audio_channels_old)
{
  g_return_if_fail(AGS_IS_PAD(pad));

  //  fprintf(stdout, "ags_pad_resize_lines: audio_channels = %u ; audio_channels_old = %u\n\0", audio_channels, audio_channels_old);

  g_object_ref((GObject *) pad);
  g_signal_emit(G_OBJECT(pad),
		pad_signals[RESIZE_LINES], 0,
		line_type,
		audio_channels, audio_channels_old);
  g_object_unref((GObject *) pad);
}

AgsPad*
ags_pad_new(AgsChannel *channel)
{
  AgsPad *pad;

  pad = (AgsPad *) g_object_new(AGS_TYPE_PAD, NULL);

  return(pad);
}

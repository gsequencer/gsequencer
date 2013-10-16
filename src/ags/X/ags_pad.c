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
#include <ags/object/ags_marshal.h>

#include <ags/X/ags_machine.h>

void ags_pad_class_init(AgsPadClass *pad);
void ags_pad_connectable_interface_init(AgsConnectableInterface *connectable);
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

    ags_type_pad = g_type_register_static(GTK_TYPE_VBOX,
					  "AgsPad\0", &ags_pad_info,
					  0);

    g_type_add_interface_static(ags_type_pad,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
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
  connectable->connect = ags_pad_connect;
  connectable->disconnect = ags_pad_disconnect;
}

void
ags_pad_init(AgsPad *pad)
{
  GtkMenu *menu;
  GtkHBox *hbox;

  g_signal_connect((GObject *) pad, "parent_set\0",
		   G_CALLBACK(ags_pad_parent_set_callback), (gpointer) pad);

  pad->flags = 0;

  pad->option = (GtkOptionMenu *) gtk_option_menu_new();
  menu = (GtkMenu *) gtk_menu_new();

  gtk_option_menu_set_menu(pad->option, (GtkWidget *) menu);
  gtk_box_pack_start((GtkBox *) pad, (GtkWidget *) pad->option, FALSE, FALSE, 0);

  hbox = (GtkHBox *) gtk_hbox_new(TRUE, 0);
  gtk_box_pack_start((GtkBox *) pad, (GtkWidget *) hbox, FALSE, FALSE, 0);

  pad->group = (GtkToggleButton *) gtk_toggle_button_new_with_label(g_strdup("G\0"));
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

  /* GtkOptionMenu */
  g_signal_connect((GObject *) pad->option, "changed\0",
		   G_CALLBACK(ags_pad_option_changed_callback), (gpointer) pad);

  /* GtkButton */
  g_signal_connect_after((GObject *) pad->group, "clicked\0",
			 G_CALLBACK(ags_pad_group_clicked_callback), (gpointer) pad);

  g_signal_connect_after((GObject *) pad->mute, "clicked\0",
			 G_CALLBACK(ags_pad_mute_clicked_callback), (gpointer) pad);

  g_signal_connect_after((GObject *) pad->solo, "clicked\0",
			 G_CALLBACK(ags_pad_solo_clicked_callback), (gpointer) pad);

  /* AgsLine */
  line_list = gtk_container_get_children(GTK_CONTAINER(pad->option->menu));

  while(line_list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(line_list->data));

    line_list = line_list->next;
  }
}

void
ags_pad_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_pad_destroy(GtkObject *object)
{
  /* empty */
}

void
ags_pad_show(GtkWidget *widget)
{
  AgsPad *pad;

  fprintf(stdout, "ags_pad_show\n\0");

  pad = AGS_PAD(widget);

  gtk_widget_show_all(gtk_option_menu_get_menu(pad->option));
}

void
ags_pad_real_set_channel(AgsPad *pad, AgsChannel *channel)
{
  pad->channel = channel;
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
  guint i;

  //  fprintf(stdout, "ags_pad_real_resize_lines: audio_channels = %u ; audio_channels_old = %u\n\0", audio_channels, audio_channels_old);

  if(audio_channels > audio_channels_old){
    machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) pad, AGS_TYPE_MACHINE);
    channel = ags_channel_nth(pad->channel, audio_channels_old);

    /* create AgsLine */
    for(i = audio_channels_old; i < audio_channels; i++){
      line = (AgsLine *) g_object_new(line_type,
				      "pad\0", pad,
				      "channel\0", channel,
				      NULL);
      channel->line_widget = (GtkWidget *) line;
      gtk_menu_shell_insert((GtkMenuShell *) pad->option->menu,
			    (GtkWidget *) line, i);

      channel = channel->next;
    }

    /* set selected AgsLine in AgsPad */
    if(audio_channels_old == 0){
      pad->selected_line = AGS_LINE(gtk_container_get_children((GtkContainer *) pad->option->menu)->data);
    }

    /* check if we should show and connect the AgsLine */
    if(machine != NULL && GTK_WIDGET_VISIBLE((GtkWidget *) machine)){
      GList *list;

      list = g_list_nth(gtk_container_get_children(GTK_CONTAINER(gtk_option_menu_get_menu(pad->option))),
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
      list = g_list_nth(gtk_container_get_children(GTK_CONTAINER(gtk_option_menu_get_menu(pad->option))),
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

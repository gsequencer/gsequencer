/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ags/X/ags_pad.h>
#include <ags/X/ags_pad_callbacks.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_marshal.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_soundcard.h>
#include <ags/object/ags_plugin.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_task_thread.h>
#include <ags/thread/ags_task_completion.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_playback.h>
#include <ags/audio/ags_pattern.h>
#include <ags/audio/ags_recall.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_soundcard_thread.h>

#include <ags/audio/task/ags_start_soundcard.h>
#include <ags/audio/task/ags_init_channel.h>
#include <ags/audio/task/ags_append_channel.h>
#include <ags/audio/task/ags_append_recall.h>
#include <ags/audio/task/ags_add_audio_signal.h>
#include <ags/audio/task/ags_open_single_file.h>
#include <ags/audio/task/ags_cancel_channel.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>

#include <ags/X/thread/ags_gui_thread.h>

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
gchar* ags_pad_get_version(AgsPlugin *plugin);
void ags_pad_set_version(AgsPlugin *plugin, gchar *version);
gchar* ags_pad_get_build_id(AgsPlugin *plugin);
void ags_pad_set_build_id(AgsPlugin *plugin, gchar *build_id);

void ags_pad_real_set_channel(AgsPad *pad, AgsChannel *channel);
void ags_pad_real_resize_lines(AgsPad *pad, GType line_type,
			       guint audio_channels, guint audio_channels_old);
void ags_pad_real_map_recall(AgsPad *pad,
			     guint output_pad_start);
GList* ags_pad_real_find_port(AgsPad *pad);

/**
 * SECTION:ags_pad
 * @short_description: A composite widget to visualize a bunch of #AgsChannel
 * @title: AgsPad
 * @section_id:
 * @include: ags/X/ags_pad.h
 *
 * #AgsPad is a composite widget to visualize a bunch of #AgsChannel. It should be
 * packed by an #AgsMachine.
 */

enum{
  SET_CHANNEL,
  RESIZE_LINES,
  MAP_RECALL,
  FIND_PORT,
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

  /* properties */
  //TODO:JK: add finalize
  /**
   * AgsPad:channel:
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

  /* AgsPadClass */
  pad->set_channel = ags_pad_real_set_channel;
  pad->resize_lines = ags_pad_real_resize_lines;
  pad->map_recall = ags_pad_real_map_recall;
  pad->find_port = ags_pad_real_find_port;

  /* signals */
  /**
   * AgsPad::set-channel:
   * @pad: the #AgsPad to modify
   * @channel: the #AgsChannel to set
   *
   * The ::set-channel signal notifies about changed channel.
   */
  pad_signals[SET_CHANNEL] =
    g_signal_new("set-channel\0",
		 G_TYPE_FROM_CLASS(pad),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsPadClass, set_channel),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);

  /**
   * AgsPad::resize-lines:
   * @pad: the #AgsPad to resize
   * @line_type: the channel type
   * @audio_channels: count of lines
   * @audio_channels_old: old count of lines
   *
   * The ::resize-lines is emitted as count of lines pack is modified.
   */
  pad_signals[RESIZE_LINES] =
    g_signal_new("resize-lines\0",
		 G_TYPE_FROM_CLASS(pad),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsPadClass, resize_lines),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__ULONG_UINT_UINT,
		 G_TYPE_NONE, 3,
		 G_TYPE_ULONG, G_TYPE_UINT, G_TYPE_UINT);

  
  /**
   * AgsPad::map-recall:
   * @pad: the #AgsPad to resize
   * @output_pad_start: start of output pad
   *
   * The ::map-recall as recall should be mapped
   */
  pad_signals[MAP_RECALL] =
    g_signal_new("map-recall\0",
		 G_TYPE_FROM_CLASS(pad),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsPadClass, map_recall),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);

  /**
   * AgsPad::find-port:
   * @pad: the #AgsPad to resize
   * Returns: a #GList with associated ports
   *
   * The ::find-port retrieves all associated ports
   */
  pad_signals[FIND_PORT] =
    g_signal_new("find-port\0",
		 G_TYPE_FROM_CLASS(pad),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsPadClass, find_port),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__VOID,
		 G_TYPE_POINTER, 0);
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
  plugin->get_name = NULL;
  plugin->set_name = NULL;
  plugin->get_version = ags_pad_get_version;
  plugin->set_version = ags_pad_set_version;
  plugin->get_build_id = ags_pad_get_build_id;
  plugin->set_build_id = ags_pad_set_build_id;
  plugin->get_xml_type = NULL;
  plugin->set_xml_type = NULL;
  plugin->get_ports = NULL;
  plugin->read = NULL;
  plugin->write = NULL;
  plugin->set_ports = NULL;
}

void
ags_pad_init(AgsPad *pad)
{
  GtkMenu *menu;
  GtkHBox *hbox;

  pad->flags = 0;

  pad->name = NULL;

  pad->version = AGS_VERSION;
  pad->build_id = AGS_BUILD_ID;

  pad->cols = 2;

  pad->expander_set = ags_expander_set_new(1, 1);
  gtk_box_pack_start((GtkBox *) pad, (GtkWidget *) pad->expander_set, TRUE, TRUE, 0);

  hbox = (GtkHBox *) gtk_hbox_new(TRUE, 0);
  gtk_box_pack_start((GtkBox *) pad, (GtkWidget *) hbox, FALSE, FALSE, 0);

  pad->group = (GtkToggleButton *) gtk_toggle_button_new_with_label("G\0");
  gtk_toggle_button_set_active(pad->group, TRUE);
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) pad->group, FALSE, FALSE, 0);

  pad->mute = (GtkToggleButton *) gtk_toggle_button_new_with_label("M\0");
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) pad->mute, FALSE, FALSE, 0);

  pad->solo = (GtkToggleButton *) gtk_toggle_button_new_with_label("S\0");
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) pad->solo, FALSE, FALSE, 0);

  pad->play = NULL;
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
  GList *line_list, *line_list_start;

  /* AgsPad */
  pad = AGS_PAD(connectable);

  if((AGS_PAD_CONNECTED & (pad->flags)) != 0){
    return;
  }
  
  pad->flags |= AGS_PAD_CONNECTED;

  if((AGS_PAD_PREMAPPED_RECALL & (pad->flags)) == 0){
    if((AGS_PAD_MAPPED_RECALL & (pad->flags)) == 0){
      ags_pad_map_recall(pad,
			 0);
    }
  }else{
    pad->flags &= (~AGS_PAD_PREMAPPED_RECALL);

    ags_pad_find_port(pad);
  }

  /* GtkButton */
  g_signal_connect_after((GObject *) pad->group, "clicked\0",
			 G_CALLBACK(ags_pad_group_clicked_callback), (gpointer) pad);

  g_signal_connect_after((GObject *) pad->mute, "clicked\0",
			 G_CALLBACK(ags_pad_mute_clicked_callback), (gpointer) pad);

  g_signal_connect_after((GObject *) pad->solo, "clicked\0",
			 G_CALLBACK(ags_pad_solo_clicked_callback), (gpointer) pad);

  /* AgsLine */
  line_list_start =  
    line_list = gtk_container_get_children(GTK_CONTAINER(pad->expander_set));

  while(line_list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(line_list->data));

    line_list = line_list->next;
  }

  g_list_free(line_list_start);
}

void
ags_pad_disconnect(AgsConnectable *connectable)
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
  AgsPad *pad;

  pad = AGS_PAD(plugin);

  pad->version = version;
}

gchar*
ags_pad_get_build_id(AgsPlugin *plugin)
{
  return(AGS_PAD(plugin)->build_id);
}

void
ags_pad_set_build_id(AgsPlugin *plugin, gchar *build_id)
{
  AgsPad *pad;

  pad = AGS_PAD(plugin);

  pad->build_id = build_id;
}

void
ags_pad_real_set_channel(AgsPad *pad, AgsChannel *channel)
{
  AgsChannel *current;

  AgsMutexManager *mutex_manager;

  GList *line, *line_start;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *current_mutex;

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

  line_start = 
    line = gtk_container_get_children(GTK_CONTAINER(AGS_PAD(pad)->expander_set));
  current = channel;

  /* get mutex manager */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* set channel */
  while(line != NULL){
    if(current != NULL){
      /* lookup current mutex */
      pthread_mutex_lock(application_mutex);
      
      current_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) current);
  
      pthread_mutex_unlock(application_mutex);
    }
    
    g_object_set(G_OBJECT(line->data),
		 "channel\0", current,
		 NULL);

    /* iterate */
    if(current != NULL){
      pthread_mutex_lock(current_mutex);

      current = current->next;

      pthread_mutex_unlock(current_mutex);
    }
    
    line = line->next;
  }

  g_list_free(line_start);
}

/**
 * ags_pad_set_channel:
 * @pad: an #AgsPad
 * @channel: the #AgsChannel to set
 *
 * Is emitted as channel gets modified.
 *
 * Since: 0.3
 */
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
  AgsLine *line;

  AgsChannel *channel;

  AgsMutexManager *mutex_manager;

  guint i, j;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *channel_mutex;

#ifdef AGS_DEBUG
  g_message("ags_pad_real_resize_lines: audio_channels = %u ; audio_channels_old = %u\n\0", audio_channels, audio_channels_old);
#endif
  
  /* get mutex manager */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);  

  /* resize */
  if(audio_channels > audio_channels_old){
    channel = ags_channel_nth(pad->channel, audio_channels_old);

    /* create AgsLine */
    for(i = audio_channels_old; i < audio_channels;){
      for(j = audio_channels_old % pad->cols; j < pad->cols && i < audio_channels; j++, i++){
	/* lookup channel mutex */
	if(channel != NULL){
	  pthread_mutex_lock(application_mutex);
      
	  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
						   (GObject *) channel);
  
	  pthread_mutex_unlock(application_mutex);
	}

	/* instantiate line */
	line = (AgsLine *) g_object_new(line_type,
					"pad\0", pad,
					"channel\0", channel,
					NULL);

	if(channel != NULL){
	  channel->line_widget = (GObject *) line;
	}

	ags_expander_set_add(pad->expander_set,
			     (GtkWidget *) line,
			     j, floor(i / pad->cols),
			     1, 1);
	
	/* iterate */
	if(channel != NULL){
	  pthread_mutex_lock(channel_mutex);

	  channel = channel->next;

	  pthread_mutex_unlock(channel_mutex);
	}
      }
    }
  }else if(audio_channels < audio_channels_old){
    GList *list, *list_start;

    list_start =
      list = g_list_nth(g_list_reverse(gtk_container_get_children(GTK_CONTAINER(pad->expander_set))),
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

    g_list_free(list_start);
  }
}

/**
 * ags_pad_resize_lines:
 * @pad: the #AgsPad to resize
 * @line_type: channel type, either %AGS_TYPE_INPUT or %AGS_TYPE_OUTPUT
 * @audio_channels: count of lines
 * @audio_channels_old: old count of lines
 *
 * Resize the count of #AgsLine packe by #AgsPad.
 *
 * Since: 0.3
 */
void
ags_pad_resize_lines(AgsPad *pad, GType line_type,
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

void
ags_pad_real_map_recall(AgsPad *pad, guint output_pad_start)
{
  if((AGS_PAD_MAPPED_RECALL & (pad->flags)) != 0){
    return;
  }
  
  pad->flags |= AGS_PAD_MAPPED_RECALL;

  ags_pad_find_port(pad);
}

/**
 * ags_pad_map_recall:
 * @pad: the #AgsPad to resize
 * @output_pad_start: start of output pad
 *
 * Start of output pad
 *
 * Since: 0.4
 */
void
ags_pad_map_recall(AgsPad *pad, guint output_pad_start)
{
  g_return_if_fail(AGS_IS_PAD(pad));

  g_object_ref((GObject *) pad);
  g_signal_emit(G_OBJECT(pad),
		pad_signals[MAP_RECALL], 0,
		output_pad_start);
  g_object_unref((GObject *) pad);
}

GList*
ags_pad_real_find_port(AgsPad *pad)
{
  GList *line;
  
  GList *port, *tmp_port;

  port = NULL;

  /* find output ports */
  if(pad->expander_set != NULL){
    line = gtk_container_get_children((GtkContainer *) pad->expander_set);

    while(line != NULL){
      tmp_port = ags_line_find_port(AGS_LINE(line->data));
      
      if(port != NULL){
	port = g_list_concat(port,
			     tmp_port);
      }else{
	port = tmp_port;
      }

      line = line->next;
    }
  }

  return(port);
}

/**
 * ags_pad_find_port:
 * @pad: an #AgsPad
 *
 * Lookup ports of assigned recalls.
 *
 * Returns: an #GList containing all related #AgsPort
 *
 * Since: 0.4
 */
GList*
ags_pad_find_port(AgsPad *pad)
{
  GList *list;

  list = NULL;
  g_return_val_if_fail(AGS_IS_PAD(pad),
		       NULL);

  g_object_ref((GObject *) pad);
  g_signal_emit((GObject *) pad,
		pad_signals[FIND_PORT], 0,
		&list);
  g_object_unref((GObject *) pad);

  return(list);
}

void
ags_pad_play(AgsPad *pad)
{
  AgsWindow *window;
  AgsMachine *machine;

  AgsChannel *channel;

  AgsStartSoundcard *start_soundcard;
  AgsInitChannel *init_channel;
  AgsAppendChannel *append_channel;

  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;
  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;
  
  GList *tasks;

  gboolean no_soundcard;
  gboolean play_all;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *channel_mutex;
  
  machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) pad,
						   AGS_TYPE_MACHINE);
  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) machine);
  
  application_context = (AgsApplicationContext *) window->application_context;
  
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  no_soundcard = FALSE;
  
  pthread_mutex_lock(application_mutex);

  if(ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context)) == NULL){
    no_soundcard = TRUE;
  }

  pthread_mutex_unlock(application_mutex);

  if(no_soundcard){
    g_message("No soundcard available\0");
    
    return;
  }

  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

  pthread_mutex_unlock(application_mutex);

  /* get task */
  task_thread = (AgsTaskThread *) ags_thread_find_type(main_loop,
						       AGS_TYPE_TASK_THREAD);

  /*  */
  tasks = NULL;

  play_all = pad->group->active;

  channel = pad->channel;

  /* lookup channel mutex */
  pthread_mutex_lock(application_mutex);

  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) channel);
  
  pthread_mutex_unlock(application_mutex);

  if(pad->play->active){
    /* init channel for playback */
    init_channel = ags_init_channel_new(channel, play_all,
					TRUE, FALSE, FALSE);
    g_signal_connect_after(G_OBJECT(init_channel), "launch\0",
			   G_CALLBACK(ags_pad_init_channel_launch_callback), pad);
    tasks = g_list_prepend(tasks, init_channel);

    if(play_all){
      AgsChannel *next_pad;

      pthread_mutex_lock(channel_mutex);

      next_pad = channel->next_pad;
      
      pthread_mutex_unlock(channel_mutex);

      while(channel != next_pad){
	/* lookup channel mutex */
	pthread_mutex_lock(application_mutex);

	channel_mutex = ags_mutex_manager_lookup(mutex_manager,
						 (GObject *) channel);
  
	pthread_mutex_unlock(application_mutex);

	/* append channel for playback */
	append_channel = ags_append_channel_new((GObject *) main_loop,
						(GObject *) channel);
	tasks = g_list_prepend(tasks, append_channel);

	/* iterate */
	pthread_mutex_lock(channel_mutex);

	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }
    }else{
      AgsLine *line;
      GList *list;

      list = gtk_container_get_children(GTK_CONTAINER(pad->expander_set));
      line = AGS_LINE(ags_line_find_next_grouped(list)->data);

      /* append channel for playback */
      append_channel = ags_append_channel_new((GObject *) main_loop,
					      (GObject *) line->channel);
      tasks = g_list_prepend(tasks, append_channel);

      g_list_free(list);
    }

    /* create start task */
    if(tasks != NULL){
      AgsGuiThread *gui_thread;
      AgsTaskCompletion *task_completion;
      
      gui_thread = (AgsGuiThread *) ags_thread_find_type(main_loop,
							 AGS_TYPE_GUI_THREAD);

      start_soundcard = ags_start_soundcard_new(application_context);
      tasks = g_list_prepend(tasks, start_soundcard);

      task_completion = ags_task_completion_new((GObject *) start_soundcard,
						NULL);
      g_signal_connect_after(G_OBJECT(task_completion), "complete\0",
			     G_CALLBACK(ags_pad_start_complete_callback), pad);
      ags_connectable_connect(AGS_CONNECTABLE(task_completion));
      
      pthread_mutex_lock(gui_thread->task_completion_mutex);

      g_atomic_pointer_set(&(gui_thread->task_completion),
			   g_list_prepend(g_atomic_pointer_get(&(gui_thread->task_completion)),
					  task_completion));

      pthread_mutex_unlock(gui_thread->task_completion_mutex);

      /* append AgsStartSoundcard */
      tasks = g_list_reverse(tasks);

      ags_task_thread_append_tasks((AgsTaskThread *) task_thread,
				   tasks);
    }
  }else{
    AgsPlayback *playback;
    AgsRecallID *recall_id;
    
    AgsCancelChannel *cancel_channel;

    guint flags;
    
    channel = pad->channel;

    /* lookup channel mutex */
    pthread_mutex_lock(application_mutex);

    channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) channel);
  
    pthread_mutex_unlock(application_mutex);

    /* return if not playing */
    pthread_mutex_lock(channel_mutex);

    playback = (AgsPlayback *) channel->playback;
    flags = g_atomic_int_get(&(playback->flags));

    recall_id = playback->recall_id[0];

    pthread_mutex_unlock(channel_mutex);
    
    if(recall_id == NULL ||
       (AGS_PLAYBACK_DONE & (flags)) != 0){
      return;
    }

    if((AGS_PLAYBACK_PAD & (flags)) != 0){
      AgsChannel *next_pad;

      pthread_mutex_lock(channel_mutex);

      next_pad = channel->next_pad;

      pthread_mutex_unlock(channel_mutex);

      if((AGS_PLAYBACK_DONE & (flags)) == 0){
	/* cancel request */
	while(channel != next_pad){
	  /* lookup channel mutex */
	  pthread_mutex_lock(application_mutex);

	  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
						   (GObject *) channel);
  
	  pthread_mutex_unlock(application_mutex);

	  /* create cancel task */
	  pthread_mutex_lock(channel_mutex);

	  playback = (AgsPlayback *) channel->playback;
	  recall_id = playback->recall_id[0];

	  pthread_mutex_unlock(channel_mutex);

	  cancel_channel = ags_cancel_channel_new(channel, recall_id,
						  (GObject *) playback);

	  ags_task_thread_append_task(task_thread, (AgsTask *) cancel_channel);

	  channel = channel->next;
	}
      }else{
	/* done */
	while(channel != next_pad){
	  /* lookup channel mutex */
	  pthread_mutex_lock(application_mutex);

	  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
						   (GObject *) channel);
  
	  pthread_mutex_unlock(application_mutex);

	  /* set flags */
	  pthread_mutex_lock(channel_mutex);
	  
	  g_atomic_int_or(&(playback->flags),
			  AGS_PLAYBACK_REMOVE);
	  g_atomic_int_and(&(AGS_PLAYBACK(channel->playback)->flags),
			   (~AGS_PLAYBACK_DONE));

	  channel = channel->next;

	  pthread_mutex_unlock(channel_mutex);
	}
      }
    }else{
      AgsLine *line;
      GList *list;

      list = gtk_container_get_children(GTK_CONTAINER(pad->expander_set));
      line = AGS_LINE(ags_line_find_next_grouped(list)->data);

      g_list_free(list);

      /*  */
      channel = line->channel;

      /* lookup channel mutex */
      pthread_mutex_lock(application_mutex);

      channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) channel);
  
      pthread_mutex_unlock(application_mutex);

      /* create cancel task or set flags */
      pthread_mutex_lock(channel_mutex);

      playback = (AgsPlayback *) channel->playback;
      flags = g_atomic_int_get(&(playback->flags));

      recall_id = playback->recall_id[0];
	  
      pthread_mutex_unlock(channel_mutex);

      if((AGS_PLAYBACK_DONE & (flags)) == 0){
	/* cancel request */
	cancel_channel = ags_cancel_channel_new(channel, recall_id,
						(GObject *) playback);

	ags_task_thread_append_task(task_thread, (AgsTask *) cancel_channel);
      }else{
	/* done */
	pthread_mutex_lock(channel_mutex);
	
	AGS_PLAYBACK(channel->playback)->flags |= AGS_PLAYBACK_REMOVE;
	AGS_PLAYBACK(channel->playback)->flags &= (~AGS_PLAYBACK_DONE);

	pthread_mutex_unlock(channel_mutex);
      }
    }
  }
}

/**
 * ags_pad_new:
 * @channel: the bunch of channel to visualize
 *
 * Creates an #AgsPad
 *
 * Returns: a new #AgsPad
 *
 * Since: 0.3
 */
AgsPad*
ags_pad_new(AgsChannel *channel)
{
  AgsPad *pad;

  pad = (AgsPad *) g_object_new(AGS_TYPE_PAD, NULL);

  return(pad);
}

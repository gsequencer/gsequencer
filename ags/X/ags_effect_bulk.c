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

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_marshal.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_plugin.h>
#include <ags/object/ags_soundcard.h>

#ifdef AGS_USE_LINUX_THREADS
#include <ags/thread/ags_thread-kthreads.h>
#else
#include <ags/thread/ags_thread-posix.h>
#endif 
#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/plugin/ags_base_plugin.h>
#include <ags/plugin/ags_ladspa_manager.h>
#include <ags/plugin/ags_ladspa_plugin.h>
#include <ags/plugin/ags_dssi_manager.h>
#include <ags/plugin/ags_dssi_plugin.h>
#include <ags/plugin/ags_lv2_manager.h>
#include <ags/plugin/ags_lv2_plugin.h>
#include <ags/plugin/ags_ladspa_conversion.h>
#include <ags/plugin/ags_lv2_conversion.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_audio_run.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_recall_channel_run.h>
#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_recall_channel_run_dummy.h>
#include <ags/audio/ags_recall_recycling_dummy.h>
#include <ags/audio/ags_recall_ladspa.h>
#include <ags/audio/ags_recall_ladspa_run.h>
#include <ags/audio/ags_recall_dssi.h>
#include <ags/audio/ags_recall_dssi_run.h>
#include <ags/audio/ags_recall_lv2.h>
#include <ags/audio/ags_recall_lv2_run.h>

#include <ags/audio/task/ags_add_recall_container.h>
#include <ags/audio/task/ags_add_recall.h>

#include <ags/widget/ags_dial.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_plugin_browser.h>
#include <ags/X/ags_bulk_member.h>

#include <ags/X/task/ags_add_bulk_member.h>
#include <ags/X/task/ags_update_bulk_member.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <ladspa.h>
#include <dssi.h>
#include <lv2.h>

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
void ags_effect_bulk_show(GtkWidget *widget);

GList* ags_effect_bulk_add_ladspa_effect(AgsEffectBulk *effect_bulk,
					 GList *control_type_name,
					 gchar *filename,
					 gchar *effect);
GList* ags_effect_bulk_add_dssi_effect(AgsEffectBulk *effect_bulk,
				       GList *control_type_name,
				       gchar *filename,
				       gchar *effect);
GList* ags_effect_bulk_add_lv2_effect(AgsEffectBulk *effect_bulk,
				      GList *control_type_name,
				      gchar *filename,
				      gchar *effect);
GList* ags_effect_bulk_real_add_effect(AgsEffectBulk *effect_bulk,
				       GList *control_type_name,
				       gchar *filename,
				       gchar *effect);
void ags_effect_bulk_real_remove_effect(AgsEffectBulk *effect_bulk,
					guint nth);

void ags_effect_bulk_real_resize_audio_channels(AgsEffectBulk *effect_bulk,
						guint new_size,
						guint old_size);
void ags_effect_bulk_real_resize_pads(AgsEffectBulk *effect_bulk,
				      guint new_size,
				      guint old_size);
void ags_effect_bulk_real_map_recall(AgsEffectBulk *effect_bulk);
GList* ags_effect_bulk_real_find_port(AgsEffectBulk *effect_bulk);

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
  RESIZE_AUDIO_CHANNELS,
  RESIZE_PADS,
  MAP_RECALL,
  FIND_PORT,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_CHANNEL_TYPE,
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
  GtkWidgetClass *widget;
  GParamSpec *param_spec;

  ags_effect_bulk_parent_class = g_type_class_peek_parent(effect_bulk);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(effect_bulk);

  gobject->set_property = ags_effect_bulk_set_property;
  gobject->get_property = ags_effect_bulk_get_property;

  
  /* properties */
  /**
   * AgsEffectBulk:audio:
   *
   * The #AgsAudio to visualize.
   * 
   * Since: 0.4.3
   */
  param_spec = g_param_spec_object("audio\0",
				   "assigned audio\0",
				   "The audio it is assigned with\0",
				   AGS_TYPE_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);

  /**
   * AgsEffectBulk:channel-type:
   *
   * The target channel.
   * 
   * Since: 0.4.3
   */
  param_spec = g_param_spec_gtype("channel-type\0",
				  "assigned channel type\0",
				  "The channel type it is assigned with\0",
				  AGS_TYPE_CHANNEL,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL_TYPE,
				  param_spec);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) effect_bulk;

  widget->show = ags_effect_bulk_show;

  /* AgsEffectBulkClass */
  effect_bulk->add_effect = ags_effect_bulk_real_add_effect;
  effect_bulk->remove_effect = ags_effect_bulk_real_remove_effect;

  effect_bulk->resize_audio_channels = ags_effect_bulk_real_resize_audio_channels;
  effect_bulk->resize_pads = ags_effect_bulk_real_resize_pads;

  effect_bulk->map_recall = ags_effect_bulk_real_map_recall;
  effect_bulk->find_port = ags_effect_bulk_real_find_port;

  /* signals */
  /**
   * AgsEffectBulk::add-effect:
   * @effect_bulk: the #AgsEffectBulk to modify
   * @effect: the effect's name
   *
   * The ::add-effect signal notifies about added effect.
   */
  effect_bulk_signals[ADD_EFFECT] =
    g_signal_new("add-effect\0",
		 G_TYPE_FROM_CLASS(effect_bulk),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectBulkClass, add_effect),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__POINTER_STRING_STRING,
		 G_TYPE_POINTER, 3,
		 G_TYPE_POINTER,
		 G_TYPE_STRING,
		 G_TYPE_STRING);

  /**
   * AgsEffectBulk::remove-effect:
   * @effect_bulk: the #AgsEffectBulk to modify
   * @nth: the nth effect
   *
   * The ::remove-effect signal notifies about removed effect.
   */
  effect_bulk_signals[REMOVE_EFFECT] =
    g_signal_new("remove-effect\0",
		 G_TYPE_FROM_CLASS(effect_bulk),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectBulkClass, remove_effect),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);

  /**
   * AgsEffectBulk::resize-audio-channels:
   * @effect_bulk: the object to adjust the channels.
   * @new_size: new audio channel count
   * @old_size: old audio channel count
   *
   * The ::resize-audio-channels signal notifies about changes in channel
   * alignment.
   */
  effect_bulk_signals[RESIZE_AUDIO_CHANNELS] = 
    g_signal_new("resize-audio-channels\0",
		 G_TYPE_FROM_CLASS(effect_bulk),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectBulkClass, resize_audio_channels),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT, G_TYPE_UINT);

  /**
   * AgsEffectBulk::resize-pads:
   * @effect_bulk: the object to adjust the channels.
   * @new_size: new pad count
   * @old_size: old pad count
   *
   * The ::resize-pads signal notifies about changes in channel
   * alignment.
   */
  effect_bulk_signals[RESIZE_PADS] = 
    g_signal_new("resize_pads\0",
		 G_TYPE_FROM_CLASS(effect_bulk),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectBulkClass, resize_pads),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT, G_TYPE_UINT);

  /**
   * AgsEffectBulk::map-recall:
   * @effect_bulk: the #AgsEffectBulk
   *
   * The ::map-recall should be used to add the effect_bulk's default recall.
   */
  effect_bulk_signals[MAP_RECALL] =
    g_signal_new("map-recall\0",
                 G_TYPE_FROM_CLASS (effect_bulk),
                 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsEffectBulkClass, map_recall),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__UINT,
                 G_TYPE_NONE, 0);

  /**
   * AgsEffectBulk::find-port:
   * @effect_bulk: the #AgsEffectBulk to resize
   * Returns: a #GList with associated ports
   *
   * The ::find-port as recall should be mapped
   */
  effect_bulk_signals[FIND_PORT] =
    g_signal_new("find-port\0",
		 G_TYPE_FROM_CLASS(effect_bulk),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectBulkClass, find_port),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__VOID,
		 G_TYPE_POINTER, 0);
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

  effect_bulk->name = NULL;
  
  effect_bulk->version = AGS_EFFECT_BULK_DEFAULT_VERSION;
  effect_bulk->build_id = AGS_EFFECT_BULK_DEFAULT_BUILD_ID;

  effect_bulk->channel_type = G_TYPE_NONE;
  effect_bulk->audio = NULL;

  effect_bulk->plugin = NULL;

  alignment = (GtkAlignment *) g_object_new(GTK_TYPE_ALIGNMENT,
					    "xalign\0", 1.0,
					    NULL);
  gtk_box_pack_start((GtkBox *) effect_bulk,
		     (GtkWidget *) alignment,
		     FALSE, FALSE,
		     0);

  hbox = (GtkHBox *) gtk_hbox_new(FALSE,
				  0);
  gtk_widget_set_no_show_all((GtkWidget *) hbox,
			     TRUE);
  gtk_container_add((GtkContainer *) alignment,
		    (GtkWidget *) hbox);

  effect_bulk->add = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_ADD);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) effect_bulk->add,
		     FALSE, FALSE,
		     0);
  gtk_widget_show((GtkWidget *) effect_bulk->add);
  
  effect_bulk->remove = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_REMOVE);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) effect_bulk->remove,
		     FALSE, FALSE,
		     0);
  gtk_widget_show((GtkWidget *) effect_bulk->remove);
  
  hbox = (GtkHBox *) gtk_hbox_new(FALSE,
				  0);
  gtk_box_pack_start((GtkBox *) effect_bulk,
		     (GtkWidget *) hbox,
		     FALSE, FALSE,
		     0);

  effect_bulk->bulk_member = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_widget_set_no_show_all((GtkWidget *) effect_bulk->bulk_member,
			     TRUE);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) effect_bulk->bulk_member,
		     FALSE, FALSE,
		     0);

  effect_bulk->table = (GtkTable *) gtk_table_new(1, AGS_EFFECT_BULK_COLUMNS_COUNT,
						  FALSE);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) effect_bulk->table,
		     FALSE, FALSE,
		     0);

  effect_bulk->plugin_browser = (GtkDialog *) ags_plugin_browser_new((GtkWidget *) effect_bulk);
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
  case PROP_AUDIO:
    {
      AgsAudio *audio;

      audio = (AgsAudio *) g_value_get_object(value);

      if(effect_bulk->audio == audio){
	return;
      }

      if(effect_bulk->audio != NULL){
	if((AGS_EFFECT_BULK_CONNECTED & (effect_bulk->flags)) != 0){
	  //TODO:JK: implement me
	}

	if(effect_bulk->channel_type == AGS_TYPE_OUTPUT){
	  ags_effect_bulk_resize_pads(effect_bulk,
				      0,
				      effect_bulk->audio->output_pads);
	}else{
	  ags_effect_bulk_resize_pads(effect_bulk,
				      0,
				      effect_bulk->audio->input_pads);
	}
	
	g_object_unref(effect_bulk->audio);
      }

      if(audio != NULL){
	g_object_ref(audio);
      }

      effect_bulk->audio = audio;

      if(audio != NULL){
      	if((AGS_EFFECT_BULK_CONNECTED & (effect_bulk->flags)) != 0){
	  g_signal_connect_after(effect_bulk->audio, "set-audio-channels\0",
				 G_CALLBACK(ags_effect_bulk_set_audio_channels_callback), effect_bulk);

	  g_signal_connect_after(effect_bulk->audio, "set-pads\0",
				 G_CALLBACK(ags_effect_bulk_set_pads_callback), effect_bulk);
	  if(effect_bulk->channel_type == AGS_TYPE_OUTPUT){
	    ags_effect_bulk_resize_pads(effect_bulk,
					audio->output_pads,
					0);
	  }else{
	    ags_effect_bulk_resize_pads(effect_bulk,
					audio->input_pads,
					0);
	  }
	}
      }
    }
    break;
  case PROP_CHANNEL_TYPE:
    {
      GType channel_type;

      channel_type = (GType) g_value_get_gtype(value);

      effect_bulk->channel_type = channel_type;
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
  case PROP_AUDIO:
    {
      g_value_set_object(value,
			 effect_bulk->audio);
    }
    break;
  case PROP_CHANNEL_TYPE:
    {
      g_value_set_gtype(value,
			effect_bulk->channel_type);
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
  AgsEffectBulk *effect_bulk;

  GList *list, *list_start;
  
  effect_bulk = AGS_EFFECT_BULK(connectable);

  if((AGS_EFFECT_BULK_CONNECTED & (effect_bulk->flags)) != 0){
    return;
  }

  g_signal_connect(G_OBJECT(effect_bulk->add), "clicked\0",
		   G_CALLBACK(ags_effect_bulk_add_callback), effect_bulk);

  g_signal_connect(G_OBJECT(effect_bulk->remove), "clicked\0",
		   G_CALLBACK(ags_effect_bulk_remove_callback), effect_bulk);

  ags_connectable_connect(AGS_CONNECTABLE(effect_bulk->plugin_browser));

  g_signal_connect(G_OBJECT(effect_bulk->plugin_browser), "response\0",
		   G_CALLBACK(ags_effect_bulk_plugin_browser_response_callback), effect_bulk);

  if(effect_bulk->audio != NULL){
    g_signal_connect_after(effect_bulk->audio, "set-audio-channels\0",
			   G_CALLBACK(ags_effect_bulk_set_audio_channels_callback), effect_bulk);

    g_signal_connect_after(effect_bulk->audio, "set-pads\0",
			   G_CALLBACK(ags_effect_bulk_set_pads_callback), effect_bulk);
  }

  list =
    list_start = gtk_container_get_children((GtkContainer *) effect_bulk->table);

  while(list != NULL){
    if(AGS_IS_CONNECTABLE(list->data)){
      ags_connectable_connect(AGS_CONNECTABLE(list->data));
    }

    list = list->next;
  }

  g_list_free(list_start);
}

void
ags_effect_bulk_disconnect(AgsConnectable *connectable)
{
  AgsEffectBulk *effect_bulk;

  effect_bulk = AGS_EFFECT_BULK(connectable);

  if((AGS_EFFECT_BULK_CONNECTED & (effect_bulk->flags)) == 0){
    return;
  }

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

void
ags_effect_bulk_show(GtkWidget *widget)
{
  AgsEffectBulk *effect_bulk;
    
  effect_bulk = AGS_EFFECT_BULK(widget);
  
  GTK_WIDGET_CLASS(ags_effect_bulk_parent_class)->show(widget);

  if((AGS_EFFECT_BULK_HIDE_BUTTONS & (effect_bulk->flags)) == 0){
    gtk_widget_show(GTK_WIDGET(effect_bulk->add)->parent);
  }

  if((AGS_EFFECT_BULK_HIDE_ENTRIES & (effect_bulk->flags)) == 0){
    gtk_widget_show((GtkWidget *) effect_bulk->bulk_member);
  }
}

AgsEffectBulkPlugin*
ags_effect_bulk_plugin_alloc(gchar *filename,
			     gchar *effect)
{
  AgsEffectBulkPlugin *effect_plugin;

  effect_plugin = (AgsEffectBulkPlugin *) malloc(sizeof(AgsEffectBulkPlugin));

  effect_plugin->filename = filename;
  effect_plugin->effect = effect;

  effect_plugin->control_type_name = NULL;
  
  return(effect_plugin);
}

GList*
ags_effect_bulk_add_ladspa_effect(AgsEffectBulk *effect_bulk,
				  GList *control_type_name,
				  gchar *filename,
				  gchar *effect)
{
  AgsWindow *window;
  AgsBulkMember *bulk_member;

  GtkAdjustment *adjustment;
  AgsEffectBulkPlugin *effect_bulk_plugin;

  GObject *soundcard;
  AgsChannel *current;
  AgsRecallContainer *recall_container;
  AgsRecallChannelRunDummy *recall_channel_run_dummy;
  AgsRecallLadspa *recall_ladspa;

  AgsAddRecallContainer *add_recall_container;
  AgsAddRecall *add_recall;

  AgsLadspaPlugin *ladspa_plugin;
  
  AgsThread *main_loop;
  AgsTaskThread *task_thread;
  AgsMutexManager *mutex_manager;

  AgsApplicationContext *application_context;
  
  GList *retport;
  GList *port, *recall_port;
  GList *list;
  GList *task;
  GList *port_descriptor;

  guint pads, audio_channels;
  gdouble step;
  guint port_count;
  guint x, y;
  guint i, j;
  guint k;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;

  /* get window and application context */
  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget * )effect_bulk,
						 AGS_TYPE_WINDOW);
  
  application_context = (AgsApplicationContext *) window->application_context;

  /* get mutex manager and application mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* alloc effect bulk plugin */
  effect_bulk_plugin = ags_effect_bulk_plugin_alloc(filename,
						    effect);
  effect_bulk_plugin->control_type_name = control_type_name;
  
  effect_bulk->plugin = g_list_append(effect_bulk->plugin,
				      effect_bulk_plugin);  

  /* get main loop and task thread */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

  pthread_mutex_unlock(application_mutex);

  task_thread = (AgsTaskThread *) ags_thread_find_type(main_loop,
						       AGS_TYPE_TASK_THREAD);

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) effect_bulk->audio);
  
  pthread_mutex_unlock(application_mutex);

  /* get audio properties */
  pthread_mutex_lock(audio_mutex);

  soundcard = effect_bulk->audio->soundcard;
  
  audio_channels = effect_bulk->audio->audio_channels;

  if(effect_bulk->channel_type == AGS_TYPE_OUTPUT){
    current = effect_bulk->audio->output;
    
    pads = effect_bulk->audio->output_pads;
  }else{
    current = effect_bulk->audio->input;

    pads = effect_bulk->audio->input_pads;
  }

  pthread_mutex_unlock(audio_mutex);

  /* load plugin */
  ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(ags_ladspa_manager_get_instance(),
							filename, effect);

  task = NULL;
  retport = NULL;
  
  for(i = 0; i < pads; i++){
    for(j = 0; j < audio_channels; j++){
      /* get channel mutex */
      pthread_mutex_lock(application_mutex);

      channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) current);
  
      pthread_mutex_unlock(application_mutex);

      /* ladspa play */
      recall_container = ags_recall_container_new();
      ags_audio_add_recall_container(effect_bulk->audio,
				     (GObject *) recall_container);

      recall_ladspa = ags_recall_ladspa_new(current,
					    filename,
					    effect,
					    AGS_BASE_PLUGIN(ladspa_plugin)->effect_index);
      g_object_set(G_OBJECT(recall_ladspa),
		   "soundcard\0", soundcard,
		   "recall-container\0", recall_container,
		   NULL);
      AGS_RECALL(recall_ladspa)->flags |= (AGS_RECALL_TEMPLATE |
					   AGS_RECALL_INPUT_ORIENTATED |
					   AGS_RECALL_PLAYBACK |
					   AGS_RECALL_SEQUENCER |
					   AGS_RECALL_NOTATION |
					   AGS_RECALL_BULK_MODE);
      ags_recall_ladspa_load(recall_ladspa);

      port = ags_recall_ladspa_load_ports(recall_ladspa);

      if(retport == NULL){
	retport = port;
      }else{
	retport = g_list_concat(retport,
				port);
      }
      
      ags_channel_add_recall(current,
			     (GObject *) recall_ladspa,
			     TRUE);
      ags_connectable_connect(AGS_CONNECTABLE(recall_ladspa));

      /* dummy */
      recall_channel_run_dummy = ags_recall_channel_run_dummy_new(current,
								  AGS_TYPE_RECALL_RECYCLING_DUMMY,
								  AGS_TYPE_RECALL_LADSPA_RUN);
      AGS_RECALL(recall_channel_run_dummy)->flags |= AGS_RECALL_TEMPLATE;
      g_object_set(G_OBJECT(recall_channel_run_dummy),
		   "soundcard\0", soundcard,
		   "recall-container\0", recall_container,
		   "recall-channel\0", recall_ladspa,
		   NULL);
      AGS_RECALL(recall_channel_run_dummy)->flags |= (AGS_RECALL_TEMPLATE |
						      AGS_RECALL_INPUT_ORIENTATED |
						      AGS_RECALL_PLAYBACK |
						      AGS_RECALL_SEQUENCER |
						      AGS_RECALL_NOTATION |
						      AGS_RECALL_BULK_MODE);
      ags_channel_add_recall(current,
			     (GObject *) recall_channel_run_dummy,
			     FALSE);
      ags_connectable_connect(AGS_CONNECTABLE(recall_channel_run_dummy));

      /* ladspa recall */
      recall_container = ags_recall_container_new();
      ags_audio_add_recall_container(effect_bulk->audio,
				     (GObject *) recall_container);

      recall_ladspa = ags_recall_ladspa_new(current,
					    filename,
					    effect,
					    AGS_BASE_PLUGIN(ladspa_plugin)->effect_index);
      g_object_set(G_OBJECT(recall_ladspa),
		   "soundcard\0", soundcard,
		   "recall-container\0", recall_container,
		   NULL);
      AGS_RECALL(recall_ladspa)->flags |= (AGS_RECALL_TEMPLATE |
					   AGS_RECALL_INPUT_ORIENTATED |
					   AGS_RECALL_PLAYBACK |
					   AGS_RECALL_SEQUENCER |
					   AGS_RECALL_NOTATION |
					   AGS_RECALL_BULK_MODE);
      ags_recall_ladspa_load(recall_ladspa);

      recall_port = ags_recall_ladspa_load_ports(recall_ladspa);

      if(retport == NULL){
	retport = recall_port;
      }else{
	retport = g_list_concat(retport,
				recall_port);
      }

      ags_channel_add_recall(current,
			     (GObject *) recall_ladspa,
			     FALSE);
      ags_connectable_connect(AGS_CONNECTABLE(recall_ladspa));

      /* dummy */
      recall_channel_run_dummy = ags_recall_channel_run_dummy_new(current,
								  AGS_TYPE_RECALL_RECYCLING_DUMMY,
								  AGS_TYPE_RECALL_LADSPA_RUN);
      AGS_RECALL(recall_channel_run_dummy)->flags |= AGS_RECALL_TEMPLATE;
      g_object_set(G_OBJECT(recall_channel_run_dummy),
		   "soundcard\0", soundcard,
		   "recall-container\0", recall_container,
		   "recall-channel\0", recall_ladspa,
		   NULL);
      AGS_RECALL(recall_channel_run_dummy)->flags |= (AGS_RECALL_TEMPLATE |
						      AGS_RECALL_INPUT_ORIENTATED |
						      AGS_RECALL_PLAYBACK |
						      AGS_RECALL_SEQUENCER |
						      AGS_RECALL_NOTATION |
						      AGS_RECALL_BULK_MODE);
      ags_channel_add_recall(current,
			     (GObject *) recall_channel_run_dummy,
			     FALSE);
      ags_connectable_connect(AGS_CONNECTABLE(recall_channel_run_dummy));

      /* iterate */
      pthread_mutex_lock(channel_mutex);
      
      current = current->next;

      pthread_mutex_unlock(channel_mutex);
    }
  }

  /* retrieve position within table  */
  x = 0;
  y = 0;
  
  list = effect_bulk->table->children;

  while(list != NULL){
    if(y <= ((GtkTableChild *) list->data)->top_attach){
      y = ((GtkTableChild *) list->data)->top_attach + 1;
    }

    list = list->next;
  }
  
  /* load ports */
  port_descriptor = AGS_BASE_PLUGIN(ladspa_plugin)->port;

  port_count = g_list_length(port_descriptor);
  k = 0;

  while(port_descriptor != NULL){
    if((AGS_PORT_DESCRIPTOR_CONTROL & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
      GtkWidget *child_widget;

      AgsLadspaConversion *ladspa_conversion;

      LADSPA_Data default_value;
      
      GType widget_type;

      guint step_count;
      
      if(x == AGS_EFFECT_BULK_COLUMNS_COUNT){
	x = 0;
	y++;
	gtk_table_resize(effect_bulk->table,
			 y + 1, AGS_EFFECT_BULK_COLUMNS_COUNT);
      }
      
      if((AGS_PORT_DESCRIPTOR_TOGGLED & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	widget_type = GTK_TYPE_TOGGLE_BUTTON;
      }else{
	widget_type = AGS_TYPE_DIAL;
      }
      
      step_count = AGS_DIAL_DEFAULT_PRECISION;

      if((AGS_PORT_DESCRIPTOR_INTEGER & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	step_count = AGS_PORT_DESCRIPTOR(port_descriptor->data)->scale_steps;
      }

      /* add bulk member */
      bulk_member = (AgsBulkMember *) g_object_new(AGS_TYPE_BULK_MEMBER,
						   "widget-type\0", widget_type,
						   "widget-label\0", AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_name,
						   "plugin-name\0", g_strdup_printf("ladspa-%u\0", ladspa_plugin->unique_id),
						   "filename\0", filename,
						   "effect\0", effect,
						   "specifier\0", g_strdup(AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_name),
						   "control-port\0", g_strdup_printf("%u/%u\0",
										     k,
										     port_count),
						   "steps\0", step_count,
						   NULL);
      child_widget = ags_bulk_member_get_widget(bulk_member);

      /* ladspa conversion */
      ladspa_conversion = NULL;

      if((AGS_PORT_DESCRIPTOR_BOUNDED_BELOW & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	if(ladspa_conversion == NULL ||
	   !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
	  ladspa_conversion = ags_ladspa_conversion_new();
	}

	ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_BOUNDED_BELOW;
      }

      if((AGS_PORT_DESCRIPTOR_BOUNDED_ABOVE & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	if(ladspa_conversion == NULL ||
	   !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
	  ladspa_conversion = ags_ladspa_conversion_new();
	}

	ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_BOUNDED_ABOVE;
      }
      if((AGS_PORT_DESCRIPTOR_SAMPLERATE & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	if(ladspa_conversion == NULL ||
	   !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
	  ladspa_conversion = ags_ladspa_conversion_new();
	}

	ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_SAMPLERATE;
      }

      if((AGS_PORT_DESCRIPTOR_LOGARITHMIC & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	if(ladspa_conversion == NULL ||
	   !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
	  ladspa_conversion = ags_ladspa_conversion_new();
	}
    
	ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_LOGARITHMIC;
      }

      bulk_member->conversion = (AgsConversion *) ladspa_conversion;
      
      /* child widget */
      if((AGS_PORT_DESCRIPTOR_TOGGLED & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	bulk_member->port_flags = AGS_BULK_MEMBER_PORT_BOOLEAN;
      }
      
      if((AGS_PORT_DESCRIPTOR_INTEGER & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	bulk_member->port_flags = AGS_BULK_MEMBER_PORT_INTEGER;
      }

      if(AGS_IS_DIAL(child_widget)){
	AgsDial *dial;

	GtkAdjustment *adjustment;

	LADSPA_Data lower_bound, upper_bound;
	
	dial = (AgsDial *) child_widget;

	/* add controls of ports and apply range  */
	lower_bound = g_value_get_float(AGS_PORT_DESCRIPTOR(port_descriptor->data)->lower_value);
	upper_bound = g_value_get_float(AGS_PORT_DESCRIPTOR(port_descriptor->data)->upper_value);
	
	adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.1, 0.0);
	g_object_set(dial,
		     "adjustment", adjustment,
		     NULL);

	if(upper_bound >= 0.0 && lower_bound >= 0.0){
	  step = (upper_bound - lower_bound) / step_count;
	}else if(upper_bound < 0.0 && lower_bound < 0.0){
	  step = -1.0 * (lower_bound - upper_bound) / step_count;
	}else{
	  step = (upper_bound - lower_bound) / step_count;
	}

	gtk_adjustment_set_step_increment(adjustment,
					  step);
	gtk_adjustment_set_lower(adjustment,
				 lower_bound);
	gtk_adjustment_set_upper(adjustment,
				 upper_bound);


	default_value = g_value_get_float(AGS_PORT_DESCRIPTOR(port_descriptor->data)->default_value);

	if(ladspa_conversion != NULL){
	  //	  default_value = ags_ladspa_conversion_convert(ladspa_conversion,
	  //						default_value,
	  //						TRUE);
	}
	
	gtk_adjustment_set_value(adjustment,
				 default_value);
      }

#ifdef AGS_DEBUG
      g_message("ladspa bounds: %f %f\0", lower_bound, upper_bound);
#endif

      gtk_table_attach(effect_bulk->table,
		       (GtkWidget *) bulk_member,
		       x, x + 1,
		       y, y + 1,
		       GTK_FILL, GTK_FILL,
		       0, 0);
      ags_connectable_connect(AGS_CONNECTABLE(bulk_member));
      gtk_widget_show_all((GtkWidget *) effect_bulk->table);

      x++;
    }

    port_descriptor = port_descriptor->next;
    k++;
  }

  /* launch tasks */
  task = g_list_reverse(task);      
  ags_task_thread_append_tasks(task_thread,
			       task);

  return(retport);
}

GList*
ags_effect_bulk_add_dssi_effect(AgsEffectBulk *effect_bulk,
				GList *control_type_name,
				gchar *filename,
				gchar *effect)
{
  AgsWindow *window;
  AgsBulkMember *bulk_member;
   
  GtkAdjustment *adjustment;
  AgsEffectBulkPlugin *effect_bulk_plugin;

  GObject *soundcard;
  AgsChannel *current;
  AgsRecallContainer *recall_container;
  AgsRecallChannelRunDummy *recall_channel_run_dummy;
  AgsRecallDssi *recall_dssi;

  AgsAddRecallContainer *add_recall_container;
  AgsAddRecall *add_recall;

  AgsDssiPlugin *dssi_plugin;
  
  AgsThread *main_loop;
  AgsTaskThread *task_thread;
  AgsMutexManager *mutex_manager;

  AgsApplicationContext *application_context;

  GList *retport;
  GList *port, *recall_port;
  GList *list;
  GList *task;
  GList *port_descriptor;

  guint pads, audio_channels;
  gdouble step;
  guint port_count;
  guint x, y;
  guint i, j;
  guint k;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;

  /* get window and application context */
  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) effect_bulk,
						 AGS_TYPE_WINDOW);
  
  application_context = (AgsApplicationContext *) window->application_context;

  /* get mutex manager and application mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* alloc effect bulk plugin */
  effect_bulk_plugin = ags_effect_bulk_plugin_alloc(filename,
						    effect);
  effect_bulk_plugin->control_type_name = control_type_name;
  
  effect_bulk->plugin = g_list_append(effect_bulk->plugin,
				      effect_bulk_plugin);  

  /* get main loop and task thread */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

  pthread_mutex_unlock(application_mutex);

  task_thread = (AgsTaskThread *) ags_thread_find_type(main_loop,
						       AGS_TYPE_TASK_THREAD);

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) effect_bulk->audio);
  
  pthread_mutex_unlock(application_mutex);

  /* get audio properties */
  pthread_mutex_lock(audio_mutex);

  soundcard = effect_bulk->audio->soundcard;
  
  audio_channels = effect_bulk->audio->audio_channels;

  if(effect_bulk->channel_type == AGS_TYPE_OUTPUT){
    current = effect_bulk->audio->output;
    
    pads = effect_bulk->audio->output_pads;
  }else{
    current = effect_bulk->audio->input;

    pads = effect_bulk->audio->input_pads;
  }

  pthread_mutex_unlock(audio_mutex);

  /* load plugin */
  dssi_plugin = ags_dssi_manager_find_dssi_plugin(ags_dssi_manager_get_instance(),
						  filename, effect);

  task = NULL;
  retport = NULL;
  
  for(i = 0; i < pads; i++){
    for(j = 0; j < audio_channels; j++){
      /* get channel mutex */
      pthread_mutex_lock(application_mutex);

      channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) current);
  
      pthread_mutex_unlock(application_mutex);

      /* dssi play */
      recall_container = ags_recall_container_new();
      ags_audio_add_recall_container(effect_bulk->audio,
				     (GObject *) recall_container);

      //      add_recall_container = ags_add_recall_container_new(current->audio,
      //						  recall_container);
      //      task = g_list_prepend(task,
      //		    add_recall_container);

      recall_dssi = ags_recall_dssi_new(current,
					filename,
					effect,
					AGS_BASE_PLUGIN(dssi_plugin)->effect_index);
      g_object_set(G_OBJECT(recall_dssi),
		   "soundcard\0", soundcard,
		   "source\0", current,
		   "recall-container\0", recall_container,
		   NULL);
      ags_recall_set_flags(AGS_RECALL(recall_dssi), (AGS_RECALL_TEMPLATE |
						     AGS_RECALL_INPUT_ORIENTATED |
						     AGS_RECALL_PLAYBACK |
						     AGS_RECALL_SEQUENCER |
						     AGS_RECALL_NOTATION |
						     AGS_RECALL_BULK_MODE));
      ags_recall_dssi_load(recall_dssi);

      port = ags_recall_dssi_load_ports(recall_dssi);

      if(retport == NULL){
	retport = port;
      }else{
	retport = g_list_concat(retport,
				port);
      }

      ags_channel_add_recall(current,
			     (GObject *) recall_dssi,
			     TRUE);
      ags_connectable_connect(AGS_CONNECTABLE(recall_dssi));

      /* dummy */
      recall_channel_run_dummy = ags_recall_channel_run_dummy_new(current,
								  AGS_TYPE_RECALL_RECYCLING_DUMMY,
								  AGS_TYPE_RECALL_DSSI_RUN);
      ags_recall_set_flags(AGS_RECALL(recall_channel_run_dummy), (AGS_RECALL_TEMPLATE |
								  AGS_RECALL_INPUT_ORIENTATED |
								  AGS_RECALL_PLAYBACK |
								  AGS_RECALL_SEQUENCER |
								  AGS_RECALL_NOTATION |
								  AGS_RECALL_BULK_MODE));
      g_object_set(G_OBJECT(recall_channel_run_dummy),
		   "soundcard\0", soundcard,
		   "source\0", current,
		   "recall-channel\0", recall_dssi,
		   "recall-container\0", recall_container,
		   NULL);
      ags_channel_add_recall(current,
			     (GObject *) recall_channel_run_dummy,
			     TRUE);
      ags_connectable_connect(AGS_CONNECTABLE(recall_channel_run_dummy));

      /* dssi recall */
      recall_container = ags_recall_container_new();
      ags_audio_add_recall_container(effect_bulk->audio,
				     (GObject *) recall_container);

      //      add_recall_container = ags_add_recall_container_new(current->audio,
      //						  recall_container);
      //      task = g_list_prepend(task,
      //		    add_recall_container);

      recall_dssi = ags_recall_dssi_new(current,
					filename,
					effect,
					AGS_BASE_PLUGIN(dssi_plugin)->effect_index);
      g_object_set(G_OBJECT(recall_dssi),
		   "soundcard\0", soundcard,
		   "source\0", current,
		   "recall-container\0", recall_container,
		   NULL);
      ags_recall_set_flags(AGS_RECALL(recall_dssi), (AGS_RECALL_TEMPLATE |
						     AGS_RECALL_INPUT_ORIENTATED |
						     AGS_RECALL_PLAYBACK |
						     AGS_RECALL_SEQUENCER |
						     AGS_RECALL_NOTATION |
						     AGS_RECALL_BULK_MODE));
      ags_recall_dssi_load(recall_dssi);

      recall_port = ags_recall_dssi_load_ports(recall_dssi);

      if(retport == NULL){
	retport = port;
      }else{
	retport = g_list_concat(retport,
				recall_port);
      }

      ags_channel_add_recall(current,
			     (GObject *) recall_dssi,
			     FALSE);
      ags_connectable_connect(AGS_CONNECTABLE(recall_dssi));

      /* dummy */
      recall_channel_run_dummy = ags_recall_channel_run_dummy_new(current,
								  AGS_TYPE_RECALL_RECYCLING_DUMMY,
								  AGS_TYPE_RECALL_DSSI_RUN);
      ags_recall_set_flags(AGS_RECALL(recall_channel_run_dummy), (AGS_RECALL_TEMPLATE |
								  AGS_RECALL_INPUT_ORIENTATED |
								  AGS_RECALL_PLAYBACK |
								  AGS_RECALL_SEQUENCER |
								  AGS_RECALL_NOTATION |
								  AGS_RECALL_BULK_MODE));
      g_object_set(G_OBJECT(recall_channel_run_dummy),
		   "soundcard\0", soundcard,
		   "source\0", current,
		   "recall-channel\0", recall_dssi,
		   "recall-container\0", recall_container,
		   NULL);
      ags_channel_add_recall(current,
			     (GObject *) recall_channel_run_dummy,
			     FALSE);
      ags_connectable_connect(AGS_CONNECTABLE(recall_channel_run_dummy));
      
      /* iterate */
      pthread_mutex_lock(channel_mutex);
      
      current = current->next;

      pthread_mutex_unlock(channel_mutex);
    }
  }

  /* retrieve position within table  */
  x = 0;
  y = 0;
  
  list = effect_bulk->table->children;

  while(list != NULL){
    if(y <= ((GtkTableChild *) list->data)->top_attach){
      y = ((GtkTableChild *) list->data)->top_attach + 1;
    }

    list = list->next;
  }
  
  /* load ports */
  port_descriptor = AGS_BASE_PLUGIN(dssi_plugin)->port;

  port_count = g_list_length(port_descriptor);
  k = 0;

  while(port_descriptor != NULL){
    if((AGS_PORT_DESCRIPTOR_CONTROL & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
      GtkWidget *child_widget;

      AgsLadspaConversion *ladspa_conversion;
      
      GType widget_type;

      guint step_count;
      
      if(x == AGS_EFFECT_BULK_COLUMNS_COUNT){
	x = 0;
	y++;
	gtk_table_resize(effect_bulk->table,
			 y + 1, AGS_EFFECT_BULK_COLUMNS_COUNT);
      }

      if((AGS_PORT_DESCRIPTOR_TOGGLED & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	widget_type = GTK_TYPE_TOGGLE_BUTTON;
      }else{
	widget_type = AGS_TYPE_DIAL;
      }

      step_count = AGS_DIAL_DEFAULT_PRECISION;

      if((AGS_PORT_DESCRIPTOR_INTEGER & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	step_count = AGS_PORT_DESCRIPTOR(port_descriptor->data)->scale_steps;
      }

      /* add bulk member */
      bulk_member = (AgsBulkMember *) g_object_new(AGS_TYPE_BULK_MEMBER,
						   "widget-type\0", widget_type,
						   "widget-label\0", AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_name,
						   "plugin-name\0", g_strdup_printf("dssi-%u\0", dssi_plugin->unique_id),
						   "filename\0", filename,
						   "effect\0", effect,
						   "specifier\0", AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_name,
						   "control-port\0", g_strdup_printf("%u/%u\0",
										     k,
										     port_count),
						   "steps\0", step_count,
						   NULL);
      child_widget = ags_bulk_member_get_widget(bulk_member);

      /* ladspa conversion */
      ladspa_conversion = NULL;

      if((AGS_PORT_DESCRIPTOR_BOUNDED_BELOW & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	if(ladspa_conversion == NULL ||
	   !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
	  ladspa_conversion = ags_ladspa_conversion_new();
	}

	ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_BOUNDED_BELOW;
      }

      if((AGS_PORT_DESCRIPTOR_BOUNDED_ABOVE & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	if(ladspa_conversion == NULL ||
	   !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
	  ladspa_conversion = ags_ladspa_conversion_new();
	}

	ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_BOUNDED_ABOVE;
      }
      
      if((AGS_PORT_DESCRIPTOR_SAMPLERATE & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	if(ladspa_conversion == NULL ||
	   !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
	  ladspa_conversion = ags_ladspa_conversion_new();
	}

	ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_SAMPLERATE;
      }

      if((AGS_PORT_DESCRIPTOR_LOGARITHMIC & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	if(ladspa_conversion == NULL ||
	   !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
	  ladspa_conversion = ags_ladspa_conversion_new();
	}
    
	ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_LOGARITHMIC;
      }

      bulk_member->conversion = (AgsConversion *) ladspa_conversion;

      /* child widget */
      if((AGS_PORT_DESCRIPTOR_TOGGLED & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	bulk_member->port_flags = AGS_BULK_MEMBER_PORT_BOOLEAN;
      }
      
      if((AGS_PORT_DESCRIPTOR_INTEGER & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	bulk_member->port_flags = AGS_BULK_MEMBER_PORT_INTEGER;
      }

      if(AGS_IS_DIAL(child_widget)){
	AgsDial *dial;
	GtkAdjustment *adjustment;

	LADSPA_Data lower_bound, upper_bound;
	LADSPA_Data default_value;
	
	dial = (AgsDial *) child_widget;

	/* add controls of ports and apply range  */
	lower_bound = g_value_get_float(AGS_PORT_DESCRIPTOR(port_descriptor->data)->lower_value);
	upper_bound = g_value_get_float(AGS_PORT_DESCRIPTOR(port_descriptor->data)->upper_value);

	adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.1, 0.0);
	g_object_set(dial,
		     "adjustment", adjustment,
		     NULL);

	if(upper_bound >= 0.0 && lower_bound >= 0.0){
	  step = (upper_bound - lower_bound) / step_count;
	}else if(upper_bound < 0.0 && lower_bound < 0.0){
	  step = -1.0 * (lower_bound - upper_bound) / step_count;
	}else{
	  step = (upper_bound - lower_bound) / step_count;
	}

	gtk_adjustment_set_step_increment(adjustment,
					  step);
	gtk_adjustment_set_lower(adjustment,
				 lower_bound);
	gtk_adjustment_set_upper(adjustment,
				 upper_bound);

	default_value = (LADSPA_Data) g_value_get_float(AGS_PORT_DESCRIPTOR(port_descriptor->data)->default_value);

	if(ladspa_conversion != NULL){
	  //	  default_value = ags_ladspa_conversion_convert(ladspa_conversion,
	  //						default_value,
	  //						TRUE);
	}
	
	gtk_adjustment_set_value(adjustment,
				 default_value);

#ifdef AGS_DEBUG
	g_message("dssi bounds: %f %f\0", lower_bound, upper_bound);
#endif
      }

      gtk_table_attach(effect_bulk->table,
		       (GtkWidget *) bulk_member,
		       x, x + 1,
		       y, y + 1,
		       GTK_FILL, GTK_FILL,
		       0, 0);
      ags_connectable_connect(AGS_CONNECTABLE(bulk_member));
      gtk_widget_show_all((GtkWidget *) effect_bulk->table);

      x++;
    }

    port_descriptor = port_descriptor->next;    
    k++;
  }

  /* launch tasks */
  task = g_list_reverse(task);      
  ags_task_thread_append_tasks(task_thread,
			       task);

  return(retport);
}

GList*
ags_effect_bulk_add_lv2_effect(AgsEffectBulk *effect_bulk,
			       GList *control_type_name,
			       gchar *filename,
			       gchar *effect)
{
  AgsWindow *window;
  AgsBulkMember *bulk_member;
  
  GtkAdjustment *adjustment;
  AgsEffectBulkPlugin *effect_bulk_plugin;

  GObject *soundcard;
  AgsChannel *current;
  AgsRecallContainer *recall_container;
  AgsRecallChannelRunDummy *recall_channel_run_dummy;
  AgsRecallLv2 *recall_lv2;

  AgsAddRecallContainer *add_recall_container;
  AgsAddRecall *add_recall;

  AgsLv2Plugin *lv2_plugin;
  
  AgsThread *main_loop;
  AgsTaskThread *task_thread;
  AgsMutexManager *mutex_manager;

  AgsApplicationContext *application_context;

  xmlNode *parent;

  GList *retport;
  GList *port, *recall_port;
  GList *list;
  GList *task;
  GList *port_descriptor;

  gchar *uri;
  gchar *port_name;
  gchar *str;

  gdouble step;
  guint pads, audio_channels;
  guint port_count;
  guint x, y;
  guint i, j;
  guint k;
  
  float lower_bound, upper_bound, default_bound;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;

  /* get window and application context */
  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) effect_bulk,
						 AGS_TYPE_WINDOW);
  
  application_context = (AgsApplicationContext *) window->application_context;

  /* get mutex manager and application mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* alloc effect bulk plugin */
  effect_bulk_plugin = ags_effect_bulk_plugin_alloc(filename,
						    effect);
  effect_bulk_plugin->control_type_name = control_type_name;
  
  effect_bulk->plugin = g_list_append(effect_bulk->plugin,
				      effect_bulk_plugin);  

  /* get main loop and task thread */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

  pthread_mutex_unlock(application_mutex);

  task_thread = (AgsTaskThread *) ags_thread_find_type(main_loop,
						       AGS_TYPE_TASK_THREAD);

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) effect_bulk->audio);
  
  pthread_mutex_unlock(application_mutex);

  /* get audio properties */
  pthread_mutex_lock(audio_mutex);
  
  soundcard = effect_bulk->audio->soundcard;
  
  audio_channels = effect_bulk->audio->audio_channels;

  if(effect_bulk->channel_type == AGS_TYPE_OUTPUT){
    current = effect_bulk->audio->output;
    
    pads = effect_bulk->audio->output_pads;
  }else{
    current = effect_bulk->audio->input;

    pads = effect_bulk->audio->input_pads;
  }

  pthread_mutex_unlock(audio_mutex);
  
  /* load plugin */
  lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
					       filename, effect);

  task = NULL;
  retport = NULL;
  
  for(i = 0; i < pads; i++){
    for(j = 0; j < audio_channels; j++){
      /* get channel mutex */
      pthread_mutex_lock(application_mutex);

      channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) current);
  
      pthread_mutex_unlock(application_mutex);

      /* lv2 play */
      recall_container = ags_recall_container_new();
      ags_audio_add_recall_container(effect_bulk->audio,
				     (GObject *) recall_container);

      //      add_recall_container = ags_add_recall_container_new(current->audio,
      //						  recall_container);
      //      task = g_list_prepend(task,
      //		    add_recall_container);

      recall_lv2 = ags_recall_lv2_new(current,
				      lv2_plugin->turtle,
				      filename,
				      effect,
				      lv2_plugin->uri,
				      AGS_BASE_PLUGIN(lv2_plugin)->effect_index);
      g_object_set(G_OBJECT(recall_lv2),
		   "soundcard\0", soundcard,
		   "source\0", current,
		   "recall-container\0", recall_container,
		   NULL);
      ags_recall_set_flags(AGS_RECALL(recall_lv2), (AGS_RECALL_TEMPLATE |
						    AGS_RECALL_INPUT_ORIENTATED |
						    AGS_RECALL_PLAYBACK |
						    AGS_RECALL_SEQUENCER |
						    AGS_RECALL_NOTATION |
						    AGS_RECALL_BULK_MODE));
      ags_recall_lv2_load(recall_lv2);

      port = ags_recall_lv2_load_ports(recall_lv2);

      if(retport == NULL){
	retport = port;
      }else{
	retport = g_list_concat(retport,
				port);
      }

      ags_channel_add_recall(current,
			     (GObject *) recall_lv2,
			     TRUE);
      ags_connectable_connect(AGS_CONNECTABLE(recall_lv2));

      /* dummy */
      recall_channel_run_dummy = ags_recall_channel_run_dummy_new(current,
								  AGS_TYPE_RECALL_RECYCLING_DUMMY,
								  AGS_TYPE_RECALL_LV2_RUN);
      ags_recall_set_flags(AGS_RECALL(recall_channel_run_dummy), (AGS_RECALL_TEMPLATE |
								  AGS_RECALL_INPUT_ORIENTATED |
								  AGS_RECALL_PLAYBACK |
								  AGS_RECALL_SEQUENCER |
								  AGS_RECALL_NOTATION |
								  AGS_RECALL_BULK_MODE));
      g_object_set(G_OBJECT(recall_channel_run_dummy),
		   "soundcard\0", soundcard,
		   "source\0", current,
		   "recall-channel\0", recall_lv2,
		   "recall-container\0", recall_container,
		   NULL);
      ags_channel_add_recall(current,
			     (GObject *) recall_channel_run_dummy,
			     TRUE);
      ags_connectable_connect(AGS_CONNECTABLE(recall_channel_run_dummy));

      /* lv2 recall */
      recall_container = ags_recall_container_new();
      ags_audio_add_recall_container(effect_bulk->audio,
				     (GObject *) recall_container);

      //      add_recall_container = ags_add_recall_container_new(current->audio,
      //						  recall_container);
      //      task = g_list_prepend(task,
      //		    add_recall_container);

      recall_lv2 = ags_recall_lv2_new(current,
				      lv2_plugin->turtle,
				      filename,
				      effect,
				      lv2_plugin->uri,
				      AGS_BASE_PLUGIN(lv2_plugin)->effect_index);
      g_object_set(G_OBJECT(recall_lv2),
		   "soundcard\0", soundcard,
		   "source\0", current,
		   "recall-container\0", recall_container,
		   NULL);
      ags_recall_set_flags(AGS_RECALL(recall_lv2), (AGS_RECALL_TEMPLATE |
						    AGS_RECALL_INPUT_ORIENTATED |
						    AGS_RECALL_PLAYBACK |
						    AGS_RECALL_SEQUENCER |
						    AGS_RECALL_NOTATION |
						    AGS_RECALL_BULK_MODE));
      ags_recall_lv2_load(recall_lv2);

      recall_port = ags_recall_lv2_load_ports(recall_lv2);

      if(retport == NULL){
	retport = port;
      }else{
	retport = g_list_concat(retport,
				recall_port);
      }

      ags_channel_add_recall(current,
			     (GObject *) recall_lv2,
			     FALSE);
      ags_connectable_connect(AGS_CONNECTABLE(recall_lv2));

      /* dummy */
      recall_channel_run_dummy = ags_recall_channel_run_dummy_new(current,
								  AGS_TYPE_RECALL_RECYCLING_DUMMY,
								  AGS_TYPE_RECALL_LV2_RUN);
      ags_recall_set_flags(AGS_RECALL(recall_channel_run_dummy), (AGS_RECALL_TEMPLATE |
								  AGS_RECALL_INPUT_ORIENTATED |
								  AGS_RECALL_PLAYBACK |
								  AGS_RECALL_SEQUENCER |
								  AGS_RECALL_NOTATION |
								  AGS_RECALL_BULK_MODE));
      g_object_set(G_OBJECT(recall_channel_run_dummy),
		   "soundcard\0", soundcard,
		   "source\0", current,
		   "recall-channel\0", recall_lv2,
		   "recall-container\0", recall_container,
		   NULL);
      ags_channel_add_recall(current,
			     (GObject *) recall_channel_run_dummy,
			     FALSE);
      ags_connectable_connect(AGS_CONNECTABLE(recall_channel_run_dummy));
      
      /* iterate */
      pthread_mutex_lock(channel_mutex);
      
      current = current->next;

      pthread_mutex_unlock(channel_mutex);
    }
  }

  /* retrieve position within table  */
  x = 0;
  y = 0;
  
  list = effect_bulk->table->children;

  while(list != NULL){
    if(y <= ((GtkTableChild *) list->data)->top_attach){
      y = ((GtkTableChild *) list->data)->top_attach + 1;
    }

    list = list->next;
  }

  /* load ports */
  port_descriptor = AGS_BASE_PLUGIN(lv2_plugin)->port;

  port_count = g_list_length(port_descriptor);
  k = 0;

  while(port_descriptor != NULL){
    if((AGS_PORT_DESCRIPTOR_CONTROL & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
      GtkWidget *child_widget;

      AgsLv2Conversion *lv2_conversion;
      
      GType widget_type;

      guint step_count;
      
      if(x == AGS_EFFECT_BULK_COLUMNS_COUNT){
	x = 0;
	y++;
	gtk_table_resize(effect_bulk->table,
			 y + 1, AGS_EFFECT_BULK_COLUMNS_COUNT);
      }

      if((AGS_PORT_DESCRIPTOR_TOGGLED & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	widget_type = GTK_TYPE_TOGGLE_BUTTON;
      }else{
	widget_type = AGS_TYPE_DIAL;
      }

      step_count = AGS_DIAL_DEFAULT_PRECISION;

      if((AGS_PORT_DESCRIPTOR_INTEGER & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	step_count = AGS_PORT_DESCRIPTOR(port_descriptor->data)->scale_steps;
      }

      /* add bulk member */
      bulk_member = (AgsBulkMember *) g_object_new(AGS_TYPE_BULK_MEMBER,
						   "widget-type\0", widget_type,
						   "widget-label\0", AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_name,
						   "plugin-name\0", g_strdup_printf("lv2-<%s>\0", lv2_plugin->uri),
						   "filename\0", filename,
						   "effect\0", effect,
						   "specifier\0", g_strdup(AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_name),
						   "control-port\0", g_strdup_printf("%u/%u\0",
										     k,
										     port_count),
						   "steps\0", step_count,
						   NULL);
      child_widget = ags_bulk_member_get_widget(bulk_member);

      /* lv2 conversion */
      lv2_conversion = NULL;

      if((AGS_PORT_DESCRIPTOR_LOGARITHMIC & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	if(lv2_conversion == NULL ||
	   !AGS_IS_LV2_CONVERSION(lv2_conversion)){
	  lv2_conversion = ags_lv2_conversion_new();
	}
    
	lv2_conversion->flags |= AGS_LV2_CONVERSION_LOGARITHMIC;
      }

      bulk_member->conversion = (AgsConversion *) lv2_conversion;

      /* child widget */
      if((AGS_PORT_DESCRIPTOR_TOGGLED & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	bulk_member->port_flags = AGS_BULK_MEMBER_PORT_BOOLEAN;
      }
      
      if((AGS_PORT_DESCRIPTOR_INTEGER & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	bulk_member->port_flags = AGS_BULK_MEMBER_PORT_INTEGER;
      }

      if(AGS_IS_DIAL(child_widget)){
	AgsDial *dial;
	GtkAdjustment *adjustment;

	float lower_bound, upper_bound;
	
	dial = (AgsDial *) child_widget;

	/* add controls of ports and apply range  */
	lower_bound = g_value_get_float(AGS_PORT_DESCRIPTOR(port_descriptor->data)->lower_value);
	upper_bound = g_value_get_float(AGS_PORT_DESCRIPTOR(port_descriptor->data)->upper_value);

	adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.1, 0.0);
	g_object_set(dial,
		     "adjustment", adjustment,
		     NULL);

	if(upper_bound >= 0.0 && lower_bound >= 0.0){
	  step = (upper_bound - lower_bound) / step_count;
	}else if(upper_bound < 0.0 && lower_bound < 0.0){
	  step = -1.0 * (lower_bound - upper_bound) / step_count;
	}else{
	  step = (upper_bound - lower_bound) / step_count;
	}

	gtk_adjustment_set_step_increment(adjustment,
					  step);
	gtk_adjustment_set_lower(adjustment,
				 lower_bound);
	gtk_adjustment_set_upper(adjustment,
				 upper_bound);
	gtk_adjustment_set_value(adjustment,
				 g_value_get_float(AGS_PORT_DESCRIPTOR(port_descriptor->data)->default_value));
      }

#ifdef AGS_DEBUG
      g_message("lv2 bounds: %f %f\0", lower_bound, upper_bound);
#endif
      
      gtk_table_attach(effect_bulk->table,
		       (GtkWidget *) bulk_member,
		       x, x + 1,
		       y, y + 1,
		       GTK_FILL, GTK_FILL,
		       0, 0);
      ags_connectable_connect(AGS_CONNECTABLE(bulk_member));
      gtk_widget_show_all((GtkWidget *) effect_bulk->table);

      x++;
    }

    port_descriptor = port_descriptor->next;    
    k++;
  }

  /* launch tasks */
  task = g_list_reverse(task);      
  ags_task_thread_append_tasks(task_thread,
			       task);

  return(retport);
}

GList*
ags_effect_bulk_real_add_effect(AgsEffectBulk *effect_bulk,
				GList *control_type_name,
				gchar *filename,
				gchar *effect)
{
  AgsLadspaPlugin *ladspa_plugin;
  AgsDssiPlugin *dssi_plugin;
  AgsLv2Plugin *lv2_plugin;
  
  GList *port;

  /* load plugin */
  ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(ags_lv2_manager_get_instance(),
							filename, effect);
  port = NULL;
  
  if(ladspa_plugin != NULL){
    port = ags_effect_bulk_add_ladspa_effect(effect_bulk,
					     control_type_name,
					     filename,
					     effect);
  }

  if(ladspa_plugin == NULL){
    dssi_plugin = ags_dssi_manager_find_dssi_plugin(ags_dssi_manager_get_instance(),
						    filename, effect);

    if(dssi_plugin != NULL){
      port = ags_effect_bulk_add_dssi_effect(effect_bulk,
					     control_type_name,
					     filename,
					     effect);
    }
  }
  
  if(ladspa_plugin == NULL &&
     dssi_plugin == NULL){
    GList *ui_node;
    gchar *str;
    
    lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
						 filename, effect);

    if(lv2_plugin != NULL){
      port = ags_effect_bulk_add_lv2_effect(effect_bulk,
					    control_type_name,
					    filename,
					    effect);
    }
  }
  
  return(port);
}

GList*
ags_effect_bulk_add_effect(AgsEffectBulk *effect_bulk,
			   GList *control_type_name,
			   gchar *filename,
			   gchar *effect)
{
  GList *list;
  
  g_return_val_if_fail(AGS_IS_EFFECT_BULK(effect_bulk), NULL);

  g_object_ref((GObject *) effect_bulk);
  g_signal_emit(G_OBJECT(effect_bulk),
		effect_bulk_signals[ADD_EFFECT], 0,
		control_type_name,
		filename,
		effect,
		&list);
  g_object_unref((GObject *) effect_bulk);

  return(list);
}

void
ags_effect_bulk_real_remove_effect(AgsEffectBulk *effect_bulk,
				   guint nth)
{
  AgsAddBulkMember *add_bulk_member;
  GtkAdjustment *adjustment;
  
  AgsChannel *current;

  AgsEffectBulkPlugin *effect_bulk_plugin;

  AgsMutexManager *mutex_manager;

  GList *recall;
  GList *list, *list_next;

  gchar *filename, *effect;

  guint nth_effect, n_recall;

  guint pads, audio_channels;
  guint i, j;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;

  /* get mutex manager and application mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) effect_bulk->audio);
  
  pthread_mutex_unlock(application_mutex);

  /* free plugin specification */
  effect_bulk_plugin = g_list_nth_data(effect_bulk->plugin,
				       nth);
  effect_bulk->plugin = g_list_remove(effect_bulk->plugin,
				      effect_bulk_plugin);
  free(effect_bulk_plugin);

  /* retrieve audio properties and channel */
  pthread_mutex_lock(audio_mutex);

  audio_channels = effect_bulk->audio->audio_channels;

  if(effect_bulk->channel_type == AGS_TYPE_OUTPUT){
    current = effect_bulk->audio->output;
    
    pads = effect_bulk->audio->output_pads;
  }else{
    current = effect_bulk->audio->input;

    pads = effect_bulk->audio->input_pads;
  }

  pthread_mutex_unlock(audio_mutex);

  if(current != NULL){
    /* get channel mutex */
    pthread_mutex_lock(application_mutex);

    channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) current);
  
    pthread_mutex_unlock(application_mutex);

    /* get nth_effect */
    pthread_mutex_lock(channel_mutex);
  
    recall = current->play;
    nth_effect = 0;
    n_recall = 0;
  
    while((recall = ags_recall_template_find_all_type(recall,
						      AGS_TYPE_RECALL_LADSPA,
						      AGS_TYPE_RECALL_DSSI,
						      AGS_TYPE_RECALL_LV2,
						      G_TYPE_NONE)) != NULL){
      if((AGS_RECALL_TEMPLATE & (AGS_RECALL(recall->data)->flags)) != 0){
	nth_effect++;
      }

      if((AGS_RECALL_BULK_MODE & (AGS_RECALL(recall->data)->flags)) == 0){
	n_recall++;
      }

      if(nth_effect - n_recall == nth){
	break;
      }
    
      recall = recall->next;
    }

    pthread_mutex_unlock(channel_mutex);
  }

  nth_effect--;
  
  /* destroy control */
  list = gtk_container_get_children((GtkContainer *) effect_bulk->table);

  filename = AGS_BULK_MEMBER(list->data)->filename;
  effect = AGS_BULK_MEMBER(list->data)->effect;

  i = 0;
  
  while(list != NULL && i <= nth){
    list_next = list->next;
    
    if(AGS_IS_BULK_MEMBER(list->data)){
      if(!(!g_strcmp0(AGS_BULK_MEMBER(list->data)->filename, filename) &&
	   !g_strcmp0(AGS_BULK_MEMBER(list->data)->effect, effect))){
	filename = AGS_BULK_MEMBER(list->data)->filename;
	effect = AGS_BULK_MEMBER(list->data)->effect;

	i++;
      }
      
      if(i == nth){
	gtk_widget_destroy(list->data);
      }
      
    }

    list = list_next;
  }
  
  /* remove recalls */
  for(i = 0; i < pads; i++){
    for(j = 0; j < audio_channels; j++){
      /* get channel mutex */
      pthread_mutex_lock(application_mutex);

      channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) current);
  
      pthread_mutex_unlock(application_mutex);

      /* remove effect */
      ags_channel_remove_effect(current,
				nth_effect);

      /* iterate */
      pthread_mutex_lock(channel_mutex);
      
      current = current->next;

      pthread_mutex_unlock(channel_mutex);	  
    }
  }
}

void
ags_effect_bulk_remove_effect(AgsEffectBulk *effect_bulk,
			      guint nth)
{
  g_return_if_fail(AGS_IS_EFFECT_BULK(effect_bulk));

  g_object_ref((GObject *) effect_bulk);
  g_signal_emit(G_OBJECT(effect_bulk),
		effect_bulk_signals[REMOVE_EFFECT], 0,
		nth);
  g_object_unref((GObject *) effect_bulk);
}

void
ags_effect_bulk_real_resize_audio_channels(AgsEffectBulk *effect_bulk,
					   guint new_size,
					   guint old_size)
{
  AgsWindow *window;
  AgsUpdateBulkMember *update_bulk_member;

  AgsChannel *current;

  AgsThread *main_loop;
  AgsTaskThread *task_thread;
  AgsMutexManager *mutex_manager;

  AgsApplicationContext *application_context;
  
  GList *task;
  GList *effect_bulk_plugin;
  GList *list;

  guint pads;
  guint i, j;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) effect_bulk,
						 AGS_TYPE_WINDOW);
  
  application_context = (AgsApplicationContext *) window->application_context;

  /* get mutex manager and application mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /*  */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

  pthread_mutex_unlock(application_mutex);

  task_thread = (AgsTaskThread *) ags_thread_find_type(main_loop,
						       AGS_TYPE_TASK_THREAD);
  
  /* get audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) effect_bulk->audio);
  
  pthread_mutex_unlock(application_mutex);
  
  /* retrieve channel */
  pthread_mutex_lock(audio_mutex);

  if(effect_bulk->channel_type == AGS_TYPE_OUTPUT){
    current = effect_bulk->audio->output;
    
    pads = effect_bulk->audio->output_pads;
  }else{
    current = effect_bulk->audio->input;

    pads = effect_bulk->audio->input_pads;
  }

  pthread_mutex_unlock(audio_mutex);

  if(pads == 0){
    return;
  }

  /* collect bulk member */
  task = NULL;

  list = gtk_container_get_children((GtkContainer *) effect_bulk->table);

  while(list != NULL){
    if(AGS_IS_BULK_MEMBER(list->data)){
      /* create task */
      update_bulk_member = ags_update_bulk_member_new((GtkWidget *) effect_bulk,
						      list->data,
						      new_size,
						      old_size,
						      FALSE);
      task = g_list_prepend(task,
			    update_bulk_member);
    }
      
    list = list->next;
  }

  if(new_size > old_size){  
    /* add effect */
    for(i = 0; i < pads; i++){
      current = ags_channel_nth(current,
				old_size);

      /* get channel mutex */
      pthread_mutex_lock(application_mutex);

      audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) current);
  
      pthread_mutex_unlock(application_mutex);

      /*  */      
      for(j = old_size; j < new_size; j++){
	effect_bulk_plugin = effect_bulk->plugin;

	while(effect_bulk_plugin != NULL){
	  ags_channel_add_effect(current,
				 AGS_EFFECT_BULK_PLUGIN(effect_bulk_plugin->data)->filename,
				 AGS_EFFECT_BULK_PLUGIN(effect_bulk_plugin->data)->effect);

	  effect_bulk_plugin = effect_bulk_plugin->next;
	}

	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	current = current->next;

	pthread_mutex_unlock(channel_mutex);
      }
    }
  }
  
  /* launch tasks */
  task = g_list_reverse(task);      
  ags_task_thread_append_tasks(task_thread,
			       task);
}

void
ags_effect_bulk_resize_audio_channels(AgsEffectBulk *effect_bulk,
				      guint new_size,
				      guint old_size)
{
  g_return_if_fail(AGS_IS_EFFECT_BULK(effect_bulk));

  g_object_ref((GObject *) effect_bulk);
  g_signal_emit(G_OBJECT(effect_bulk),
		effect_bulk_signals[RESIZE_AUDIO_CHANNELS], 0,
		new_size,
		old_size);
  g_object_unref((GObject *) effect_bulk);
}

void
ags_effect_bulk_real_resize_pads(AgsEffectBulk *effect_bulk,
				 guint new_size,
				 guint old_size)
{
  AgsWindow *window;
  AgsUpdateBulkMember *update_bulk_member;

  AgsChannel *current;

  AgsThread *main_loop;
  AgsTaskThread *task_thread;
  AgsMutexManager *mutex_manager;

  AgsApplicationContext *application_context;
  
  GList *task;
  GList *effect_bulk_plugin;
  GList *list;

  guint audio_channels;
  guint i, j;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) effect_bulk,
						 AGS_TYPE_WINDOW);
  
  application_context = (AgsApplicationContext *) window->application_context;

  /* get mutex manager and application mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /*  */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

  pthread_mutex_unlock(application_mutex);

  task_thread = (AgsTaskThread *) ags_thread_find_type(main_loop,
						       AGS_TYPE_TASK_THREAD);

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) effect_bulk->audio);
  
  pthread_mutex_unlock(application_mutex);
  
  /* retrieve channel */
  pthread_mutex_lock(audio_mutex);
  
  audio_channels = effect_bulk->audio->audio_channels;

  if(effect_bulk->channel_type == AGS_TYPE_OUTPUT){
    current = effect_bulk->audio->output;
  }else{
    current = effect_bulk->audio->input;
  }

  pthread_mutex_unlock(audio_mutex);

  if(audio_channels == 0){
    return;
  }
  
  /* collect bulk member */
  task = NULL;

  list = gtk_container_get_children((GtkContainer *) effect_bulk->table);

  while(list != NULL){
    if(AGS_IS_BULK_MEMBER(list->data)){
      /* create task */
      update_bulk_member = ags_update_bulk_member_new((GtkWidget *) effect_bulk,
						      list->data,
						      new_size,
						      old_size,
						      TRUE);
      task = g_list_prepend(task,
			    update_bulk_member);
      
    }
      
    list = list->next;
  }
   
  if(new_size > old_size){ 
    /* add effect */
    current = ags_channel_pad_nth(current,
				  old_size);

    /* get channel mutex */
    pthread_mutex_lock(application_mutex);

    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) current);
  
    pthread_mutex_unlock(application_mutex);

    /*  */
    for(i = old_size; i < new_size; i++){
      for(j = 0; j < audio_channels; j++){    
	effect_bulk_plugin = effect_bulk->plugin;

	while(effect_bulk_plugin != NULL){
	  ags_channel_add_effect(current,
				 AGS_EFFECT_BULK_PLUGIN(effect_bulk_plugin->data)->filename,
				 AGS_EFFECT_BULK_PLUGIN(effect_bulk_plugin->data)->effect);

	  effect_bulk_plugin = effect_bulk_plugin->next;
	}

	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	current = current->next;

	pthread_mutex_unlock(channel_mutex);
      }
    }
  }
  
  /* launch tasks */
  task = g_list_reverse(task);      
  ags_task_thread_append_tasks(task_thread,
			       task);
}

void
ags_effect_bulk_resize_pads(AgsEffectBulk *effect_bulk,
			    guint new_size,
			    guint old_size)
{
  g_return_if_fail(AGS_IS_EFFECT_BULK(effect_bulk));

  g_object_ref((GObject *) effect_bulk);
  g_signal_emit(G_OBJECT(effect_bulk),
		effect_bulk_signals[RESIZE_PADS], 0,
		new_size,
		old_size);
  g_object_unref((GObject *) effect_bulk);
}

void
ags_effect_bulk_real_map_recall(AgsEffectBulk *effect_bulk)
{
  if((AGS_MACHINE_PREMAPPED_RECALL & (effect_bulk->flags)) == 0 ||
     (AGS_EFFECT_BULK_MAPPED_RECALL & (effect_bulk->flags)) != 0){
    return;
  }

  effect_bulk->flags |= AGS_EFFECT_BULK_MAPPED_RECALL;

  ags_effect_bulk_find_port(effect_bulk);
}

/**
 * ags_effect_bulk_map_recall:
 * @effect_bulk: the #AgsEffectBulk to add its default recall.
 *
 * You may want the @effect_bulk to add its default recall.
 */
void
ags_effect_bulk_map_recall(AgsEffectBulk *effect_bulk)
{
  g_return_if_fail(AGS_IS_EFFECT_BULK(effect_bulk));

  g_object_ref((GObject *) effect_bulk);
  g_signal_emit((GObject *) effect_bulk,
		effect_bulk_signals[MAP_RECALL], 0);
  g_object_unref((GObject *) effect_bulk);
}

GList*
ags_effect_bulk_real_find_port(AgsEffectBulk *effect_bulk)
{
  GList *bulk_member, *bulk_member_start;
  
  GList *port, *tmp_port;

  port = NULL;

  /* find output ports */
  bulk_member_start = 
    bulk_member = gtk_container_get_children((GtkContainer *) effect_bulk->table);

  if(bulk_member != NULL){
    while(bulk_member != NULL){
      tmp_port = ags_bulk_member_find_port(AGS_BULK_MEMBER(bulk_member->data));
      
      if(port != NULL){
	port = g_list_concat(port,
			     tmp_port);
      }else{
	port = tmp_port;
      }

      bulk_member = bulk_member->next;
    }

    g_list_free(bulk_member_start);
  }
  
  return(port);
}

/**
 * ags_effect_bulk_find_port:
 * @effect_bulk: the #AgsEffectBulk
 * Returns: an #GList containing all related #AgsPort
 *
 * Lookup ports of associated recalls.
 *
 * Since: 0.7.8
 */
GList*
ags_effect_bulk_find_port(AgsEffectBulk *effect_bulk)
{
  GList *list;

  list = NULL;
  g_return_val_if_fail(AGS_IS_EFFECT_BULK(effect_bulk),
		       NULL);

  g_object_ref((GObject *) effect_bulk);
  g_signal_emit((GObject *) effect_bulk,
		effect_bulk_signals[FIND_PORT], 0,
		&list);
  g_object_unref((GObject *) effect_bulk);

  return(list);
}

/**
 * ags_effect_bulk_new:
 * @audio: the #AgsAudio to visualize
 * @channel_type: either %AGS_TYPE_INPUT or %AGS_TYPE_OUTPUT
 *
 * Creates an #AgsEffectBulk
 *
 * Returns: a new #AgsEffectBulk
 *
 * Since: 0.4
 */
AgsEffectBulk*
ags_effect_bulk_new(AgsAudio *audio,
		    GType channel_type)
{
  AgsEffectBulk *effect_bulk;

  effect_bulk = (AgsEffectBulk *) g_object_new(AGS_TYPE_EFFECT_BULK,
					       "audio\0", audio,
					       "channel-type\0", channel_type,
					       NULL);

  return(effect_bulk);
}

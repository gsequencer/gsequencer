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

#include <ags/main.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/object/ags_marshal.h>
#include <ags/object/ags_plugin.h>

#include <ags/plugin/ags_ladspa_manager.h>

#include <ags/thread/ags_audio_loop.h>
#include <ags/thread/ags_task_thread.h>

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

#include <ags/audio/task/ags_add_bulk_member.h>
#include <ags/audio/task/ags_update_bulk_member.h>
#include <ags/audio/task/ags_add_recall_container.h>
#include <ags/audio/task/ags_add_recall.h>

#include <ags/widget/ags_dial.h>

#include <ags/X/ags_ladspa_browser.h>
#include <ags/X/ags_bulk_member.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <ladspa.h>

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

GList* ags_effect_bulk_real_add_effect(AgsEffectBulk *effect_bulk,
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
   * Since: 0.4
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
   * Since: 0.4
   */
  param_spec = g_param_spec_gtype("channel-type\0",
				  "assigned channel type\0",
				  "The channel type it is assigned with\0",
				  AGS_TYPE_CHANNEL,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL_TYPE,
				  param_spec);

  /* AgsEffectBulkClass */
  effect_bulk->add_effect = ags_effect_bulk_real_add_effect;
  effect_bulk->remove_effect = ags_effect_bulk_real_remove_effect;

  effect_bulk->resize_audio_channels = ags_effect_bulk_real_resize_audio_channels;
  effect_bulk->resize_pads = ags_effect_bulk_real_resize_pads;

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
		 g_cclosure_user_marshal_POINTER__STRING_STRING,
		 G_TYPE_POINTER, 2,
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

  alignment = g_object_new(GTK_TYPE_ALIGNMENT,
			   "xalign\0", 1.0,
			   NULL);
  gtk_box_pack_start(effect_bulk,
		     alignment,
		     FALSE, FALSE,
		     0);

  hbox = gtk_hbox_new(FALSE, 0);
  gtk_container_add(alignment,
		    hbox);

  effect_bulk->add = gtk_button_new_from_stock(GTK_STOCK_ADD);
  gtk_box_pack_start(hbox,
		     effect_bulk->add,
		     FALSE, FALSE,
		     0);

  effect_bulk->remove = gtk_button_new_from_stock(GTK_STOCK_REMOVE);
  gtk_box_pack_start(hbox,
		     effect_bulk->remove,
		     FALSE, FALSE,
		     0);

  hbox = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(effect_bulk,
		     hbox,
		     FALSE, FALSE,
		     0);

  effect_bulk->bulk_member = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_box_pack_start(hbox,
		     effect_bulk->bulk_member,
		     FALSE, FALSE,
		     0);

  effect_bulk->table = (GtkTable *) gtk_table_new(1, AGS_EFFECT_BULK_COLUMNS_COUNT,
						  FALSE);
  gtk_box_pack_start(hbox,
		     effect_bulk->table,
		     FALSE, FALSE,
		     0);

  effect_bulk->ladspa_browser = ags_ladspa_browser_new(effect_bulk);
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
	  g_signal_handler_disconnect(effect_bulk->audio,
				      effect_bulk->set_audio_channels_handler);
	  g_signal_handler_disconnect(effect_bulk->audio,
				      effect_bulk->set_pads_handler);
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
	  effect_bulk->set_audio_channels_handler = g_signal_connect_after(effect_bulk->audio, "set-audio-channels\0",
									   G_CALLBACK(ags_effect_bulk_set_audio_channels_callback), effect_bulk);

	  effect_bulk->set_pads_handler = g_signal_connect_after(effect_bulk->audio, "set-pads\0",
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

  effect_bulk = AGS_EFFECT_BULK(connectable);

  if((AGS_EFFECT_BULK_CONNECTED & (effect_bulk->flags)) != 0){
    return;
  }

  g_signal_connect(G_OBJECT(effect_bulk->add), "clicked\0",
		   G_CALLBACK(ags_effect_bulk_add_callback), effect_bulk);

  g_signal_connect(G_OBJECT(effect_bulk->remove), "clicked\0",
		   G_CALLBACK(ags_effect_bulk_remove_callback), effect_bulk);

  ags_connectable_connect(AGS_CONNECTABLE(effect_bulk->ladspa_browser));

  g_signal_connect(G_OBJECT(effect_bulk->ladspa_browser), "response\0",
		   G_CALLBACK(ags_effect_bulk_ladspa_browser_response_callback), effect_bulk);

  if(effect_bulk->audio != NULL){
    effect_bulk->set_audio_channels_handler = g_signal_connect_after(effect_bulk->audio, "set-audio-channels\0",
								     G_CALLBACK(ags_effect_bulk_set_audio_channels_callback), effect_bulk);

    effect_bulk->set_pads_handler = g_signal_connect_after(effect_bulk->audio, "set-pads\0",
							   G_CALLBACK(ags_effect_bulk_set_pads_callback), effect_bulk);
  }
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

AgsEffectBulkPlugin*
ags_effect_bulk_plugin_alloc(gchar *filename,
			     gchar *effect)
{
  AgsEffectBulkPlugin *effect_plugin;

  effect_plugin = (AgsEffectBulkPlugin *) malloc(sizeof(AgsEffectBulkPlugin));

  effect_plugin->filename = filename;
  effect_plugin->effect = effect;
  
  return(effect_plugin);
}

GList*
ags_effect_bulk_real_add_effect(AgsEffectBulk *effect_bulk,
				gchar *filename,
				gchar *effect)
{
  AgsBulkMember *bulk_member;
  AgsAddBulkMember *add_bulk_member;
  AgsUpdateBulkMember *update_bulk_member;
  GtkAdjustment *adjustment;

  AgsChannel *current;
  AgsRecallContainer *recall_container;
  AgsRecallChannelRunDummy *recall_channel_run_dummy;
  AgsRecallLadspa *recall_ladspa;
  AgsLadspaPlugin *ladspa_plugin;
  AgsAddRecallContainer *add_recall_container;
  AgsAddRecall *add_recall;

  AgsAudioLoop *audio_loop;
  AgsTaskThread *task_thread;

  GList *port, *recall_port;
  GList *list, *list_start;
  GList *task;
  guint pads, audio_channels;
  gdouble step;
  guint x, y;
  guint i, j;

  void *plugin_so;
  LADSPA_Descriptor_Function ladspa_descriptor;
  LADSPA_Descriptor *plugin_descriptor;
  LADSPA_PortDescriptor *port_descriptor;
  LADSPA_Data lower_bound, upper_bound;
  unsigned long index;
  unsigned long k;

  effect_bulk->plugin = g_list_append(effect_bulk->plugin,
				      ags_effect_bulk_plugin_alloc(filename,
								   effect));
  
  audio_loop = (AgsAudioLoop *) AGS_MAIN(AGS_DEVOUT(effect_bulk->audio->devout)->ags_main)->main_loop;
  task_thread = (AgsTaskThread *) audio_loop->task_thread;

  audio_channels = effect_bulk->audio->audio_channels;

  if(effect_bulk->channel_type == AGS_TYPE_OUTPUT){
    current = effect_bulk->audio->output;
    
    pads = effect_bulk->audio->output_pads;
  }else{
    current = effect_bulk->audio->input;

    pads = effect_bulk->audio->input_pads;
  }

  /*  */
  index = ags_ladspa_manager_effect_index(filename,
					  effect);

  task = NULL;
  
  /* load plugin */
  ags_ladspa_manager_load_file(filename);
  ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(filename);

  plugin_so = ladspa_plugin->plugin_so;

  for(i = 0; i < pads; i++){
    for(j = 0; j < audio_channels; j++){

      /* ladspa play */
      recall_container = ags_recall_container_new();

      add_recall_container = ags_add_recall_container_new(current->audio,
							  recall_container);
      task = g_list_prepend(task,
			    add_recall_container);

      recall_ladspa = ags_recall_ladspa_new(current,
					    filename,
					    effect,
					    index);
      g_object_set(G_OBJECT(recall_ladspa),
		   "devout\0", AGS_AUDIO(current->audio)->devout,
		   "recall-container\0", recall_container,
		   NULL);
      AGS_RECALL(recall_ladspa)->flags |= AGS_RECALL_TEMPLATE;
      ags_recall_ladspa_load(recall_ladspa);
      port = ags_recall_ladspa_load_ports(recall_ladspa);

      add_recall = ags_add_recall_new(current,
				      recall_ladspa,
				      TRUE);
      task = g_list_prepend(task,
			    add_recall);

      /* dummy */
      recall_channel_run_dummy = ags_recall_channel_run_dummy_new(current,
								  AGS_TYPE_RECALL_RECYCLING_DUMMY,
								  AGS_TYPE_RECALL_LADSPA_RUN);
      AGS_RECALL(recall_channel_run_dummy)->flags |= AGS_RECALL_TEMPLATE;
      g_object_set(G_OBJECT(recall_channel_run_dummy),
		   "devout\0", AGS_AUDIO(current->audio)->devout,
		   "recall-container\0", recall_container,
		   "recall-channel\0", recall_ladspa,
		   NULL);

      add_recall = ags_add_recall_new(current,
				      recall_channel_run_dummy,
				      TRUE);
      task = g_list_prepend(task,
			    add_recall);

      /* ladspa recall */
      recall_container = ags_recall_container_new();

      add_recall_container = ags_add_recall_container_new(current->audio,
							  recall_container);
      task = g_list_prepend(task,
			    add_recall_container);

      recall_ladspa = ags_recall_ladspa_new(current,
					    filename,
					    effect,
					    index);
      g_object_set(G_OBJECT(recall_ladspa),
		   "devout\0", AGS_AUDIO(current->audio)->devout,
		   "recall-container\0", recall_container,
		   NULL);
      AGS_RECALL(recall_ladspa)->flags |= AGS_RECALL_TEMPLATE;
      ags_recall_ladspa_load(recall_ladspa);
      recall_port = ags_recall_ladspa_load_ports(recall_ladspa);
            
      add_recall = ags_add_recall_new(current,
				      recall_ladspa,
				      FALSE);
      task = g_list_prepend(task,
			    add_recall);

      /* dummy */
      recall_channel_run_dummy = ags_recall_channel_run_dummy_new(current,
								  AGS_TYPE_RECALL_RECYCLING_DUMMY,
								  AGS_TYPE_RECALL_LADSPA_RUN);
      AGS_RECALL(recall_channel_run_dummy)->flags |= AGS_RECALL_TEMPLATE;
      g_object_set(G_OBJECT(recall_channel_run_dummy),
		   "devout\0", AGS_AUDIO(current->audio)->devout,
		   "recall-container\0", recall_container,
		   "recall-channel\0", recall_ladspa,
		   NULL);

      add_recall = ags_add_recall_new(current,
				      recall_channel_run_dummy,
				      FALSE);
      task = g_list_prepend(task,
			    add_recall);

      
      current = current->next;
    }
  }

  /* retrieve position within table  */
  x = 0;
  y = 0;
  
  list_start = 
    list = effect_bulk->table->children;

  while(list != NULL){
    if(y <= ((GtkTableChild *) list->data)->top_attach){
      y = ((GtkTableChild *) list->data)->top_attach + 1;
    }

    list = list->next;
  }
  
  /* load ports */
  if(index != -1 &&
     plugin_so){
    ladspa_descriptor = (LADSPA_Descriptor_Function) dlsym(plugin_so,
							   "ladspa_descriptor\0");

    if(dlerror() == NULL && ladspa_descriptor){
      plugin_descriptor = ladspa_descriptor(index);

      port_descriptor = plugin_descriptor->PortDescriptors;

      for(k = 0; k < plugin_descriptor->PortCount; k++){
	if((LADSPA_IS_PORT_CONTROL(port_descriptor[k]) && 
	    (LADSPA_IS_PORT_INPUT(port_descriptor[k]) ||
	     LADSPA_IS_PORT_OUTPUT(port_descriptor[k])))){
	  AgsDial *dial;
	  GtkAdjustment *adjustment;

	  if(x == AGS_EFFECT_BULK_COLUMNS_COUNT){
	    x = 0;
	    y++;
	    gtk_table_resize(effect_bulk->table,
			     y + 1, AGS_EFFECT_BULK_COLUMNS_COUNT);
	  }

	  /* add bulk member */
	  bulk_member = (AgsBulkMember *) g_object_new(AGS_TYPE_BULK_MEMBER,
						       "widget-type\0", AGS_TYPE_DIAL,
						       "widget-label\0", plugin_descriptor->PortNames[k],
						       "plugin-name\0", g_strdup_printf("ladspa-%lu\0", plugin_descriptor->UniqueID),
						       "filename\0", filename,
						       "effect\0", effect,
						       "specifier\0", g_strdup(plugin_descriptor->PortNames[i]),
						       "control-port\0", g_strdup_printf("%d/%d\0",
											 k,
											 plugin_descriptor->PortCount),
						       NULL);
	  dial = ags_bulk_member_get_widget(bulk_member);
	  gtk_widget_set_size_request(dial,
				      2 * dial->radius + 2 * dial->outline_strength + dial->button_width + 1,
				      2 * dial->radius + 2 * dial->outline_strength + 1);
		
	  /* add controls of ports and apply range  */
	  lower_bound = plugin_descriptor->PortRangeHints[k].LowerBound;
	  upper_bound = plugin_descriptor->PortRangeHints[k].UpperBound;

	  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.1, 0.0);
	  g_object_set(dial,
		       "adjustment", adjustment,
		       NULL);

	  if(upper_bound >= 0.0 && lower_bound >= 0.0){
	    step = (upper_bound - lower_bound) / AGS_DIAL_DEFAULT_PRECISION;
	  }else if(upper_bound < 0.0 && lower_bound < 0.0){
	    step = -1.0 * (upper_bound + lower_bound) / AGS_DIAL_DEFAULT_PRECISION;
	  }else{
	    step = (upper_bound - lower_bound) / AGS_DIAL_DEFAULT_PRECISION;
	  }

	  gtk_adjustment_set_step_increment(adjustment,
					    step);
	  gtk_adjustment_set_lower(adjustment,
				   lower_bound);
	  gtk_adjustment_set_upper(adjustment,
				   upper_bound);
	  gtk_adjustment_set_value(adjustment,
				   lower_bound);

	  /* create task */
	  add_bulk_member = ags_add_bulk_member_new(effect_bulk,
						    bulk_member,
						    x, y,
						    1, 1);
	  task = g_list_prepend(task,
				add_bulk_member);

	  /* update ports */
	  update_bulk_member = ags_update_bulk_member_new(effect_bulk,
							  bulk_member,
							  pads,
							  0,
							  TRUE);
	  task = g_list_prepend(task,
				update_bulk_member);

	  x++;
	}
      }
    }
  }

  /* launch tasks */
  task = g_list_reverse(task);      
  ags_task_thread_append_tasks(task_thread,
			       task);

  return(port);
}

GList*
ags_effect_bulk_add_effect(AgsEffectBulk *effect_bulk,
			   gchar *filename,
			   gchar *effect)
{
  GList *list;
  
  g_return_val_if_fail(AGS_IS_EFFECT_BULK(effect_bulk), NULL);

  g_object_ref((GObject *) effect_bulk);
  g_signal_emit(G_OBJECT(effect_bulk),
		effect_bulk_signals[ADD_EFFECT], 0,
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
  GList *list, *list_next;
  gchar *filename, *effect;
  guint pads, audio_channels;
  guint i, j;

  /* free plugin specification */
  effect_bulk_plugin = g_list_nth_data(effect_bulk->plugin,
				       nth);
  effect_bulk->plugin = g_list_remove(effect_bulk->plugin,
				      effect_bulk_plugin);
  free(effect_bulk_plugin);

  /* retrieve channel */
  audio_channels = effect_bulk->audio->audio_channels;

  if(effect_bulk->channel_type == AGS_TYPE_OUTPUT){
    current = effect_bulk->audio->output;
    
    pads = effect_bulk->audio->output_pads;
  }else{
    current = effect_bulk->audio->input;

    pads = effect_bulk->audio->input_pads;
  }

  /* destroy control */
  list = gtk_container_get_children(effect_bulk->table);

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
      ags_channel_remove_effect(current,
				nth);

      current = current->next;
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
  AgsUpdateBulkMember *update_bulk_member;

  AgsChannel *current;

  AgsAudioLoop *audio_loop;
  AgsTaskThread *task_thread;

  GList *task;
  GList *bulk_member;
  GList *effect_bulk_plugin;
  GList *list;
  guint pads;
  guint i, j;

  audio_loop = (AgsAudioLoop *) AGS_MAIN(AGS_DEVOUT(effect_bulk->audio->devout)->ags_main)->main_loop;
  task_thread = (AgsTaskThread *) audio_loop->task_thread;
  
  /* retrieve channel */
  if(effect_bulk->channel_type == AGS_TYPE_OUTPUT){
    current = effect_bulk->audio->output;
    
    pads = effect_bulk->audio->output_pads;
  }else{
    current = effect_bulk->audio->input;

    pads = effect_bulk->audio->input_pads;
  }

  /* collect bulk member */
  task = NULL;
  bulk_member = NULL;

  list = gtk_container_get_children(effect_bulk->table);

  while(list != NULL){
    if(AGS_IS_BULK_MEMBER(list->data)){
      bulk_member = g_list_prepend(bulk_member,
				   list->data);
    }
      
    list = list->next;
  }

  bulk_member = g_list_reverse(bulk_member);

  /* create task */
  update_bulk_member = ags_update_bulk_member_new(effect_bulk,
						  bulk_member,
						  new_size,
						  old_size,
						  FALSE);
  task = g_list_prepend(task,
			update_bulk_member);

  if(new_size > old_size){  
    /* add effect */
    for(i = 0; i < pads; i++){
      current = ags_channel_nth(current,
				old_size);
    
      for(j = old_size; j < new_size; j++){
	effect_bulk_plugin = effect_bulk->plugin;

	while(effect_bulk_plugin != NULL){
	  ags_channel_add_effect(current,
				 AGS_EFFECT_BULK_PLUGIN(effect_bulk_plugin->data)->filename,
				 AGS_EFFECT_BULK_PLUGIN(effect_bulk_plugin->data)->effect);

	  effect_bulk_plugin = effect_bulk_plugin->next;
	}

	current = current->next;
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
  AgsUpdateBulkMember *update_bulk_member;

  AgsChannel *current;

  AgsAudioLoop *audio_loop;
  AgsTaskThread *task_thread;

  GList *task;
  GList *bulk_member;
  GList *effect_bulk_plugin;
  GList *list;
  guint audio_channels;
  guint i, j;

  audio_loop = (AgsAudioLoop *) AGS_MAIN(AGS_DEVOUT(effect_bulk->audio->devout)->ags_main)->main_loop;
  task_thread = (AgsTaskThread *) audio_loop->task_thread;
  
  audio_channels = effect_bulk->audio->audio_channels;
  
  /* retrieve channel */
  if(effect_bulk->channel_type == AGS_TYPE_OUTPUT){
    current = effect_bulk->audio->output;
  }else{
    current = effect_bulk->audio->input;
  }

  /* collect bulk member */
  task = NULL;
  bulk_member = NULL;

  list = gtk_container_get_children(effect_bulk->table);

  while(list != NULL){
    if(AGS_IS_BULK_MEMBER(list->data)){
      bulk_member = g_list_prepend(bulk_member,
				   list->data);
    }
      
    list = list->next;
  }

  bulk_member = g_list_reverse(bulk_member);

  /* create task */
  update_bulk_member = ags_update_bulk_member_new(effect_bulk,
						  bulk_member,
						  new_size,
						  old_size,
						  TRUE);
  task = g_list_prepend(task,
			update_bulk_member);
   
  if(new_size > old_size){ 
    /* add effect */
    current = ags_channel_pad_nth(current,
				  old_size);

    for(i = old_size; i < new_size; i++){
      for(j = 0; j < audio_channels; j++){    
	effect_bulk_plugin = effect_bulk->plugin;

	while(effect_bulk_plugin != NULL){
	  ags_channel_add_effect(current,
				 AGS_EFFECT_BULK_PLUGIN(effect_bulk_plugin->data)->filename,
				 AGS_EFFECT_BULK_PLUGIN(effect_bulk_plugin->data)->effect);

	  effect_bulk_plugin = effect_bulk_plugin->next;
	}

	current = current->next;
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

/**
 * ags_effect_bulk_new:
 * @effect_bulk: the parent effect_bulk
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

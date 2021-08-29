/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/X/ags_effect_bulk.h>
#include <ags/X/ags_effect_bulk_callbacks.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_plugin_browser.h>
#include <ags/X/ags_bulk_member.h>

#include <ags/config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <ladspa.h>
#include <dssi.h>
#include <lv2.h>

#include <ags/i18n.h>

void ags_effect_bulk_class_init(AgsEffectBulkClass *effect_bulk);
void ags_effect_bulk_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_effect_bulk_init(AgsEffectBulk *effect_bulk);
void ags_effect_bulk_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_effect_bulk_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_effect_bulk_dispose(GObject *gobject);
void ags_effect_bulk_finalize(GObject *gobject);

void ags_effect_bulk_connect(AgsConnectable *connectable);
void ags_effect_bulk_disconnect(AgsConnectable *connectable);

void ags_effect_bulk_show(GtkWidget *widget);

void ags_effect_bulk_add_ladspa_plugin(AgsEffectBulk *effect_bulk,
				       GList *control_type_name,
				       AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				       gchar *plugin_name,
				       gchar *filename,
				       gchar *effect,
				       guint start_audio_channel, guint stop_audio_channel,
				       guint start_pad, guint stop_pad,
				       gint position,
				       guint create_flags, guint recall_flags);
void ags_effect_bulk_add_dssi_plugin(AgsEffectBulk *effect_bulk,
				     GList *control_type_name,
				     AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				     gchar *plugin_name,
				     gchar *filename,
				     gchar *effect,
				     guint start_audio_channel, guint stop_audio_channel,
				     guint start_pad, guint stop_pad,
				     gint position,
				     guint create_flags, guint recall_flags);
void ags_effect_bulk_add_lv2_plugin(AgsEffectBulk *effect_bulk,
				    GList *control_type_name,
				    AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				    gchar *plugin_name,
				    gchar *filename,
				    gchar *effect,
				    guint start_audio_channel, guint stop_audio_channel,
				    guint start_pad, guint stop_pad,
				    gint position,
				    guint create_flags, guint recall_flags);

#if defined(AGS_WITH_VST3)
void ags_effect_bulk_add_vst3_plugin(AgsEffectBulk *effect_bulk,
				     GList *control_type_name,
				     AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				     gchar *plugin_name,
				     gchar *filename,
				     gchar *effect,
				     guint start_audio_channel, guint stop_audio_channel,
				     guint start_pad, guint stop_pad,
				     gint position,
				     guint create_flags, guint recall_flags);
#endif

void ags_effect_bulk_real_add_plugin(AgsEffectBulk *effect_bulk,
				     GList *control_type_name,
				     AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				     gchar *plugin_name,
				     gchar *filename,
				     gchar *effect,
				     guint start_audio_channel, guint stop_audio_channel,
				     guint start_pad, guint stop_pad,
				     gint position,
				     guint create_flags, guint recall_flags);
void ags_effect_bulk_real_remove_plugin(AgsEffectBulk *effect_bulk,
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
  ADD_PLUGIN,
  REMOVE_PLUGIN,
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

GHashTable *ags_effect_bulk_indicator_queue_draw = NULL;

GType
ags_effect_bulk_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_effect_bulk = 0;

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

    ags_type_effect_bulk = g_type_register_static(GTK_TYPE_BOX,
						  "AgsEffectBulk", &ags_effect_bulk_info,
						  0);

    g_type_add_interface_static(ags_type_effect_bulk,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_effect_bulk);
  }

  return g_define_type_id__volatile;
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

  gobject->dispose = ags_effect_bulk_dispose;
  gobject->finalize = ags_effect_bulk_finalize;
  
  /* properties */
  /**
   * AgsEffectBulk:audio:
   *
   * The #AgsAudio to visualize.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("audio",
				   i18n_pspec("assigned audio"),
				   i18n_pspec("The audio it is assigned with"),
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
   * Since: 3.0.0
   */
  param_spec = g_param_spec_gtype("channel-type",
				  i18n_pspec("assigned channel type"),
				  i18n_pspec("The channel type it is assigned with"),
				  AGS_TYPE_CHANNEL,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL_TYPE,
				  param_spec);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) effect_bulk;

  widget->show = ags_effect_bulk_show;

  /* AgsEffectBulkClass */
  effect_bulk->add_plugin = ags_effect_bulk_real_add_plugin;
  effect_bulk->remove_plugin = ags_effect_bulk_real_remove_plugin;

  effect_bulk->resize_audio_channels = ags_effect_bulk_real_resize_audio_channels;
  effect_bulk->resize_pads = ags_effect_bulk_real_resize_pads;

  effect_bulk->map_recall = ags_effect_bulk_real_map_recall;
  effect_bulk->find_port = ags_effect_bulk_real_find_port;

  /* signals */
  /**
   * AgsEffectBulk::add-plugin:
   * @effect_bulk: the #AgsEffectBulk to modify
   * @effect: the effect's name
   *
   * The ::add-plugin signal notifies about added effect.
   * 
   * Since: 3.3.0
   */
  effect_bulk_signals[ADD_PLUGIN] =
    g_signal_new("add-plugin",
		 G_TYPE_FROM_CLASS(effect_bulk),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectBulkClass, add_plugin),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__POINTER_OBJECT_OBJECT_STRING_STRING_STRING_UINT_UINT_UINT_UINT_INT_UINT_UINT,
		 G_TYPE_NONE, 13,
		 G_TYPE_POINTER,
		 G_TYPE_OBJECT,
		 G_TYPE_OBJECT,
		 G_TYPE_STRING,
		 G_TYPE_STRING,
		 G_TYPE_STRING,
		 G_TYPE_UINT,
		 G_TYPE_UINT,
		 G_TYPE_UINT,
		 G_TYPE_UINT,
		 G_TYPE_INT,
		 G_TYPE_UINT,
		 G_TYPE_UINT);

  /**
   * AgsEffectBulk::remove-plugin:
   * @effect_bulk: the #AgsEffectBulk to modify
   * @nth: the nth effect
   *
   * The ::remove-plugin signal notifies about removed effect.
   * 
   * Since: 3.3.0
   */
  effect_bulk_signals[REMOVE_PLUGIN] =
    g_signal_new("remove-plugin",
		 G_TYPE_FROM_CLASS(effect_bulk),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectBulkClass, remove_plugin),
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
   * 
   * Since: 3.0.0
   */
  effect_bulk_signals[RESIZE_AUDIO_CHANNELS] = 
    g_signal_new("resize-audio-channels",
		 G_TYPE_FROM_CLASS(effect_bulk),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectBulkClass, resize_audio_channels),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT_UINT,
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
   * 
   * Since: 3.0.0
   */
  effect_bulk_signals[RESIZE_PADS] = 
    g_signal_new("resize_pads",
		 G_TYPE_FROM_CLASS(effect_bulk),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectBulkClass, resize_pads),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT, G_TYPE_UINT);

  /**
   * AgsEffectBulk::map-recall:
   * @effect_bulk: the #AgsEffectBulk
   *
   * The ::map-recall should be used to add the effect_bulk's default recall.
   * 
   * Since: 3.0.0
   */
  effect_bulk_signals[MAP_RECALL] =
    g_signal_new("map-recall",
                 G_TYPE_FROM_CLASS (effect_bulk),
                 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsEffectBulkClass, map_recall),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__UINT,
                 G_TYPE_NONE, 0);

  /**
   * AgsEffectBulk::find-port:
   * @effect_bulk: the #AgsEffectBulk to resize
   *
   * The ::find-port as recall should be mapped
   *
   * Returns: a #GList-struct with associated ports
   * 
   * Since: 3.0.0
   */
  effect_bulk_signals[FIND_PORT] =
    g_signal_new("find-port",
		 G_TYPE_FROM_CLASS(effect_bulk),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectBulkClass, find_port),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__VOID,
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
ags_effect_bulk_init(AgsEffectBulk *effect_bulk)
{
  GtkBox *hbox;

  gtk_orientable_set_orientation(GTK_ORIENTABLE(effect_bulk),
				 GTK_ORIENTATION_VERTICAL);

  if(ags_effect_bulk_indicator_queue_draw == NULL){
    ags_effect_bulk_indicator_queue_draw = g_hash_table_new_full(g_direct_hash, g_direct_equal,
								 NULL,
								 NULL);
  }
  
  effect_bulk->flags = 0;

  effect_bulk->name = NULL;
  
  effect_bulk->version = AGS_EFFECT_BULK_DEFAULT_VERSION;
  effect_bulk->build_id = AGS_EFFECT_BULK_DEFAULT_BUILD_ID;

  effect_bulk->channel_type = G_TYPE_NONE;
  effect_bulk->audio = NULL;

  effect_bulk->plugin = NULL;

  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);
  gtk_widget_set_halign(hbox,
			GTK_ALIGN_END);
  gtk_widget_set_no_show_all((GtkWidget *) hbox,
			     TRUE);
  gtk_box_pack_start((GtkBox *) effect_bulk,
		     (GtkWidget *) hbox,
		     FALSE, FALSE,
		     0);

  effect_bulk->add = (GtkButton *) gtk_button_new_from_icon_name("list-add",
								 GTK_ICON_SIZE_BUTTON);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) effect_bulk->add,
		     FALSE, FALSE,
		     0);
  gtk_widget_show((GtkWidget *) effect_bulk->add);
  
  effect_bulk->remove = (GtkButton *) gtk_button_new_from_icon_name("list-remove",
								    GTK_ICON_SIZE_BUTTON);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) effect_bulk->remove,
		     FALSE, FALSE,
		     0);
  gtk_widget_show((GtkWidget *) effect_bulk->remove);
  
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);
  gtk_box_pack_start((GtkBox *) effect_bulk,
		     (GtkWidget *) hbox,
		     FALSE, FALSE,
		     0);

  effect_bulk->bulk_member = (GtkVBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
						     0);
  gtk_widget_set_no_show_all((GtkWidget *) effect_bulk->bulk_member,
			     TRUE);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) effect_bulk->bulk_member,
		     FALSE, FALSE,
		     0);

  effect_bulk->grid = (GtkGrid *) gtk_grid_new();
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) effect_bulk->grid,
		     FALSE, FALSE,
		     0);

  effect_bulk->plugin_browser = (GtkDialog *) ags_plugin_browser_new((GtkWidget *) effect_bulk);

  effect_bulk->queued_drawing = NULL;
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

      guint output_pads, input_pads;
      
      audio = (AgsAudio *) g_value_get_object(value);

      if(effect_bulk->audio == audio){
	return;
      }

      if(effect_bulk->audio != NULL){
	if((AGS_EFFECT_BULK_CONNECTED & (effect_bulk->flags)) != 0){
	  //TODO:JK: implement me
	}
	
	if(effect_bulk->channel_type == AGS_TYPE_OUTPUT){
	  g_object_get(effect_bulk->audio,
		       "output-pads", &output_pads,
		       NULL);
	  
	  ags_effect_bulk_resize_pads(effect_bulk,
				      0,
				      output_pads);
	}else{
	  g_object_get(effect_bulk->audio,
		       "input-pads", &input_pads,
		       NULL);
	  
	  ags_effect_bulk_resize_pads(effect_bulk,
				      0,
				      input_pads);
	}
	
	g_object_unref(effect_bulk->audio);
      }

      if(audio != NULL){
	g_object_ref(audio);
      }

      effect_bulk->audio = audio;

      if((AGS_EFFECT_BULK_CONNECTED & (effect_bulk->flags)) != 0){
	if(audio != NULL){
	  if(effect_bulk->channel_type == AGS_TYPE_OUTPUT){
	    g_object_get(audio,
			 "output-pads", &output_pads,
			 NULL);
	    
	    ags_effect_bulk_resize_pads(effect_bulk,
					output_pads,
					0);
	  }else{
	    g_object_get(audio,
			 "input-pads", &input_pads,
			 NULL);
	    
	    ags_effect_bulk_resize_pads(effect_bulk,
					input_pads,
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
ags_effect_bulk_dispose(GObject *gobject)
{
  AgsEffectBulk *effect_bulk;
  
  effect_bulk = (AgsEffectBulk *) gobject;

  /* unref audio */
  if(effect_bulk->audio != NULL){
    g_object_unref(effect_bulk->audio);

    effect_bulk->audio = NULL;
  }
  
  /* call parent */  
  G_OBJECT_CLASS(ags_effect_bulk_parent_class)->dispose(gobject);
}

void
ags_effect_bulk_finalize(GObject *gobject)
{
  AgsEffectBulk *effect_bulk;

  GList *list;
  
  effect_bulk = (AgsEffectBulk *) gobject;

  /* unref audio */
  if(effect_bulk->audio != NULL){
    g_object_unref(effect_bulk->audio);
  }

  /* free plugin list */
  g_list_free_full(effect_bulk->plugin,
		   (GDestroyNotify) ags_effect_bulk_plugin_free);

  /* destroy plugin browser */
  gtk_widget_destroy(GTK_WIDGET(effect_bulk->plugin_browser));

  /* remove of the queued drawing hash */
  list = effect_bulk->queued_drawing;

  while(list != NULL){
    g_hash_table_remove(ags_effect_bulk_indicator_queue_draw,
			(GDestroyNotify) list->data);

    list = list->next;
  }
  
  /* call parent */  
  G_OBJECT_CLASS(ags_effect_bulk_parent_class)->finalize(gobject);
}

void
ags_effect_bulk_connect(AgsConnectable *connectable)
{
  AgsMachine *machine;
  AgsEffectBulk *effect_bulk;

  GList *list, *list_start;
  
  effect_bulk = AGS_EFFECT_BULK(connectable);

  if((AGS_EFFECT_BULK_CONNECTED & (effect_bulk->flags)) != 0){
    return;
  }

  effect_bulk->flags |= AGS_EFFECT_BULK_CONNECTED;

  machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) effect_bulk,
						   AGS_TYPE_MACHINE);
  
  g_signal_connect_after(machine, "resize-audio-channels",
			 G_CALLBACK(ags_effect_bulk_resize_audio_channels_callback), effect_bulk);

  g_signal_connect_after(machine, "resize-pads",
			 G_CALLBACK(ags_effect_bulk_resize_pads_callback), effect_bulk);

  /*  */
  g_signal_connect(G_OBJECT(effect_bulk->add), "clicked",
		   G_CALLBACK(ags_effect_bulk_add_callback), effect_bulk);

  g_signal_connect(G_OBJECT(effect_bulk->remove), "clicked",
		   G_CALLBACK(ags_effect_bulk_remove_callback), effect_bulk);

  ags_connectable_connect(AGS_CONNECTABLE(effect_bulk->plugin_browser));

  g_signal_connect(G_OBJECT(effect_bulk->plugin_browser), "response",
		   G_CALLBACK(ags_effect_bulk_plugin_browser_response_callback), effect_bulk);

  list =
    list_start = gtk_container_get_children((GtkContainer *) effect_bulk->grid);

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
  AgsMachine *machine;
  AgsEffectBulk *effect_bulk;

  GList *list, *list_start;

  effect_bulk = AGS_EFFECT_BULK(connectable);

  if((AGS_EFFECT_BULK_CONNECTED & (effect_bulk->flags)) == 0){
    return;
  }

  effect_bulk->flags &= (~AGS_EFFECT_BULK_CONNECTED);

  machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) effect_bulk,
						   AGS_TYPE_MACHINE);

  g_object_disconnect(G_OBJECT(machine),
		      "any_signal::resize-audio-channels",
		      G_CALLBACK(ags_effect_bulk_resize_audio_channels_callback),
		      effect_bulk,
		      "any_signal::resize-pads",
		      G_CALLBACK(ags_effect_bulk_resize_pads_callback),
		      effect_bulk,
		      NULL);

  g_object_disconnect(G_OBJECT(effect_bulk->add),
		      "any_signal::clicked",
		      G_CALLBACK(ags_effect_bulk_add_callback),
		      effect_bulk,
		      NULL);

  g_object_disconnect(G_OBJECT(effect_bulk->remove),
		      "any_signal::clicked",
		      G_CALLBACK(ags_effect_bulk_remove_callback),
		      effect_bulk,
		      NULL);

  ags_connectable_disconnect(AGS_CONNECTABLE(effect_bulk->plugin_browser));

  g_object_disconnect(G_OBJECT(effect_bulk->plugin_browser),
		      "any_signal::response",
		      G_CALLBACK(ags_effect_bulk_plugin_browser_response_callback),
		      effect_bulk,
		      NULL);

  list =
    list_start = gtk_container_get_children((GtkContainer *) effect_bulk->grid);

  while(list != NULL){
    if(AGS_IS_CONNECTABLE(list->data)){
      ags_connectable_disconnect(AGS_CONNECTABLE(list->data));
    }

    list = list->next;
  }

  g_list_free(list_start);
}

void
ags_effect_bulk_show(GtkWidget *widget)
{
  AgsEffectBulk *effect_bulk;
    
  effect_bulk = AGS_EFFECT_BULK(widget);
  
  GTK_WIDGET_CLASS(ags_effect_bulk_parent_class)->show(widget);

  if((AGS_EFFECT_BULK_HIDE_BUTTONS & (effect_bulk->flags)) == 0){
    gtk_widget_show(gtk_widget_get_parent(GTK_WIDGET(effect_bulk->add)));
  }

  if((AGS_EFFECT_BULK_HIDE_ENTRIES & (effect_bulk->flags)) == 0){
    gtk_widget_show((GtkWidget *) effect_bulk->bulk_member);
  }
}

/**
 * ags_effect_bulk_plugin_alloc:
 * @play_container: the #AgsRecallContainer
 * @recall_container: the #AgsRecallContainer
 * @plugin_name: the plugin name
 * @filename: the filename as string
 * @effect: the effect as string
 * 
 * Allocate #AgsEffectBulkPlugin-struct.
 * 
 * Returns: the newly allocated #AgsEffectBulkPlugin-struct
 * 
 * Since: 3.3.0
 */
AgsEffectBulkPlugin*
ags_effect_bulk_plugin_alloc(AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			     gchar *plugin_name,
			     gchar *filename,
			     gchar *effect)
{
  AgsEffectBulkPlugin *effect_bulk_plugin;

  effect_bulk_plugin = (AgsEffectBulkPlugin *) g_malloc(sizeof(AgsEffectBulkPlugin));

  if(play_container != NULL){
    g_object_ref(play_container);
  }

  effect_bulk_plugin->play_container = play_container;

  if(recall_container != NULL){
    g_object_ref(recall_container);
  }
  
  effect_bulk_plugin->recall_container = recall_container;
  
  effect_bulk_plugin->plugin_name = g_strdup(plugin_name);

  effect_bulk_plugin->filename = g_strdup(filename);
  effect_bulk_plugin->effect = g_strdup(effect);

  effect_bulk_plugin->control_type_name = NULL;

  effect_bulk_plugin->control_count = 0;
  
  return(effect_bulk_plugin);
}

/**
 * ags_effect_bulk_plugin_free:
 * @effect_bulk_plugin: the #AgsEffectBulkPlugin-struct
 * 
 * Free @effect_bulk_plugin.
 * 
 * Since: 3.3.0
 */
void
ags_effect_bulk_plugin_free(AgsEffectBulkPlugin *effect_bulk_plugin)
{
  if(effect_bulk_plugin == NULL){
    return;
  }

  if(effect_bulk_plugin->play_container != NULL){
    g_object_unref(effect_bulk_plugin->play_container);
  }

  if(effect_bulk_plugin->recall_container != NULL){
    g_object_unref(effect_bulk_plugin->recall_container);
  }
  
  if(effect_bulk_plugin->filename != NULL){
    g_free(effect_bulk_plugin->filename);
  }

  if(effect_bulk_plugin->effect != NULL){
    g_free(effect_bulk_plugin->effect);
  }

  if(effect_bulk_plugin->control_type_name != NULL){
    g_list_free(effect_bulk_plugin->control_type_name);
  }
  
  g_free(effect_bulk_plugin);
}

void
ags_effect_bulk_add_ladspa_plugin(AgsEffectBulk *effect_bulk,
				  GList *control_type_name,
				  AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				  gchar *plugin_name,
				  gchar *filename,
				  gchar *effect,
				  guint start_audio_channel, guint stop_audio_channel,
				  guint start_pad, guint stop_pad,
				  gint position,
				  guint create_flags, guint recall_flags)
{
  AgsBulkMember *bulk_member;

  AgsEffectBulkPlugin *effect_bulk_plugin;

  AgsLadspaPlugin *ladspa_plugin;

  GList *start_recall, *recall;
  GList *start_list, *list;
  GList *start_plugin_port, *plugin_port;

  guint pads, audio_channels;
  gdouble page, step;
  guint port_count;
  guint control_count;
  
  guint x, y;
  guint k;

  pads = 0;
  audio_channels = 0;
  
  /* alloc effect bulk plugin */
  effect_bulk_plugin = ags_effect_bulk_plugin_alloc(play_container, recall_container,
						    plugin_name,
						    filename,
						    effect);
  effect_bulk_plugin->control_type_name = control_type_name;
  
  effect_bulk->plugin = g_list_append(effect_bulk->plugin,
				      effect_bulk_plugin);

  /* get audio properties */
  g_object_get(effect_bulk->audio,
	       "audio-channels", &audio_channels,
	       NULL);
  
  if(g_type_is_a(effect_bulk->channel_type, AGS_TYPE_OUTPUT)){
    g_object_get(effect_bulk->audio,
		 "output-pads", &pads,
		 NULL);
  }else{
    g_object_get(effect_bulk->audio,
		 "input-pads", &pads,
		 NULL);
  }

  /* load plugin */
  ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(ags_ladspa_manager_get_instance(),
							filename, effect);

  /* ags-fx-ladspa */
  start_recall = ags_fx_factory_create(effect_bulk->audio,
				       effect_bulk_plugin->play_container, effect_bulk_plugin->recall_container,
				       plugin_name,
				       filename,
				       effect,
				       0, audio_channels,
				       0, pads,
				       position,
				       create_flags | (g_type_is_a(effect_bulk->channel_type, AGS_TYPE_OUTPUT) ? AGS_FX_FACTORY_OUTPUT: AGS_FX_FACTORY_INPUT), recall_flags);

  recall = start_recall;

  while(recall != NULL){
    ags_recall_set_behaviour_flags(recall->data, AGS_SOUND_BEHAVIOUR_BULK_MODE);

    recall = recall->next;
  }
  
  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
  
  /* retrieve position within table  */
  x = 0;
  y = 0;
  
  list =
    start_list = gtk_container_get_children(GTK_CONTAINER(effect_bulk->grid));

  while(list != NULL){
    guint top_attach;

    gtk_container_child_get(GTK_CONTAINER(effect_bulk->grid),
			    list->data,
			    "top-attach", &top_attach,
			    NULL);
    
    if(y <= top_attach){
      y = top_attach + 1;
    }

    list = list->next;
  }

  g_list_free(start_list);
  
  /* load ports */
  g_object_get(ladspa_plugin,
	       "plugin-port", &start_plugin_port,
	       NULL);

  plugin_port = start_plugin_port;
  
  port_count = g_list_length(plugin_port);

  control_count = 0;
  
  k = 0;

  while(plugin_port != NULL){
    if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_CONTROL)){
      GtkWidget *child_widget;

      AgsLadspaConversion *ladspa_conversion;

      LADSPA_Data default_value;
      
      GType widget_type;

      gchar *plugin_name;
      gchar *control_port;
      gchar *port_name;

      guint unique_id;
      guint scale_precision;
      guint port_index;
      gdouble step_count;
      gboolean disable_seemless;
      gboolean do_step_conversion;
      
      GRecMutex *plugin_port_mutex;

      control_count++;
      
      disable_seemless = FALSE;
      do_step_conversion = FALSE;
      
      if(x == AGS_EFFECT_BULK_COLUMNS_COUNT){
	x = 0;
	y++;
      }
      
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_TOGGLED)){
	disable_seemless = TRUE;
	
	if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_OUTPUT)){
	  widget_type = AGS_TYPE_LED;
	}else{
	  widget_type = GTK_TYPE_TOGGLE_BUTTON;
	}
      }else{
	if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_OUTPUT)){
	  widget_type = AGS_TYPE_HINDICATOR;
	}else{
	  widget_type = AGS_TYPE_DIAL;
	}
      }
      
      scale_precision = AGS_DIAL_DEFAULT_PRECISION;
      step_count = AGS_LADSPA_CONVERSION_DEFAULT_STEP_COUNT;
      
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_INTEGER)){
	guint scale_steps;
	
	g_object_get(plugin_port->data,
		     "scale-steps", &scale_steps,
		     NULL);

	step_count =
	  scale_precision = (gdouble) scale_steps;
	
	disable_seemless = TRUE;	
      }

      /* get plugin port mutex */
      plugin_port_mutex = AGS_PLUGIN_PORT_GET_OBJ_MUTEX(plugin_port->data);

      /* get port name */
      g_rec_mutex_lock(plugin_port_mutex);

      port_name = g_strdup(AGS_PLUGIN_PORT(plugin_port->data)->port_name);
      port_index = AGS_PLUGIN_PORT(plugin_port->data)->port_index;

      unique_id = ladspa_plugin->unique_id;
      
      g_rec_mutex_unlock(plugin_port_mutex);

      /* add bulk member */
      plugin_name = g_strdup_printf("ladspa-%u",
				    unique_id);
      control_port = g_strdup_printf("%u/%u",
				     k + 1,
				     port_count);
      bulk_member = (AgsBulkMember *) g_object_new(AGS_TYPE_BULK_MEMBER,
						   "widget-type", widget_type,
						   "widget-label", port_name,
						   "play-container", play_container,
						   "recall-container", recall_container,
						   "plugin-name", plugin_name,
						   "filename", filename,
						   "effect", effect,
						   "port-index", port_index,
						   "specifier", port_name,
						   "control-port", control_port,
						   "scale-precision", scale_precision,
						   "step-count", step_count,
						   NULL);

      child_widget = ags_bulk_member_get_widget(bulk_member);

      g_free(plugin_name);
      g_free(control_port);
      g_free(port_name);
      
      /* ladspa conversion */
      ladspa_conversion = NULL;

      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_BOUNDED_BELOW)){
	if(ladspa_conversion == NULL ||
	   !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
	  ladspa_conversion = ags_ladspa_conversion_new();
	}

	ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_BOUNDED_BELOW;
      }

      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_BOUNDED_ABOVE)){
	if(ladspa_conversion == NULL ||
	   !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
	  ladspa_conversion = ags_ladspa_conversion_new();
	}

	ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_BOUNDED_ABOVE;
      }
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_SAMPLERATE)){
	if(ladspa_conversion == NULL ||
	   !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
	  ladspa_conversion = ags_ladspa_conversion_new();
	}

	ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_SAMPLERATE;
      }

      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_LOGARITHMIC)){
	if(ladspa_conversion == NULL ||
	   !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
	  ladspa_conversion = ags_ladspa_conversion_new();
	}
    
	ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_LOGARITHMIC;

	do_step_conversion = TRUE;
      }

      g_object_set(bulk_member,
		   "conversion", ladspa_conversion,
		   NULL);
      
      /* child widget */
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_TOGGLED)){
	bulk_member->port_flags = AGS_BULK_MEMBER_PORT_BOOLEAN;
      }
      
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_INTEGER)){
	bulk_member->port_flags = AGS_BULK_MEMBER_PORT_INTEGER;
      }

      if(AGS_IS_DIAL(child_widget)){
	AgsDial *dial;

	GtkAdjustment *adjustment;

	LADSPA_Data lower_bound, upper_bound;
	gdouble lower, upper;
	gdouble control_value;
	
	dial = (AgsDial *) child_widget;

	if(disable_seemless){
	  dial->flags &= (~AGS_DIAL_SEEMLESS_MODE);
	}

	/* add controls of ports and apply range  */
	g_rec_mutex_lock(plugin_port_mutex);
	
	lower_bound = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->lower_value);
	upper_bound = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->upper_value);

	g_rec_mutex_unlock(plugin_port_mutex);

	if(do_step_conversion){
	  g_object_set(ladspa_conversion,
		       "lower", lower_bound,
		       "upper", upper_bound,
		       NULL);

	  lower = 0.0;
	  upper = AGS_LADSPA_CONVERSION_DEFAULT_STEP_COUNT - 1.0;

#if 0
	  if(!disable_seemless){
	    g_object_get(ladspa_conversion,
			 "step-count", &step_count,
			 NULL);
	  }
#endif
	}else{
	  lower = lower_bound;
	  upper = upper_bound;
	}
	
	adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.1, 0.0);
	g_object_set(dial,
		     "adjustment", adjustment,
		     NULL);
	
	if(upper >= 0.0 && lower >= 0.0){
	  step = (upper - lower) / step_count;
	}else if(upper < 0.0 && lower < 0.0){
	  step = -1.0 * (lower - upper) / step_count;
	}else{
	  step = (upper - lower) / step_count;
	}

	if(step_count > 8){
	  if(upper >= 0.0 && lower >= 0.0){
	    page = (upper - lower) / AGS_DIAL_DEFAULT_PRECISION;
	  }else if(upper < 0.0 && lower < 0.0){
	    page = -1.0 * (lower - upper) / AGS_DIAL_DEFAULT_PRECISION;
	  }else{
	    page = (upper - lower) / AGS_DIAL_DEFAULT_PRECISION;
	  }
	}else{
	  page = step;
	}
	
	gtk_adjustment_set_step_increment(adjustment,
					  step);
	gtk_adjustment_set_page_increment(adjustment,
					  page);
	gtk_adjustment_set_lower(adjustment,
				 lower);
	gtk_adjustment_set_upper(adjustment,
				 upper);

	/* get/set default value */
	g_rec_mutex_lock(plugin_port_mutex);
	
	default_value = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->default_value);

	g_rec_mutex_unlock(plugin_port_mutex);

	control_value = default_value;
	
	if(ladspa_conversion != NULL){
	  control_value = ags_conversion_convert((AgsConversion *) ladspa_conversion,
						 default_value,
						 TRUE);
	}
	
	gtk_adjustment_set_value(adjustment,
				 control_value);
      }else if(AGS_IS_INDICATOR(child_widget) ||
	       AGS_IS_LED(child_widget)){
	g_hash_table_insert(ags_effect_bulk_indicator_queue_draw,
			    child_widget, ags_effect_bulk_indicator_queue_draw_timeout);

	effect_bulk->queued_drawing = g_list_prepend(effect_bulk->queued_drawing,
						     child_widget);

	g_timeout_add(AGS_UI_PROVIDER_DEFAULT_TIMEOUT * 1000.0,
		      (GSourceFunc) ags_effect_bulk_indicator_queue_draw_timeout,
		      (gpointer) child_widget);
      }

#ifdef AGS_DEBUG
      g_message("ladspa bounds: %f %f", lower, upper);
#endif

      gtk_widget_set_halign((GtkWidget *) bulk_member,
			    GTK_ALIGN_FILL);
      gtk_widget_set_valign((GtkWidget *) bulk_member,
			    GTK_ALIGN_FILL);
      
      gtk_grid_attach(effect_bulk->grid,
		      (GtkWidget *) bulk_member,
		      x, y,
		      1, 1);
      ags_connectable_connect(AGS_CONNECTABLE(bulk_member));
      gtk_widget_show_all((GtkWidget *) effect_bulk->grid);

      /* iterate */
      x++;
    }

    /* iterate */
    plugin_port = plugin_port->next;
    k++;
  }

  effect_bulk_plugin->control_count = control_count;
  
  g_list_free_full(start_plugin_port,
		   g_object_unref);
}

void
ags_effect_bulk_add_dssi_plugin(AgsEffectBulk *effect_bulk,
				GList *control_type_name,
				AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				gchar *plugin_name,
				gchar *filename,
				gchar *effect,
				guint start_audio_channel, guint stop_audio_channel,
				guint start_pad, guint stop_pad,
				gint position,
				guint create_flags, guint recall_flags)
{
  AgsBulkMember *bulk_member;
   
  AgsEffectBulkPlugin *effect_bulk_plugin;

  AgsDssiPlugin *dssi_plugin;
  
  GList *start_recall, *recall;
  GList *start_list, *list;
  GList *start_plugin_port, *plugin_port;

  guint unique_id;
  guint pads, audio_channels;
  gdouble page, step;
  guint port_count;
  guint control_count;
  
  guint x, y;
  guint k;
  
  pads = 0;
  audio_channels = 0;

  /* alloc effect bulk plugin */
  effect_bulk_plugin = ags_effect_bulk_plugin_alloc(play_container, recall_container,
						    plugin_name,
						    filename,
						    effect);
  effect_bulk_plugin->control_type_name = control_type_name;
  
  effect_bulk->plugin = g_list_append(effect_bulk->plugin,
				      effect_bulk_plugin);  

  /* get audio properties */
  g_object_get(effect_bulk->audio,
	       "audio-channels", &audio_channels,
	       NULL);
  
  if(effect_bulk->channel_type == AGS_TYPE_OUTPUT){
    g_object_get(effect_bulk->audio,
		 "output-pads", &pads,
		 NULL);
  }else{
    g_object_get(effect_bulk->audio,
		 "input-pads", &pads,
		 NULL);
  }

  /* load plugin */
  dssi_plugin = ags_dssi_manager_find_dssi_plugin(ags_dssi_manager_get_instance(),
						  filename, effect);

  g_object_get(dssi_plugin,
	       "unique-id", &unique_id,
	       NULL);

  /* ags-fx-dssi */
  start_recall = ags_fx_factory_create(effect_bulk->audio,
				       effect_bulk_plugin->play_container, effect_bulk_plugin->recall_container,
				       "ags-fx-dssi",
				       filename,
				       effect,
				       0, audio_channels,
				       0, pads,
				       position,
				       create_flags | (g_type_is_a(effect_bulk->channel_type, AGS_TYPE_OUTPUT) ? AGS_FX_FACTORY_OUTPUT: AGS_FX_FACTORY_INPUT), recall_flags);
  
  recall = start_recall;

  while(recall != NULL){
    ags_recall_set_behaviour_flags(recall->data, AGS_SOUND_BEHAVIOUR_BULK_MODE);

    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
  
  /* retrieve position within table  */
  x = 0;
  y = 0;
  
  list =
    start_list = gtk_container_get_children(GTK_CONTAINER(effect_bulk->grid));

  while(list != NULL){
    guint top_attach;

    gtk_container_child_get(GTK_CONTAINER(effect_bulk->grid),
			    list->data,
			    "top-attach", &top_attach,
			    NULL);
    
    if(y <= top_attach){
      y = top_attach + 1;
    }

    list = list->next;
  }

  g_list_free(start_list);
  
  /* load ports */
  g_object_get(dssi_plugin,
	       "plugin-port", &start_plugin_port,
	       NULL);

  plugin_port = start_plugin_port;

  port_count = g_list_length(start_plugin_port);

  control_count = 0;
  
  k = 0;

  while(plugin_port != NULL){
    if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_CONTROL)){
      GtkWidget *child_widget;

      AgsLadspaConversion *ladspa_conversion;
      
      GType widget_type;

      gchar *plugin_name;
      gchar *port_name;
      gchar *control_port;

      guint scale_precision;
      guint port_index;
      gdouble step_count;
      gboolean disable_seemless;
      gboolean do_step_conversion;

      GRecMutex *plugin_port_mutex;

      control_count++;
      
      disable_seemless = FALSE;
      do_step_conversion = FALSE;
      
      if(x == AGS_EFFECT_BULK_COLUMNS_COUNT){
	x = 0;
	y++;
      }

      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_TOGGLED)){
	disable_seemless = TRUE;
	
	if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_OUTPUT)){
	  widget_type = AGS_TYPE_LED;
	}else{
	  widget_type = GTK_TYPE_TOGGLE_BUTTON;
	}
      }else{
	if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_OUTPUT)){
	  widget_type = AGS_TYPE_HINDICATOR;
	}else{
	  widget_type = AGS_TYPE_DIAL;
	}
      }

      scale_precision = AGS_DIAL_DEFAULT_PRECISION;
      step_count = AGS_LADSPA_CONVERSION_DEFAULT_STEP_COUNT;

      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_INTEGER)){
	guint scale_steps;
	
	g_object_get(plugin_port->data,
		     "scale-steps", &scale_steps,
		     NULL);

	step_count =
	  scale_precision = (gdouble) scale_steps;

	disable_seemless = TRUE;	
      }

      /* get plugin port mutex */
      plugin_port_mutex = AGS_PLUGIN_PORT_GET_OBJ_MUTEX(plugin_port->data);

      /* get port name */
      g_rec_mutex_lock(plugin_port_mutex);

      port_name = g_strdup(AGS_PLUGIN_PORT(plugin_port->data)->port_name);
      port_index = AGS_PLUGIN_PORT(plugin_port->data)->port_index;
	
      g_rec_mutex_unlock(plugin_port_mutex);

      /* add bulk member */
      plugin_name = g_strdup_printf("dssi-%u",
				    unique_id);
      control_port = g_strdup_printf("%u/%u",
				     k + 1,
				     port_count);
      bulk_member = (AgsBulkMember *) g_object_new(AGS_TYPE_BULK_MEMBER,
						   "widget-type", widget_type,
						   "widget-label", port_name,
						   "play-container", play_container,
						   "recall-container", recall_container,
						   "plugin-name", plugin_name,
						   "filename", filename,
						   "effect", effect,
						   "port-index", port_index,
						   "specifier", port_name,
						   "control-port", control_port,
						   "scale-precision", scale_precision,
						   "step-count", step_count,
						   NULL);
      child_widget = ags_bulk_member_get_widget(bulk_member);

      g_free(plugin_name);
      g_free(control_port);
      g_free(port_name);

      /* ladspa conversion */
      ladspa_conversion = NULL;

      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_BOUNDED_BELOW)){
	if(ladspa_conversion == NULL ||
	   !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
	  ladspa_conversion = ags_ladspa_conversion_new();
	}

	ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_BOUNDED_BELOW;
      }

      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_BOUNDED_ABOVE)){
	if(ladspa_conversion == NULL ||
	   !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
	  ladspa_conversion = ags_ladspa_conversion_new();
	}

	ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_BOUNDED_ABOVE;
      }
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_SAMPLERATE)){
	if(ladspa_conversion == NULL ||
	   !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
	  ladspa_conversion = ags_ladspa_conversion_new();
	}

	ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_SAMPLERATE;
      }

      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_LOGARITHMIC)){
	if(ladspa_conversion == NULL ||
	   !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
	  ladspa_conversion = ags_ladspa_conversion_new();
	}
    
	ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_LOGARITHMIC;

	do_step_conversion = TRUE;
      }

      g_object_set(bulk_member,
		   "conversion", ladspa_conversion,
		   NULL);

      /* child widget */
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_TOGGLED)){
	bulk_member->port_flags = AGS_BULK_MEMBER_PORT_BOOLEAN;
      }
      
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_INTEGER)){
	bulk_member->port_flags = AGS_BULK_MEMBER_PORT_INTEGER;
      }

      if(AGS_IS_DIAL(child_widget)){
	AgsDial *dial;
	GtkAdjustment *adjustment;

	LADSPA_Data lower_bound, upper_bound;
	gdouble lower, upper;
	LADSPA_Data default_value;
	gdouble control_value;
	
	dial = (AgsDial *) child_widget;

	if(disable_seemless){
	  dial->flags &= (~AGS_DIAL_SEEMLESS_MODE);
	}

	/* add controls of ports and apply range  */
	g_rec_mutex_lock(plugin_port_mutex);
	
	lower_bound = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->lower_value);
	upper_bound = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->upper_value);

	g_rec_mutex_unlock(plugin_port_mutex);

	if(do_step_conversion){
	  g_object_set(ladspa_conversion,
		       "lower", lower_bound,
		       "upper", upper_bound,
		       NULL);

	  lower = 0.0;
	  upper = AGS_LADSPA_CONVERSION_DEFAULT_STEP_COUNT - 1.0;

#if 0
	  if(!disable_seemless){
	    g_object_get(ladspa_conversion,
			 "step-count", &step_count,
			 NULL);
	  }
#endif
	}else{
	  lower = lower_bound;
	  upper = upper_bound;
	}
	
	adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.1, 0.0);
	g_object_set(dial,
		     "adjustment", adjustment,
		     NULL);

	if(upper >= 0.0 && lower >= 0.0){
	  step = (upper - lower) / step_count;
	}else if(upper < 0.0 && lower < 0.0){
	  step = -1.0 * (lower - upper) / step_count;
	}else{
	  step = (upper - lower) / step_count;
	}

	if(step_count > 8){
	  if(upper >= 0.0 && lower >= 0.0){
	    page = (upper - lower) / AGS_DIAL_DEFAULT_PRECISION;
	  }else if(upper < 0.0 && lower < 0.0){
	    page = -1.0 * (lower - upper) / AGS_DIAL_DEFAULT_PRECISION;
	  }else{
	    page = (upper - lower) / AGS_DIAL_DEFAULT_PRECISION;
	  }
	}else{
	  page = step;
	}

	gtk_adjustment_set_step_increment(adjustment,
					  step);
	gtk_adjustment_set_page_increment(adjustment,
					  page);
	gtk_adjustment_set_lower(adjustment,
				 lower);
	gtk_adjustment_set_upper(adjustment,
				 upper);

	g_rec_mutex_lock(plugin_port_mutex);
	
	default_value = (LADSPA_Data) g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->default_value);

	g_rec_mutex_unlock(plugin_port_mutex);

	control_value = default_value;
	
	if(ladspa_conversion != NULL){
	  control_value = ags_conversion_convert((AgsConversion *) ladspa_conversion,
						 default_value,
						 TRUE);
	}
	
	gtk_adjustment_set_value(adjustment,
				 control_value);

#ifdef AGS_DEBUG
	g_message("dssi bounds: %f %f", lower, upper);
#endif
      }else if(AGS_IS_INDICATOR(child_widget) ||
	       AGS_IS_LED(child_widget)){
	g_hash_table_insert(ags_effect_bulk_indicator_queue_draw,
			    child_widget, ags_effect_bulk_indicator_queue_draw_timeout);
	effect_bulk->queued_drawing = g_list_prepend(effect_bulk->queued_drawing,
						     child_widget);
	g_timeout_add(AGS_UI_PROVIDER_DEFAULT_TIMEOUT * 1000.0, (GSourceFunc) ags_effect_bulk_indicator_queue_draw_timeout, (gpointer) child_widget);
      }

      gtk_widget_set_halign(bulk_member,
			    GTK_ALIGN_FILL);
      gtk_widget_set_valign(bulk_member,
			    GTK_ALIGN_FILL);

      gtk_grid_attach(effect_bulk->grid,
		      (GtkWidget *) bulk_member,
		      x, y,
		      1, 1);
      ags_connectable_connect(AGS_CONNECTABLE(bulk_member));
      gtk_widget_show_all((GtkWidget *) effect_bulk->grid);

      /* iterate */
      x++;
    }

    /* iterate */
    plugin_port = plugin_port->next;
    k++;
  }

  effect_bulk_plugin->control_count = control_count;
  
  g_list_free_full(start_plugin_port,
		   g_object_unref);
}

void
ags_effect_bulk_add_lv2_plugin(AgsEffectBulk *effect_bulk,
			       GList *control_type_name,
			       AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			       gchar *plugin_name,
			       gchar *filename,
			       gchar *effect,
			       guint start_audio_channel, guint stop_audio_channel,
			       guint start_pad, guint stop_pad,
			       gint position,
			       guint create_flags, guint recall_flags)
{
  AgsBulkMember *bulk_member;
  
  AgsEffectBulkPlugin *effect_bulk_plugin;

  AgsLv2Manager *lv2_manager;
  AgsLv2Plugin *lv2_plugin;
  
  GList *start_recall, *recall;
  GList *start_list, *list;
  GList *start_plugin_port, *plugin_port;

  gchar *uri;
  gchar *port_name;

  gboolean is_lv2_plugin;
  
  gdouble page, step;
  guint pads, audio_channels;
  guint port_count;
  guint control_count;
  
  guint x, y;
  guint k;
  
  GRecMutex *lv2_manager_mutex;
  GRecMutex *base_plugin_mutex;

  lv2_manager = ags_lv2_manager_get_instance();
    
  lv2_manager_mutex = AGS_LV2_MANAGER_GET_OBJ_MUTEX(lv2_manager);

  pads = 0;
  audio_channels = 0;

  /* make sure turtle is parsed */
  g_rec_mutex_lock(lv2_manager_mutex);
	      
  is_lv2_plugin = ((lv2_manager->quick_scan_plugin_filename != NULL &&
		    g_strv_contains(lv2_manager->quick_scan_plugin_filename,
				    filename)) ||
		   (lv2_manager->quick_scan_instrument_filename != NULL &&
		    g_strv_contains(lv2_manager->quick_scan_instrument_filename,
				    filename))) ? TRUE: FALSE;
	      
  g_rec_mutex_unlock(lv2_manager_mutex);

  if(filename != NULL &&
     effect != NULL &&
     is_lv2_plugin){
    AgsTurtle *manifest;
    AgsTurtleManager *turtle_manager;
    
    gchar *path;
    gchar *manifest_filename;

    turtle_manager = ags_turtle_manager_get_instance();
    
    path = g_path_get_dirname(filename);

    manifest_filename = g_strdup_printf("%s%c%s",
					path,
					G_DIR_SEPARATOR,
					"manifest.ttl");

    manifest = (AgsTurtle *) ags_turtle_manager_find(turtle_manager,
						     manifest_filename);

    if(manifest == NULL){
      AgsLv2TurtleParser *lv2_turtle_parser;
	
      AgsTurtle **turtle;

      guint n_turtle;

      g_message("new turtle [Manifest] - %s", manifest_filename);
	
      manifest = ags_turtle_new(manifest_filename);
      ags_turtle_load(manifest,
		      NULL);
      ags_turtle_manager_add(turtle_manager,
			     (GObject *) manifest);

      lv2_turtle_parser = ags_lv2_turtle_parser_new(manifest);

      n_turtle = 1;
      turtle = (AgsTurtle **) malloc(2 * sizeof(AgsTurtle *));

      turtle[0] = manifest;
      turtle[1] = NULL;
	
      ags_lv2_turtle_parser_parse(lv2_turtle_parser,
				  turtle, n_turtle);
    
      g_object_run_dispose((GObject *) lv2_turtle_parser);
      g_object_unref(lv2_turtle_parser);
	
      g_object_unref(manifest);
	
      free(turtle);
    }
    
    g_free(manifest_filename);
  }
  
  /* alloc effect bulk plugin */
  effect_bulk_plugin = ags_effect_bulk_plugin_alloc(play_container, recall_container,
						    plugin_name,
						    filename,
						    effect);
  effect_bulk_plugin->control_type_name = control_type_name;
  
  effect_bulk->plugin = g_list_append(effect_bulk->plugin,
				      effect_bulk_plugin);  

  /* get audio properties */
  g_object_get(effect_bulk->audio,
	       "audio-channels", &audio_channels,
	       NULL);

  if(effect_bulk->channel_type == AGS_TYPE_OUTPUT){
    g_object_get(effect_bulk->audio,
		 "output-pads", &pads,
		 NULL);
  }else{
    g_object_get(effect_bulk->audio,
		 "input-pads", &pads,
		 NULL);
  }
  
  /* load plugin */
  lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
					       filename, effect);

  /* get base plugin mutex */
  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(lv2_plugin);

  /* get uri */
  g_rec_mutex_lock(base_plugin_mutex);

  uri = g_strdup(lv2_plugin->uri);
  
  g_rec_mutex_unlock(base_plugin_mutex);

  /* ags-fx-lv2 */
  start_recall = ags_fx_factory_create(effect_bulk->audio,
				       effect_bulk_plugin->play_container, effect_bulk_plugin->recall_container,
				       plugin_name,
				       filename,
				       effect,
				       0, audio_channels,
				       0, pads,
				       position,
				       create_flags | (g_type_is_a(effect_bulk->channel_type, AGS_TYPE_OUTPUT) ? AGS_FX_FACTORY_OUTPUT: AGS_FX_FACTORY_INPUT), recall_flags);

  recall = start_recall;

  while(recall != NULL){
    ags_recall_set_behaviour_flags(recall->data, AGS_SOUND_BEHAVIOUR_BULK_MODE);

    recall = recall->next;
  }
  
  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* retrieve position within table  */
  x = 0;
  y = 0;
  
  list =
    start_list = gtk_container_get_children(GTK_CONTAINER(effect_bulk->grid));

  while(list != NULL){
    guint top_attach;

    gtk_container_child_get(GTK_CONTAINER(effect_bulk->grid),
			    list->data,
			    "top-attach", &top_attach,
			    NULL);
    
    if(y <= top_attach){
      y = top_attach + 1;
    }

    list = list->next;
  }

  g_list_free(start_list);

  /* load ports */
  g_object_get(lv2_plugin,
	       "plugin-port", &start_plugin_port,
	       NULL);

  plugin_port = start_plugin_port;

  port_count = g_list_length(plugin_port);

  control_count = 0;
  
  k = 0;

  while(plugin_port != NULL){
    if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_CONTROL)){
      GtkWidget *child_widget;

      AgsLv2Conversion *lv2_conversion;
      
      GType widget_type;

      gchar *plugin_name;
      gchar *control_port;

      guint port_index;
      guint scale_precision;
      gdouble step_count;
      gboolean disable_seemless;
      gboolean do_step_conversion;

      GRecMutex *plugin_port_mutex;

      control_count++;
      
      disable_seemless = FALSE;
      do_step_conversion = FALSE;
            
      if(x == AGS_EFFECT_BULK_COLUMNS_COUNT){
	x = 0;
	y++;
      }

      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_TOGGLED)){
	disable_seemless = TRUE;
	
	if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_OUTPUT)){
	  widget_type = AGS_TYPE_LED;
	}else{
	  widget_type = GTK_TYPE_TOGGLE_BUTTON;
	}
      }else{
	if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_OUTPUT)){
	  widget_type = AGS_TYPE_HINDICATOR;
	}else{
	  widget_type = AGS_TYPE_DIAL;
	}
      }

      scale_precision = AGS_DIAL_DEFAULT_PRECISION;
      step_count = AGS_LV2_CONVERSION_DEFAULT_STEP_COUNT;

      if((AGS_PLUGIN_PORT_INTEGER & (AGS_PLUGIN_PORT(plugin_port->data)->flags)) != 0){
	guint scale_steps;
	
	g_object_get(plugin_port->data,
		     "scale-steps", &scale_steps,
		     NULL);

	step_count =
	  scale_precision = (gdouble) scale_steps;

	disable_seemless = TRUE;	
      }

      /* get plugin port mutex */
      plugin_port_mutex = AGS_PLUGIN_PORT_GET_OBJ_MUTEX(plugin_port->data);

      /* get port name */
      g_rec_mutex_lock(plugin_port_mutex);

      port_name = g_strdup(AGS_PLUGIN_PORT(plugin_port->data)->port_name);
      port_index = AGS_PLUGIN_PORT(plugin_port->data)->port_index;
      
      g_rec_mutex_unlock(plugin_port_mutex);

      /* add bulk member */
      plugin_name = g_strdup_printf("lv2-<%s>",
				    uri);

      control_port = g_strdup_printf("%u/%u",
				     k + 1,
				     port_count);

      bulk_member = (AgsBulkMember *) g_object_new(AGS_TYPE_BULK_MEMBER,
						   "widget-type", widget_type,
						   "widget-label", port_name,
						   "play-container", play_container,
						   "recall-container", recall_container,
						   "plugin-name", plugin_name,
						   "filename", filename,
						   "effect", effect,
						   "specifier", port_name,
						   "port-index", port_index,
						   "control-port", control_port,
						   "scale-precision", scale_precision,
						   "step-count", step_count,
						   NULL);
      child_widget = ags_bulk_member_get_widget(bulk_member);

      g_free(plugin_name);
      g_free(control_port);
      g_free(port_name);

      /* lv2 conversion */
      lv2_conversion = NULL;

      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_LOGARITHMIC)){
	if(lv2_conversion == NULL ||
	   !AGS_IS_LV2_CONVERSION(lv2_conversion)){
	  lv2_conversion = ags_lv2_conversion_new();
	}
    
	lv2_conversion->flags |= AGS_LV2_CONVERSION_LOGARITHMIC;

	do_step_conversion = TRUE;
      }

      g_object_set(bulk_member,
		   "conversion", lv2_conversion,
		   NULL);

      /* child widget */
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_TOGGLED)){
	bulk_member->port_flags = AGS_BULK_MEMBER_PORT_BOOLEAN;
      }
      
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_INTEGER)){
	bulk_member->port_flags = AGS_BULK_MEMBER_PORT_INTEGER;
      }

      if(AGS_IS_DIAL(child_widget)){
	AgsDial *dial;
	GtkAdjustment *adjustment;

	float lower_bound, upper_bound;
	gdouble lower, upper;
	float default_value;
	gdouble control_value;
	
	dial = (AgsDial *) child_widget;

	if(disable_seemless){
	  dial->flags &= (~AGS_DIAL_SEEMLESS_MODE);
	}

	/* add controls of ports and apply range  */
	g_rec_mutex_lock(plugin_port_mutex);
	
	lower_bound = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->lower_value);
	upper_bound = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->upper_value);

	g_rec_mutex_unlock(plugin_port_mutex);

	if(do_step_conversion){
	  g_object_set(lv2_conversion,
		       "lower", lower_bound,
		       "upper", upper_bound,
		       NULL);

	  lower = 0.0;
	  upper = AGS_LV2_CONVERSION_DEFAULT_STEP_COUNT - 1.0;
	  
#if 0
	  if(!disable_seemless){
	    g_object_get(lv2_conversion,
			 "step-count", &step_count,
			 NULL);
	  }
#endif
	}else{
	  lower = lower_bound;
	  upper = upper_bound;
	}
	
	adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.1, 0.0);
	g_object_set(dial,
		     "adjustment", adjustment,
		     NULL);

	if(upper >= 0.0 && lower >= 0.0){
	  step = (upper - lower) / step_count;
	}else if(upper < 0.0 && lower < 0.0){
	  step = -1.0 * (lower - upper) / step_count;
	}else{
	  step = (upper - lower) / step_count;
	}	

	if(step_count > 8){
	  if(upper >= 0.0 && lower >= 0.0){
	    page = (upper - lower) / AGS_DIAL_DEFAULT_PRECISION;
	  }else if(upper < 0.0 && lower < 0.0){
	    page = -1.0 * (lower - upper) / AGS_DIAL_DEFAULT_PRECISION;
	  }else{
	    page = (upper - lower) / AGS_DIAL_DEFAULT_PRECISION;
	  }
	}else{
	  page = step;
	}
	
	gtk_adjustment_set_step_increment(adjustment,
					  step);
	gtk_adjustment_set_page_increment(adjustment,
					  page);
	gtk_adjustment_set_lower(adjustment,
				 lower);
	gtk_adjustment_set_upper(adjustment,
				 upper);

	/* get/set default value */
	g_rec_mutex_lock(plugin_port_mutex);
	
	default_value = (float) g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->default_value);

	g_rec_mutex_unlock(plugin_port_mutex);

	control_value = default_value;
	
	if(lv2_conversion != NULL){
	  control_value = ags_conversion_convert((AgsConversion *) lv2_conversion,
						 default_value,
						 TRUE);
	}

	gtk_adjustment_set_value(adjustment,
				 control_value);
      }else if(AGS_IS_INDICATOR(child_widget) ||
	       AGS_IS_LED(child_widget)){
	g_hash_table_insert(ags_effect_bulk_indicator_queue_draw,
			    child_widget, ags_effect_bulk_indicator_queue_draw_timeout);

	effect_bulk->queued_drawing = g_list_prepend(effect_bulk->queued_drawing,
						     child_widget);

	g_timeout_add(AGS_UI_PROVIDER_DEFAULT_TIMEOUT * 1000.0,
		      (GSourceFunc) ags_effect_bulk_indicator_queue_draw_timeout,
		      (gpointer) child_widget);
      }

#ifdef AGS_DEBUG
      g_message("lv2 bounds: %f %f", lower, upper);
#endif

      gtk_widget_set_halign(bulk_member,
			    GTK_ALIGN_FILL);
      gtk_widget_set_valign(bulk_member,
			    GTK_ALIGN_FILL);
      
      gtk_grid_attach(effect_bulk->grid,
		      (GtkWidget *) bulk_member,
		      x, y,
		      1, 1);
      ags_connectable_connect(AGS_CONNECTABLE(bulk_member));
      gtk_widget_show_all((GtkWidget *) effect_bulk->grid);

      /* iterate */
      x++;
    }

    /* iterate */
    plugin_port = plugin_port->next;    
    k++;
  }

  effect_bulk_plugin->control_count = control_count;

  g_list_free_full(start_plugin_port,
		   g_object_unref);

  g_free(uri);
}

#if defined(AGS_WITH_VST3)
void
ags_effect_bulk_add_vst3_plugin(AgsEffectBulk *effect_bulk,
				GList *control_type_name,
				AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				gchar *plugin_name,
				gchar *filename,
				gchar *effect,
				guint start_audio_channel, guint stop_audio_channel,
				guint start_pad, guint stop_pad,
				gint position,
				guint create_flags, guint recall_flags)
{
  AgsBulkMember *bulk_member;
  
  AgsEffectBulkPlugin *effect_bulk_plugin;

  AgsVst3Manager *vst3_manager;
  AgsVst3Plugin *vst3_plugin;
  
  GList *start_recall, *recall;
  GList *start_list, *list;
  GList *start_plugin_port, *plugin_port;

  gchar *port_name;
  
  gdouble page, step;
  guint pads, audio_channels;
  guint port_count;
  guint control_count;
  
  guint x, y;
  guint k;
  
  GRecMutex *vst3_manager_mutex;
  GRecMutex *base_plugin_mutex;

  vst3_manager = ags_vst3_manager_get_instance();
    
  vst3_manager_mutex = AGS_VST3_MANAGER_GET_OBJ_MUTEX(vst3_manager);

  pads = 0;
  audio_channels = 0;
  
  /* alloc effect bulk plugin */
  effect_bulk_plugin = ags_effect_bulk_plugin_alloc(play_container, recall_container,
						    plugin_name,
						    filename,
						    effect);
  effect_bulk_plugin->control_type_name = control_type_name;
  
  effect_bulk->plugin = g_list_append(effect_bulk->plugin,
				      effect_bulk_plugin);  

  /* get audio properties */
  g_object_get(effect_bulk->audio,
	       "audio-channels", &audio_channels,
	       NULL);

  if(effect_bulk->channel_type == AGS_TYPE_OUTPUT){
    g_object_get(effect_bulk->audio,
		 "output-pads", &pads,
		 NULL);
  }else{
    g_object_get(effect_bulk->audio,
		 "input-pads", &pads,
		 NULL);
  }
  
  /* load plugin */
  vst3_plugin = ags_vst3_manager_find_vst3_plugin(ags_vst3_manager_get_instance(),
					       filename, effect);

  /* get base plugin mutex */
  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(vst3_plugin);

  /* ags-fx-vst3 */
  start_recall = ags_fx_factory_create(effect_bulk->audio,
				       effect_bulk_plugin->play_container, effect_bulk_plugin->recall_container,
				       plugin_name,
				       filename,
				       effect,
				       0, audio_channels,
				       0, pads,
				       position,
				       create_flags | (g_type_is_a(effect_bulk->channel_type, AGS_TYPE_OUTPUT) ? AGS_FX_FACTORY_OUTPUT: AGS_FX_FACTORY_INPUT), recall_flags);

  recall = start_recall;

  while(recall != NULL){
    ags_recall_set_behaviour_flags(recall->data, AGS_SOUND_BEHAVIOUR_BULK_MODE);

    recall = recall->next;
  }
  
  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* retrieve position within table  */
  x = 0;
  y = 0;
  
  list =
    start_list = gtk_container_get_children(GTK_CONTAINER(effect_bulk->grid));

  while(list != NULL){
    guint top_attach;

    gtk_container_child_get(GTK_CONTAINER(effect_bulk->grid),
			    list->data,
			    "top-attach", &top_attach,
			    NULL);
    
    if(y <= top_attach){
      y = top_attach + 1;
    }

    list = list->next;
  }

  g_list_free(start_list);

  /* load ports */
  g_object_get(vst3_plugin,
	       "plugin-port", &start_plugin_port,
	       NULL);

  plugin_port = start_plugin_port;

  port_count = g_list_length(plugin_port);

  control_count = 0;
  
  k = 0;

  while(plugin_port != NULL){
    if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_CONTROL)){
      GtkWidget *child_widget;

      AgsVst3Conversion *vst3_conversion;
      
      GType widget_type;

      gchar *plugin_name;
      gchar *control_port;

      guint port_index;
      guint scale_precision;
      gdouble step_count;
      gboolean disable_seemless;
      gboolean do_step_conversion;

      GRecMutex *plugin_port_mutex;

      control_count++;
      
      disable_seemless = FALSE;
      do_step_conversion = FALSE;
            
      if(x == AGS_EFFECT_BULK_COLUMNS_COUNT){
	x = 0;
	y++;
      }

      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_TOGGLED)){
	disable_seemless = TRUE;
	
	if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_OUTPUT)){
	  widget_type = AGS_TYPE_LED;
	}else{
	  widget_type = GTK_TYPE_TOGGLE_BUTTON;
	}
      }else{
	if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_OUTPUT)){
	  widget_type = AGS_TYPE_HINDICATOR;
	}else{
	  widget_type = AGS_TYPE_DIAL;
	}
      }

      scale_precision = AGS_DIAL_DEFAULT_PRECISION;
      step_count = AGS_VST3_CONVERSION_DEFAULT_STEP_COUNT;

      if((AGS_PLUGIN_PORT_INTEGER & (AGS_PLUGIN_PORT(plugin_port->data)->flags)) != 0){
	guint scale_steps;
	
	g_object_get(plugin_port->data,
		     "scale-steps", &scale_steps,
		     NULL);

	step_count =
	  scale_precision = (gdouble) scale_steps;

	disable_seemless = TRUE;	
      }

      /* get plugin port mutex */
      plugin_port_mutex = AGS_PLUGIN_PORT_GET_OBJ_MUTEX(plugin_port->data);

      /* get port name */
      g_rec_mutex_lock(plugin_port_mutex);

      port_name = g_strdup(AGS_PLUGIN_PORT(plugin_port->data)->port_name);
      port_index = AGS_PLUGIN_PORT(plugin_port->data)->port_index;
      
      g_rec_mutex_unlock(plugin_port_mutex);

      /* add bulk member */
      plugin_name = g_strdup_printf("vst3-<%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x>",
				    vst3_plugin->cid[0],
				    vst3_plugin->cid[1],
				    vst3_plugin->cid[2],
				    vst3_plugin->cid[3],
				    vst3_plugin->cid[4],
				    vst3_plugin->cid[5],
				    vst3_plugin->cid[6],
				    vst3_plugin->cid[7],
				    vst3_plugin->cid[8],
				    vst3_plugin->cid[9],
				    vst3_plugin->cid[10],
				    vst3_plugin->cid[11],
				    vst3_plugin->cid[12],
				    vst3_plugin->cid[13],
				    vst3_plugin->cid[14],
				    vst3_plugin->cid[15]);

      control_port = g_strdup_printf("%u/%u",
				     k + 1,
				     port_count);

      bulk_member = (AgsBulkMember *) g_object_new(AGS_TYPE_BULK_MEMBER,
						   "widget-type", widget_type,
						   "widget-label", port_name,
						   "play-container", play_container,
						   "recall-container", recall_container,
						   "plugin-name", plugin_name,
						   "filename", filename,
						   "effect", effect,
						   "specifier", port_name,
						   "port-index", port_index,
						   "control-port", control_port,
						   "scale-precision", scale_precision,
						   "step-count", step_count,
						   NULL);
      child_widget = ags_bulk_member_get_widget(bulk_member);

      g_free(plugin_name);
      g_free(control_port);
      g_free(port_name);

      /* vst3 conversion */
      vst3_conversion = NULL;

      g_object_set(bulk_member,
		   "conversion", vst3_conversion,
		   NULL);

      /* child widget */
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_TOGGLED)){
	bulk_member->port_flags = AGS_BULK_MEMBER_PORT_BOOLEAN;
      }
      
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_INTEGER)){
	bulk_member->port_flags = AGS_BULK_MEMBER_PORT_INTEGER;
      }

      if(AGS_IS_DIAL(child_widget)){
	AgsDial *dial;
	GtkAdjustment *adjustment;

	float lower_bound, upper_bound;
	gdouble lower, upper;
	float default_value;
	gdouble control_value;
	
	dial = (AgsDial *) child_widget;

	if(disable_seemless){
	  dial->flags &= (~AGS_DIAL_SEEMLESS_MODE);
	}

	/* add controls of ports and apply range  */
	g_rec_mutex_lock(plugin_port_mutex);
	
	lower_bound = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->lower_value);
	upper_bound = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->upper_value);

	g_rec_mutex_unlock(plugin_port_mutex);

	if(do_step_conversion){
	  g_object_set(vst3_conversion,
		       "lower", lower_bound,
		       "upper", upper_bound,
		       NULL);

	  lower = 0.0;
	  upper = AGS_VST3_CONVERSION_DEFAULT_STEP_COUNT - 1.0;
	  
#if 0
	  if(!disable_seemless){
	    g_object_get(vst3_conversion,
			 "step-count", &step_count,
			 NULL);
	  }
#endif
	}else{
	  lower = lower_bound;
	  upper = upper_bound;
	}
	
	adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.1, 0.0);
	g_object_set(dial,
		     "adjustment", adjustment,
		     NULL);

	if(upper >= 0.0 && lower >= 0.0){
	  step = (upper - lower) / step_count;
	}else if(upper < 0.0 && lower < 0.0){
	  step = -1.0 * (lower - upper) / step_count;
	}else{
	  step = (upper - lower) / step_count;
	}	

	if(step_count > 8){
	  if(upper >= 0.0 && lower >= 0.0){
	    page = (upper - lower) / AGS_DIAL_DEFAULT_PRECISION;
	  }else if(upper < 0.0 && lower < 0.0){
	    page = -1.0 * (lower - upper) / AGS_DIAL_DEFAULT_PRECISION;
	  }else{
	    page = (upper - lower) / AGS_DIAL_DEFAULT_PRECISION;
	  }
	}else{
	  page = step;
	}
	
	gtk_adjustment_set_step_increment(adjustment,
					  step);
	gtk_adjustment_set_page_increment(adjustment,
					  page);
	gtk_adjustment_set_lower(adjustment,
				 lower);
	gtk_adjustment_set_upper(adjustment,
				 upper);

	/* get/set default value */
	g_rec_mutex_lock(plugin_port_mutex);
	
	default_value = (float) g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->default_value);

	g_rec_mutex_unlock(plugin_port_mutex);

	control_value = default_value;
	
	if(vst3_conversion != NULL){
	  control_value = ags_conversion_convert((AgsConversion *) vst3_conversion,
						 default_value,
						 TRUE);
	}

	gtk_adjustment_set_value(adjustment,
				 control_value);
      }else if(AGS_IS_INDICATOR(child_widget) ||
	       AGS_IS_LED(child_widget)){
	g_hash_table_insert(ags_effect_bulk_indicator_queue_draw,
			    child_widget, ags_effect_bulk_indicator_queue_draw_timeout);

	effect_bulk->queued_drawing = g_list_prepend(effect_bulk->queued_drawing,
						     child_widget);

	g_timeout_add(AGS_UI_PROVIDER_DEFAULT_TIMEOUT * 1000.0,
		      (GSourceFunc) ags_effect_bulk_indicator_queue_draw_timeout,
		      (gpointer) child_widget);
      }

#ifdef AGS_DEBUG
      g_message("vst3 bounds: %f %f", lower, upper);
#endif

      gtk_widget_set_halign(bulk_member,
			    GTK_ALIGN_FILL);
      gtk_widget_set_valign(bulk_member,
			    GTK_ALIGN_FILL);
      
      gtk_grid_attach(effect_bulk->grid,
		      (GtkWidget *) bulk_member,
		      x, y,
		      1, 1);
      ags_connectable_connect(AGS_CONNECTABLE(bulk_member));
      gtk_widget_show_all((GtkWidget *) effect_bulk->grid);

      /* iterate */
      x++;
    }

    /* iterate */
    plugin_port = plugin_port->next;    
    k++;
  }

  effect_bulk_plugin->control_count = control_count;

  g_list_free_full(start_plugin_port,
		   g_object_unref);
}
#endif

void
ags_effect_bulk_real_add_plugin(AgsEffectBulk *effect_bulk,
				GList *control_type_name,
				AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				gchar *plugin_name,
				gchar *filename,
				gchar *effect,
				guint start_audio_channel, guint stop_audio_channel,
				guint start_pad, guint stop_pad,
				gint position,
				guint create_flags, guint recall_flags)
{
  AgsBasePlugin *base_plugin;

  gchar *fallback_filename;
  
  base_plugin = NULL;

  fallback_filename = NULL;
  
  if(!g_ascii_strncasecmp(plugin_name,
			  "ags-fx-ladspa",
			  14)){   
    base_plugin = (AgsBasePlugin *) ags_ladspa_manager_find_ladspa_plugin_with_fallback(ags_ladspa_manager_get_instance(),
											filename, effect);
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-dssi",
				12)){
    base_plugin = (AgsBasePlugin *) ags_dssi_manager_find_dssi_plugin_with_fallback(ags_dssi_manager_get_instance(),
										    filename, effect);
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-lv2",
				11)){
    base_plugin = (AgsBasePlugin *) ags_lv2_manager_find_lv2_plugin_with_fallback(ags_lv2_manager_get_instance(),
										  filename, effect);
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-vst3",
				11)){
#if defined(AGS_WITH_VST3)
    base_plugin = (AgsBasePlugin *) ags_vst3_manager_find_vst3_plugin_with_fallback(ags_vst3_manager_get_instance(),
										    filename, effect);
#endif
  }

  if(base_plugin != NULL){
    g_object_get(base_plugin,
		 "filename", &fallback_filename,
		 NULL);
  }else{
    fallback_filename = g_strdup(filename);
  }
  
  if((AGS_FX_FACTORY_ADD & (create_flags)) != 0){
    if(!g_ascii_strncasecmp(plugin_name,
			    "ags-fx-ladspa",
			    14)){
      ags_effect_bulk_add_ladspa_plugin(effect_bulk,
					control_type_name,
					play_container, recall_container,
					plugin_name,
					fallback_filename,
					effect,
					start_audio_channel, stop_audio_channel,
					start_pad, stop_pad,
					position,
					create_flags, recall_flags);
    }else if(!g_ascii_strncasecmp(plugin_name,
				  "ags-fx-dssi",
				  12)){
      ags_effect_bulk_add_dssi_plugin(effect_bulk,
				      control_type_name,
				      play_container, recall_container,
				      plugin_name,
				      fallback_filename,
				      effect,
				      start_audio_channel, stop_audio_channel,
				      start_pad, stop_pad,
				      position,
				      create_flags, recall_flags);
    }else if(!g_ascii_strncasecmp(plugin_name,
				  "ags-fx-lv2",
				  11)){
      ags_effect_bulk_add_lv2_plugin(effect_bulk,
				     control_type_name,
				     play_container, recall_container,
				     plugin_name,
				     fallback_filename,
				     effect,
				     start_audio_channel, stop_audio_channel,
				     start_pad, stop_pad,
				     position,
				     create_flags, recall_flags);
    }else if(!g_ascii_strncasecmp(plugin_name,
				  "ags-fx-vst3",
				  12)){
#if defined(AGS_WITH_VST3)
      ags_effect_bulk_add_vst3_plugin(effect_bulk,
				      control_type_name,
				      play_container, recall_container,
				      plugin_name,
				      fallback_filename,
				      effect,
				      start_audio_channel, stop_audio_channel,
				      start_pad, stop_pad,
				      position,
				      create_flags, recall_flags);
#endif
    }
  }else if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    GList *start_list, *list;
    GList *start_recall, *recall;

    /* plugin_name */
    start_recall = ags_fx_factory_create(effect_bulk->audio,
					 play_container, recall_container,
					 plugin_name,
					 fallback_filename,
					 effect,
					 start_audio_channel, stop_audio_channel,
					 start_pad, stop_pad,
					 position,
					 create_flags | (g_type_is_a(effect_bulk->channel_type, AGS_TYPE_OUTPUT) ? AGS_FX_FACTORY_OUTPUT: AGS_FX_FACTORY_INPUT), recall_flags);

    recall = start_recall;

    while(recall != NULL){
      ags_recall_set_behaviour_flags(recall->data, AGS_SOUND_BEHAVIOUR_BULK_MODE);

      recall = recall->next;
    }
    
    /* unref */
    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);

    /*  */
    list = 
      start_list = gtk_container_get_children((GtkContainer *) effect_bulk->grid);

    while(list != NULL){
      if(AGS_IS_BULK_MEMBER(list->data) &&
	 AGS_BULK_MEMBER(list->data)->play_container == play_container){
	ags_bulk_member_remap_bulk_port(list->data);
      }

      list = list->next;
    }

    g_list_free(start_list);
  }

  g_free(fallback_filename);
}

/**
 * ags_effect_bulk_add_plugin:
 * @effect_bulk: the #AgsEffectBulk to modify
 * @control_type_name: the #GList-struct containing string representation of a #GType
 * @play_container: an #AgsRecallContainer to indetify what recall to use
 * @recall_container: an #AgsRecallContainer to indetify what recall to use
 * @plugin_name: the plugin identifier
 * @filename: the effect's filename
 * @effect: the effect's name
 * @start_audio_channel: the first audio channel to apply
 * @stop_audio_channel: the last audio channel to apply
 * @start_pad: the first pad to apply
 * @stop_pad: the last pad to apply
 * @position: the position to insert the recall
 * @create_flags: modify the behaviour of this function
 * @recall_flags: flags to be set for #AgsRecall
 *
 * Add an effect by its filename and effect specifier.
 *
 * Since: 3.3.0
 */
void
ags_effect_bulk_add_plugin(AgsEffectBulk *effect_bulk,
			   GList *control_type_name,
			   AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			   gchar *plugin_name,
			   gchar *filename,
			   gchar *effect,
			   guint start_audio_channel, guint stop_audio_channel,
			   guint start_pad, guint stop_pad,
			   gint position,
			   guint create_flags, guint recall_flags)
{
  g_return_if_fail(AGS_IS_EFFECT_BULK(effect_bulk));

  g_object_ref((GObject *) effect_bulk);
  g_signal_emit(G_OBJECT(effect_bulk),
		effect_bulk_signals[ADD_PLUGIN], 0,
		control_type_name,
		play_container, recall_container,
		plugin_name,
		filename,
		effect,
		start_audio_channel, stop_audio_channel,
		start_pad, stop_pad,
		position,
		create_flags, recall_flags);
  g_object_unref((GObject *) effect_bulk);
}

void
ags_effect_bulk_real_remove_plugin(AgsEffectBulk *effect_bulk,
				   guint nth)
{
  AgsEffectBulkPlugin *effect_bulk_plugin;

  AgsChannel *start_channel;
  AgsChannel *channel, *next_channel;

  GList *start_list, *list;
  GList *start_recall, *recall;
  
  if(!AGS_IS_EFFECT_BULK(effect_bulk)){
    return;
  }
  
  start_channel = NULL;
  
  list = g_list_nth(effect_bulk->plugin,
		    nth);

  if(list == NULL){
    return;
  }
  
  effect_bulk_plugin = list->data;

  /*  */  
  effect_bulk->plugin = g_list_remove(effect_bulk->plugin,
				      effect_bulk_plugin);

  /* AgsRecallAudio */
  ags_audio_remove_recall(effect_bulk->audio, (GObject *) ags_recall_container_get_recall_audio(effect_bulk_plugin->play_container),
			  TRUE);

  ags_audio_remove_recall(effect_bulk->audio, (GObject *) ags_recall_container_get_recall_audio(effect_bulk_plugin->recall_container),
			  FALSE);

  /* AgsRecallAudioRun - play context */
  g_object_get(effect_bulk_plugin->play_container,
	       "recall-audio-run", &start_recall,
	       NULL);
  
  recall = start_recall;

  while(recall != NULL){
    ags_audio_remove_recall(effect_bulk->audio, (GObject *) recall->data,
			    TRUE);

    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* AgsRecallAudioRun - recall context */
  g_object_get(effect_bulk_plugin->recall_container,
	       "recall-audio-run", &start_recall,
	       NULL);
  
  recall = start_recall;

  while(recall != NULL){
    ags_audio_remove_recall(effect_bulk->audio, (GObject *) recall->data,
			    FALSE);

    recall = recall->next;    
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* AgsRecallChannel - play context */
  g_object_get(effect_bulk_plugin->play_container,
	       "recall-channel", &start_recall,
	       NULL);
  
  recall = start_recall;

  while(recall != NULL){
    AgsChannel *channel;

    g_object_get(recall->data,
		 "source", &channel,
		 NULL);
    
    ags_channel_remove_recall(channel, (GObject *) recall->data,
			      TRUE);

    if(channel != NULL){
      g_object_unref(channel);
    }

    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* AgsRecallChannel - recall context */
  g_object_get(effect_bulk_plugin->recall_container,
	       "recall-channel", &start_recall,
	       NULL);
  
  recall = start_recall;

  while(recall != NULL){
    AgsChannel *channel;

    g_object_get(recall->data,
		 "source", &channel,
		 NULL);
    
    ags_channel_remove_recall(channel, (GObject *) recall->data,
			      FALSE);
    

    if(channel != NULL){
      g_object_unref(channel);
    }

    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
  
  /* AgsRecallChannelRun - play context */
  g_object_get(effect_bulk_plugin->play_container,
	       "recall-channel-run", &start_recall,
	       NULL);
  
  recall = start_recall;

  while(recall != NULL){
    AgsChannel *channel;

    g_object_get(recall->data,
		 "source", &channel,
		 NULL);
    
    ags_channel_remove_recall(channel, (GObject *) recall->data,
			      TRUE);    

    if(channel != NULL){
      g_object_unref(channel);
    }

    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* AgsRecallChannelRun - recall context */
  g_object_get(effect_bulk_plugin->recall_container,
	       "recall-channel-run", &start_recall,
	       NULL);
  
  recall = start_recall;

  while(recall != NULL){
    AgsChannel *channel;

    g_object_get(recall->data,
		 "source", &channel,
		 NULL);
    
    ags_channel_remove_recall(channel, (GObject *) recall->data,
			      FALSE);    

    if(channel != NULL){
      g_object_unref(channel);
    }

    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* recall container */
  ags_audio_remove_recall_container(effect_bulk->audio, (GObject *) effect_bulk_plugin->play_container);
  ags_audio_remove_recall_container(effect_bulk->audio, (GObject *) effect_bulk_plugin->recall_container);

  g_object_get(effect_bulk->audio,
	       "input", &start_channel,
	       NULL);

  if(start_channel != NULL){
    channel = start_channel;

    g_object_ref(channel);

    while(channel != NULL){
      ags_channel_remove_recall_container(channel, effect_bulk_plugin->play_container);
      ags_channel_remove_recall_container(channel, effect_bulk_plugin->recall_container);

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }
  }
  
  if(start_channel != NULL){
    g_object_unref(start_channel);
  }

  /* destroy controls - bulk member */
  start_list = gtk_container_get_children((GtkContainer *) effect_bulk->bulk_member);

  list = g_list_nth(start_list,
		    nth);

  if(list != NULL){
    gtk_widget_destroy(list->data);
  }

  g_list_free(start_list);

  /* destroy controls - table */
  start_list = gtk_container_get_children((GtkContainer *) effect_bulk->grid);

  list = start_list;

  while(list != NULL){
    if(AGS_IS_BULK_MEMBER(list->data) &&
       AGS_BULK_MEMBER(list->data)->play_container == effect_bulk_plugin->play_container){
      GtkWidget *child_widget;

      child_widget = gtk_bin_get_child(list->data);
      
      if(AGS_IS_INDICATOR(child_widget) ||
	 AGS_IS_LED(child_widget)){
	g_hash_table_remove(ags_effect_bulk_indicator_queue_draw,
			    child_widget);
      }
      
      gtk_widget_destroy(list->data);
    }
    
    list = list->next;
  }
  
  g_list_free(start_list);
  
  /* free AgsEffectBulkPlugin */
  ags_effect_bulk_plugin_free(effect_bulk_plugin);
}

/**
 * ags_effect_bulk_remove_plugin:
 * @effect_bulk: the #AgsEffectBulk to modify
 * @nth: the nth effect to remove
 *
 * Remove an effect by its position.
 *
 * Since: 3.3.0
 */
void
ags_effect_bulk_remove_plugin(AgsEffectBulk *effect_bulk,
			      guint nth)
{
  g_return_if_fail(AGS_IS_EFFECT_BULK(effect_bulk));

  g_object_ref((GObject *) effect_bulk);
  g_signal_emit(G_OBJECT(effect_bulk),
		effect_bulk_signals[REMOVE_PLUGIN], 0,
		nth);
  g_object_unref((GObject *) effect_bulk);
}

void
ags_effect_bulk_real_resize_audio_channels(AgsEffectBulk *effect_bulk,
					   guint new_size,
					   guint old_size)
{
  //TODO:JK: implement me
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
  //TODO:JK: implement me
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
 *
 * Since: 3.0.0
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
    bulk_member = gtk_container_get_children((GtkContainer *) effect_bulk->grid);

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
 * Since: 3.0.0
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
 * ags_effect_bulk_indicator_queue_draw_timeout:
 * @widget: the indicator widgt
 *
 * Queue draw widget
 *
 * Returns: %TRUE if proceed with redraw, otherwise %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_effect_bulk_indicator_queue_draw_timeout(GtkWidget *widget)
{
  if(g_hash_table_lookup(ags_effect_bulk_indicator_queue_draw,
			 widget) != NULL){
    AgsBulkMember *bulk_member;

    GList *list;

    gdouble val;
    
    bulk_member = (AgsBulkMember *) gtk_widget_get_ancestor(widget,
							    AGS_TYPE_BULK_MEMBER);

    list = bulk_member->bulk_port;

    val = 0.0;
    
    while(list != NULL){
      GValue value = {0,};

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(AGS_BULK_PORT(list->data)->port,
			 &value);

      val += g_value_get_float(&value);
      
      list = list->next;
    }

    if(AGS_IS_LED(widget)){
      if(val != 0.0){
	ags_led_set_active((AgsLed *) widget);
      }else{
	ags_led_unset_active((AgsLed *) widget);
      }
    }else if(AGS_IS_INDICATOR(widget)){
      gtk_adjustment_set_value(AGS_INDICATOR(widget)->adjustment,
			       val);
    }
    
    gtk_widget_queue_draw(widget);
    
    return(TRUE);
  }else{
    return(FALSE);
  }
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
 * Since: 3.0.0
 */
AgsEffectBulk*
ags_effect_bulk_new(AgsAudio *audio,
		    GType channel_type)
{
  AgsEffectBulk *effect_bulk;

  effect_bulk = (AgsEffectBulk *) g_object_new(AGS_TYPE_EFFECT_BULK,
					       "audio", audio,
					       "channel-type", channel_type,
					       NULL);

  return(effect_bulk);
}

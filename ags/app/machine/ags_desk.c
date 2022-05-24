/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/app/machine/ags_desk.h>
#include <ags/app/machine/ags_desk_file_chooser.h>
#include <ags/app/machine/ags_desk_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_navigation.h>

#include <ags/i18n.h>

void ags_desk_class_init(AgsDeskClass *desk);
void ags_desk_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_desk_init(AgsDesk *desk);
void ags_desk_finalize(GObject *gobject);

void ags_desk_map_recall(AgsMachine *machine);

void ags_desk_resize_audio_channels(AgsMachine *machine,
				    guint audio_channels, guint audio_channels_old,
				    gpointer data);
void ags_desk_resize_pads(AgsMachine *machine, GType channel_type,
			  guint pads, guint pads_old,
			  gpointer data);

void ags_desk_output_map_recall(AgsDesk *desk,
				guint audio_channel_start,
				guint output_pad_start);
void ags_desk_input_map_recall(AgsDesk *desk,
			       guint audio_channel_start,
			       guint input_pad_start);

void ags_desk_connect(AgsConnectable *connectable);
void ags_desk_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_desk
 * @short_description: desk sequencer
 * @title: AgsDesk
 * @section_id:
 * @include: ags/app/machine/ags_desk.h
 *
 * The #AgsDesk is a composite widget to act as desk sequencer.
 */

static gpointer ags_desk_parent_class = NULL;

static AgsConnectableInterface *ags_desk_parent_connectable_interface;

GType
ags_desk_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_desk = 0;

    static const GTypeInfo ags_desk_info = {
      sizeof(AgsDeskClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_desk_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsDesk),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_desk_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_desk_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_desk = g_type_register_static(AGS_TYPE_MACHINE,
					   "AgsDesk", &ags_desk_info,
					   0);
    
    g_type_add_interface_static(ags_type_desk,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_desk);
  }

  return g_define_type_id__volatile;
}

void
ags_desk_class_init(AgsDeskClass *desk)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  AgsMachineClass *machine;

  ags_desk_parent_class = g_type_class_peek_parent(desk);

  /* GObjectClass */
  gobject = (GObjectClass *) desk;

  gobject->finalize = ags_desk_finalize;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) desk;

  /*  */
  machine = (AgsMachineClass *) desk;

  machine->map_recall = ags_desk_map_recall;
}

void
ags_desk_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_desk_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_desk_connect;
  connectable->disconnect = ags_desk_disconnect;
}

void
ags_desk_init(AgsDesk *desk)
{
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  GtkBox *hbox;
  GtkBox *balance_hbox;
  GtkBox *file_hbox;
  
  AgsAudio *audio;
  
  AgsMachineCounterManager *machine_counter_manager;
  AgsMachineCounter *machine_counter;

  AgsApplicationContext *application_context;
  
  gchar *machine_name;

  gint position;
  gint baseline_allocation;

  application_context = ags_application_context_get_instance();
  
  /* machine counter */
  machine_counter_manager = ags_machine_counter_manager_get_instance();

  machine_counter = ags_machine_counter_manager_find_machine_counter(machine_counter_manager,
								     AGS_TYPE_DESK);

  machine_name = NULL;

  if(machine_counter != NULL){
    machine_name = g_strdup_printf("Default %d",
				   machine_counter->counter);
  
    ags_machine_counter_increment(machine_counter);
  }
  
  g_object_set(desk,
	       "machine-name", machine_name,
	       NULL);

  g_free(machine_name);


  /* machine selector */
  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  composite_editor = ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  position = g_list_length(window->machine);
  
  ags_machine_selector_popup_insert_machine(composite_editor->machine_selector,
					    position,
					    desk);

  audio = AGS_MACHINE(desk)->audio;

  ags_audio_set_flags(audio, (AGS_AUDIO_SYNC |
			      AGS_AUDIO_OUTPUT_HAS_RECYCLING |
			      AGS_AUDIO_INPUT_HAS_RECYCLING));
  
  AGS_MACHINE(desk)->flags |= (AGS_MACHINE_IS_WAVE_PLAYER);

  /* audio resize */
  g_signal_connect_after(G_OBJECT(desk), "resize-audio-channels",
			 G_CALLBACK(ags_desk_resize_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(desk), "resize-pads",
			 G_CALLBACK(ags_desk_resize_pads), NULL);

  desk->name = NULL;
  desk->xml_type = "ags-desk";

  desk->playback_play_container = ags_recall_container_new();
  desk->playback_recall_container = ags_recall_container_new();

  desk->buffer_play_container = ags_recall_container_new();
  desk->buffer_recall_container = ags_recall_container_new();

  /* create widgets */
  desk->vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				      0);
  gtk_frame_set_child(AGS_MACHINE(desk)->frame,
		      (GtkWidget *) desk->vbox);

  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);
  gtk_box_append(desk->vbox,
		 (GtkWidget *) hbox);

  /* left pad */
  desk->left_pad = ags_desk_pad_new();
  gtk_box_append(hbox,
		 (GtkWidget *) desk->left_pad);

  /* console */  
  desk->console = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
					 0);
  gtk_box_append(hbox,
		 (GtkWidget *) desk->console);
  
  balance_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
					0);
  gtk_box_append(desk->console,
		 (GtkWidget *) balance_hbox);
  
  desk->move_left = (GtkButton *) gtk_button_new_with_label(i18n("left"));
  gtk_box_append(balance_hbox,
		 (GtkWidget *) desk->move_left);

  desk->balance = (GtkScale *) gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL,
							-1.0, 1.0, 0.1);
  gtk_widget_set_size_request((GtkWidget *) desk->balance,
			      200, -1);
  gtk_box_append(balance_hbox,
		 (GtkWidget *) desk->balance);

  desk->move_right = (GtkButton *) gtk_button_new_with_label(i18n("right"));
  gtk_box_append(balance_hbox,
		 (GtkWidget *) desk->move_right);

  /* left pad */
  desk->right_pad = ags_desk_pad_new();
  gtk_box_append(hbox,
		 (GtkWidget *) desk->right_pad);

  /* file chooser */
  file_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				     0);
  gtk_box_append(desk->vbox,
		 (GtkWidget *) file_hbox);

  desk->file_chooser = ags_desk_file_chooser_new();
  gtk_widget_set_size_request((GtkWidget *) desk->file_chooser,
			      800, 480);
  gtk_box_append(file_hbox,
		 (GtkWidget *) desk->file_chooser);
}

void
ags_desk_finalize(GObject *gobject)
{  
  G_OBJECT_CLASS(ags_desk_parent_class)->finalize(gobject);
}

void
ags_desk_connect(AgsConnectable *connectable)
{
  AgsDesk *desk;

  int i;

  if((AGS_CONNECTABLE_CONNECTED & (AGS_MACHINE(connectable)->connectable_flags)) != 0){
    return;
  }

  desk = AGS_DESK(connectable);

  /* call parent */
  ags_desk_parent_connectable_interface->connect(connectable);

  ags_connectable_connect(AGS_CONNECTABLE(desk->left_pad));
  ags_connectable_connect(AGS_CONNECTABLE(desk->right_pad));
}

void
ags_desk_disconnect(AgsConnectable *connectable)
{
  AgsDesk *desk;

  int i;

  if((AGS_CONNECTABLE_CONNECTED & (AGS_MACHINE(connectable)->connectable_flags)) == 0){
    return;
  }

  desk = AGS_DESK(connectable);

  /* call parent */
  ags_desk_parent_connectable_interface->disconnect(connectable);

  ags_connectable_disconnect(AGS_CONNECTABLE(desk->left_pad));
  ags_connectable_disconnect(AGS_CONNECTABLE(desk->right_pad));
}

void
ags_desk_resize_audio_channels(AgsMachine *machine,
			       guint audio_channels, guint audio_channels_old,
			       gpointer data)
{
  AgsDesk *desk;

  desk = AGS_DESK(machine);
  
  if(audio_channels > audio_channels_old){    
    /* recall */
    if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
      ags_desk_input_map_recall(desk,
				audio_channels_old,
				0);

      ags_desk_output_map_recall(desk,
				 audio_channels_old,
				 0);
    }    
  }
}

void
ags_desk_resize_pads(AgsMachine *machine,
		     GType channel_type,
		     guint pads, guint pads_old,
		     gpointer data)
{
  AgsDesk *desk;

  desk = AGS_DESK(machine);
  
  if(g_type_is_a(channel_type, AGS_TYPE_INPUT)){
    if(pads > pads_old){
      /* depending on destination */
      ags_desk_input_map_recall(desk,
				0,
				pads_old);
    }else{
      desk->mapped_input_pad = pads;
    }
  }else{
    if(pads > pads_old){
      /* depending on destination */
      ags_desk_output_map_recall(desk,
				 0,
				 pads_old);
    }else{
      desk->mapped_output_pad = pads;
    }
  }
}

void
ags_desk_map_recall(AgsMachine *machine)
{  
  AgsNavigation *navigation;
  AgsDesk *desk;
  
  AgsAudio *audio;

  AgsApplicationContext *application_context;

  GList *start_recall, *recall;

  gint position;

  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }

  application_context = ags_application_context_get_instance();

  navigation = ags_ui_provider_get_navigation(AGS_UI_PROVIDER(application_context));

  desk = AGS_DESK(machine);
  
  audio = machine->audio;

  position = 0;
  
  /* ags-fx-playback */
  start_recall = ags_fx_factory_create(audio,
				       desk->playback_play_container, desk->playback_recall_container,
				       "ags-fx-playback",
				       NULL,
				       NULL,
				       0, 0,
				       0, 0,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  recall = start_recall;

  while((recall = ags_recall_template_find_type(recall, AGS_TYPE_FX_PLAYBACK_AUDIO)) != NULL){
    AgsPort *port;

    GValue value = G_VALUE_INIT;
    
    /* loop */
    port = NULL;
    
    g_object_get(recall->data,
		 "loop", &port,
		 NULL);

    g_value_init(&value,
		 G_TYPE_BOOLEAN);

    g_value_set_boolean(&value,
			gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(navigation->loop)));

    ags_port_safe_write(port,
			&value);

    if(port != NULL){
      g_object_unref(port);
    }
    
    /* loop start */
    port = NULL;
    
    g_object_get(recall->data,
		 "loop-start", &port,
		 NULL);

    g_value_unset(&value);
    g_value_init(&value,
		 G_TYPE_UINT64);

    g_value_set_uint64(&value,
		       16 * gtk_spin_button_get_value_as_int(navigation->loop_left_tact));

    ags_port_safe_write(port,
			&value);

    if(port != NULL){
      g_object_unref(port);
    }
    
    /* loop end */
    port = NULL;
    
    g_object_get(recall->data,
		 "loop-end", &port,
		 NULL);

    g_value_unset(&value);
    g_value_init(&value,
		 G_TYPE_UINT64);

    g_value_set_uint64(&value,
		       16 * gtk_spin_button_get_value_as_int(navigation->loop_right_tact));

    ags_port_safe_write(port,
			&value);

    if(port != NULL){
      g_object_unref(port);
    }

    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-buffer */
  start_recall = ags_fx_factory_create(audio,
				       desk->buffer_play_container, desk->buffer_recall_container,
				       "ags-fx-buffer",
				       NULL,
				       NULL,
				       0, 0,
				       0, 0,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
  
  /* depending on destination */
  ags_desk_input_map_recall((AgsDesk *) machine,
			    0,
			    0);

  /* depending on destination */
  ags_desk_output_map_recall((AgsDesk *) machine,
			     0,
			     0);

  /* call parent */
  AGS_MACHINE_CLASS(ags_desk_parent_class)->map_recall(machine);  
}

void
ags_desk_output_map_recall(AgsDesk *desk,
			   guint audio_channel_start,
			   guint output_pad_start)
{  
  AgsAudio *audio;

  guint output_pads;

  if(desk->mapped_output_pad > output_pad_start){
    return;
  }

  audio = AGS_MACHINE(desk)->audio;
  
  /* get some fields */
  g_object_get(audio,
	       "output-pads", &output_pads,
	       NULL);
  
  desk->mapped_output_pad = output_pads;
}

void
ags_desk_input_map_recall(AgsDesk *desk,
			  guint audio_channel_start,
			  guint input_pad_start)
{
  AgsAudio *audio;

  GList *start_recall;

  guint input_pads;
  guint audio_channels;
  gint position;

  if(desk->mapped_input_pad > input_pad_start){
    return;
  }

  audio = AGS_MACHINE(desk)->audio;

  position = 0;

  input_pads = 0;
  audio_channels = 0;

  /* get some fields */
  g_object_get(audio,
	       "input-pads", &input_pads,
	       "audio-channels", &audio_channels,
	       NULL);

  /* ags-fx-playback */
  start_recall = ags_fx_factory_create(audio,
				       desk->playback_play_container, desk->playback_recall_container,
				       "ags-fx-playback",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT),
				       0);

  /* unref */
  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-buffer */
  start_recall = ags_fx_factory_create(audio,
				       desk->buffer_play_container, desk->buffer_recall_container,
				       "ags-fx-buffer",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT),
				       0);

  /* unref */
  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
  
  desk->mapped_input_pad = input_pads;
}

/**
 * ags_desk_new:
 * @soundcard: the assigned soundcard.
 *
 * Creates an #AgsDesk
 *
 * Returns: a new #AgsDesk
 *
 * Since: 3.0.0
 */
AgsDesk*
ags_desk_new(GObject *soundcard)
{
  AgsDesk *desk;

  desk = (AgsDesk *) g_object_new(AGS_TYPE_DESK,
				  NULL);

  g_object_set(G_OBJECT(AGS_MACHINE(desk)->audio),
	       "output-soundcard", soundcard,
	       NULL);

  return(desk);
}

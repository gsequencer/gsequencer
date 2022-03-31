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

#include <ags/app/machine/ags_panel.h>
#include <ags/app/machine/ags_panel_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>

#include <ags/app/machine/ags_panel_input_pad.h>
#include <ags/app/machine/ags_panel_input_line.h>

void ags_panel_class_init(AgsPanelClass *panel);
void ags_panel_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_panel_init(AgsPanel *panel);
static void ags_panel_finalize(GObject *gobject);

void ags_panel_connect(AgsConnectable *connectable);
void ags_panel_disconnect(AgsConnectable *connectable);

void ags_panel_map_recall(AgsMachine *machine);

void ags_file_read_panel(AgsFile *file, xmlNode *node, AgsMachine *panel);
xmlNode* ags_file_write_panel(AgsFile *file, xmlNode *parent, AgsMachine *panel);

void ags_panel_resize_audio_channels(AgsMachine *machine,
				     guint audio_channels, guint audio_channels_old,
				     gpointer data);
void ags_panel_resize_pads(AgsMachine *machine, GType channel_type,
			   guint pads, guint pads_old,
			   gpointer data);

/**
 * SECTION:ags_panel
 * @short_description: panel
 * @title: AgsPanel
 * @section_id:
 * @include: ags/app/machine/ags_panel.h
 *
 * The #AgsPanel is a composite widget to act as panel.
 */

static gpointer ags_panel_parent_class = NULL;
static AgsConnectableInterface *ags_panel_parent_connectable_interface;

GType
ags_panel_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_panel = 0;

    static const GTypeInfo ags_panel_info = {
      sizeof(AgsPanelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_panel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsPanel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_panel_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_panel_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_panel = g_type_register_static(AGS_TYPE_MACHINE,
					    "AgsPanel", &ags_panel_info,
					    0);
    
    g_type_add_interface_static(ags_type_panel,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_panel);
  }

  return g_define_type_id__volatile;
}

void
ags_panel_class_init(AgsPanelClass *panel)
{
  GObjectClass *gobject;
  AgsMachineClass *machine;

  ags_panel_parent_class = g_type_class_peek_parent(panel);

  /* GtkObjectClass */
  gobject = (GObjectClass *) panel;

  gobject->finalize = ags_panel_finalize;

  /* AgsMachine */
  machine = (AgsMachineClass *) panel;

  machine->map_recall = ags_panel_map_recall;
}

void
ags_panel_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_panel_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_panel_connect;
  connectable->disconnect = ags_panel_disconnect;
}

void
ags_panel_init(AgsPanel *panel)
{
  AgsMachineCounterManager *machine_counter_manager;
  AgsMachineCounter *machine_counter;
  
  gchar *machine_name;

  machine_counter_manager = ags_machine_counter_manager_get_instance();

  machine_counter = ags_machine_counter_manager_find_machine_counter(machine_counter_manager,
								     AGS_TYPE_PANEL);

  machine_name = NULL;

  if(machine_counter != NULL){
    machine_name = g_strdup_printf("Default %d",
				   machine_counter->counter);
  
    ags_machine_counter_increment(machine_counter);
  }
  
  g_object_set(panel,
	       "machine-name", machine_name,
	       NULL);

  g_free(machine_name);

  AGS_MACHINE(panel)->connection_flags |= AGS_MACHINE_SHOW_AUDIO_OUTPUT_CONNECTION;

  ags_audio_set_flags(AGS_MACHINE(panel)->audio, (AGS_AUDIO_SYNC));
  g_object_set(AGS_MACHINE(panel)->audio,
	       "min-audio-channels", 1,
	       "min-output-pads", 1,
	       "min-input-pads", 1,
	       NULL);

  AGS_MACHINE(panel)->input_pad_type = AGS_TYPE_PANEL_INPUT_PAD;

  AGS_MACHINE(panel)->input_pad_type = AGS_TYPE_PANEL_INPUT_PAD;
  AGS_MACHINE(panel)->input_line_type = AGS_TYPE_PANEL_INPUT_LINE;
  AGS_MACHINE(panel)->output_pad_type = G_TYPE_NONE;
  AGS_MACHINE(panel)->output_line_type = G_TYPE_NONE;

  g_signal_connect(G_OBJECT(panel), "resize-audio-channels",
		   G_CALLBACK(ags_panel_resize_audio_channels), NULL);

  g_signal_connect(G_OBJECT(panel), "resize-pads",
		   G_CALLBACK(ags_panel_resize_pads), NULL);

  /* */
  panel->name = NULL;
  panel->xml_type = "ags-panel";

  panel->playback_play_container = ags_recall_container_new();
  panel->playback_recall_container = ags_recall_container_new();

  panel->volume_play_container = ags_recall_container_new();
  panel->volume_recall_container = ags_recall_container_new();
  
  panel->vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				       0);
  gtk_frame_set_child(AGS_MACHINE(panel)->frame,
		      (GtkWidget *) panel->vbox);

  /* input */
  AGS_MACHINE(panel)->input_pad_grid = (GtkGrid *) gtk_grid_new();

  gtk_widget_set_valign(AGS_MACHINE(panel)->input_pad_grid,
			GTK_ALIGN_START);  
  gtk_widget_set_halign(AGS_MACHINE(panel)->input_pad_grid,
			GTK_ALIGN_START);
  
  gtk_widget_set_hexpand(AGS_MACHINE(panel)->input_pad_grid,
			 FALSE);  
  
  gtk_box_append(panel->vbox,
		 (GtkWidget *) AGS_MACHINE(panel)->input_pad_grid);
}

static void
ags_panel_finalize(GObject *gobject)
{  
  /* call parent */
  G_OBJECT_CLASS(ags_panel_parent_class)->finalize(gobject);
}

void
ags_panel_connect(AgsConnectable *connectable)
{
  if((AGS_CONNECTABLE_CONNECTED & (AGS_MACHINE(connectable)->connectable_flags)) != 0){
    return;
  }

  ags_panel_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_panel_disconnect(AgsConnectable *connectable)
{
  if((AGS_CONNECTABLE_CONNECTED & (AGS_MACHINE(connectable)->connectable_flags)) == 0){
    return;
  }

  ags_panel_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_panel_map_recall(AgsMachine *machine)
{
  AgsPanel *panel;
  
  AgsAudio *audio;

  GList *start_recall;

  gint position;
  
  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }

  panel = AGS_PANEL(machine);

  audio = machine->audio;

  position = 0;
  
  /* ags-fx-playback */
  start_recall = ags_fx_factory_create(audio,
				       panel->playback_play_container, panel->playback_recall_container,
				       "ags-fx-playback",
				       NULL,
				       NULL,
				       0, 0,
				       0, 0,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
  
  /* ags-fx-volume */
  start_recall = ags_fx_factory_create(audio,
				       panel->volume_play_container, panel->volume_recall_container,
				       "ags-fx-volume",
				       NULL,
				       NULL,
				       0, 0,
				       0, 0,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* call parent */
  AGS_MACHINE_CLASS(ags_panel_parent_class)->map_recall(machine);
}

void
ags_panel_resize_audio_channels(AgsMachine *machine,
				guint audio_channels, guint audio_channels_old,
				gpointer data)
{
  //empty
}

void
ags_panel_resize_pads(AgsMachine *machine, GType channel_type,
		      guint pads, guint pads_old,
		      gpointer data)
{
  //empty
}

/**
 * ags_panel_new:
 * @soundcard: the assigned soundcard.
 *
 * Create a new instance of #AgsPanel
 *
 * Returns: the new #AgsPanel
 *
 * Since: 3.0.0
 */
AgsPanel*
ags_panel_new(GObject *soundcard)
{
  AgsPanel *panel;

  panel = (AgsPanel *) g_object_new(AGS_TYPE_PANEL,
				    NULL);

  g_object_set(AGS_MACHINE(panel)->audio,
	       "output-soundcard", soundcard,
	       NULL);

  return(panel);
}

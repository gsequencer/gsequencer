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

#include <ags/app/machine/ags_mixer.h>
#include <ags/app/machine/ags_mixer_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_pad.h>
#include <ags/app/ags_line.h>

#include <ags/app/machine/ags_mixer_input_pad.h>
#include <ags/app/machine/ags_mixer_input_line.h>

void ags_mixer_class_init(AgsMixerClass *mixer);
void ags_mixer_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_mixer_init(AgsMixer *mixer);
void ags_mixer_finalize(GObject *gobject);

void ags_mixer_connect(AgsConnectable *connectable);
void ags_mixer_disconnect(AgsConnectable *connectable);

void ags_mixer_map_recall(AgsMachine *machine);

/**
 * SECTION:ags_mixer
 * @short_description: mixer
 * @title: AgsMixer
 * @section_id:
 * @include: ags/app/machine/ags_mixer.h
 *
 * The #AgsMixer is a composite widget to act as mixer.
 */

static gpointer ags_mixer_parent_class = NULL;
static AgsConnectableInterface *ags_mixer_parent_connectable_interface;

GType
ags_mixer_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_mixer = 0;

    static const GTypeInfo ags_mixer_info = {
      sizeof(AgsMixerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_mixer_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsMixer),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_mixer_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_mixer_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_mixer = g_type_register_static(AGS_TYPE_MACHINE,
					    "AgsMixer", &ags_mixer_info,
					    0);
    
    g_type_add_interface_static(ags_type_mixer,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_mixer);
  }

  return g_define_type_id__volatile;
}

void
ags_mixer_class_init(AgsMixerClass *mixer)
{
  GObjectClass *gobject;
  AgsMachineClass *machine;

  ags_mixer_parent_class = g_type_class_peek_parent(mixer);

  /* GObjectClass */
  gobject = (GObjectClass *) mixer;

  gobject->finalize = ags_mixer_finalize;

  /* AgsMachine */
  machine = (AgsMachineClass *) mixer;

  machine->map_recall = ags_mixer_map_recall;
}

void
ags_mixer_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_mixer_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_mixer_connect;
  connectable->disconnect = ags_mixer_disconnect;
}

void
ags_mixer_init(AgsMixer *mixer)
{
  g_signal_connect_after((GObject *) mixer, "parent_set",
			 G_CALLBACK(ags_mixer_parent_set_callback), (gpointer) mixer);

  ags_audio_set_flags(AGS_MACHINE(mixer)->audio, (AGS_AUDIO_ASYNC));
  g_object_set(AGS_MACHINE(mixer)->audio,
	       "min-audio-channels", 1,
	       "max-output-pads", 1,
	       "min-output-pads", 1,
	       "min-input-pads", 1,
	       NULL);

  AGS_MACHINE(mixer)->input_pad_type = AGS_TYPE_MIXER_INPUT_PAD;
  AGS_MACHINE(mixer)->input_line_type = AGS_TYPE_MIXER_INPUT_LINE;
  AGS_MACHINE(mixer)->output_pad_type = G_TYPE_NONE;
  AGS_MACHINE(mixer)->output_line_type = G_TYPE_NONE;
  
  /*  */
  mixer->name = NULL;
  mixer->xml_type = "ags-mixer";

  mixer->volume_play_container = ags_recall_container_new();
  mixer->volume_recall_container = ags_recall_container_new();

  mixer->peak_play_container = ags_recall_container_new();
  mixer->peak_recall_container = ags_recall_container_new();
  
  /* input */
  mixer->input_pad = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
					    0);
  AGS_MACHINE(mixer)->input = (GtkContainer *) mixer->input_pad;
  gtk_container_add((GtkContainer *) gtk_bin_get_child((GtkBin *) mixer),
		    (GtkWidget *) mixer->input_pad);
}

void
ags_mixer_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_mixer_parent_class)->finalize(gobject);
}

void
ags_mixer_connect(AgsConnectable *connectable)
{
  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) != 0){
    return;
  }

  ags_mixer_parent_connectable_interface->connect(connectable);
}

void
ags_mixer_disconnect(AgsConnectable *connectable)
{
  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) == 0){
    return;
  }

  ags_mixer_parent_connectable_interface->disconnect(connectable);
}

void
ags_mixer_map_recall(AgsMachine *machine)
{
  AgsAudio *audio;

  GList *start_recall;

  gint position;

  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }
  
  audio = machine->audio;

  position = 0;

  /* ags-fx-volume */
  start_recall = ags_fx_factory_create(audio,
				       AGS_MIXER(machine)->volume_play_container, AGS_MIXER(machine)->volume_recall_container,
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


  /* ags-fx-peak */
  start_recall = ags_fx_factory_create(audio,
				       AGS_MIXER(machine)->peak_play_container, AGS_MIXER(machine)->peak_recall_container,
				       "ags-fx-peak",
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
  AGS_MACHINE_CLASS(ags_mixer_parent_class)->map_recall(machine);  
}

/**
 * ags_mixer_new:
 * @soundcard: the assigned soundcard.
 *
 * Create a new instance of #AgsMixer
 *
 * Returns: the new #AgsMixer
 *
 * Since: 3.0.0
 */
AgsMixer*
ags_mixer_new(GObject *soundcard)
{
  AgsMixer *mixer;

  mixer = (AgsMixer *) g_object_new(AGS_TYPE_MIXER,
				    NULL);

  g_object_set(AGS_MACHINE(mixer)->audio,
	       "output-soundcard", soundcard,
	       NULL);

  return(mixer);
}

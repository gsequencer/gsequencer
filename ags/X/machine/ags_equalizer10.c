/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/X/machine/ags_equalizer10.h>
#include <ags/X/machine/ags_equalizer10_callbacks.h>

#include <ags/X/ags_ui_provider.h>

#include <ags/i18n.h>

void ags_equalizer10_class_init(AgsEqualizer10Class *equalizer10);
void ags_equalizer10_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_equalizer10_init(AgsEqualizer10 *equalizer10);
void ags_equalizer10_finalize(GObject *gobject);

void ags_equalizer10_connect(AgsConnectable *connectable);
void ags_equalizer10_disconnect(AgsConnectable *connectable);

void ags_equalizer10_resize_audio_channels(AgsMachine *machine,
					   guint audio_channels, guint audio_channels_old,
					   gpointer data);
void ags_equalizer10_resize_pads(AgsMachine *machine, GType channel_type,
				 guint pads, guint pads_old,
				 gpointer data);

void ags_equalizer10_map_recall(AgsMachine *machine);

/**
 * SECTION:ags_equalizer10
 * @short_description: equalizer10 sequencer
 * @title: AgsEqualizer10
 * @section_id:
 * @include: ags/X/machine/ags_equalizer10.h
 *
 * The #AgsEqualizer10 is a composite widget to act as equalizer10 sequencer.
 */

static gpointer ags_equalizer10_parent_class = NULL;
static AgsConnectableInterface *ags_equalizer10_parent_connectable_interface;

GType
ags_equalizer10_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_equalizer10 = 0;

    static const GTypeInfo ags_equalizer10_info = {
      sizeof(AgsEqualizer10Class),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_equalizer10_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsEqualizer10),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_equalizer10_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_equalizer10_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_equalizer10 = g_type_register_static(AGS_TYPE_MACHINE,
						  "AgsEqualizer10", &ags_equalizer10_info,
						  0);
    
    g_type_add_interface_static(ags_type_equalizer10,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_equalizer10);
  }

  return g_define_type_id__volatile;
}

void
ags_equalizer10_class_init(AgsEqualizer10Class *equalizer10)
{
  GObjectClass *gobject;
  AgsMachineClass *machine;

  ags_equalizer10_parent_class = g_type_class_peek_parent(equalizer10);

  /* GObjectClass */
  gobject = (GObjectClass *) equalizer10;

  gobject->finalize = ags_equalizer10_finalize;

  /*  */
  machine = (AgsMachineClass *) equalizer10;

  machine->map_recall = ags_equalizer10_map_recall;
}

void
ags_equalizer10_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_equalizer10_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_equalizer10_connect;
  connectable->disconnect = ags_equalizer10_disconnect;
}

void
ags_equalizer10_init(AgsEqualizer10 *equalizer10)
{
  GtkVBox *vbox;
  GtkHBox *hbox;
  GtkVBox *control_vbox;
  GtkLabel *label;
  
  g_signal_connect_after((GObject *) equalizer10, "parent_set",
			 G_CALLBACK(ags_equalizer10_parent_set_callback), (gpointer) equalizer10);

  ags_audio_set_flags(AGS_MACHINE(equalizer10)->audio, (AGS_AUDIO_SYNC));
  g_object_set(AGS_MACHINE(equalizer10)->audio,
	       "min-audio-channels", 1,
	       "min-output-pads", 1,
	       "min-input-pads", 1,
	       NULL);

  g_signal_connect_after(G_OBJECT(equalizer10), "resize-audio-channels",
			 G_CALLBACK(ags_equalizer10_resize_audio_channels), NULL);
  
  g_signal_connect_after(G_OBJECT(equalizer10), "resize-pads",
			 G_CALLBACK(ags_equalizer10_resize_pads), NULL);

  equalizer10->name = NULL;
  equalizer10->xml_type = "ags-equalizer10";
  
  equalizer10->mapped_output_pad = 0;
  equalizer10->mapped_input_pad = 0;

  equalizer10->eq10_play_container = ags_recall_container_new();
  equalizer10->eq10_recall_container = ags_recall_container_new();

  vbox = (GtkVBox *) gtk_vbox_new(FALSE,
				  0);
  gtk_container_add((GtkContainer*) gtk_bin_get_child((GtkBin *) equalizer10), (GtkWidget *) vbox);

  /* controls */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE,
				  4);
  gtk_box_pack_start((GtkBox *) vbox,
		     (GtkWidget *) hbox,
		     FALSE, FALSE,
		     0);

  /* peak 28 hz */
  control_vbox = (GtkVBox *) gtk_vbox_new(FALSE,
					  0);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) control_vbox,
		     FALSE, FALSE,
		     0);

  equalizer10->peak_28hz = (GtkScale *) gtk_vscale_new_with_range(0.0, 2.0, 0.01);
  gtk_range_set_inverted((GtkRange *) equalizer10->peak_28hz,
			 TRUE);
  gtk_range_set_value((GtkRange *) equalizer10->peak_28hz,
		      1.0);
  gtk_widget_set_size_request((GtkWidget *) equalizer10->peak_28hz,
			      -1, 100);
  gtk_box_pack_start((GtkBox *) control_vbox,
		     (GtkWidget *) equalizer10->peak_28hz,
		     FALSE, FALSE,
		     0);

  label = (GtkLabel *) gtk_label_new(i18n("28Hz"));
  gtk_box_pack_start((GtkBox *) control_vbox,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  /* peak 56 hz */
  control_vbox = (GtkVBox *) gtk_vbox_new(FALSE,
					  0);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) control_vbox,
		     FALSE, FALSE,
		     0);

  equalizer10->peak_56hz = (GtkScale *) gtk_vscale_new_with_range(0.0, 2.0, 0.01);
  gtk_range_set_inverted((GtkRange *) equalizer10->peak_56hz,
			 TRUE);
  gtk_range_set_value((GtkRange *) equalizer10->peak_56hz,
		      1.0);
  gtk_widget_set_size_request((GtkWidget *) equalizer10->peak_56hz,
			      -1, 100);
  gtk_box_pack_start((GtkBox *) control_vbox,
		     (GtkWidget *) equalizer10->peak_56hz,
		     FALSE, FALSE,
		     0);

  label = (GtkLabel *) gtk_label_new(i18n("56Hz"));
  gtk_box_pack_start((GtkBox *) control_vbox,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  /* peak 112 hz */
  control_vbox = (GtkVBox *) gtk_vbox_new(FALSE,
					  0);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) control_vbox,
		     FALSE, FALSE,
		     0);

  equalizer10->peak_112hz = (GtkScale *) gtk_vscale_new_with_range(0.0, 2.0, 0.01);
  gtk_range_set_inverted((GtkRange *) equalizer10->peak_112hz,
			 TRUE);
  gtk_range_set_value((GtkRange *) equalizer10->peak_112hz,
		      1.0);
  gtk_widget_set_size_request((GtkWidget *) equalizer10->peak_112hz,
			      -1, 100);
  gtk_box_pack_start((GtkBox *) control_vbox,
		     (GtkWidget *) equalizer10->peak_112hz,
		     FALSE, FALSE,
		     0);

  label = (GtkLabel *) gtk_label_new(i18n("112Hz"));
  gtk_box_pack_start((GtkBox *) control_vbox,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  /* peak 224 hz */
  control_vbox = (GtkVBox *) gtk_vbox_new(FALSE,
					  0);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) control_vbox,
		     FALSE, FALSE,
		     0);

  equalizer10->peak_224hz = (GtkScale *) gtk_vscale_new_with_range(0.0, 2.0, 0.01);
  gtk_range_set_inverted((GtkRange *) equalizer10->peak_224hz,
			 TRUE);
  gtk_range_set_value((GtkRange *) equalizer10->peak_224hz,
		      1.0);
  gtk_widget_set_size_request((GtkWidget *) equalizer10->peak_224hz,
			      -1, 100);
  gtk_box_pack_start((GtkBox *) control_vbox,
		     (GtkWidget *) equalizer10->peak_224hz,
		     FALSE, FALSE,
		     0);

  label = (GtkLabel *) gtk_label_new(i18n("224Hz"));
  gtk_box_pack_start((GtkBox *) control_vbox,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  /* peak 448 hz */
  control_vbox = (GtkVBox *) gtk_vbox_new(FALSE,
					  0);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) control_vbox,
		     FALSE, FALSE,
		     0);

  equalizer10->peak_448hz = (GtkScale *) gtk_vscale_new_with_range(0.0, 2.0, 0.01);
  gtk_range_set_inverted((GtkRange *) equalizer10->peak_448hz,
			 TRUE);
  gtk_range_set_value((GtkRange *) equalizer10->peak_448hz,
		      1.0);
  gtk_widget_set_size_request((GtkWidget *) equalizer10->peak_448hz,
			      -1, 100);
  gtk_box_pack_start((GtkBox *) control_vbox,
		     (GtkWidget *) equalizer10->peak_448hz,
		     FALSE, FALSE,
		     0);

  label = (GtkLabel *) gtk_label_new(i18n("448Hz"));
  gtk_box_pack_start((GtkBox *) control_vbox,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  /* peak 896 hz */
  control_vbox = (GtkVBox *) gtk_vbox_new(FALSE,
					  0);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) control_vbox,
		     FALSE, FALSE,
		     0);

  equalizer10->peak_896hz = (GtkScale *) gtk_vscale_new_with_range(0.0, 2.0, 0.01);
  gtk_range_set_inverted((GtkRange *) equalizer10->peak_896hz,
			 TRUE);
  gtk_range_set_value((GtkRange *) equalizer10->peak_896hz,
		      1.0);
  gtk_widget_set_size_request((GtkWidget *) equalizer10->peak_896hz,
			      -1, 100);
  gtk_box_pack_start((GtkBox *) control_vbox,
		     (GtkWidget *) equalizer10->peak_896hz,
		     FALSE, FALSE,
		     0);

  label = (GtkLabel *) gtk_label_new(i18n("896Hz"));
  gtk_box_pack_start((GtkBox *) control_vbox,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  /* peak 1792 hz */
  control_vbox = (GtkVBox *) gtk_vbox_new(FALSE,
					  0);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) control_vbox,
		     FALSE, FALSE,
		     0);

  equalizer10->peak_1792hz = (GtkScale *) gtk_vscale_new_with_range(0.0, 2.0, 0.01);
  gtk_range_set_inverted((GtkRange *) equalizer10->peak_1792hz,
			 TRUE);
  gtk_range_set_value((GtkRange *) equalizer10->peak_1792hz,
		      1.0);
  gtk_widget_set_size_request((GtkWidget *) equalizer10->peak_1792hz,
			      -1, 100);
  gtk_box_pack_start((GtkBox *) control_vbox,
		     (GtkWidget *) equalizer10->peak_1792hz,
		     FALSE, FALSE,
		     0);

  label = (GtkLabel *) gtk_label_new(i18n("1792Hz"));
  gtk_box_pack_start((GtkBox *) control_vbox,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  /* peak 3584 hz */
  control_vbox = (GtkVBox *) gtk_vbox_new(FALSE,
					  0);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) control_vbox,
		     FALSE, FALSE,
		     0);

  equalizer10->peak_3584hz = (GtkScale *) gtk_vscale_new_with_range(0.0, 2.0, 0.01);
  gtk_range_set_inverted((GtkRange *) equalizer10->peak_3584hz,
			 TRUE);
  gtk_range_set_value((GtkRange *) equalizer10->peak_3584hz,
		      1.0);
  gtk_widget_set_size_request((GtkWidget *) equalizer10->peak_3584hz,
			      -1, 100);
  gtk_box_pack_start((GtkBox *) control_vbox,
		     (GtkWidget *) equalizer10->peak_3584hz,
		     FALSE, FALSE,
		     0);

  label = (GtkLabel *) gtk_label_new(i18n("3584Hz"));
  gtk_box_pack_start((GtkBox *) control_vbox,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  /* peak 7168 hz */
  control_vbox = (GtkVBox *) gtk_vbox_new(FALSE,
					  0);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) control_vbox,
		     FALSE, FALSE,
		     0);

  equalizer10->peak_7168hz = (GtkScale *) gtk_vscale_new_with_range(0.0, 2.0, 0.01);
  gtk_range_set_inverted((GtkRange *) equalizer10->peak_7168hz,
			 TRUE);
  gtk_range_set_value((GtkRange *) equalizer10->peak_7168hz,
		      1.0);
  gtk_widget_set_size_request((GtkWidget *) equalizer10->peak_7168hz,
			      -1, 100);
  gtk_box_pack_start((GtkBox *) control_vbox,
		     (GtkWidget *) equalizer10->peak_7168hz,
		     FALSE, FALSE,
		     0);

  label = (GtkLabel *) gtk_label_new(i18n("7168Hz"));
  gtk_box_pack_start((GtkBox *) control_vbox,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  /* peak 14336 hz */
  control_vbox = (GtkVBox *) gtk_vbox_new(FALSE,
					  0);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) control_vbox,
		     FALSE, FALSE,
		     0);

  equalizer10->peak_14336hz = (GtkScale *) gtk_vscale_new_with_range(0.0, 2.0, 0.01);
  gtk_range_set_inverted((GtkRange *) equalizer10->peak_14336hz,
			 TRUE);
  gtk_range_set_value((GtkRange *) equalizer10->peak_14336hz,
		      1.0);
  gtk_widget_set_size_request((GtkWidget *) equalizer10->peak_14336hz,
			      -1, 100);
  gtk_box_pack_start((GtkBox *) control_vbox,
		     (GtkWidget *) equalizer10->peak_14336hz,
		     FALSE, FALSE,
		     0);

  label = (GtkLabel *) gtk_label_new(i18n("14336Hz"));
  gtk_box_pack_start((GtkBox *) control_vbox,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  /* pressure */
  control_vbox = (GtkVBox *) gtk_vbox_new(FALSE,
					  0);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) control_vbox,
		     FALSE, FALSE,
		     0);

  equalizer10->pressure = (GtkScale *) gtk_vscale_new_with_range(0.0, 2.0, 0.01);
  gtk_range_set_inverted((GtkRange *) equalizer10->pressure,
			 TRUE);
  gtk_range_set_value((GtkRange *) equalizer10->pressure,
		      1.0);
  gtk_widget_set_size_request((GtkWidget *) equalizer10->pressure,
			      -1, 100);
  gtk_box_pack_start((GtkBox *) control_vbox,
		     (GtkWidget *) equalizer10->pressure,
		     FALSE, FALSE,
		     0);

  label = (GtkLabel *) gtk_label_new(i18n("pressure"));
  gtk_box_pack_start((GtkBox *) control_vbox,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  equalizer10->peak_28hz_play_port = NULL;
  equalizer10->peak_28hz_recall_port = NULL;

  equalizer10->peak_56hz_play_port = NULL;
  equalizer10->peak_56hz_recall_port = NULL;

  equalizer10->peak_112hz_play_port = NULL;
  equalizer10->peak_112hz_recall_port = NULL;

  equalizer10->peak_224hz_play_port = NULL;
  equalizer10->peak_224hz_recall_port = NULL;

  equalizer10->peak_448hz_play_port = NULL;
  equalizer10->peak_448hz_recall_port = NULL;

  equalizer10->peak_896hz_play_port = NULL;
  equalizer10->peak_896hz_recall_port = NULL;

  equalizer10->peak_1792hz_play_port = NULL;
  equalizer10->peak_1792hz_recall_port = NULL;

  equalizer10->peak_3584hz_play_port = NULL;
  equalizer10->peak_3584hz_recall_port = NULL;

  equalizer10->peak_7168hz_play_port = NULL;
  equalizer10->peak_7168hz_recall_port = NULL;

  equalizer10->peak_14336hz_play_port = NULL;
  equalizer10->peak_14336hz_recall_port = NULL;

  equalizer10->pressure_play_port = NULL;
  equalizer10->pressure_recall_port = NULL;
}

void
ags_equalizer10_finalize(GObject *gobject)
{
  AgsEqualizer10 *equalizer10;

  equalizer10 = (AgsEqualizer10 *) gobject;

  g_object_disconnect(G_OBJECT(equalizer10),
		      "any_signal::resize-audio-channels",
		      G_CALLBACK(ags_equalizer10_resize_audio_channels),
		      NULL,
		      "any_signal::resize-pads",
		      G_CALLBACK(ags_equalizer10_resize_pads),
		      NULL,
		      NULL);

  g_list_free_full(equalizer10->peak_28hz_play_port,
		   g_object_unref);

  g_list_free_full(equalizer10->peak_28hz_recall_port,
		   g_object_unref);

  g_list_free_full(equalizer10->peak_56hz_play_port,
		   g_object_unref);

  g_list_free_full(equalizer10->peak_56hz_recall_port,
		   g_object_unref);

  g_list_free_full(equalizer10->peak_112hz_play_port,
		   g_object_unref);

  g_list_free_full(equalizer10->peak_112hz_recall_port,
		   g_object_unref);

  g_list_free_full(equalizer10->peak_224hz_play_port,
		   g_object_unref);

  g_list_free_full(equalizer10->peak_224hz_recall_port,
		   g_object_unref);

  g_list_free_full(equalizer10->peak_448hz_play_port,
		   g_object_unref);

  g_list_free_full(equalizer10->peak_448hz_recall_port,
		   g_object_unref);

  g_list_free_full(equalizer10->peak_896hz_play_port,
		   g_object_unref);

  g_list_free_full(equalizer10->peak_896hz_recall_port,
		   g_object_unref);

  g_list_free_full(equalizer10->peak_1792hz_play_port,
		   g_object_unref);

  g_list_free_full(equalizer10->peak_1792hz_recall_port,
		   g_object_unref);

  g_list_free_full(equalizer10->peak_3584hz_play_port,
		   g_object_unref);

  g_list_free_full(equalizer10->peak_3584hz_recall_port,
		   g_object_unref);

  g_list_free_full(equalizer10->peak_14336hz_play_port,
		   g_object_unref);

  g_list_free_full(equalizer10->peak_14336hz_recall_port,
		   g_object_unref);

  g_list_free_full(equalizer10->pressure_play_port,
		   g_object_unref);

  g_list_free_full(equalizer10->pressure_recall_port,
		   g_object_unref);
  
  /* call parent */
  G_OBJECT_CLASS(ags_equalizer10_parent_class)->finalize(gobject);
}

void
ags_equalizer10_connect(AgsConnectable *connectable)
{
  AgsEqualizer10 *equalizer10;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) != 0){
    return;
  }

  /* call parent */
  ags_equalizer10_parent_connectable_interface->connect(connectable);

  equalizer10 = AGS_EQUALIZER10(connectable);

  ags_equalizer10_remap_port(equalizer10);

  /* controls */
  g_signal_connect_after(equalizer10->peak_28hz, "value-changed",
			 G_CALLBACK(ags_equalizer10_peak_28hz_callback), equalizer10);

  g_signal_connect_after(equalizer10->peak_56hz, "value-changed",
			 G_CALLBACK(ags_equalizer10_peak_56hz_callback), equalizer10);

  g_signal_connect_after(equalizer10->peak_112hz, "value-changed",
			 G_CALLBACK(ags_equalizer10_peak_112hz_callback), equalizer10);

  g_signal_connect_after(equalizer10->peak_224hz, "value-changed",
			 G_CALLBACK(ags_equalizer10_peak_224hz_callback), equalizer10);

  g_signal_connect_after(equalizer10->peak_448hz, "value-changed",
			 G_CALLBACK(ags_equalizer10_peak_448hz_callback), equalizer10);

  g_signal_connect_after(equalizer10->peak_896hz, "value-changed",
			 G_CALLBACK(ags_equalizer10_peak_896hz_callback), equalizer10);

  g_signal_connect_after(equalizer10->peak_1792hz, "value-changed",
			 G_CALLBACK(ags_equalizer10_peak_1792hz_callback), equalizer10);

  g_signal_connect_after(equalizer10->peak_3584hz, "value-changed",
			 G_CALLBACK(ags_equalizer10_peak_3584hz_callback), equalizer10);

  g_signal_connect_after(equalizer10->peak_7168hz, "value-changed",
			 G_CALLBACK(ags_equalizer10_peak_7168hz_callback), equalizer10);

  g_signal_connect_after(equalizer10->peak_14336hz, "value-changed",
			 G_CALLBACK(ags_equalizer10_peak_14336hz_callback), equalizer10);
  
  g_signal_connect_after(equalizer10->pressure, "value-changed",
			 G_CALLBACK(ags_equalizer10_pressure_callback), equalizer10);
}

void
ags_equalizer10_disconnect(AgsConnectable *connectable)
{
  AgsEqualizer10 *equalizer10;

  int i;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) == 0){
    return;
  }

  equalizer10 = AGS_EQUALIZER10(connectable);

  /* controls */
  g_object_disconnect(equalizer10->peak_28hz,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_equalizer10_peak_28hz_callback),
		      equalizer10,
		      NULL);

  g_object_disconnect(equalizer10->peak_56hz,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_equalizer10_peak_56hz_callback),
		      equalizer10,
		      NULL);

  g_object_disconnect(equalizer10->peak_112hz,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_equalizer10_peak_112hz_callback),
		      equalizer10,
		      NULL);

  g_object_disconnect(equalizer10->peak_224hz,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_equalizer10_peak_224hz_callback),
		      equalizer10,
		      NULL);

  g_object_disconnect(equalizer10->peak_448hz,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_equalizer10_peak_448hz_callback),
		      equalizer10,
		      NULL);

  g_object_disconnect(equalizer10->peak_896hz,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_equalizer10_peak_896hz_callback),
		      equalizer10,
		      NULL);

  g_object_disconnect(equalizer10->peak_1792hz,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_equalizer10_peak_1792hz_callback),
		      equalizer10,
		      NULL);

  g_object_disconnect(equalizer10->peak_3584hz,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_equalizer10_peak_3584hz_callback),
		      equalizer10,
		      NULL);

  g_object_disconnect(equalizer10->peak_7168hz,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_equalizer10_peak_7168hz_callback),
		      equalizer10,
		      NULL);

  g_object_disconnect(equalizer10->peak_14336hz,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_equalizer10_peak_14336hz_callback),
		      equalizer10,
		      NULL);

  g_object_disconnect(equalizer10->pressure,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_equalizer10_pressure_callback),
		      equalizer10,
		      NULL);

  /* call parent */
  ags_equalizer10_parent_connectable_interface->disconnect(connectable);
}

void
ags_equalizer10_resize_audio_channels(AgsMachine *machine,
				      guint audio_channels, guint audio_channels_old,
				      gpointer data)
{
  AgsEqualizer10 *equalizer10;

  AgsAudio *audio;
  
  guint output_pads, input_pads;

  equalizer10 = (AgsEqualizer10 *) machine;  

  audio = machine->audio;

  /* get some fields */
  g_object_get(audio,
	       "output-pads", &output_pads,
	       "input-pads", &input_pads,
	       NULL);
  
  /* check available */
  if(input_pads == 0 &&
     output_pads == 0){
    return;
  }

  if(audio_channels > audio_channels_old){
    /* recall */
    if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
      ags_equalizer10_input_map_recall(equalizer10,
				       audio_channels_old,
				       0);

      ags_equalizer10_output_map_recall(equalizer10,
					audio_channels_old,
					0);

      ags_equalizer10_remap_port(equalizer10);
    }
  }
}

void
ags_equalizer10_resize_pads(AgsMachine *machine, GType type,
			    guint pads, guint pads_old,
			    gpointer data)
{
  AgsEqualizer10 *equalizer10;
  
  AgsAudio *audio;
  
  guint audio_channels;
  gboolean grow;

  equalizer10 = (AgsEqualizer10 *) machine;

  audio = machine->audio;

  /* get some fields */
  audio_channels = machine->audio_channels;
  
  /* check available */
  if(pads == pads_old ||
     audio_channels == 0){
    return;
  }

  if(pads_old < pads){
    grow = TRUE;
  }else{
    grow = FALSE;
  }
  
  if(g_type_is_a(type, AGS_TYPE_INPUT)){
    if(grow){      
      /* recall */
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	ags_equalizer10_input_map_recall(equalizer10,
					 0,
					 pads_old);

	ags_equalizer10_remap_port(equalizer10);
      }
    }else{
      equalizer10->mapped_input_pad = pads;

      ags_equalizer10_remap_port(equalizer10);
    }
  }else{
    if(grow){
      /* recall */
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	ags_equalizer10_output_map_recall(equalizer10,
					  0,
					  pads_old);
      }
    }else{
      equalizer10->mapped_output_pad = pads;
    }
  }
}

void
ags_equalizer10_map_recall(AgsMachine *machine)
{  
  AgsEqualizer10 *equalizer10;
  
  AgsAudio *audio;

  GList *start_recall;

  gint position;
  
  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }

  equalizer10 = (AgsEqualizer10 *) machine;

  audio = machine->audio;

  position = 0;
  
  /* add new controls */
  ags_fx_factory_create(machine->audio,
			equalizer10->eq10_play_container, equalizer10->eq10_recall_container,
			"ags-fx-eq10",
			NULL,
			NULL,
			0, 0,
			0, 0,
			position,
			(AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT), 0);

  /* depending on destination */
  ags_equalizer10_input_map_recall(equalizer10,
				   0,
				   0);

  /* depending on destination */
  ags_equalizer10_output_map_recall(equalizer10,
				    0,
				    0);

  /* call parent */
  AGS_MACHINE_CLASS(ags_equalizer10_parent_class)->map_recall(machine);
}

void
ags_equalizer10_input_map_recall(AgsEqualizer10 *equalizer10,
				 guint audio_channel_start,
				 guint input_pad_start)
{
  AgsAudio *audio;

  GList *start_recall;

  gint position;
  guint input_pads;
  guint audio_channels;

  if(equalizer10->mapped_input_pad > input_pad_start){
    return;
  }

  audio = AGS_MACHINE(equalizer10)->audio;

  position = 0;

  input_pads = 0;
  audio_channels = 0;

  /* get some fields */
  g_object_get(audio,
	       "input-pads", &input_pads,
	       "audio-channels", &audio_channels,
	       NULL);

  /* add to machine */
  ags_fx_factory_create(AGS_MACHINE(equalizer10)->audio,
			equalizer10->eq10_play_container, equalizer10->eq10_recall_container,
			"ags-fx-eq10",
			NULL,
			NULL,
			audio_channel_start, audio_channels,
			input_pad_start, input_pads,
			position,
			(AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);
  
  equalizer10->mapped_input_pad = input_pads;
}

void
ags_equalizer10_output_map_recall(AgsEqualizer10 *equalizer10,
				  guint audio_channel_start,
				  guint output_pad_start)
{
  AgsAudio *audio;

  guint output_pads;

  if(equalizer10->mapped_output_pad > output_pad_start){
    return;
  }

  audio = AGS_MACHINE(equalizer10)->audio;

  /* get some fields */
  g_object_get(audio,
	       "output-pads", &output_pads,
	       NULL);

  equalizer10->mapped_output_pad = output_pads;
}

void
ags_equalizer10_remap_port(AgsEqualizer10 *equalizer10)
{
  AgsAudio *audio;
  AgsChannel *start_input;
  AgsChannel *channel, *next_channel;
  
  guint audio_channels;
  guint i;

  audio = AGS_MACHINE(equalizer10)->audio;
  
  /* get some fields */
  g_object_get(audio,
	       "audio-channels", &audio_channels,
	       "input", &start_input,
	       NULL);

  /* destroy old */
  g_list_free_full(equalizer10->peak_28hz_play_port,
		   (GDestroyNotify) g_object_unref);

  g_list_free_full(equalizer10->peak_28hz_recall_port,
		   (GDestroyNotify) g_object_unref);

  g_list_free_full(equalizer10->peak_56hz_play_port,
		   (GDestroyNotify) g_object_unref);

  g_list_free_full(equalizer10->peak_56hz_recall_port,
		   (GDestroyNotify) g_object_unref);

  g_list_free_full(equalizer10->peak_112hz_play_port,
		   (GDestroyNotify) g_object_unref);

  g_list_free_full(equalizer10->peak_112hz_recall_port,
		   (GDestroyNotify) g_object_unref);

  g_list_free_full(equalizer10->peak_224hz_play_port,
		   (GDestroyNotify) g_object_unref);

  g_list_free_full(equalizer10->peak_224hz_recall_port,
		   (GDestroyNotify) g_object_unref);

  g_list_free_full(equalizer10->peak_448hz_play_port,
		   (GDestroyNotify) g_object_unref);

  g_list_free_full(equalizer10->peak_448hz_recall_port,
		   (GDestroyNotify) g_object_unref);

  g_list_free_full(equalizer10->peak_896hz_play_port,
		   (GDestroyNotify) g_object_unref);

  g_list_free_full(equalizer10->peak_896hz_recall_port,
		   (GDestroyNotify) g_object_unref);

  g_list_free_full(equalizer10->peak_1792hz_play_port,
		   (GDestroyNotify) g_object_unref);

  g_list_free_full(equalizer10->peak_1792hz_recall_port,
		   (GDestroyNotify) g_object_unref);

  g_list_free_full(equalizer10->peak_3584hz_play_port,
		   (GDestroyNotify) g_object_unref);

  g_list_free_full(equalizer10->peak_3584hz_recall_port,
		   (GDestroyNotify) g_object_unref);

  g_list_free_full(equalizer10->peak_7168hz_play_port,
		   (GDestroyNotify) g_object_unref);

  g_list_free_full(equalizer10->peak_7168hz_recall_port,
		   (GDestroyNotify) g_object_unref);

  g_list_free_full(equalizer10->peak_14336hz_play_port,
		   (GDestroyNotify) g_object_unref);

  g_list_free_full(equalizer10->peak_14336hz_recall_port,
		   (GDestroyNotify) g_object_unref);
  
  g_list_free_full(equalizer10->pressure_play_port,
		   (GDestroyNotify) g_object_unref);

  g_list_free_full(equalizer10->pressure_recall_port,
		   (GDestroyNotify) g_object_unref);

  equalizer10->peak_28hz_play_port = NULL;
  equalizer10->peak_28hz_recall_port = NULL;

  equalizer10->peak_56hz_play_port = NULL;
  equalizer10->peak_56hz_recall_port = NULL;

  equalizer10->peak_112hz_play_port = NULL;
  equalizer10->peak_112hz_recall_port = NULL;

  equalizer10->peak_224hz_play_port = NULL;
  equalizer10->peak_224hz_recall_port = NULL;

  equalizer10->peak_448hz_play_port = NULL;
  equalizer10->peak_448hz_recall_port = NULL;

  equalizer10->peak_896hz_play_port = NULL;
  equalizer10->peak_896hz_recall_port = NULL;

  equalizer10->peak_1792hz_play_port = NULL;
  equalizer10->peak_1792hz_recall_port = NULL;

  equalizer10->peak_3584hz_play_port = NULL;
  equalizer10->peak_3584hz_recall_port = NULL;

  equalizer10->peak_7168hz_play_port = NULL;
  equalizer10->peak_7168hz_recall_port = NULL;

  equalizer10->peak_14336hz_play_port = NULL;
  equalizer10->peak_14336hz_recall_port = NULL;

  equalizer10->pressure_play_port = NULL;
  equalizer10->pressure_recall_port = NULL;

  /* prepend new */
  channel = start_input;

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  for(i = 0; i < audio_channels && channel != NULL; i++){
    AgsPort *port;

    GList *start_play, *play;
    GList *start_recall, *recall;

    g_object_get(channel,
		 "play", &start_play,
		 "recall", &start_recall,
		 NULL);
    
    /* peak 28Hz  - find port */
    port = ags_equalizer10_find_specifier(start_play,
					  "./peak-28hz[0]");

    if(port != NULL){
      g_object_ref(port);

      equalizer10->peak_28hz_play_port = g_list_prepend(equalizer10->peak_28hz_play_port,
							port);
    }

    port = ags_equalizer10_find_specifier(start_recall,
					  "./peak-28hz[0]");

    if(port != NULL){
      g_object_ref(port);

      equalizer10->peak_28hz_recall_port = g_list_prepend(equalizer10->peak_28hz_recall_port,
							  port);
    }

    /* peak 56Hz  - find port */
    port = ags_equalizer10_find_specifier(start_play,
					  "./peak-56hz[0]");

    if(port != NULL){
      g_object_ref(port);

      equalizer10->peak_56hz_play_port = g_list_prepend(equalizer10->peak_56hz_play_port,
							port);
    }

    port = ags_equalizer10_find_specifier(start_recall,
					  "./peak-56hz[0]");

    if(port != NULL){
      g_object_ref(port);

      equalizer10->peak_56hz_recall_port = g_list_prepend(equalizer10->peak_56hz_recall_port,
							  port);
    }

    /* peak 112Hz  - find port */
    port = ags_equalizer10_find_specifier(start_play,
					  "./peak-112hz[0]");

    if(port != NULL){
      g_object_ref(port);

      equalizer10->peak_112hz_play_port = g_list_prepend(equalizer10->peak_112hz_play_port,
							 port);
    }

    port = ags_equalizer10_find_specifier(start_recall,
					  "./peak-112hz[0]");

    if(port != NULL){
      g_object_ref(port);

      equalizer10->peak_112hz_recall_port = g_list_prepend(equalizer10->peak_112hz_recall_port,
							   port);
    }

    /* peak 224Hz  - find port */
    port = ags_equalizer10_find_specifier(start_play,
					  "./peak-224hz[0]");

    if(port != NULL){
      g_object_ref(port);

      equalizer10->peak_224hz_play_port = g_list_prepend(equalizer10->peak_224hz_play_port,
							 port);
    }

    port = ags_equalizer10_find_specifier(start_recall,
					  "./peak-224hz[0]");

    if(port != NULL){
      g_object_ref(port);

      equalizer10->peak_224hz_recall_port = g_list_prepend(equalizer10->peak_224hz_recall_port,
							   port);
    }

    /* peak 448Hz  - find port */
    port = ags_equalizer10_find_specifier(start_play,
					  "./peak-448hz[0]");

    if(port != NULL){
      g_object_ref(port);

      equalizer10->peak_448hz_play_port = g_list_prepend(equalizer10->peak_448hz_play_port,
							 port);
    }

    port = ags_equalizer10_find_specifier(start_recall,
					  "./peak-448hz[0]");

    if(port != NULL){
      g_object_ref(port);

      equalizer10->peak_448hz_recall_port = g_list_prepend(equalizer10->peak_448hz_recall_port,
							   port);
    }

    /* peak 896Hz  - find port */
    port = ags_equalizer10_find_specifier(start_play,
					  "./peak-896hz[0]");

    if(port != NULL){
      g_object_ref(port);

      equalizer10->peak_896hz_play_port = g_list_prepend(equalizer10->peak_896hz_play_port,
							 port);
    }

    port = ags_equalizer10_find_specifier(start_recall,
					  "./peak-896hz[0]");

    if(port != NULL){
      g_object_ref(port);

      equalizer10->peak_896hz_recall_port = g_list_prepend(equalizer10->peak_896hz_recall_port,
							   port);
    }

    /* peak 1792Hz  - find port */
    port = ags_equalizer10_find_specifier(start_play,
					  "./peak-1792hz[0]");

    if(port != NULL){
      g_object_ref(port);

      equalizer10->peak_1792hz_play_port = g_list_prepend(equalizer10->peak_1792hz_play_port,
							  port);
    }

    port = ags_equalizer10_find_specifier(start_recall,
					  "./peak-1792hz[0]");

    if(port != NULL){
      g_object_ref(port);

      equalizer10->peak_1792hz_recall_port = g_list_prepend(equalizer10->peak_1792hz_recall_port,
							    port);
    }

    /* peak 3584Hz  - find port */
    port = ags_equalizer10_find_specifier(start_play,
					  "./peak-3584hz[0]");

    if(port != NULL){
      g_object_ref(port);

      equalizer10->peak_3584hz_play_port = g_list_prepend(equalizer10->peak_3584hz_play_port,
							  port);
    }

    port = ags_equalizer10_find_specifier(start_recall,
					  "./peak-3584hz[0]");

    if(port != NULL){
      g_object_ref(port);

      equalizer10->peak_3584hz_recall_port = g_list_prepend(equalizer10->peak_3584hz_recall_port,
							    port);
    }

    /* peak 7168Hz  - find port */
    port = ags_equalizer10_find_specifier(start_play,
					  "./peak-7168hz[0]");

    if(port != NULL){
      g_object_ref(port);

      equalizer10->peak_7168hz_play_port = g_list_prepend(equalizer10->peak_7168hz_play_port,
							  port);
    }

    port = ags_equalizer10_find_specifier(start_recall,
					  "./peak-7168hz[0]");

    if(port != NULL){
      g_object_ref(port);

      equalizer10->peak_7168hz_recall_port = g_list_prepend(equalizer10->peak_7168hz_recall_port,
							    port);
    }

    /* peak 14336Hz  - find port */
    port = ags_equalizer10_find_specifier(start_play,
					  "./peak-14336hz[0]");

    if(port != NULL){
      g_object_ref(port);

      equalizer10->peak_14336hz_play_port = g_list_prepend(equalizer10->peak_14336hz_play_port,
							   port);
    }

    port = ags_equalizer10_find_specifier(start_recall,
					  "./peak-14336hz[0]");

    if(port != NULL){
      g_object_ref(port);

      equalizer10->peak_14336hz_recall_port = g_list_prepend(equalizer10->peak_14336hz_recall_port,
							     port);
    }

    /* pressure  - find port */
    port = ags_equalizer10_find_specifier(start_play,
					  "./pressure[0]");

    if(port != NULL){
      g_object_ref(port);

      equalizer10->pressure_play_port = g_list_prepend(equalizer10->pressure_play_port,
						       port);
    }

    port = ags_equalizer10_find_specifier(start_recall,
					  "./pressure[0]");

    if(port != NULL){
      g_object_ref(port);

      equalizer10->pressure_recall_port = g_list_prepend(equalizer10->pressure_recall_port,
							 port);
    }

    g_list_free_full(start_play,
		     g_object_unref);
    g_list_free_full(start_recall,
		     g_object_unref);
    
    /* iterate */
    next_channel = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next_channel;
  }

  if(start_input != NULL){
    g_object_unref(start_input);
  }

  if(channel != NULL){
    g_object_unref(channel);
  }
}

AgsPort*
ags_equalizer10_find_specifier(GList *recall, gchar *specifier)
{
  GList *port;
    
  while(recall != NULL){
    port = AGS_RECALL(recall->data)->port;

#ifdef AGS_DEBUG
    g_message("search port in %s", G_OBJECT_TYPE_NAME(recall->data));
#endif

    while(port != NULL){
      if(!g_strcmp0(AGS_PORT(port->data)->specifier,
		    specifier)){
	return(AGS_PORT(port->data));
      }

      port = port->next;
    }

    recall = recall->next;
  }

  return(NULL);
}

/**
 * ags_equalizer10_new:
 * @soundcard: the assigned soundcard.
 *
 * Creates an #AgsEqualizer10
 *
 * Returns: a new #AgsEqualizer10
 *
 * Since: 3.0.0
 */
AgsEqualizer10*
ags_equalizer10_new(GObject *soundcard)
{
  AgsEqualizer10 *equalizer10;

  equalizer10 = (AgsEqualizer10 *) g_object_new(AGS_TYPE_EQUALIZER10,
						NULL);

  g_object_set(G_OBJECT(AGS_MACHINE(equalizer10)->audio),
	       "output-soundcard", soundcard,
	       NULL);

  return(equalizer10);
}

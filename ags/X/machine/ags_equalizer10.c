/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/i18n.h>

void ags_equalizer10_class_init(AgsEqualizer10Class *equalizer10);
void ags_equalizer10_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_equalizer10_plugin_interface_init(AgsPluginInterface *plugin);
void ags_equalizer10_init(AgsEqualizer10 *equalizer10);
void ags_equalizer10_finalize(GObject *gobject);

void ags_equalizer10_map_recall(AgsMachine *machine);

void ags_equalizer10_connect(AgsConnectable *connectable);
void ags_equalizer10_disconnect(AgsConnectable *connectable);

gchar* ags_equalizer10_get_name(AgsPlugin *plugin);
void ags_equalizer10_set_name(AgsPlugin *plugin, gchar *name);
gchar* ags_equalizer10_get_xml_type(AgsPlugin *plugin);
void ags_equalizer10_set_xml_type(AgsPlugin *plugin, gchar *xml_type);
void ags_equalizer10_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_equalizer10_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

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

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_equalizer10_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_equalizer10 = g_type_register_static(AGS_TYPE_MACHINE,
						  "AgsEqualizer10", &ags_equalizer10_info,
						  0);
    
    g_type_add_interface_static(ags_type_equalizer10,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_equalizer10,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_equalizer10);
  }

  return g_define_type_id__volatile;
}

void
ags_equalizer10_class_init(AgsEqualizer10Class *equalizer10)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  AgsMachineClass *machine;

  ags_equalizer10_parent_class = g_type_class_peek_parent(equalizer10);

  /* GObjectClass */
  gobject = (GObjectClass *) equalizer10;

  gobject->finalize = ags_equalizer10_finalize;

  /* GtkWidget */
  widget = (GtkWidgetClass *) equalizer10;

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
ags_equalizer10_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = ags_equalizer10_get_name;
  plugin->set_name = ags_equalizer10_set_name;
  plugin->get_xml_type = ags_equalizer10_get_xml_type;
  plugin->set_xml_type = ags_equalizer10_set_xml_type;
  plugin->read = ags_equalizer10_read;
  plugin->write = ags_equalizer10_write;
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

  equalizer10->name = NULL;
  equalizer10->xml_type = "ags-equalizer10";

  vbox = (GtkVBox *) gtk_vbox_new(FALSE,
				  0);
  gtk_container_add((GtkContainer*) gtk_bin_get_child((GtkBin *) equalizer10), (GtkWidget *) vbox);

  /* controls */
  hbox = (GtkVBox *) gtk_hbox_new(FALSE,
				  4);
  gtk_box_pack_start(vbox,
		     hbox,
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
  gtk_range_set_inverted(equalizer10->peak_28hz,
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
  gtk_range_set_inverted(equalizer10->peak_56hz,
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
  gtk_range_set_inverted(equalizer10->peak_112hz,
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
  gtk_range_set_inverted(equalizer10->peak_224hz,
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
  gtk_range_set_inverted(equalizer10->peak_448hz,
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
  gtk_range_set_inverted(equalizer10->peak_896hz,
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
  gtk_range_set_inverted(equalizer10->peak_1792hz,
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
  gtk_range_set_inverted(equalizer10->peak_3584hz,
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
  gtk_range_set_inverted(equalizer10->peak_7168hz,
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
  gtk_range_set_inverted(equalizer10->peak_14336hz,
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
  gtk_range_set_inverted(equalizer10->pressure,
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

  equalizer10 = gobject;
  
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

  equalizer10 = AGS_EQUALIZER10(connectable);

  g_signal_connect_after(equalizer10, "resize-audio-channels",
			 G_CALLBACK(ags_equalizer10_resize_audio_channels_callback), NULL);
  
  g_signal_connect_after(equalizer10, "resize-pads",
			 G_CALLBACK(ags_equalizer10_resize_pads_callback), NULL);

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

  /* call parent */
  ags_equalizer10_parent_connectable_interface->connect(connectable);
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

  g_object_disconnect(equalizer10,
		      "resize-audio-channels",
		      G_CALLBACK(ags_equalizer10_resize_audio_channels_callback),
		      NULL,
		      NULL);

  g_object_disconnect(equalizer10,
		      "resize-pads",
		      G_CALLBACK(ags_equalizer10_resize_pads_callback),
		      NULL,
		      NULL);

  /* controls */
  g_object_disconnect(equalizer10->peak_28hz,
		      "value-changed",
		      G_CALLBACK(ags_equalizer10_peak_28hz_callback),
		      equalizer10,
		      NULL);

  g_object_disconnect(equalizer10->peak_56hz,
		      "value-changed",
		      G_CALLBACK(ags_equalizer10_peak_56hz_callback),
		      equalizer10,
		      NULL);

  g_object_disconnect(equalizer10->peak_112hz,
		      "value-changed",
		      G_CALLBACK(ags_equalizer10_peak_112hz_callback),
		      equalizer10,
		      NULL);

  g_object_disconnect(equalizer10->peak_224hz,
		      "value-changed",
		      G_CALLBACK(ags_equalizer10_peak_224hz_callback),
		      equalizer10,
		      NULL);

  g_object_disconnect(equalizer10->peak_448hz,
		      "value-changed",
		      G_CALLBACK(ags_equalizer10_peak_448hz_callback),
		      equalizer10,
		      NULL);

  g_object_disconnect(equalizer10->peak_896hz,
		      "value-changed",
		      G_CALLBACK(ags_equalizer10_peak_896hz_callback),
		      equalizer10,
		      NULL);

  g_object_disconnect(equalizer10->peak_1792hz,
		      "value-changed",
		      G_CALLBACK(ags_equalizer10_peak_1792hz_callback),
		      equalizer10,
		      NULL);

  g_object_disconnect(equalizer10->peak_3584hz,
		      "value-changed",
		      G_CALLBACK(ags_equalizer10_peak_3584hz_callback),
		      equalizer10,
		      NULL);

  g_object_disconnect(equalizer10->peak_7168hz,
		      "value-changed",
		      G_CALLBACK(ags_equalizer10_peak_7168hz_callback),
		      equalizer10,
		      NULL);

  g_object_disconnect(equalizer10->peak_14336hz,
		      "value-changed",
		      G_CALLBACK(ags_equalizer10_peak_14336hz_callback),
		      equalizer10,
		      NULL);

  g_object_disconnect(equalizer10->pressure,
		      "value-changed",
		      G_CALLBACK(ags_equalizer10_pressure_callback),
		      equalizer10,
		      NULL);

  /* call parent */
  ags_equalizer10_parent_connectable_interface->disconnect(connectable);
}

void
ags_equalizer10_map_recall(AgsMachine *machine)
{
  AgsEqualizer10 *equalizer10;
  
  AgsAudio *audio;
  AgsChannel *channel;
  
  guint audio_channels;
  guint i;

  equalizer10 = machine;
  
  audio = machine->audio;
  
  /* get some fields */
  g_object_get(audio,
	       "audio-channels", &audio_channels,
	       "input", &channel,
	       NULL);

  /* ags-eq10 */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-eq10",
			    0, audio_channels,
			    0, 1,
			    (AGS_RECALL_FACTORY_INPUT |
			     AGS_RECALL_FACTORY_PLAY |
			     AGS_RECALL_FACTORY_RECALL |
			     AGS_RECALL_FACTORY_ADD),
			    0);
  
  for(i = 0; i < audio_channels; i++){
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

    g_list_free(start_play);
    g_list_free(start_recall);
    
    /* iterate */
    g_object_get(channel,
		 "next", &channel,
		 NULL);
  }
  
  /* call parent */
  AGS_MACHINE_CLASS(ags_equalizer10_parent_class)->map_recall(machine);
}

gchar*
ags_equalizer10_get_name(AgsPlugin *plugin)
{
  return(AGS_EQUALIZER10(plugin)->name);
}

void
ags_equalizer10_set_name(AgsPlugin *plugin, gchar *name)
{
  AGS_EQUALIZER10(plugin)->name = name;
}

gchar*
ags_equalizer10_get_xml_type(AgsPlugin *plugin)
{
  return(AGS_EQUALIZER10(plugin)->xml_type);
}

void
ags_equalizer10_set_xml_type(AgsPlugin *plugin, gchar *xml_type)
{
  AGS_EQUALIZER10(plugin)->xml_type = xml_type;
}

void
ags_equalizer10_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  AgsEqualizer10 *gobject;

  gobject = AGS_EQUALIZER10(plugin);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference", gobject,
				   NULL));
}

xmlNode*
ags_equalizer10_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  AgsEqualizer10 *equalizer10;
  xmlNode *node;
  GList *list;
  gchar *id;
  guint i;

  equalizer10 = AGS_EQUALIZER10(plugin);

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    "ags-equalizer10");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", id),
				   "reference", equalizer10,
				   NULL));

  return(node);
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
 * Since: 2.0.0
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

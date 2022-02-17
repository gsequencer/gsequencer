/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/GSequencer/machine/ags_fm_synth_input_line.h>
#include <ags/GSequencer/machine/ags_fm_synth_input_line_callbacks.h>

#include <ags/GSequencer/ags_window.h>
#include <ags/GSequencer/ags_line.h>
#include <ags/GSequencer/ags_line_member.h>

#include <ags/GSequencer/machine/ags_fm_synth.h>
#include <ags/GSequencer/machine/ags_fm_oscillator.h>

void ags_fm_synth_input_line_class_init(AgsFMSynthInputLineClass *fm_synth_input_line);
void ags_fm_synth_input_line_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_fm_synth_input_line_init(AgsFMSynthInputLine *fm_synth_input_line);

void ags_fm_synth_input_line_connect(AgsConnectable *connectable);
void ags_fm_synth_input_line_disconnect(AgsConnectable *connectable);

void ags_fm_synth_input_line_show(GtkWidget *line);
void ags_fm_synth_input_line_show_all(GtkWidget *line);

void ags_fm_synth_input_line_set_channel(AgsLine *line, AgsChannel *channel);
void ags_fm_synth_input_line_map_recall(AgsLine *line,
					guint output_pad_start);

/**
 * SECTION:ags_fm_synth_input_line
 * @short_description: fm synth input line
 * @title: AgsFMSynthInputLine
 * @section_id:
 * @include: ags/X/machine/ags_fm_synth_input_line.h
 *
 * The #AgsFMSynthInputLine is a composite widget to act as fm synth input line.
 */

static gpointer ags_fm_synth_input_line_parent_class = NULL;
static AgsConnectableInterface *ags_fm_synth_input_line_parent_connectable_interface;

GType
ags_fm_synth_input_line_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fm_synth_input_line = 0;

    static const GTypeInfo ags_fm_synth_input_line_info = {
      sizeof(AgsFMSynthInputLineClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fm_synth_input_line_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsFMSynthInputLine),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fm_synth_input_line_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_fm_synth_input_line_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_fm_synth_input_line = g_type_register_static(AGS_TYPE_LINE,
							  "AgsFMSynthInputLine", &ags_fm_synth_input_line_info,
							  0);

    g_type_add_interface_static(ags_type_fm_synth_input_line,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fm_synth_input_line);
  }

  return g_define_type_id__volatile;
}

void
ags_fm_synth_input_line_class_init(AgsFMSynthInputLineClass *fm_synth_input_line)
{
  AgsLineClass *line;
  GtkWidgetClass *widget;
  
  ags_fm_synth_input_line_parent_class = g_type_class_peek_parent(fm_synth_input_line);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) fm_synth_input_line;

  widget->show = ags_fm_synth_input_line_show;
  widget->show_all = ags_fm_synth_input_line_show_all;

  /* AgsLineClass */
  line = AGS_LINE_CLASS(fm_synth_input_line);

  line->set_channel = ags_fm_synth_input_line_set_channel;
  line->map_recall = ags_fm_synth_input_line_map_recall;
}

void
ags_fm_synth_input_line_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_fm_synth_input_line_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_fm_synth_input_line_connect;
  connectable->disconnect = ags_fm_synth_input_line_disconnect;
}

void
ags_fm_synth_input_line_init(AgsFMSynthInputLine *fm_synth_input_line)
{
  AgsFMOscillator *fm_oscillator;

  fm_synth_input_line->name = NULL;
  fm_synth_input_line->xml_type = "ags-fm-synth-input-line";

  /* fm_oscillator */
  fm_oscillator = ags_fm_oscillator_new();
  fm_synth_input_line->fm_oscillator = fm_oscillator;
  ags_expander_add(AGS_LINE(fm_synth_input_line)->expander,
		   GTK_WIDGET(fm_oscillator),
		   0, 0,
		   1, 1);

  g_signal_connect((GObject *) fm_synth_input_line, "samplerate-changed",
		   G_CALLBACK(ags_fm_synth_input_line_samplerate_changed_callback), NULL);
}

void
ags_fm_synth_input_line_connect(AgsConnectable *connectable)
{
  AgsFMSynthInputLine *fm_synth_input_line;

  fm_synth_input_line = AGS_FM_SYNTH_INPUT_LINE(connectable);

  if((AGS_LINE_CONNECTED & (AGS_LINE(fm_synth_input_line)->flags)) != 0){
    return;
  }

  ags_fm_synth_input_line_parent_connectable_interface->connect(connectable);

  ags_connectable_connect(AGS_CONNECTABLE(fm_synth_input_line->fm_oscillator));

  g_signal_connect_after((GObject *) fm_synth_input_line->fm_oscillator, "control-changed",
			 G_CALLBACK(ags_fm_synth_input_line_fm_oscillator_control_changed_callback), (gpointer) fm_synth_input_line);
}

void
ags_fm_synth_input_line_disconnect(AgsConnectable *connectable)
{
  AgsFMSynthInputLine *fm_synth_input_line;

  fm_synth_input_line = AGS_FM_SYNTH_INPUT_LINE(connectable);

  if((AGS_LINE_CONNECTED & (AGS_LINE(fm_synth_input_line)->flags)) == 0){
    return;
  }

  ags_fm_synth_input_line_parent_connectable_interface->disconnect(connectable);

  ags_connectable_disconnect(AGS_CONNECTABLE(fm_synth_input_line->fm_oscillator));

  g_object_disconnect((GObject *) fm_synth_input_line->fm_oscillator,
		      "any_signal::control-changed",
		      G_CALLBACK(ags_fm_synth_input_line_fm_oscillator_control_changed_callback),
		      (gpointer) fm_synth_input_line,
		      NULL);
}

void
ags_fm_synth_input_line_set_channel(AgsLine *line, AgsChannel *channel)
{
  AGS_LINE_CLASS(ags_fm_synth_input_line_parent_class)->set_channel(line, channel);

  /* empty */
}

void
ags_fm_synth_input_line_map_recall(AgsLine *line,
				   guint output_pad_start)
{
  if((AGS_LINE_MAPPED_RECALL & (line->flags)) != 0 ||
     (AGS_LINE_PREMAPPED_RECALL & (line->flags)) != 0){
    return;
  }
  
  /* empty */
  
  /* call parent */
  AGS_LINE_CLASS(ags_fm_synth_input_line_parent_class)->map_recall(line,
								   output_pad_start);
}

void
ags_fm_synth_input_line_show(GtkWidget *line)
{
  GTK_WIDGET_CLASS(ags_fm_synth_input_line_parent_class)->show(line);

  gtk_widget_hide(GTK_WIDGET(AGS_LINE(line)->group));
}

void
ags_fm_synth_input_line_show_all(GtkWidget *line)
{
  GTK_WIDGET_CLASS(ags_fm_synth_input_line_parent_class)->show_all(line);

  gtk_widget_hide(GTK_WIDGET(AGS_LINE(line)->group));
}

/**
 * ags_fm_synth_input_line_new:
 * @channel: the assigned channel
 *
 * Create a new instance of #AgsFMSynthInputLine
 *
 * Returns: the new #AgsFMSynthInputLine
 *
 * Since: 3.0.0
 */
AgsFMSynthInputLine*
ags_fm_synth_input_line_new(AgsChannel *channel)
{
  AgsFMSynthInputLine *fm_synth_input_line;

  fm_synth_input_line = (AgsFMSynthInputLine *) g_object_new(AGS_TYPE_FM_SYNTH_INPUT_LINE,
							     "channel", channel,
							     NULL);

  return(fm_synth_input_line);
}

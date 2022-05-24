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

#include <ags/app/machine/ags_synth_input_line.h>
#include <ags/app/machine/ags_synth_input_line_callbacks.h>

#include <ags/app/ags_window.h>
#include <ags/app/ags_line.h>
#include <ags/app/ags_line_member.h>

#include <ags/app/machine/ags_synth.h>
#include <ags/app/machine/ags_oscillator.h>

void ags_synth_input_line_class_init(AgsSynthInputLineClass *synth_input_line);
void ags_synth_input_line_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_synth_input_line_init(AgsSynthInputLine *synth_input_line);

void ags_synth_input_line_connect(AgsConnectable *connectable);
void ags_synth_input_line_disconnect(AgsConnectable *connectable);

void ags_synth_input_line_show(GtkWidget *line);

void ags_synth_input_line_set_channel(AgsLine *line, AgsChannel *channel);
void ags_synth_input_line_map_recall(AgsLine *line,
				     guint output_pad_start);

/**
 * SECTION:ags_synth_input_line
 * @short_description: synth input line
 * @title: AgsSynthInputLine
 * @section_id:
 * @include: ags/app/machine/ags_synth_input_line.h
 *
 * The #AgsSynthInputLine is a composite widget to act as synth input line.
 */

static gpointer ags_synth_input_line_parent_class = NULL;
static AgsConnectableInterface *ags_synth_input_line_parent_connectable_interface;

GType
ags_synth_input_line_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_synth_input_line = 0;

    static const GTypeInfo ags_synth_input_line_info = {
      sizeof(AgsSynthInputLineClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_synth_input_line_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsSynthInputLine),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_synth_input_line_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_synth_input_line_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_synth_input_line = g_type_register_static(AGS_TYPE_LINE,
						       "AgsSynthInputLine", &ags_synth_input_line_info,
						       0);

    g_type_add_interface_static(ags_type_synth_input_line,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_synth_input_line);
  }

  return g_define_type_id__volatile;
}

void
ags_synth_input_line_class_init(AgsSynthInputLineClass *synth_input_line)
{
  AgsLineClass *line;
  GtkWidgetClass *widget;
  
  ags_synth_input_line_parent_class = g_type_class_peek_parent(synth_input_line);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) synth_input_line;

  widget->show = ags_synth_input_line_show;

  /* AgsLineClass */
  line = AGS_LINE_CLASS(synth_input_line);

  line->set_channel = ags_synth_input_line_set_channel;
  line->map_recall = ags_synth_input_line_map_recall;
}

void
ags_synth_input_line_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_synth_input_line_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_synth_input_line_connect;
  connectable->disconnect = ags_synth_input_line_disconnect;
}

void
ags_synth_input_line_init(AgsSynthInputLine *synth_input_line)
{
  AgsLineMember *line_member;

  synth_input_line->name = NULL;
  synth_input_line->xml_type = "ags-synth-input-line";

  /* oscillator */
  line_member = ags_line_member_new();
  g_object_set(line_member,
	       "widget-type", AGS_TYPE_OSCILLATOR,
	       NULL);
  ags_line_add_line_member(AGS_LINE(synth_input_line),
			   line_member,
			   0, 0,
			   1, 1);
  
  synth_input_line->oscillator = (AgsOscillator *) ags_line_member_get_widget(line_member);

  g_signal_connect((GObject *) synth_input_line, "samplerate-changed",
		   G_CALLBACK(ags_synth_input_line_samplerate_changed_callback), NULL);

  gtk_widget_set_visible(AGS_LINE(synth_input_line)->group,
			 FALSE);

  gtk_widget_set_visible(synth_input_line->oscillator->selector,
			 FALSE);
}

void
ags_synth_input_line_connect(AgsConnectable *connectable)
{
  AgsSynthInputLine *synth_input_line;

  synth_input_line = AGS_SYNTH_INPUT_LINE(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (AGS_LINE(synth_input_line)->connectable_flags)) != 0){
    return;
  }

  ags_synth_input_line_parent_connectable_interface->connect(connectable);

  ags_connectable_connect(AGS_CONNECTABLE(synth_input_line->oscillator));

  g_signal_connect_after((GObject *) synth_input_line->oscillator, "control-changed",
			 G_CALLBACK(ags_synth_input_line_oscillator_control_changed_callback), (gpointer) synth_input_line);
}

void
ags_synth_input_line_disconnect(AgsConnectable *connectable)
{
  AgsSynthInputLine *synth_input_line;

  synth_input_line = AGS_SYNTH_INPUT_LINE(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (AGS_LINE(synth_input_line)->connectable_flags)) == 0){
    return;
  }

  ags_synth_input_line_parent_connectable_interface->disconnect(connectable);

  ags_connectable_disconnect(AGS_CONNECTABLE(synth_input_line->oscillator));

  g_object_disconnect((GObject *) synth_input_line->oscillator,
		      "any_signal::control-changed",
		      G_CALLBACK(ags_synth_input_line_oscillator_control_changed_callback),
		      (gpointer) synth_input_line,
		      NULL);
}

void
ags_synth_input_line_set_channel(AgsLine *line, AgsChannel *channel)
{
  AGS_LINE_CLASS(ags_synth_input_line_parent_class)->set_channel(line, channel);

  /* empty */
}

void
ags_synth_input_line_map_recall(AgsLine *line,
				guint output_pad_start)
{
  if((AGS_LINE_MAPPED_RECALL & (line->flags)) != 0 ||
     (AGS_LINE_PREMAPPED_RECALL & (line->flags)) != 0){
    return;
  }
  
  /* empty */
  
  /* call parent */
  AGS_LINE_CLASS(ags_synth_input_line_parent_class)->map_recall(line,
								output_pad_start);
}

void
ags_synth_input_line_show(GtkWidget *line)
{
  GTK_WIDGET_CLASS(ags_synth_input_line_parent_class)->show(line);

  gtk_widget_hide(GTK_WIDGET(AGS_LINE(line)->group));
}

/**
 * ags_synth_input_line_new:
 * @channel: the assigned channel
 *
 * Create a new instance of #AgsSynthInputLine
 *
 * Returns: the new #AgsSynthInputLine
 *
 * Since: 3.0.0
 */
AgsSynthInputLine*
ags_synth_input_line_new(AgsChannel *channel)
{
  AgsSynthInputLine *synth_input_line;

  synth_input_line = (AgsSynthInputLine *) g_object_new(AGS_TYPE_SYNTH_INPUT_LINE,
							"channel", channel,
							NULL);

  return(synth_input_line);
}

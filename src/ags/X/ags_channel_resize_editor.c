/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
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

#include <ags/X/ags_channel_resize_editor.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>

#include <ags/X/ags_machine_editor.h>

void ags_channel_resize_editor_class_init(AgsChannelResizeEditorClass *channel_resize_editor);
void ags_channel_resize_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_channel_resize_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_channel_resize_editor_init(AgsChannelResizeEditor *channel_resize_editor);
void ags_channel_resize_editor_connect(AgsConnectable *connectable);
void ags_channel_resize_editor_disconnect(AgsConnectable *connectable);
void ags_channel_resize_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_channel_resize_editor_apply(AgsApplicable *applicable);
void ags_channel_resize_editor_reset(AgsApplicable *applicable);
void ags_channel_resize_editor_destroy(GtkObject *object);
void ags_channel_resize_editor_show(GtkWidget *widget);

AgsConnectableInterface *ags_channel_resize_editor_parent_connectable_interface;

GType
ags_channel_resize_editor_get_type(void)
{
  static GType ags_type_channel_resize_editor = 0;

  if(!ags_type_channel_resize_editor){
    static const GTypeInfo ags_channel_resize_editor_info = {
      sizeof (AgsChannelResizeEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_channel_resize_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsChannelResizeEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_channel_resize_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_channel_resize_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_channel_resize_editor_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_channel_resize_editor = g_type_register_static(AGS_TYPE_PROPERTY_EDITOR,
							    "AgsChannelResizeEditor\0",
							    &ags_channel_resize_editor_info,
							    0);
    
    g_type_add_interface_static(ags_type_channel_resize_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_channel_resize_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }

  return(ags_type_channel_resize_editor);
}

void
ags_channel_resize_editor_class_init(AgsChannelResizeEditorClass *channel_resize_editor)
{
}

void
ags_channel_resize_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_channel_resize_editor_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_channel_resize_editor_connect;
  connectable->disconnect = ags_channel_resize_editor_disconnect;
}

void
ags_channel_resize_editor_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_channel_resize_editor_set_update;
  applicable->apply = ags_channel_resize_editor_apply;
  applicable->reset = ags_channel_resize_editor_reset;
}

void
ags_channel_resize_editor_init(AgsChannelResizeEditor *channel_resize_editor)
{
  GtkTable *table;
  GtkAlignment *alignment;
  GtkLabel *label;

  table = (GtkTable *) gtk_table_new(3, 2, FALSE);
  gtk_box_pack_start(GTK_BOX(channel_resize_editor),
		     GTK_WIDGET(table),
		     FALSE, FALSE,
		     0);

  /* audio channels */
  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.5,
						 0.0, 0.0);
  gtk_table_attach(table,
		   GTK_WIDGET(alignment),
		   0, 1,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  label = (GtkLabel *) gtk_label_new("audio channels\0");
  gtk_container_add(GTK_CONTAINER(alignment),
		    GTK_WIDGET(label));

  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.5,
						 0.0, 0.0);
  gtk_table_attach(table,
		   GTK_WIDGET(alignment),
		   1, 2,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  channel_resize_editor->audio_channels = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 1024.0, 1.0);
  gtk_container_add(GTK_CONTAINER(alignment),
		    GTK_WIDGET(channel_resize_editor->audio_channels));

  /* output pads */
  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.5,
						 0.0, 0.0);
  gtk_table_attach(table,
		   GTK_WIDGET(alignment),
		   0, 1,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  label = (GtkLabel *) gtk_label_new("outputs\0");
  gtk_container_add(GTK_CONTAINER(alignment),
		    GTK_WIDGET(label));

  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.5,
						 0.0, 0.0);
  gtk_table_attach(table,
		   GTK_WIDGET(alignment),
		   1, 2,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  channel_resize_editor->output_pads = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 1024.0, 1.0);
  gtk_container_add(GTK_CONTAINER(alignment),
		    GTK_WIDGET(channel_resize_editor->output_pads));

  /* input pads */
  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.5,
						 0.0, 0.0);
  gtk_table_attach(table,
		   GTK_WIDGET(alignment),
		   0, 1,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  label = (GtkLabel *) gtk_label_new("inputs\0");
  gtk_container_add(GTK_CONTAINER(alignment),
		    GTK_WIDGET(label));

  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.5,
						 0.0, 0.0);
  gtk_table_attach(table,
		   GTK_WIDGET(alignment),
		   1, 2,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  channel_resize_editor->input_pads = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 1024.0, 1.0);
  gtk_container_add(GTK_CONTAINER(alignment),
		    GTK_WIDGET(channel_resize_editor->input_pads));
}

void
ags_channel_resize_editor_connect(AgsConnectable *connectable)
{
  AgsChannelResizeEditor *channel_resize_editor;

  ags_channel_resize_editor_parent_connectable_interface->connect(connectable);

  /* AgsChannelResizeEditor */
  channel_resize_editor = AGS_CHANNEL_RESIZE_EDITOR(connectable);
}

void
ags_channel_resize_editor_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_channel_resize_editor_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsChannelResizeEditor *channel_resize_editor;

  channel_resize_editor = AGS_CHANNEL_RESIZE_EDITOR(applicable);
}

void
ags_channel_resize_editor_apply(AgsApplicable *applicable)
{
  AgsMachineEditor *machine_editor;
  AgsChannelResizeEditor *channel_resize_editor;
  AgsAudio *audio;

  channel_resize_editor = AGS_CHANNEL_RESIZE_EDITOR(applicable);

  if((AGS_PROPERTY_EDITOR_ENABLED & (AGS_PROPERTY_EDITOR(channel_resize_editor)->flags)) == 0)
    return;
  
  machine_editor = AGS_MACHINE_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(channel_resize_editor),
							      AGS_TYPE_MACHINE_EDITOR));

  audio = machine_editor->machine->audio;

  ags_audio_set_audio_channels(audio,
			       (guint) gtk_spin_button_get_value_as_int(channel_resize_editor->audio_channels));

  ags_audio_set_pads(audio, AGS_TYPE_INPUT,
		     (guint) gtk_spin_button_get_value_as_int(channel_resize_editor->input_pads));
  ags_audio_set_pads(audio, AGS_TYPE_OUTPUT,
		     (guint) gtk_spin_button_get_value_as_int(channel_resize_editor->output_pads));
}

void
ags_channel_resize_editor_reset(AgsApplicable *applicable)
{
  AgsMachineEditor *machine_editor;
  AgsChannelResizeEditor *channel_resize_editor;
  AgsAudio *audio;

  channel_resize_editor = AGS_CHANNEL_RESIZE_EDITOR(applicable);

  machine_editor = AGS_MACHINE_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(channel_resize_editor),
							      AGS_TYPE_MACHINE_EDITOR));

  audio = machine_editor->machine->audio;

  gtk_spin_button_set_value(channel_resize_editor->audio_channels,
			    audio->audio_channels);

  gtk_spin_button_set_value(channel_resize_editor->input_pads,
			    audio->input_pads);

  gtk_spin_button_set_value(channel_resize_editor->output_pads,
			    audio->output_pads);
}

void
ags_channel_resize_editor_destroy(GtkObject *object)
{
  AgsChannelResizeEditor *channel_resize_editor;

  channel_resize_editor = (AgsChannelResizeEditor *) object;
}

void
ags_channel_resize_editor_show(GtkWidget *widget)
{
  AgsChannelResizeEditor *channel_resize_editor = (AgsChannelResizeEditor *) widget;
}

AgsChannelResizeEditor*
ags_channel_resize_editor_new()
{
  AgsChannelResizeEditor *channel_resize_editor;

  channel_resize_editor = (AgsChannelResizeEditor *) g_object_new(AGS_TYPE_CHANNEL_RESIZE_EDITOR,
								  NULL);

  return(channel_resize_editor);
}

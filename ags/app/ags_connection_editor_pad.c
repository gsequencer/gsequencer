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

#include <ags/app/ags_connection_editor_pad.h>
#include <ags/app/ags_connection_editor_pad_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_machine.h>
#include <ags/app/ags_connection_editor.h>

#include <ags/i18n.h>

void ags_connection_editor_pad_class_init(AgsConnectionEditorPadClass *connection_editor_pad);
void ags_connection_editor_pad_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_connection_editor_pad_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_connection_editor_pad_init(AgsConnectionEditorPad *connection_editor_pad);
void ags_connection_editor_pad_set_property(GObject *gobject,
					    guint prop_id,
					    const GValue *value,
					    GParamSpec *param_spec);
void ags_connection_editor_pad_get_property(GObject *gobject,
					    guint prop_id,
					    GValue *value,
					    GParamSpec *param_spec);

void ags_connection_editor_pad_connect(AgsConnectable *connectable);
void ags_connection_editor_pad_disconnect(AgsConnectable *connectable);

void ags_connection_editor_pad_set_update(AgsApplicable *applicable, gboolean update);
void ags_connection_editor_pad_apply(AgsApplicable *applicable);
void ags_connection_editor_pad_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_connection_editor_pad
 * @short_description: Edit audio related aspects
 * @title: AgsConnectionEditorPad
 * @section_id:
 * @include: ags/app/ags_connection_editor_pad.h
 *
 * #AgsConnectionEditorPad is a composite widget to edit all aspects of #AgsAudio.
 * It consists of multiple child editor_pads.
 */

enum{
  PROP_0,
  PROP_CHANNEL,
};

static gpointer ags_connection_editor_pad_parent_class = NULL;

GType
ags_connection_editor_pad_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_connection_editor_pad = 0;

    static const GTypeInfo ags_connection_editor_pad_info = {
      sizeof (AgsConnectionEditorPadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_connection_editor_pad_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsConnectionEditorPad),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_connection_editor_pad_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_connection_editor_pad_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_connection_editor_pad_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_connection_editor_pad = g_type_register_static(GTK_TYPE_BOX,
							    "AgsConnectionEditorPad", &ags_connection_editor_pad_info,
							    0);

    g_type_add_interface_static(ags_type_connection_editor_pad,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_connection_editor_pad,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_connection_editor_pad);
  }

  return g_define_type_id__volatile;
}

void
ags_connection_editor_pad_class_init(AgsConnectionEditorPadClass *connection_editor_pad)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_connection_editor_pad_parent_class = g_type_class_peek_parent(connection_editor_pad);

  /* GObjectClass */
  gobject = (GObjectClass *) connection_editor_pad;

  gobject->set_property = ags_connection_editor_pad_set_property;
  gobject->get_property = ags_connection_editor_pad_get_property;

  /* properties */
  /**
   * AgsConnectionEditorPad:channel:
   *
   * The channel.
   * 
   * Since: 4.0.0
   */
  param_spec = g_param_spec_object("channel",
				   i18n_pspec("channel"),
				   i18n_pspec("The assigned channel"),
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL,
				  param_spec);
}

void
ags_connection_editor_pad_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_connection_editor_pad_connect;
  connectable->disconnect = ags_connection_editor_pad_disconnect;
}

void
ags_connection_editor_pad_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_connection_editor_pad_set_update;
  applicable->apply = ags_connection_editor_pad_apply;
  applicable->reset = ags_connection_editor_pad_reset;
}

void
ags_connection_editor_pad_init(AgsConnectionEditorPad *connection_editor_pad)
{
  gtk_orientable_set_orientation(GTK_ORIENTABLE(connection_editor_pad),
				 GTK_ORIENTATION_VERTICAL);

  gtk_box_set_spacing(GTK_BOX(connection_editor_pad),
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  connection_editor_pad->connectable_flags = 0;

  connection_editor_pad->channel = NULL;

  connection_editor_pad->expander = (GtkExpander *) gtk_expander_new(i18n("pad"));
  gtk_box_append((GtkBox *) connection_editor_pad,
		 (GtkWidget *) connection_editor_pad->expander);
  
  connection_editor_pad->line = NULL;

  connection_editor_pad->line_box = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
							   AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_expander_set_child(connection_editor_pad->expander,
			 (GtkWidget *) connection_editor_pad->line_box);
}

void
ags_connection_editor_pad_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec)
{
  AgsConnectionEditorPad *connection_editor_pad;

  connection_editor_pad = AGS_CONNECTION_EDITOR_PAD(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      AgsChannel *channel;

      channel = g_value_get_object(value);

      if(connection_editor_pad->channel != NULL){
	g_object_unref(connection_editor_pad->channel);
      }

      if(channel != NULL){
	g_object_ref(channel);
      }
    
      connection_editor_pad->channel = channel;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_connection_editor_pad_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec)
{
  AgsConnectionEditorPad *connection_editor_pad;

  connection_editor_pad = AGS_CONNECTION_EDITOR_PAD(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      g_value_set_object(value, connection_editor_pad->channel);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_connection_editor_pad_connect(AgsConnectable *connectable)
{
  AgsConnectionEditorPad *connection_editor_pad;

  GList *start_line, *line;

  connection_editor_pad = AGS_CONNECTION_EDITOR_PAD(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (connection_editor_pad->connectable_flags)) != 0){
    return;
  }

  connection_editor_pad->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  line =
    start_line = ags_connection_editor_pad_get_line(connection_editor_pad);

  while(line != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(line->data));

    /* iterate */
    line = line->next;
  }

  g_list_free(start_line);
}

void
ags_connection_editor_pad_disconnect(AgsConnectable *connectable)
{
  AgsConnectionEditorPad *connection_editor_pad;

  GList *start_line, *line;

  connection_editor_pad = AGS_CONNECTION_EDITOR_PAD(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (connection_editor_pad->connectable_flags)) == 0){
    return;
  }
  
  connection_editor_pad->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  line =
    start_line = ags_connection_editor_pad_get_line(connection_editor_pad);

  while(line != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(line->data));

    /* iterate */
    line = line->next;
  }

  g_list_free(start_line);
}

void
ags_connection_editor_pad_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsConnectionEditorPad *connection_editor_pad;

  GList *start_line, *line;

  connection_editor_pad = AGS_CONNECTION_EDITOR_PAD(applicable);

  line =
    start_line = ags_connection_editor_pad_get_line(connection_editor_pad);

  while(line != NULL){
    ags_applicable_set_update(AGS_APPLICABLE(line->data),
			      update);

    /* iterate */
    line = line->next;
  }

  g_list_free(start_line);
}

void
ags_connection_editor_pad_apply(AgsApplicable *applicable)
{
  AgsConnectionEditorPad *connection_editor_pad;

  GList *start_line, *line;

  connection_editor_pad = AGS_CONNECTION_EDITOR_PAD(applicable);

  line =
    start_line = ags_connection_editor_pad_get_line(connection_editor_pad);

  while(line != NULL){
    ags_applicable_apply(AGS_APPLICABLE(line->data));

    /* iterate */
    line = line->next;
  }

  g_list_free(start_line);
}

void
ags_connection_editor_pad_reset(AgsApplicable *applicable)
{
  AgsMachine *machine;
  AgsConnectionEditor *connection_editor;
  AgsConnectionEditorPad *connection_editor_pad;

  AgsChannel *start_output, *start_input;

  GList *start_line, *line;

  guint audio_channels;
  guint pad;
  guint i;
  
  connection_editor_pad = AGS_CONNECTION_EDITOR_PAD(applicable);

  connection_editor = (AgsConnectionEditor *) gtk_widget_get_ancestor(connection_editor_pad,
								      AGS_TYPE_CONNECTION_EDITOR);
  
  line =
    start_line = ags_connection_editor_pad_get_line(connection_editor_pad);

  while(line != NULL){
    ags_connection_editor_pad_remove_line(connection_editor_pad,
					  line->data);

    /* iterate */
    line = line->next;
  }

  g_list_free(start_line);

  if(!AGS_IS_CONNECTION_EDITOR(connection_editor) ||
     connection_editor->machine == NULL){
    return;
  }
  
  machine = connection_editor->machine;

  if(connection_editor_pad->channel == NULL){
    return;
  }

  audio_channels = ags_audio_get_audio_channels(machine->audio);

  start_output = ags_audio_get_output(machine->audio);
  
  start_input = ags_audio_get_input(machine->audio);

  pad = ags_channel_get_pad(connection_editor_pad->channel);
  
  if(g_type_is_a(G_OBJECT_TYPE(connection_editor_pad->channel), AGS_TYPE_OUTPUT)){
    for(i = 0; i < audio_channels; i++){
      AgsConnectionEditorLine *line;

      AgsChannel *current_channel;

      current_channel = ags_channel_nth(start_output,
					pad * audio_channels + i);

      line = ags_connection_editor_line_new(current_channel);

      if((AGS_CONNECTION_EDITOR_SHOW_SOUNDCARD_OUTPUT & (connection_editor->flags)) != 0){
	gtk_widget_set_visible(line->output_box,
			       TRUE);
      }

      if((AGS_CONNECTION_EDITOR_SHOW_SOUNDCARD_INPUT & (connection_editor->flags)) != 0){
	gtk_widget_set_visible(line->input_box,
			       TRUE);
      }
      
      ags_connection_editor_pad_add_line(connection_editor_pad,
					 line);

      if(current_channel != NULL){
	g_object_unref(current_channel);
      }      
    }
  }else{
    for(i = 0; i < audio_channels; i++){
      AgsConnectionEditorLine *line;

      AgsChannel *current_channel;

      current_channel = ags_channel_nth(start_input,
					pad * audio_channels + i);

      line = ags_connection_editor_line_new(current_channel);

      if((AGS_CONNECTION_EDITOR_SHOW_SOUNDCARD_OUTPUT & (connection_editor->flags)) != 0){
	gtk_widget_set_visible(line->output_box,
			       TRUE);
      }

      if((AGS_CONNECTION_EDITOR_SHOW_SOUNDCARD_INPUT & (connection_editor->flags)) != 0){
	gtk_widget_set_visible(line->input_box,
			       TRUE);
      }

      ags_connection_editor_pad_add_line(connection_editor_pad,
					 line);

      if(current_channel != NULL){
	g_object_unref(current_channel);
      }      
    }
  }
  
  /* reset */
  line =
    start_line = ags_connection_editor_pad_get_line(connection_editor_pad);

  while(line != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(line->data));
    
    ags_applicable_reset(AGS_APPLICABLE(line->data));

    /* iterate */
    line = line->next;
  }

  g_list_free(start_line);

  if(start_output != NULL){
    g_object_unref(start_output);
  }
  
  if(start_input != NULL){
    g_object_unref(start_input);
  }
}

/**
 * ags_connection_editor_pad_get_line:
 * @connection_editor_pad: the #AgsConnectionEditorPad
 * 
 * Get line.
 * 
 * Returns: the #GList-struct containig #AgsConnectionEditorLine
 * 
 * Since: 4.0.0
 */
GList*
ags_connection_editor_pad_get_line(AgsConnectionEditorPad *connection_editor_pad)
{
  g_return_val_if_fail(AGS_IS_CONNECTION_EDITOR_PAD(connection_editor_pad), NULL);

  return(g_list_reverse(g_list_copy(connection_editor_pad->line)));
}

/**
 * ags_connection_editor_pad_add_line:
 * @connection_editor_pad: the #AgsConnectionEditorPad
 * @line: the #AgsConnectionEditorLine
 * 
 * Add @line to @connection_editor_pad.
 * 
 * Since: 4.0.0
 */
void
ags_connection_editor_pad_add_line(AgsConnectionEditorPad *connection_editor_pad,
				   AgsConnectionEditorLine *line)
{
  g_return_if_fail(AGS_IS_CONNECTION_EDITOR_PAD(connection_editor_pad));
  g_return_if_fail(AGS_IS_CONNECTION_EDITOR_LINE(line));

  if(g_list_find(connection_editor_pad->line, line) == NULL){
    connection_editor_pad->line = g_list_prepend(connection_editor_pad->line,
						 line);

    line->parent_pad = connection_editor_pad;
    
    gtk_box_append(connection_editor_pad->line_box,
		   line);
  }
}

/**
 * ags_connection_editor_pad_remove_line:
 * @connection_editor_pad: the #AgsConnectionEditorPad
 * @line: the #AgsConnectionEditorLine
 * 
 * Remove @line from @connection_editor_pad.
 * 
 * Since: 4.0.0
 */
void
ags_connection_editor_pad_remove_line(AgsConnectionEditorPad *connection_editor_pad,
				      AgsConnectionEditorLine *line)
{
  g_return_if_fail(AGS_IS_CONNECTION_EDITOR_PAD(connection_editor_pad));
  g_return_if_fail(AGS_IS_CONNECTION_EDITOR_LINE(line));

  if(g_list_find(connection_editor_pad->line, line) != NULL){
    connection_editor_pad->line = g_list_remove(connection_editor_pad->line,
						line);

    line->parent_pad = NULL;
    
    gtk_box_remove(connection_editor_pad->line_box,
		   line);
  }
}

/**
 * ags_connection_editor_pad_new:
 * @channel: the #AgsChannel
 *
 * Creates an #AgsConnectionEditorPad
 *
 * Returns: a new #AgsConnectionEditorPad
 *
 * Since: 4.0.0
 */
AgsConnectionEditorPad*
ags_connection_editor_pad_new(AgsChannel *channel)
{
  AgsConnectionEditorPad *connection_editor_pad;

  connection_editor_pad = (AgsConnectionEditorPad *) g_object_new(AGS_TYPE_CONNECTION_EDITOR_PAD,
								  "channel", channel,
								  NULL);

  return(connection_editor_pad);
}

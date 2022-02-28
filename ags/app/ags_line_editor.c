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

#include <ags/app/ags_line_editor.h>
#include <ags/app/ags_line_editor_callbacks.h>

#include <ags/app/ags_ui_provider.h>

#include <ags/i18n.h>

void ags_line_editor_class_init(AgsLineEditorClass *line_editor);
void ags_line_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_line_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_line_editor_init(AgsLineEditor *line_editor);
void ags_line_editor_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_line_editor_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_line_editor_finalize(GObject *gobject);

void ags_line_editor_connect(AgsConnectable *connectable);
void ags_line_editor_disconnect(AgsConnectable *connectable);

void ags_line_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_line_editor_apply(AgsApplicable *applicable);
void ags_line_editor_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_line_editor
 * @short_description: A composite widget to edit #AgsChannel
 * @title: AgsLineEditor
 * @section_id:
 * @include: ags/app/ags_line_editor.h
 *
 * #AgsLineEditor is a composite widget to edit #AgsChannel. It should be
 * packed by an #AgsPadEditor.
 */

enum{
  PROP_0,
  PROP_CHANNEL,
};

static gpointer ags_line_editor_parent_class = NULL;

GType
ags_line_editor_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_line_editor = 0;

    static const GTypeInfo ags_line_editor_info = {
      sizeof (AgsLineEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_line_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLineEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_line_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_line_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_line_editor_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_line_editor = g_type_register_static(GTK_TYPE_BOX,
						  "AgsLineEditor", &ags_line_editor_info,
						  0);

    g_type_add_interface_static(ags_type_line_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_line_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_line_editor);
  }

  return g_define_type_id__volatile;
}

void
ags_line_editor_class_init(AgsLineEditorClass *line_editor)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_line_editor_parent_class = g_type_class_peek_parent(line_editor);

  /* GObjectClass */
  gobject = (GObjectClass *) line_editor;

  gobject->set_property = ags_line_editor_set_property;
  gobject->get_property = ags_line_editor_get_property;

  gobject->finalize = ags_line_editor_finalize;

  /* properties */
  /**
   * AgsLineEditor:channel:
   *
   * The assigned #AgsChannel to edit.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("channel",
				   i18n_pspec("assigned channel"),
				   i18n_pspec("The channel which this line editor is assigned with"),
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL,
				  param_spec);
}

void
ags_line_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_line_editor_connect;
  connectable->disconnect = ags_line_editor_disconnect;
}

void
ags_line_editor_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_line_editor_set_update;
  applicable->apply = ags_line_editor_apply;
  applicable->reset = ags_line_editor_reset;
}

void
ags_line_editor_init(AgsLineEditor *line_editor)
{
  gtk_orientable_set_orientation(GTK_ORIENTABLE(line_editor),
				 GTK_ORIENTATION_VERTICAL);

  line_editor->flags = 0;
  
  line_editor->version = AGS_LINE_EDITOR_DEFAULT_VERSION;
  line_editor->build_id = AGS_LINE_EDITOR_DEFAULT_BUILD_ID;

  line_editor->channel = NULL;
  
  line_editor->link_editor = NULL;
  line_editor->output_editor = NULL;
  line_editor->input_editor = NULL;
  line_editor->member_editor = NULL;
}

void
ags_line_editor_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsLineEditor *line_editor;

  line_editor = AGS_LINE_EDITOR(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      AgsChannel *channel;

      channel = (AgsChannel *) g_value_get_object(value);

      ags_line_editor_set_channel(line_editor, channel);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_line_editor_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsLineEditor *line_editor;

  line_editor = AGS_LINE_EDITOR(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      g_value_set_object(value, line_editor->channel);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_line_editor_finalize(GObject *gobject)
{
  AgsLineEditor *line_editor;

  line_editor = (AgsLineEditor *) gobject;

  if(line_editor->channel != NULL){
    g_object_unref(line_editor->channel);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_line_editor_parent_class)->finalize(gobject);
}

void
ags_line_editor_connect(AgsConnectable *connectable)
{
  AgsLineEditor *line_editor;

  line_editor = AGS_LINE_EDITOR(connectable);

  if((AGS_LINE_EDITOR_CONNECTED & (line_editor->flags)) != 0){
    return;
  }

  line_editor->flags |= AGS_LINE_EDITOR_CONNECTED;
  
  g_signal_connect(G_OBJECT(line_editor), "show",
  		   G_CALLBACK(ags_line_editor_show_callback), (gpointer) line_editor);
  
  if(line_editor->link_editor != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(line_editor->link_editor));
  }
  
  if(line_editor->output_editor != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(line_editor->output_editor));
  }
  
  if(line_editor->input_editor != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(line_editor->input_editor));
  }

  if(line_editor->member_editor != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(line_editor->member_editor));
  }
}

void
ags_line_editor_disconnect(AgsConnectable *connectable)
{
  AgsLineEditor *line_editor;

  line_editor = AGS_LINE_EDITOR(connectable);

  if((AGS_LINE_EDITOR_CONNECTED & (line_editor->flags)) == 0){
    return;
  }

  line_editor->flags &= (~AGS_LINE_EDITOR_CONNECTED);

  g_object_disconnect(G_OBJECT(line_editor),
		      "any_signal::show",
		      G_CALLBACK(ags_line_editor_show_callback),
		      (gpointer) line_editor,
		      NULL);

  if(line_editor->link_editor != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(line_editor->link_editor));
  }
  
  if(line_editor->output_editor != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(line_editor->output_editor));
  }
  
  if(line_editor->input_editor != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(line_editor->input_editor));
  }

  if(line_editor->member_editor != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(line_editor->member_editor));
  }
}

void
ags_line_editor_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsLineEditor *line_editor;

  line_editor = AGS_LINE_EDITOR(applicable);

  if(line_editor->link_editor != NULL){
    ags_applicable_set_update(AGS_APPLICABLE(line_editor->link_editor), update);
  }
}

void
ags_line_editor_apply(AgsApplicable *applicable)
{
  AgsLineEditor *line_editor;

  line_editor = AGS_LINE_EDITOR(applicable);

  if(line_editor->link_editor != NULL){
    ags_applicable_apply(AGS_APPLICABLE(line_editor->link_editor));
  }

  if(line_editor->output_editor != NULL){
    ags_applicable_apply(AGS_APPLICABLE(line_editor->output_editor));
  }

  if(line_editor->input_editor != NULL){
    ags_applicable_apply(AGS_APPLICABLE(line_editor->input_editor));
  }
}

void
ags_line_editor_reset(AgsApplicable *applicable)
{
  AgsLineEditor *line_editor;

  line_editor = AGS_LINE_EDITOR(applicable);

  if(line_editor->link_editor != NULL){
    ags_applicable_reset(AGS_APPLICABLE(line_editor->link_editor));
  }

  if(line_editor->output_editor != NULL){
    ags_applicable_reset(AGS_APPLICABLE(line_editor->output_editor));
  }

  if(line_editor->input_editor != NULL){
    ags_applicable_reset(AGS_APPLICABLE(line_editor->input_editor));
  }

  if(line_editor->member_editor != NULL){
    ags_applicable_reset(AGS_APPLICABLE(line_editor->member_editor));
  }
}

/**
 * ags_line_editor_set_channel:
 * @line_editor: an #AgsLineEditor
 * @channel: the new #AgsChannel
 *
 * Is called as channel gets modified.
 *
 * Since: 3.0.0
 */
void
ags_line_editor_set_channel(AgsLineEditor *line_editor,
			    AgsChannel *channel)
{
  if(line_editor->link_editor != NULL){
    /* link editor */
    gtk_box_remove((GtkBox *) line_editor,
		   (GtkWidget *) line_editor->link_editor);
    
    g_object_run_dispose(line_editor->link_editor);
    g_object_unref(line_editor->link_editor);
    
    /* output editor */
    gtk_box_remove((GtkBox *) line_editor,
		   (GtkWidget *) line_editor->output_editor);
    
    g_object_run_dispose(line_editor->output_editor);
    g_object_unref(line_editor->output_editor);

    /* input editor */
    gtk_box_remove((GtkBox *) line_editor,
		   (GtkWidget *) line_editor->input_editor);
    
    g_object_run_dispose(line_editor->input_editor);
    g_object_unref(line_editor->input_editor);

    /* member editor */
    gtk_box_remove((GtkBox *) line_editor,
		   (GtkWidget *) line_editor->member_editor);
    
    g_object_run_dispose(line_editor->member_editor);
    g_object_unref(line_editor->member_editor);
  }

  if(line_editor->channel != channel){
    if(line_editor->channel != NULL){
      g_object_unref(line_editor->channel);
    }  

    if(channel != NULL){
      g_object_ref(channel);
    }

    line_editor->channel = channel;
  }
  
  if(channel != NULL){
    guint i;

    for(i = 0; i < line_editor->editor_type_count; i++){
      /* link */
      if(line_editor->editor_type[i] == AGS_TYPE_LINK_EDITOR){
	line_editor->link_editor = ags_link_editor_new();
	gtk_box_append((GtkBox *) line_editor,
		       (GtkWidget *) line_editor->link_editor);
      }

      /* output */
      if(line_editor->editor_type[i] == AGS_TYPE_OUTPUT_EDITOR){
	line_editor->output_editor = ags_output_editor_new();
	gtk_box_append((GtkBox *) line_editor,
		       (GtkWidget *) line_editor->output_editor);
      }
      
      /* input */
      if(line_editor->editor_type[i] == AGS_TYPE_INPUT_EDITOR){
	line_editor->input_editor = ags_input_editor_new();
	gtk_box_append((GtkBox *) line_editor,
		       (GtkWidget *) line_editor->input_editor);
      }

      /* recall */
      if(line_editor->editor_type[i] == AGS_TYPE_LINE_MEMBER_EDITOR){
	line_editor->member_editor = ags_line_member_editor_new();
	gtk_box_append((GtkBox *) line_editor,
		       (GtkWidget *) line_editor->member_editor);
      }
    }
  }
}

/**
 * ags_line_editor_new:
 * @channel: the channel to edit
 *
 * Create a new instance of #AgsLineEditor
 *
 * Returns: the new #AgsLineEditor
 *
 * Since: 3.0.0
 */
AgsLineEditor*
ags_line_editor_new(AgsChannel *channel)
{
  AgsLineEditor *line_editor;

  line_editor = (AgsLineEditor *) g_object_new(AGS_TYPE_LINE_EDITOR,
					       "channel", channel,
					       NULL);

  return(line_editor);
}

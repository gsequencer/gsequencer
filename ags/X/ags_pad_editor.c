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

#include <ags/X/ags_pad_editor.h>
#include <ags/X/ags_pad_editor_callbacks.h>

#include <ags/X/ags_machine_editor.h>
#include <ags/X/ags_connection_editor.h>
#include <ags/X/ags_line_editor.h>

#include <ags/i18n.h>

void ags_pad_editor_class_init(AgsPadEditorClass *pad_editor);
void ags_pad_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_pad_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_pad_editor_init(AgsPadEditor *pad_editor);
void ags_pad_editor_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_pad_editor_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_pad_editor_finalize(GObject *gobject);

void ags_pad_editor_connect(AgsConnectable *connectable);
void ags_pad_editor_disconnect(AgsConnectable *connectable);

void ags_pad_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_pad_editor_apply(AgsApplicable *applicable);
void ags_pad_editor_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_pad_editor
 * @short_description: A composite widget to edit #AgsChannel
 * @title: AgsPadEditor
 * @section_id:
 * @include: ags/X/ags_pad_editor.h
 *
 * #AgsPadEditor is a composite widget to edit #AgsChannel. It should be
 * packed by an #AgsPadEditor.
 */

enum{
  PROP_0,
  PROP_CHANNEL,
};

static gpointer ags_pad_editor_parent_class = NULL;

GType
ags_pad_editor_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_pad_editor = 0;

    static const GTypeInfo ags_pad_editor_info = {
      sizeof (AgsPadEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_pad_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPadEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_pad_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_pad_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_pad_editor_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_pad_editor = g_type_register_static(GTK_TYPE_VBOX,
						 "AgsPadEditor", &ags_pad_editor_info,
						 0);

    g_type_add_interface_static(ags_type_pad_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_pad_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_pad_editor);
  }

  return g_define_type_id__volatile;
}

void
ags_pad_editor_class_init(AgsPadEditorClass *pad_editor)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_pad_editor_parent_class = g_type_class_peek_parent(pad_editor);

  /* GObjectClass */
  gobject = (GObjectClass *) pad_editor;

  gobject->set_property = ags_pad_editor_set_property;
  gobject->get_property = ags_pad_editor_get_property;

  gobject->finalize = ags_pad_editor_finalize;

  /* properties */
  param_spec = g_param_spec_object("channel",
				   i18n_pspec("assigned channel"),
				   i18n_pspec("The channel which this pad editor is assigned with"),
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL,
				  param_spec);
}

void
ags_pad_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_pad_editor_connect;
  connectable->disconnect = ags_pad_editor_disconnect;
}

void
ags_pad_editor_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_pad_editor_set_update;
  applicable->apply = ags_pad_editor_apply;
  applicable->reset = ags_pad_editor_reset;
}

void
ags_pad_editor_init(AgsPadEditor *pad_editor)
{
  pad_editor->flags = 0;
  
  pad_editor->version = AGS_PAD_EDITOR_DEFAULT_VERSION;
  pad_editor->build_id = AGS_PAD_EDITOR_DEFAULT_BUILD_ID;

  pad_editor->pad = NULL;
  
  pad_editor->line_editor_expander = (GtkExpander *) gtk_expander_new(NULL);
  gtk_box_pack_start(GTK_BOX(pad_editor),
		     GTK_WIDGET(pad_editor->line_editor_expander),
		     FALSE, FALSE,
		     0);

  pad_editor->line_editor = NULL;
}


void
ags_pad_editor_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsPadEditor *pad_editor;

  pad_editor = AGS_PAD_EDITOR(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      AgsChannel *channel;

      channel = (AgsChannel *) g_value_get_object(value);

      ags_pad_editor_set_channel(pad_editor, channel);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_pad_editor_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsPadEditor *pad_editor;

  pad_editor = AGS_PAD_EDITOR(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    g_value_set_object(value, pad_editor->pad);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_pad_editor_finalize(GObject *gobject)
{
  AgsPadEditor *pad_editor;

  pad_editor = (AgsPadEditor *) gobject;

  if(pad_editor->pad != NULL){
    g_object_unref(pad_editor->pad);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_pad_editor_parent_class)->finalize(gobject);
}

void
ags_pad_editor_connect(AgsConnectable *connectable)
{
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsConnectionEditor *connection_editor;
  AgsPadEditor *pad_editor;
  
  GList *line_editor, *line_editor_start;

  pad_editor = AGS_PAD_EDITOR(connectable);

  if((AGS_PAD_EDITOR_CONNECTED & (pad_editor->flags)) != 0){
    return;
  }

  pad_editor->flags |= AGS_PAD_EDITOR_CONNECTED;

  machine = NULL;
  
  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor(GTK_WIDGET(pad_editor),
								AGS_TYPE_MACHINE_EDITOR);

  connection_editor = (AgsConnectionEditor *) gtk_widget_get_ancestor(GTK_WIDGET(pad_editor),
								      AGS_TYPE_CONNECTION_EDITOR);
  
  if(machine_editor != NULL){
    machine = machine_editor->machine;
  }else if(connection_editor != NULL){
    machine = connection_editor->machine;
  }
  
  /*  */
  if(machine != NULL){
    g_signal_connect_after(G_OBJECT(machine), "resize-audio-channels",
			   G_CALLBACK(ags_pad_editor_resize_audio_channels_callback), pad_editor);
  }
  
  /* AgsLineEditor */
  line_editor_start = 
    line_editor = gtk_container_get_children(GTK_CONTAINER(pad_editor->line_editor));

  while(line_editor != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(line_editor->data));

    line_editor = line_editor->next;
  }

  g_list_free(line_editor_start);
}

void
ags_pad_editor_disconnect(AgsConnectable *connectable)
{
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsConnectionEditor *connection_editor;
  AgsPadEditor *pad_editor;

  GList *line_editor, *line_editor_start;

  pad_editor = AGS_PAD_EDITOR(connectable);

  if((AGS_PAD_EDITOR_CONNECTED & (pad_editor->flags)) == 0){
    return;
  }

  pad_editor->flags &= (~AGS_PAD_EDITOR_CONNECTED);
  
  machine = NULL;

  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor(GTK_WIDGET(pad_editor),
								AGS_TYPE_MACHINE_EDITOR);

  connection_editor = (AgsConnectionEditor *) gtk_widget_get_ancestor(GTK_WIDGET(pad_editor),
								      AGS_TYPE_CONNECTION_EDITOR);
  
  if(machine_editor != NULL){
    machine = machine_editor->machine;
  }else if(connection_editor != NULL){
    machine = connection_editor->machine;
  }
  
  /*  */
  if(machine != NULL){
    g_object_disconnect(G_OBJECT(machine),
			"any_signal::resize-audio-channels",
			G_CALLBACK(ags_pad_editor_resize_audio_channels_callback),
			pad_editor,
			NULL);
  }

  /* AgsLineEditor */
  line_editor_start = 
    line_editor = gtk_container_get_children(GTK_CONTAINER(pad_editor->line_editor));

  while(line_editor != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(line_editor->data));

    line_editor = line_editor->next;
  }

  g_list_free(line_editor_start);
}

void
ags_pad_editor_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsPadEditor *pad_editor;
  GList *line_editor, *line_editor_start;

  pad_editor = AGS_PAD_EDITOR(applicable);

  line_editor_start = 
    line_editor = gtk_container_get_children(GTK_CONTAINER(pad_editor->line_editor));

  while(line_editor != NULL){
    ags_applicable_set_update(AGS_APPLICABLE(line_editor->data), update);

    line_editor = line_editor->next;
  }

  g_list_free(line_editor_start);
}

void
ags_pad_editor_apply(AgsApplicable *applicable)
{
  AgsPadEditor *pad_editor;
  GList *line_editor, *line_editor_start;

  pad_editor = AGS_PAD_EDITOR(applicable);

  line_editor_start = 
    line_editor = gtk_container_get_children(GTK_CONTAINER(pad_editor->line_editor));

  while(line_editor != NULL){
    ags_applicable_apply(AGS_APPLICABLE(line_editor->data));

    line_editor = line_editor->next;
  }

  g_list_free(line_editor_start);
}

void
ags_pad_editor_reset(AgsApplicable *applicable)
{
  AgsPadEditor *pad_editor;
  GList *line_editor, *line_editor_start;

  pad_editor = AGS_PAD_EDITOR(applicable);

  line_editor_start = 
    line_editor = gtk_container_get_children(GTK_CONTAINER(pad_editor->line_editor));

  while(line_editor != NULL){
    ags_applicable_reset(AGS_APPLICABLE(line_editor->data));

    line_editor = line_editor->next;
  }

  g_list_free(line_editor_start);
}

/**
 * ags_pad_editor_set_channel:
 * @pad_editor: an #AgsPadEditor
 * @channel: the new #AgsChannel
 *
 * Is called as channel gets modified.
 *
 * Since: 3.0.0
 */
void
ags_pad_editor_set_channel(AgsPadEditor *pad_editor, AgsChannel *start_channel)
{
  GtkVBox *vbox;
  
  if(pad_editor->line_editor != NULL){
    vbox = pad_editor->line_editor;
    pad_editor->line_editor = NULL;
    gtk_widget_destroy(GTK_WIDGET(vbox));
  }

  if(pad_editor->pad != start_channel){
    if(pad_editor->pad != NULL){
      g_object_unref(pad_editor->pad);
    }
    
    if(start_channel != NULL){
      g_object_ref(start_channel);
    }
    
    pad_editor->pad = start_channel;
  }
  
  if(start_channel != NULL){
    AgsLineEditor *line_editor;
    
    AgsChannel *channel;
    AgsChannel *next_pad, *next_channel;

    gchar *str;
    
    guint pad;
    guint i;

    /* get some channel fields */
    pad = 0;
    
    g_object_get(start_channel,
		 "pad", &pad,
		 NULL);
    
    /* set label */
    str = g_strdup_printf("%s: %u",
			  i18n("pad"),
			  pad + 1);
    gtk_expander_set_label(pad_editor->line_editor_expander,
			   str);

    g_free(str);
    
    pad_editor->line_editor = (GtkVBox *) gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(pad_editor->line_editor_expander),
		      GTK_WIDGET(pad_editor->line_editor));

    channel = start_channel;
    next_pad = NULL;
    
    if(channel != NULL){
      g_object_ref(channel);
    
      next_pad = ags_channel_next_pad(start_channel);
    }

    next_channel = NULL;
    
    while(channel != next_pad){
      /* instantiate line editor */
      line_editor = ags_line_editor_new(NULL);
      line_editor->editor_type_count = pad_editor->editor_type_count;
      line_editor->editor_type = (GType *) malloc(line_editor->editor_type_count * sizeof(GType));
      
      for(i = 0; i < line_editor->editor_type_count; i++){
	line_editor->editor_type[i] = pad_editor->editor_type[i];
      }

      g_object_set(line_editor,
		   "channel", channel,
		   NULL);
      
      gtk_box_pack_start(GTK_BOX(pad_editor->line_editor),
			 GTK_WIDGET(line_editor),
			 FALSE, FALSE,
			 0);

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }

    if(next_pad != NULL){
      g_object_unref(next_pad);
    }
  }else{
    gtk_expander_set_label(pad_editor->line_editor_expander,
			   NULL);
  }
}

/**
 * ags_pad_editor_new:
 * @channel: the #AgsChannel to edit
 *
 * Create a new instance of #AgsPadEditor
 *
 * Returns: the new #AgsPadEditor
 *
 * Since: 3.0.0
 */
AgsPadEditor*
ags_pad_editor_new(AgsChannel *channel)
{
  AgsPadEditor *pad_editor;

  pad_editor = (AgsPadEditor *) g_object_new(AGS_TYPE_PAD_EDITOR,
					     "channel", channel,
					     NULL);

  return(pad_editor);
}

/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/X/ags_output_listing_editor.h>
#include <ags/X/ags_output_listing_editor_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/X/ags_connection_editor.h>
#include <ags/X/ags_pad_editor.h>
#include <ags/X/ags_line_editor.h>
#include <ags/X/ags_output_editor.h>

void ags_output_listing_editor_class_init(AgsOutputListingEditorClass *output_listing_editor);
void ags_output_listing_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_output_listing_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_output_listing_editor_init(AgsOutputListingEditor *output_listing_editor);

void ags_output_listing_editor_connect(AgsConnectable *connectable);
void ags_output_listing_editor_disconnect(AgsConnectable *connectable);

void ags_output_listing_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_output_listing_editor_apply(AgsApplicable *applicable);
void ags_output_listing_editor_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_output_listing_editor
 * @short_description: pack output editors
 * @title: AgsOutputListingEditor
 * @section_id:
 * @include: ags/X/ags_output_listing_editor.h
 *
 * #AgsOutputListingEditor is a composite widget to pack #AgsOutputEditor.
 */

static gpointer ags_output_listing_editor_parent_class = NULL;
static AgsConnectableInterface* ags_output_listing_editor_parent_connectable_interface;

GType
ags_output_listing_editor_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_output_listing_editor = 0;

    static const GTypeInfo ags_output_listing_editor_info = {
      sizeof (AgsOutputListingEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_output_listing_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOutputListingEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_output_listing_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_output_listing_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_output_listing_editor_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_output_listing_editor = g_type_register_static(AGS_TYPE_PROPERTY_LISTING_EDITOR,
							    "AgsOutputListingEditor",
							    &ags_output_listing_editor_info,
							    0);
    
    g_type_add_interface_static(ags_type_output_listing_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_output_listing_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_output_listing_editor);
  }

  return g_define_type_id__volatile;
}

void
ags_output_listing_editor_class_init(AgsOutputListingEditorClass *output_listing_editor)
{
}

void
ags_output_listing_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_output_listing_editor_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_output_listing_editor_connect;
  connectable->disconnect = ags_output_listing_editor_disconnect;
}

void
ags_output_listing_editor_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_output_listing_editor_set_update;
  applicable->apply = ags_output_listing_editor_apply;
  applicable->reset = ags_output_listing_editor_reset;
}

void
ags_output_listing_editor_init(AgsOutputListingEditor *output_listing_editor)
{
  g_signal_connect_after(G_OBJECT(output_listing_editor), "parent_set",
			 G_CALLBACK(ags_output_listing_editor_parent_set_callback), output_listing_editor);

  output_listing_editor->channel_type = G_TYPE_NONE;

  output_listing_editor->child = NULL;
}

void
ags_output_listing_editor_connect(AgsConnectable *connectable)
{
  AgsConnectionEditor *connection_editor;
  AgsOutputListingEditor *output_listing_editor;

  GList *pad_editor, *pad_editor_start;

  output_listing_editor = AGS_OUTPUT_LISTING_EDITOR(connectable);

  if((AGS_PROPERTY_EDITOR_CONNECTED & (AGS_PROPERTY_EDITOR(output_listing_editor)->flags)) != 0){
    return;
  }

  ags_output_listing_editor_parent_connectable_interface->connect(connectable);

  connection_editor = (AgsConnectionEditor *) gtk_widget_get_ancestor(GTK_WIDGET(output_listing_editor),
								      AGS_TYPE_CONNECTION_EDITOR);

  if(connection_editor != NULL &&
     connection_editor->machine != NULL){
    g_signal_connect_after(G_OBJECT(connection_editor->machine), "resize-pads",
			   G_CALLBACK(ags_output_listing_editor_resize_pads_callback), output_listing_editor);
  }

  /* AgsPadEditor */
  if(output_listing_editor->child != NULL){
    pad_editor_start = 
      pad_editor = gtk_container_get_children(output_listing_editor->child);

    while(pad_editor != NULL){
      ags_connectable_connect(AGS_CONNECTABLE(pad_editor->data));

      pad_editor = pad_editor->next;
    }
  
    g_list_free(pad_editor_start);
  }
}

void
ags_output_listing_editor_disconnect(AgsConnectable *connectable)
{
  AgsConnectionEditor *connection_editor;
  AgsOutputListingEditor *output_listing_editor;

  GList *pad_editor, *pad_editor_start;

  output_listing_editor = AGS_OUTPUT_LISTING_EDITOR(connectable);

  if((AGS_PROPERTY_EDITOR_CONNECTED & (AGS_PROPERTY_EDITOR(output_listing_editor)->flags)) == 0){
    return;
  }

  ags_output_listing_editor_parent_connectable_interface->connect(connectable);

  connection_editor = (AgsConnectionEditor *) gtk_widget_get_ancestor(GTK_WIDGET(output_listing_editor),
								      AGS_TYPE_CONNECTION_EDITOR);

  if(connection_editor != NULL &&
     connection_editor->machine != NULL){
    g_object_disconnect(G_OBJECT(connection_editor->machine),
			"any_signal::resize-pads",
			G_CALLBACK(ags_output_listing_editor_resize_pads_callback),
			output_listing_editor,
			NULL);
  }

  /* AgsPadEditor */
  if(output_listing_editor->child != NULL){
    pad_editor_start = 
      pad_editor = gtk_container_get_children(output_listing_editor->child);

    while(pad_editor != NULL){
      ags_connectable_disconnect(AGS_CONNECTABLE(pad_editor->data));

      pad_editor = pad_editor->next;
    }
  
    g_list_free(pad_editor_start);
  }
}

void
ags_output_listing_editor_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsOutputListingEditor *output_listing_editor;

  GList *pad_editor, *pad_editor_start;

  output_listing_editor = AGS_OUTPUT_LISTING_EDITOR(applicable);

  if(output_listing_editor->child != NULL){
    pad_editor_start = 
      pad_editor = gtk_container_get_children(GTK_CONTAINER(output_listing_editor->child));

    while(pad_editor != NULL){
      ags_applicable_set_update(AGS_APPLICABLE(pad_editor->data), update);

      pad_editor = pad_editor->next;
    }

    g_list_free(pad_editor_start);
  }
}

void
ags_output_listing_editor_apply(AgsApplicable *applicable)
{
  AgsOutputListingEditor *output_listing_editor;

  GList *pad_editor, *pad_editor_start;

  output_listing_editor = AGS_OUTPUT_LISTING_EDITOR(applicable);

  if((AGS_PROPERTY_EDITOR_ENABLED & (AGS_PROPERTY_EDITOR(output_listing_editor)->flags)) == 0){
    return;
  }

  if(output_listing_editor->child != NULL){
    pad_editor_start = 
      pad_editor = gtk_container_get_children(GTK_CONTAINER(output_listing_editor->child));

    while(pad_editor != NULL){
      ags_applicable_apply(AGS_APPLICABLE(pad_editor->data));

      pad_editor = pad_editor->next;
    }
  
    g_list_free(pad_editor_start);
  }
}

void
ags_output_listing_editor_reset(AgsApplicable *applicable)
{
  AgsOutputListingEditor *output_listing_editor;

  GList *pad_editor, *pad_editor_start;

  output_listing_editor = AGS_OUTPUT_LISTING_EDITOR(applicable);

  if(output_listing_editor->child != NULL){
    pad_editor_start = 
      pad_editor = gtk_container_get_children(GTK_CONTAINER(output_listing_editor->child));

    while(pad_editor != NULL){
      ags_applicable_reset(AGS_APPLICABLE(pad_editor->data));

      pad_editor = pad_editor->next;
    }

    g_list_free(pad_editor_start);
  }
}

/**
 * ags_output_listing_editor_add_children:
 * @output_listing_editor: the #AgsOutputListingEditor
 * @audio: the #AgsAudio to use
 * @nth_channel: nth channel to start creation until end
 * @connect: if %TRUE widget is connected and shown
 *
 * Creates new pad editors or destroys them.
 *
 * Since: 2.0.0
 */
void
ags_output_listing_editor_add_children(AgsOutputListingEditor *output_listing_editor,
				       AgsAudio *audio, guint nth_channel,
				       gboolean connect)
{
  AgsPadEditor *pad_editor;
  GtkVBox *vbox;

  AgsChannel *channel;

  if(nth_channel == 0 &&
     output_listing_editor->child != NULL){
    vbox = output_listing_editor->child;
    output_listing_editor->child = NULL;
    gtk_widget_destroy(GTK_WIDGET(vbox));
  }

  if(audio == NULL){
    return;
  }

  /* instantiate pad editor vbox */
  if(nth_channel == 0){
    output_listing_editor->child = (GtkVBox *) gtk_vbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(output_listing_editor),
		       GTK_WIDGET(output_listing_editor->child),
		       FALSE, FALSE,
		       0);
  }

  /* get current channel */
  if(g_type_is_a(output_listing_editor->channel_type, AGS_TYPE_OUTPUT)){
    g_object_get(audio,
		 "output", &channel,
		 NULL);
  }else if(g_type_is_a(output_listing_editor->channel_type, AGS_TYPE_INPUT)){
    g_object_get(audio,
		 "input", &channel,
		 NULL); 
  }else{
    channel = NULL;
  }
  
  channel = ags_channel_nth(channel,
			    nth_channel);
  
  while(channel != NULL){
    /* instantiate pad editor */
    pad_editor = ags_pad_editor_new(NULL);

    pad_editor->editor_type_count = 1;
      pad_editor->editor_type = (GType *) malloc(pad_editor->editor_type_count * sizeof(GType));
    pad_editor->editor_type[0] = AGS_TYPE_OUTPUT_EDITOR;

    g_object_set(pad_editor,
		 "channel", channel,
		 NULL);
    
    gtk_box_pack_start(GTK_BOX(output_listing_editor->child),
		       GTK_WIDGET(pad_editor),
		       FALSE, FALSE,
		       0);

    if(connect){
      ags_connectable_connect(AGS_CONNECTABLE(pad_editor));
      gtk_widget_show_all(GTK_WIDGET(pad_editor));
    }

    /* iterate */
    g_object_get(channel,
		 "next-pad", &channel,
		 NULL);
  }
}

/**
 * ags_output_listing_editor_new:
 * @channel_type: the channel type to represent
 *
 * Create a new instance of #AgsOutputListingEditor
 *
 * Returns: the new #AgsOutputListingEditor
 *
 * Since: 2.0.0
 */
AgsOutputListingEditor*
ags_output_listing_editor_new(GType channel_type)
{
  AgsOutputListingEditor *output_listing_editor;

  output_listing_editor = (AgsOutputListingEditor *) g_object_new(AGS_TYPE_OUTPUT_LISTING_EDITOR,
								  NULL);
  
  output_listing_editor->channel_type = channel_type;

  return(output_listing_editor);
}

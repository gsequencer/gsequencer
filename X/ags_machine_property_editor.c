#include "ags_machine_property_editor.h"
#include "ags_machine_property_editor_callbacks.h"

#include "ags_machine_editor.h"
#include "ags_pad_editor.h"
#include "ags_line_editor.h"
#include "ags_link_editor.h"

#include "../audio/ags_output.h"
#include "../audio/ags_input.h"

#include "../audio/file/ags_audio_file.h"

GType ags_machine_property_editor_get_type();
void ags_machine_property_editor_class_init(AgsMachinePropertyEditorClass *machine_property_editor);
void ags_machine_property_editor_init(AgsMachinePropertyEditor *machine_property_editor);
void ags_machine_property_editor_destroy(GtkObject *object);
void ags_machine_property_editor_connect(AgsMachinePropertyEditor *machine_property_editor);

GType
ags_machine_property_editor_get_type()
{
  static GType machine_property_editor_type = 0;

  if (!machine_property_editor_type){
    static const GtkTypeInfo machine_property_editor_info = {
      "AgsMachinePropertyEditor\0",
      sizeof(AgsMachinePropertyEditor), /* base_init */
      sizeof(AgsMachinePropertyEditorClass), /* base_finalize */
      (GtkClassInitFunc) ags_machine_property_editor_class_init,
      (GtkObjectInitFunc) ags_machine_property_editor_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      (GtkClassInitFunc) NULL,
    };

    machine_property_editor_type = gtk_type_unique (GTK_TYPE_VBOX, &machine_property_editor_info);
  }

  return (machine_property_editor_type);
}

void
ags_machine_property_editor_class_init(AgsMachinePropertyEditorClass *machine_property_editor)
{
}

void
ags_machine_property_editor_init(AgsMachinePropertyEditor *machine_property_editor)
{
  g_signal_connect((GObject *) machine_property_editor, "parent_set\0",
		   G_CALLBACK(ags_machine_property_editor_parent_set_callback), machine_property_editor);
}

void
ags_machine_property_editor_destroy(GtkObject *object)
{
}

void
ags_machine_property_editor_connect(AgsMachinePropertyEditor *machine_property_editor)
{
  AgsMachineEditor *machine_editor;
  GList *list;

  machine_editor = AGS_MACHINE_EDITOR(gtk_widget_get_ancestor((GtkWidget *) machine_property_editor, AGS_TYPE_MACHINE_EDITOR));

  g_signal_connect((GObject *) machine_property_editor, "show\0",
		   G_CALLBACK(ags_machine_property_editor_show_callback), machine_property_editor);

  list = gtk_container_get_children((GtkContainer *) machine_property_editor);

  while(list != NULL){
    ags_pad_editor_connect(AGS_PAD_EDITOR(list->data));

    list = list->next; 
  }

  g_signal_connect_after((GObject *) machine_editor->machine_resize_editor, "resize_audio_channels\0",
			 G_CALLBACK(ags_machine_property_editor_resize_audio_channels_callback), machine_property_editor);

  if(machine_property_editor->type == AGS_TYPE_INPUT){
    g_signal_connect_after((GObject *) machine_editor, "add_input\0",
			   G_CALLBACK(ags_machine_property_editor_add_input_callback), machine_property_editor);

    g_signal_connect_after((GObject *) machine_editor, "remove_input\0",
			   G_CALLBACK(ags_machine_property_editor_remove_input_callback), machine_property_editor);

    g_signal_connect_after((GObject *) machine_editor->machine_resize_editor, "resize_input_pads\0",
			   G_CALLBACK(ags_machine_property_editor_resize_input_pads_callback), machine_property_editor);

    g_signal_connect_after((GObject *) machine_editor->machine_link_editor, "reset_input\0",
			   G_CALLBACK(ags_machine_property_editor_reset_input_callback), machine_property_editor);
  }else{
    g_signal_connect_after((GObject *) machine_editor, "add_output\0",
			   G_CALLBACK(ags_machine_property_editor_add_output_callback), machine_property_editor);

    g_signal_connect_after((GObject *) machine_editor, "remove_output\0",
			   G_CALLBACK(ags_machine_property_editor_remove_output_callback), machine_property_editor);

    g_signal_connect_after((GObject *) machine_editor->machine_resize_editor, "resize_output_pads\0",
			   G_CALLBACK(ags_machine_property_editor_resize_output_pads_callback), machine_property_editor);
    
    g_signal_connect_after((GObject *) machine_editor->machine_link_editor, "reset_output\0",
			   G_CALLBACK(ags_machine_property_editor_reset_output_callback), machine_property_editor);

  }
}

void
ags_machine_property_editor_show(GtkWidget *widget)
{
  AgsMachinePropertyEditor *machine_property_editor;
  GList *list;

  machine_property_editor = (AgsMachinePropertyEditor *) widget;

  list = gtk_container_get_children((GtkContainer *) machine_property_editor);

  while(list != NULL){
    gtk_widget_show(GTK_WIDGET(list->data));

    list = list->next;
  }
}

void
ags_machine_property_editor_apply(AgsMachinePropertyEditor *machine_property_editor)
{
  AgsMachine *machine;
  AgsLinkEditor *link_editor;
  AgsChannel *channel;
  AgsAudioFile *audio_file;
  GList *list_pad, *list_line;
  GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  list_pad = gtk_container_get_children((GtkContainer *) machine_property_editor);

  while(list_pad != NULL){
    list_line = AGS_PAD_EDITOR(list_pad->data)->line_editor;

    while(list_line != NULL){
      channel = AGS_LINE_EDITOR(list_line->data)->channel;
      link_editor = AGS_LINE_EDITOR(list_line->data)->link_editor;

      if(!strncmp((char *) gtk_label_get_text(GTK_LABEL(GTK_BIN(link_editor->option)->child)), "file://\0", 7)){
	AgsAudioSignal *audio_signal, *audio_signal_source_old;

	audio_file = link_editor->audio_file;

	if(audio_file == NULL){
	  ags_channel_set_link(channel, NULL);
	}else{
	  ags_audio_file_read_audio_signal(audio_file);

	  ags_channel_set_link(channel, NULL);

	  audio_signal = (AgsAudioSignal *) g_list_nth(audio_file->audio_signal, (guint) link_editor->spin_button->adjustment->value)->data;
	  audio_signal->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	  audio_signal->recycling = (GObject *) channel->first_recycling;

	  audio_signal_source_old = ags_audio_signal_get_template(channel->first_recycling->audio_signal);

	  g_static_mutex_lock(&mutex);
	  channel->first_recycling->audio_signal = g_list_remove(channel->first_recycling->audio_signal, audio_signal_source_old);
	  channel->first_recycling->audio_signal = g_list_prepend(channel->first_recycling->audio_signal, audio_signal);
	  g_static_mutex_unlock(&mutex);

	  g_object_unref(G_OBJECT(audio_signal_source_old));
	}
      }else{
	machine = (AgsMachine *) g_object_get_data((GObject *) link_editor->option->menu_item, (char *) g_type_name(AGS_TYPE_MACHINE ));

	if(machine != NULL){
	  if(machine_property_editor->type == AGS_TYPE_OUTPUT)
	    ags_channel_set_link(channel, ags_channel_nth(machine->audio->input, (guint) link_editor->spin_button->adjustment->value));
	  else
	    ags_channel_set_link(channel, ags_channel_nth(machine->audio->output, (guint) link_editor->spin_button->adjustment->value));
	}else
	  ags_channel_set_link(channel, NULL);
      }
      

      list_line = list_line->next;
    }

    list_pad = list_pad->next;
  }
}

AgsMachinePropertyEditor*
ags_machine_property_editor_new(GType type)
{
  AgsMachinePropertyEditor *machine_property_editor;

  machine_property_editor = (AgsMachinePropertyEditor *) g_object_new(AGS_TYPE_MACHINE_PROPERTY_EDITOR, NULL);
  machine_property_editor->type = type;

  return(machine_property_editor);
}

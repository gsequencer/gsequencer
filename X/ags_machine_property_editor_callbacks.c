#include "ags_machine_property_editor_callbacks.h"

#include "../audio/ags_output.h"
#include "../audio/ags_input.h"

#include "ags_pad_editor.h"
#include "ags_line_editor.h"

int
ags_machine_property_editor_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsMachinePropertyEditor *machine_property_editor)
{
  AgsMachineEditor *machine_editor;

  if(old_parent != NULL)
    return(0);

  machine_editor = AGS_MACHINE_EDITOR(gtk_widget_get_ancestor((GtkWidget *) machine_property_editor, AGS_TYPE_MACHINE_EDITOR));
  g_signal_connect_after((GObject *) machine_editor, "set_machine\0",
			 G_CALLBACK(ags_machine_property_editor_set_machine_callback), machine_property_editor);

  return(0);
}

int
ags_machine_property_editor_show_callback(GtkWidget *widget, AgsMachinePropertyEditor *machine_property_editor)
{
  ags_machine_property_editor_show(widget);

  return(0);
}

void
ags_machine_property_editor_set_machine_callback(AgsMachineEditor *machine_editor, AgsMachine *machine, AgsMachinePropertyEditor *machine_property_editor)
{
  AgsPadEditor *pad_editor;
  AgsChannel *channel;
  GList *list_pad_editor;
  guint pads;
  guint i;

  if(machine_property_editor->type == AGS_TYPE_INPUT){
    channel = machine->audio->input;
    pads = machine->audio->input_pads;
  }else{
    channel = machine->audio->output;
    pads = machine->audio->output_pads;
  }

  for(i = 0; i < pads; i++){
    pad_editor = ags_pad_editor_new();
    pad_editor->pad = channel;
    gtk_label_set_label(pad_editor->label, g_strdup_printf("pad %d\0", i));
    gtk_box_pack_start((GtkBox *) machine_property_editor, (GtkWidget *) pad_editor, FALSE, FALSE, 0);

    channel = channel->next_pad;
  }
}

void
ags_machine_property_editor_add_input_callback(AgsMachineEditor *machine_editor, AgsMachinePropertyEditor *machine_property_editor)
{
  AgsPadEditor *pad_editor;
  AgsChannel *channel;
  guint pad, nth;

  pad = machine_editor->machine->audio->input_pads;
  AGS_AUDIO_GET_CLASS(machine_editor->machine->audio)->set_pads(machine_editor->machine->audio, AGS_TYPE_INPUT, pad + 1);

  nth = machine_editor->machine->audio->input_lines - machine_editor->machine->audio->audio_channels;

  channel = ags_channel_nth(machine_editor->machine->audio->input, nth);

  pad_editor = ags_pad_editor_new();
  pad_editor->pad = channel;
  gtk_label_set_label(pad_editor->label,
		      g_strdup_printf("pad %d\0", pad));
  gtk_box_pack_start((GtkBox *) machine_property_editor, (GtkWidget *) pad_editor, FALSE, FALSE, 0);
  gtk_widget_show_all((GtkWidget *) pad_editor);
}

void
ags_machine_property_editor_add_output_callback(AgsMachineEditor *machine_editor, AgsMachinePropertyEditor *machine_property_editor)
{
  AgsPadEditor *pad_editor;
  AgsChannel *channel;
  guint pad, nth;

  pad = machine_editor->machine->audio->output_pads;
  AGS_AUDIO_GET_CLASS(machine_editor->machine->audio)->set_pads(machine_editor->machine->audio, AGS_TYPE_OUTPUT, pad + 1);

  nth = machine_editor->machine->audio->output_lines - machine_editor->machine->audio->audio_channels;

  channel = ags_channel_nth(machine_editor->machine->audio->output, nth);

  pad_editor = ags_pad_editor_new();
  pad_editor->pad = channel;
  gtk_label_set_label(pad_editor->label,
		      g_strdup_printf("pad %d\0", pad));
  gtk_box_pack_start((GtkBox *) machine_property_editor, (GtkWidget *) pad_editor, FALSE, FALSE, 0);
  gtk_widget_show_all((GtkWidget *) pad_editor);
}

void
ags_machine_property_editor_remove_input_callback(AgsMachineEditor *machine_editor, AgsMachinePropertyEditor *machine_property_editor)
{
  AgsChannel *channel;
  GList *list;
  guint pads;

  channel = machine_editor->machine->audio->input;
  pads = machine_editor->machine->audio->input_pads;

  if(pads > 0){
    pads -= 1;

    AGS_AUDIO_GET_CLASS(machine_editor->machine->audio)->set_pads(machine_editor->machine->audio, AGS_TYPE_INPUT, pads);

    list = gtk_container_get_children((GtkContainer *) machine_property_editor);
    list = g_list_last(list);
    gtk_widget_destroy((GtkWidget *) list->data);
  }
}

void
ags_machine_property_editor_remove_output_callback(AgsMachineEditor *machine_editor, AgsMachinePropertyEditor *machine_property_editor)
{
  AgsChannel *channel;
  GList *list;
  guint pads;

  channel = machine_editor->machine->audio->output;
  pads = machine_editor->machine->audio->output_pads;

  if(pads > 0){
    pads -= 1;

    AGS_AUDIO_GET_CLASS(machine_editor->machine->audio)->set_pads(machine_editor->machine->audio, AGS_TYPE_OUTPUT, pads);

    list = gtk_container_get_children((GtkContainer *) machine_property_editor);
    list = g_list_last(list);
    gtk_widget_destroy((GtkWidget *) list->data);
  }
}

void
ags_machine_property_editor_resize_audio_channels_callback(AgsMachineResizeEditor *machine_resize_editor, guint audio_channels, guint audio_channels_old, AgsMachinePropertyEditor *machine_property_editor)
{
  if(audio_channels_old > audio_channels){
    GList *list_pad, *list_line, *list_next_line;

    list_pad = gtk_container_get_children((GtkContainer *) machine_property_editor);

    while(list_pad != NULL){
      list_line = AGS_PAD_EDITOR(list_pad->data)->line_editor;
      list_line = g_list_nth(list_line, audio_channels);

      if(list_line->prev != NULL){
	list_line->prev->next = NULL;
	list_line->prev = NULL;
      }else
	AGS_PAD_EDITOR(list_pad->data)->line_editor = NULL;

      while(list_line != NULL){
	list_next_line = list_line->next;

	gtk_widget_destroy((GtkWidget *) list_line->data);
	g_list_free1(list_line);

	list_line = list_next_line;
      }

      list_pad = list_pad->next;
    }
  }else if(audio_channels_old < audio_channels){
    AgsMachineEditor *machine_editor;
    AgsLineEditor *line_editor;
    AgsChannel *channel;
    GList *list_pad, *list_line;
    guint i;

    machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor((GtkWidget *) machine_property_editor, AGS_TYPE_MACHINE_EDITOR);

    if(machine_property_editor->type == AGS_TYPE_OUTPUT)
      channel = machine_editor->machine->audio->output;
    else
      channel = machine_editor->machine->audio->input;

    list_pad = gtk_container_get_children((GtkContainer *) machine_property_editor);

    while(list_pad != NULL){
      channel = ags_channel_nth(channel, audio_channels_old);

      for(i = audio_channels_old; i < audio_channels; i++){
	line_editor = ags_line_editor_new();
	line_editor->channel = channel;
	AGS_PAD_EDITOR(list_pad->data)->line_editor = g_list_append(AGS_PAD_EDITOR(list_pad->data)->line_editor, line_editor);
	gtk_box_pack_start((GtkBox *) AGS_PAD_EDITOR(list_pad->data), (GtkWidget *) line_editor, FALSE, FALSE, 0);

	if(GTK_WIDGET_VISIBLE((GtkWidget *) machine_editor)){
	  ags_line_editor_connect(line_editor);
	}

	if(AGS_PAD_EDITOR(list_pad->data)->arrow->arrow_type == GTK_ARROW_DOWN)
	  gtk_widget_show((GtkWidget *) line_editor);

	channel = channel->next;
      }

      list_pad = list_pad->next;
    }
  }
}

void
ags_machine_property_editor_resize_input_pads_callback(AgsMachineResizeEditor *machine_resize_editor, guint pads, guint pads_old, AgsMachinePropertyEditor *machine_property_editor)
{
  if(pads_old > pads){
    GList *list, *list_next;

    list = gtk_container_get_children((GtkContainer *) machine_property_editor);
    list = g_list_nth(list, pads);
    
    while(list != NULL){
      list_next = list->next;

      gtk_widget_destroy((GtkWidget *) list->data);

      list = list_next;
    }
  }else if(pads_old < pads){
    AgsMachineEditor *machine_editor;
    AgsPadEditor *pad_editor;
    AgsChannel *channel;
    guint i;

    machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor((GtkWidget *) machine_property_editor, AGS_TYPE_MACHINE_EDITOR);
    channel = ags_channel_nth(machine_editor->machine->audio->input, pads_old * machine_editor->machine->audio->audio_channels);

    for(i = pads_old; i < pads; i++){
      pad_editor = ags_pad_editor_new();
      pad_editor->pad = channel;
      gtk_label_set_label(pad_editor->label, g_strdup_printf("pad %d\0", i));
      gtk_box_pack_start((GtkBox *) machine_property_editor, (GtkWidget *) pad_editor, FALSE, FALSE, 0);

      if(GTK_WIDGET_VISIBLE((GtkWidget *) machine_editor)){
	ags_pad_editor_connect(pad_editor);
	gtk_widget_show((GtkWidget *) pad_editor);
      }

      channel = channel->next_pad;
    }
  }
}

void
ags_machine_property_editor_resize_output_pads_callback(AgsMachineResizeEditor *machine_resize_editor, guint pads, guint pads_old, AgsMachinePropertyEditor *machine_property_editor)
{
  if(pads_old > pads){
    GList *list, *list_next;

    list = gtk_container_get_children((GtkContainer *) machine_property_editor);
    list = g_list_nth(list, pads);
    
    while(list != NULL){
      list_next = list->next;

      gtk_widget_destroy((GtkWidget *) list->data);

      list = list_next;
    }    
  }else if(pads_old < pads){
    AgsMachineEditor *machine_editor;
    AgsPadEditor *pad_editor;
    AgsChannel *channel;
    guint i;

    machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor((GtkWidget *) machine_property_editor, AGS_TYPE_MACHINE_EDITOR);
    channel = ags_channel_nth(machine_editor->machine->audio->output, pads_old * machine_editor->machine->audio->audio_channels);

    for(i = pads_old; i < pads; i++){
      pad_editor = ags_pad_editor_new();
      pad_editor->pad = channel;
      gtk_label_set_label(pad_editor->label, g_strdup_printf("pad %d\0", i));
      gtk_box_pack_start((GtkBox *) machine_property_editor, (GtkWidget *) pad_editor, FALSE, FALSE, 0);

      if(GTK_WIDGET_VISIBLE((GtkWidget *) machine_editor)){
	ags_pad_editor_connect(pad_editor);
	gtk_widget_show((GtkWidget *) pad_editor);
      }

      channel = channel->next_pad;
    }
  }
}

void
ags_machine_property_editor_reset_input_callback(AgsMachineLinkEditor *machine_link_editor, AgsMachinePropertyEditor *machine_property_editor)
{
  AgsMachineEditor *machine_editor;
  GList *list_pad_editor, *list_line_editor;
  guint first_line, nth, first_link, n;
  guint i, i_stop, j, j_set, j_stop;

  n = (guint) machine_link_editor->input_n->adjustment->value;

  if(n == 0)
    return;

  printf("n = %d\n\0", n);

  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor((GtkWidget *) machine_property_editor, AGS_TYPE_MACHINE_EDITOR);

  first_line = (guint) machine_link_editor->input_first_line->adjustment->value;
  nth = gtk_option_menu_get_history(machine_link_editor->input_link);
  first_link = (guint) machine_link_editor->input_first_link->adjustment->value;

  j_set = first_line % machine_editor->machine->audio->audio_channels;
  j_stop = (n - j_set) % machine_editor->machine->audio->audio_channels;
  i_stop = (n - j_set - (machine_editor->machine->audio->audio_channels - j_stop)) / machine_editor->machine->audio->audio_channels;

  list_pad_editor = gtk_container_get_children((GtkContainer *) machine_property_editor);
  list_pad_editor = g_list_nth(list_pad_editor, (first_line - j_set) / machine_editor->machine->audio->audio_channels);

  if(j_set != 0){
    list_line_editor = AGS_PAD_EDITOR(list_pad_editor->data)->line_editor;
    list_line_editor = g_list_nth(list_line_editor, j_set);
  
    for(j = j_set; j < machine_editor->machine->audio->audio_channels; j++){
      gtk_option_menu_set_history(AGS_LINE_EDITOR(list_line_editor->data)->link_editor->option, nth);

      list_line_editor = list_line_editor->next;
    }

    list_pad_editor = list_pad_editor->next;
  }

  for(i = 0; i < i_stop; i++){
    list_line_editor = AGS_PAD_EDITOR(list_pad_editor->data)->line_editor;

    for(j = 0; j < machine_editor->machine->audio->audio_channels; j++){
      gtk_option_menu_set_history(AGS_LINE_EDITOR(list_line_editor->data)->link_editor->option, nth);

      list_line_editor = list_line_editor->next;
    }

    list_pad_editor = list_pad_editor->next;
  }

  if(j_stop != 0){
    list_line_editor = AGS_PAD_EDITOR(list_pad_editor->data)->line_editor;

    for(j = 0; j < j_stop; j++){
      gtk_option_menu_set_history(AGS_LINE_EDITOR(list_line_editor->data)->link_editor->option, nth);

      list_line_editor = list_line_editor->next;
    }
  }
}

void
ags_machine_property_editor_reset_output_callback(AgsMachineLinkEditor *machine_link_editor, AgsMachinePropertyEditor *machine_property_editor)
{
  AgsMachineEditor *machine_editor;
  GList *list_pad_editor, *list_line_editor;
  guint first_line, nth, first_link, n;
  guint i, i_stop, j, j_set, j_stop;

  n = (guint) machine_link_editor->output_n->adjustment->value;

  if(n == 0)
    return;

  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor((GtkWidget *) machine_property_editor, AGS_TYPE_MACHINE_EDITOR);

  first_line = (guint) machine_link_editor->output_first_line->adjustment->value;
  nth = gtk_option_menu_get_history(machine_link_editor->output_link);
  first_link = (guint) machine_link_editor->output_first_link->adjustment->value;

  j_set = first_line % machine_editor->machine->audio->audio_channels;
  j_stop = (n - j_set) % machine_editor->machine->audio->audio_channels;
  i_stop = (n - j_set - (machine_editor->machine->audio->audio_channels - j_stop)) / machine_editor->machine->audio->audio_channels;

  list_pad_editor = gtk_container_get_children((GtkContainer *) machine_property_editor);
  list_pad_editor = g_list_nth(list_pad_editor, (first_line - j_set) / machine_editor->machine->audio->audio_channels);

  if(j_set != 0){
    list_line_editor = AGS_PAD_EDITOR(list_pad_editor->data)->line_editor;
    list_line_editor = g_list_nth(list_line_editor, j_set);
  
    for(j = j_set; j < machine_editor->machine->audio->audio_channels; j++){
      gtk_option_menu_set_history(AGS_LINE_EDITOR(list_line_editor->data)->link_editor->option, nth);
      
      list_line_editor = list_line_editor->next;
    }

    list_pad_editor = list_pad_editor->next;
  }

  for(i = 0; i < i_stop; i++){
    list_line_editor = AGS_PAD_EDITOR(list_pad_editor->data)->line_editor;

    for(j = 0; j < machine_editor->machine->audio->audio_channels; j++){
      gtk_option_menu_set_history(AGS_LINE_EDITOR(list_line_editor->data)->link_editor->option, nth);

      list_line_editor = list_line_editor->next;
    }

    list_pad_editor = list_pad_editor->next;
  }

  if(j_stop != 0){
    list_line_editor = AGS_PAD_EDITOR(list_pad_editor->data)->line_editor;

    for(j = 0; j < j_stop; j++){
      gtk_option_menu_set_history(AGS_LINE_EDITOR(list_line_editor->data)->link_editor->option, nth);

      list_line_editor = list_line_editor->next;
    }
  }
}


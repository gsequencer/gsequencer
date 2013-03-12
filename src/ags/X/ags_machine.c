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

#include <ags/X/ags_machine.h>
#include <ags/X/ags_machine_callbacks.h>

#include <ags/object/ags_connectable.h>

#include <ags/audio/ags_input.h>

#include <ags/audio/file/ags_audio_file.h>

#include <ags/audio/task/ags_link_channel.h>

#include <ags/X/ags_window.h>

void ags_machine_class_init(AgsMachineClass *machine);
void ags_machine_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_machine_init(AgsMachine *machine);
void ags_machine_connect(AgsConnectable *connectable);
void ags_machine_disconnect(AgsConnectable *connectable);
static void ags_machine_finalize(GObject *gobject);
void ags_machine_show(GtkWidget *widget);

GtkMenu* ags_machine_popup_new(AgsMachine *machine);

extern void ags_file_read_machine(AgsFile *file, AgsMachine *machine);
extern void ags_file_write_machine(AgsFile *file, AgsMachine *machine);

static gpointer ags_machine_parent_class = NULL;

GType
ags_machine_get_type(void)
{
  static GType ags_type_machine = 0;

  if(!ags_type_machine){
    static const GTypeInfo ags_machine_info = {
      sizeof (AgsMachineClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_machine_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMachine),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_machine_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_machine_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_machine = g_type_register_static(GTK_TYPE_HANDLE_BOX,
					      "AgsMachine", &ags_machine_info,
					      0);
    
    g_type_add_interface_static(ags_type_machine,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_machine);
}

void
ags_machine_class_init(AgsMachineClass *machine)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  ags_machine_parent_class = g_type_class_peek_parent(machine);

  /* GtkObjectClass */
  gobject = (GObjectClass *) machine;

  gobject->finalize = ags_machine_finalize;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) machine;

  widget->show = ags_machine_show;
}

void
ags_machine_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_machine_connect;
  connectable->disconnect = ags_machine_disconnect;
}

void
ags_machine_init(AgsMachine *machine)
{
  GtkFrame *frame;

  machine->flags = 0;
  machine->file_input_flags = 0;

  machine->name = NULL;

  frame = (GtkFrame *) gtk_frame_new(NULL);
  gtk_container_add((GtkContainer *) machine, (GtkWidget *) frame);

  machine->audio = ags_audio_new(NULL);
  machine->audio->machine = (GtkWidget *) machine;

  machine->output = NULL;
  machine->input = NULL;

  machine->popup = ags_machine_popup_new(machine);
  machine->properties = NULL;
  machine->rename = NULL;
}

void
ags_machine_connect(AgsConnectable *connectable)
{
  AgsMachine *machine;
  GList *pad_list;

  /* AgsMachine */
  machine = AGS_MACHINE(connectable);

  /* AgsAudio */
  ags_connectable_connect(AGS_CONNECTABLE(machine->audio));

  /* GtkObject * /

  /* GtkWidget */
  g_signal_connect(G_OBJECT (machine), "button_press_event",
		   G_CALLBACK(ags_machine_button_press_callback), (gpointer) machine);

  /* AgsPad - input */
  if(machine->input != NULL){
    pad_list = gtk_container_get_children(GTK_CONTAINER(machine->input));

    while(pad_list != NULL){
      ags_connectable_connect(AGS_CONNECTABLE(pad_list->data));
      
      pad_list = pad_list->next;
    }
  }

  /* AgsPad - output */
  if(machine->output != NULL){
    pad_list = gtk_container_get_children(GTK_CONTAINER(machine->output));
    
    while(pad_list != NULL){
      ags_connectable_connect(AGS_CONNECTABLE(pad_list->data));
      
      pad_list = pad_list->next;
    }
  }
}

void
ags_machine_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

static void
ags_machine_finalize(GObject *gobject)
{
  AgsMachine *machine;
  char *str;

  machine = (AgsMachine *) gobject;

  if(machine->name != NULL)
    g_free(machine->name);

  if(machine->audio != NULL)
    g_object_unref(G_OBJECT(machine->audio));

  G_OBJECT_CLASS(ags_machine_parent_class)->finalize(gobject);
}

void
ags_machine_show(GtkWidget *widget)
{
  AgsMachine *machine;
  AgsWindow *window;
  GtkFrame *frame;

  machine = (AgsMachine *) widget;

  GTK_WIDGET_CLASS(ags_machine_parent_class)->show(widget);

  window = (AgsWindow *) gtk_widget_get_toplevel(widget);
  window->counter->everything++;

  frame = (GtkFrame *) gtk_container_get_children((GtkContainer *) machine)->data;
  gtk_widget_show_all((GtkWidget *) frame);
}

GtkListStore*
ags_machine_get_possible_links(AgsMachine *machine)
{
  GtkListStore *model;
  GtkTreeIter iter;
  GList *list;

  model = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);
		    
  gtk_list_store_append(model, &iter);
  gtk_list_store_set(model, &iter,
		     0, "NULL",
		     1, NULL,
		     -1);

  if(GTK_WIDGET(machine)->parent != NULL){
    list = gtk_container_get_children(GTK_CONTAINER(GTK_WIDGET(machine)->parent));

    while(list != NULL){
      if(list->data != machine){
	gtk_list_store_append(model, &iter);
	gtk_list_store_set(model, &iter,
			   0, g_strdup_printf("%s: %s", 
					      G_OBJECT_TYPE_NAME(G_OBJECT(list->data)),
					      AGS_MACHINE(list->data)->name),
			   1, list->data,
			   -1);
      }

      list = list->next;
    }
  }

  return(model);
}

AgsMachine*
ags_machine_find_by_name(GList *list, char *name)
{
  while(list != NULL){
    if(!g_strcmp0(AGS_MACHINE(list->data)->name, name))
      return((AgsMachine *) list->data);

    list = list->next;
  }

  return(NULL);
}

GtkFileChooserDialog*
ags_machine_file_chooser_dialog_new(AgsMachine *machine)
{
  GtkFileChooserDialog *file_chooser;
  GtkCheckButton *check_button;

  file_chooser = (GtkFileChooserDialog *) gtk_file_chooser_dialog_new(g_strdup("open audio files"),
								      (GtkWindow *) gtk_widget_get_toplevel((GtkWidget *) machine),
								      GTK_FILE_CHOOSER_ACTION_OPEN,
								      GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
								      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
								      NULL);
  gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(file_chooser), TRUE);

  check_button = (GtkCheckButton *) gtk_check_button_new_with_label(g_strdup("open in new channel"));
  gtk_toggle_button_set_active((GtkToggleButton *) check_button, TRUE);
  gtk_box_pack_start((GtkBox *) GTK_DIALOG(file_chooser)->vbox, (GtkWidget *) check_button, FALSE, FALSE, 0);
  g_object_set_data((GObject *) file_chooser, "create", (gpointer) check_button);

  check_button = (GtkCheckButton *) gtk_check_button_new_with_label(g_strdup("overwrite existing links"));
  gtk_toggle_button_set_active((GtkToggleButton *) check_button, TRUE);
  gtk_box_pack_start((GtkBox *) GTK_DIALOG(file_chooser)->vbox, (GtkWidget *) check_button, FALSE, FALSE, 0);
  g_object_set_data((GObject *) file_chooser, "overwrite", (gpointer) check_button);

  return(file_chooser);
}

void
ags_machine_open_files(AgsMachine *machine,
		       GSList *filenames,
		       gboolean overwrite_channels,
		       gboolean create_channels)
{
  AgsLinkChannel *link_channel;
  AgsChannel *channel;
  AgsAudioFile *audio_file;
  AgsAudioSignal *audio_signal_source_old;
  GList *audio_signal_list;
  guint i, j;
  guint list_length;

  channel = machine->audio->input;

  /* overwriting existing channels */
  if(overwrite_channels){
    if(channel != NULL){
      for(i = 0; i < machine->audio->input_pads && filenames != NULL; i++){
	audio_file = ags_audio_file_new((gchar *) filenames->data,
					(AgsDevout *) machine->audio->devout,
					0, machine->audio->audio_channels);
	if(!ags_audio_file_open(audio_file)){
	  filenames = filenames->next;
	  continue;
	}

	ags_audio_file_read_audio_signal(audio_file);
	ags_audio_file_close(audio_file);
	
	audio_signal_list = audio_file->audio_signal;
	
	for(j = 0; j < machine->audio->audio_channels && audio_signal_list != NULL; j++){
	  /* create task */
	  link_channel = ags_link_channel_new(channel, NULL);
	  
	  /* append AgsLinkChannel */
	  // FIXME:JK: has a need for the unavaible task
	  //	    ags_devout_append_task(AGS_DEVOUT(AGS_AUDIO(channel->audio)->devout),
	  //				   AGS_TASK(link_channel));
	  
	  AGS_AUDIO_SIGNAL(audio_signal_list->data)->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	  AGS_AUDIO_SIGNAL(audio_signal_list->data)->recycling = (GObject *) channel->first_recycling;
	  audio_signal_source_old = ags_audio_signal_get_template(channel->first_recycling->audio_signal);

	    // FIXME:JK: create a task
	  channel->first_recycling->audio_signal = g_list_remove(channel->first_recycling->audio_signal,
								 (gpointer) audio_signal_source_old);
	  channel->first_recycling->audio_signal = g_list_prepend(channel->first_recycling->audio_signal,
								  audio_signal_list->data);

	  g_object_unref(G_OBJECT(audio_signal_source_old));

	  audio_signal_list = audio_signal_list->next;
	  channel = channel->next;
	}

	if(audio_file->channels < machine->audio->audio_channels)
	  channel = ags_channel_nth(channel,
				    machine->audio->audio_channels - audio_file->channels);
	
	filenames = filenames->next;
      }
    }
  }

  /* appending to channels */
  if(create_channels && filenames != NULL){
    list_length = g_slist_length(filenames);
    
    ags_audio_set_pads((AgsAudio *) machine->audio, AGS_TYPE_INPUT,
		       list_length + AGS_AUDIO(machine->audio)->input_pads);
    channel = ags_channel_nth(AGS_AUDIO(machine->audio)->input,
			      (AGS_AUDIO(machine->audio)->input_pads - list_length) * AGS_AUDIO(machine->audio)->audio_channels);
    
    while(filenames != NULL){
      audio_file = ags_audio_file_new((gchar *) filenames->data,
				      (AgsDevout *) machine->audio->devout,
				      0, machine->audio->audio_channels);
      if(!ags_audio_file_open(audio_file)){
	filenames = filenames->next;
	continue;
      }
      
      ags_audio_file_read_audio_signal(audio_file);
      ags_audio_file_close(audio_file);
	
      audio_signal_list = audio_file->audio_signal;
      
      for(j = 0; j < machine->audio->audio_channels && audio_signal_list != NULL; j++){
	AGS_AUDIO_SIGNAL(audio_signal_list->data)->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	AGS_AUDIO_SIGNAL(audio_signal_list->data)->recycling = (GObject *) channel->first_recycling;
	audio_signal_source_old = ags_audio_signal_get_template(channel->first_recycling->audio_signal);
	
	channel->first_recycling->audio_signal = g_list_remove(channel->first_recycling->audio_signal,
							       (gpointer) audio_signal_source_old);
	channel->first_recycling->audio_signal = g_list_prepend(channel->first_recycling->audio_signal,
								audio_signal_list->data);
	
	g_object_unref(G_OBJECT(audio_signal_source_old));
	
	audio_signal_list = audio_signal_list->next;
	channel = channel->next;
      }
      
      if(machine->audio->audio_channels > audio_file->channels)
	channel = ags_channel_nth(channel,
				  machine->audio->audio_channels - audio_file->channels);
      
      filenames = filenames->next;
    }
  }
}

AgsMachine*
ags_machine_new(GObject *devout)
{
  AgsMachine *machine;
  GValue value;

  machine = (AgsMachine *) g_object_new(AGS_TYPE_MACHINE,
					NULL);
  
  g_value_init(&value, G_TYPE_OBJECT);
  g_value_set_object(&value, devout);
  g_object_set_property(G_OBJECT(machine->audio),
			"devout", &value);
  g_value_unset(&value);

  return(machine);
}

GtkMenu*
ags_machine_popup_new(AgsMachine *machine)
{
  GtkMenu *popup;
  GtkMenuItem *item;
  GList *list;

  popup = (GtkMenu *) gtk_menu_new();

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("move up"));
  gtk_menu_shell_append((GtkMenuShell *) popup, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("move down"));
  gtk_menu_shell_append((GtkMenuShell *) popup, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("hide"));
  gtk_menu_shell_append((GtkMenuShell *) popup, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("show"));
  gtk_menu_shell_append((GtkMenuShell *) popup, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("destroy"));
  gtk_menu_shell_append((GtkMenuShell *) popup, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("rename"));
  gtk_menu_shell_append((GtkMenuShell *) popup, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("properties"));
  gtk_menu_shell_append((GtkMenuShell *) popup, (GtkWidget*) item);

  list = gtk_container_get_children((GtkContainer *) popup);

  g_signal_connect((GObject*) list->data, "activate",
		   G_CALLBACK(ags_machine_popup_move_up_activate_callback), (gpointer) machine);
  list = list->next;

  g_signal_connect((GObject*) list->data, "activate",
		   G_CALLBACK(ags_machine_popup_move_down_activate_callback), (gpointer) machine);
  list = list->next;

  g_signal_connect((GObject*) list->data, "activate",
		   G_CALLBACK(ags_machine_popup_hide_activate_callback), (gpointer) machine);
  list = list->next;

  g_signal_connect((GObject*) list->data, "activate",
		   G_CALLBACK(ags_machine_popup_show_activate_callback), (gpointer) machine);
  list = list->next;

  g_signal_connect((GObject*) list->data, "activate",
		   G_CALLBACK(ags_machine_popup_destroy_activate_callback), (gpointer) machine);
  list = list->next;

  g_signal_connect((GObject*) list->data, "activate",
		   G_CALLBACK(ags_machine_popup_rename_activate_callback), (gpointer) machine);
  list = list->next;

  g_signal_connect((GObject*) list->data, "activate",
		   G_CALLBACK(ags_machine_popup_properties_activate_callback), (gpointer) machine);

  gtk_widget_show_all((GtkWidget*) popup);

  return(popup);
}

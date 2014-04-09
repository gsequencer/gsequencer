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

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/object/ags_plugin.h>

#include <ags/thread/ags_audio_loop.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_input.h>

#include <ags/audio/file/ags_audio_file.h>

#include <ags/audio/task/ags_open_file.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_pad.h>

void ags_machine_class_init(AgsMachineClass *machine);
void ags_machine_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_machine_plugin_interface_init(AgsPluginInterface *plugin);
void ags_machine_init(AgsMachine *machine);
void ags_machine_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec);
void ags_machine_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec);
void ags_machine_connect(AgsConnectable *connectable);
void ags_machine_disconnect(AgsConnectable *connectable);
gchar* ags_machine_get_name(AgsPlugin *plugin);
void ags_machine_set_name(AgsPlugin *plugin, gchar *name);
gchar* ags_machine_get_version(AgsPlugin *plugin);
void ags_machine_set_version(AgsPlugin *plugin, gchar *version);
gchar* ags_machine_get_build_id(AgsPlugin *plugin);
void ags_machine_set_build_id(AgsPlugin *plugin, gchar *build_id);
gchar* ags_machine_get_xml_type(AgsPlugin *plugin);
void ags_machine_set_xml_type(AgsPlugin *plugin, gchar *xml_type);
GList* ags_machine_get_ports(AgsPlugin *plugin);
void ags_machine_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_machine_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);
static void ags_machine_finalize(GObject *gobject);
void ags_machine_show(GtkWidget *widget);

GtkMenu* ags_machine_popup_new(AgsMachine *machine);

static gpointer ags_machine_parent_class = NULL;

enum{
  PROP_0,
  PROP_AUDIO,
};

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

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_machine_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_machine = g_type_register_static(GTK_TYPE_HANDLE_BOX,
					      "AgsMachine\0", &ags_machine_info,
					      0);
    
    g_type_add_interface_static(ags_type_machine,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_machine,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_machine);
}

void
ags_machine_class_init(AgsMachineClass *machine)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GParamSpec *param_spec;

  ags_machine_parent_class = g_type_class_peek_parent(machine);

  /* GtkObjectClass */
  gobject = (GObjectClass *) machine;
  
  gobject->set_property = ags_machine_set_property;
  gobject->get_property = ags_machine_get_property;

  gobject->finalize = ags_machine_finalize;

  /* properties */
  param_spec = g_param_spec_object("audio\0",
				   "assigned audio\0",
				   "The audio it is assigned to\0",
				   AGS_TYPE_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) machine;

  widget->show = ags_machine_show;
}

void
ags_machine_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_machine_connect;
  connectable->disconnect = ags_machine_disconnect;
}

void
ags_machine_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = ags_machine_get_name;
  plugin->set_name = ags_machine_set_name;
  plugin->get_version = ags_machine_get_version;
  plugin->set_version = ags_machine_set_version;
  plugin->get_build_id = ags_machine_get_build_id;
  plugin->set_build_id = ags_machine_set_build_id;
  plugin->get_xml_type = ags_machine_get_xml_type;
  plugin->set_xml_type = ags_machine_set_xml_type;
  plugin->get_ports = ags_machine_get_ports;
  plugin->read = ags_machine_read;
  plugin->write = ags_machine_write;
  plugin->set_ports = NULL;
}

void
ags_machine_init(AgsMachine *machine)
{
  GtkFrame *frame;

  machine->flags = 0;
  machine->file_input_flags = 0;

  machine->version = AGS_MACHINE_DEFAULT_VERSION;
  machine->build_id = AGS_MACHINE_DEFAULT_BUILD_ID;

  machine->name = NULL;

  machine->output_pad_type = G_TYPE_NONE;
  machine->input_pad_type = G_TYPE_NONE;

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
ags_machine_set_property(GObject *gobject,
			 guint prop_id,
			 const GValue *value,
			 GParamSpec *param_spec)
{
  AgsMachine *machine;

  machine = AGS_MACHINE(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      AgsAudio *audio;
      gboolean reset;

      audio = (AgsAudio *) g_value_get_object(value);
      
      reset = TRUE;

      if(machine->audio != NULL){
	GList *pad;

	g_object_unref(G_OBJECT(machine->audio));

	if(audio == NULL){
	  /* destroy pad */
	  pad = gtk_container_get_children(machine->output);
	  pad = g_list_nth(pad, audio->output_pads);
	  
	  while(pad != NULL){
	    gtk_widget_destroy(pad->data);

	    pad = pad->next;
	  }

	  pad = gtk_container_get_children(machine->input);
	  pad = g_list_nth(pad, audio->input_pads);
	  
	  while(pad != NULL){
	    gtk_widget_destroy(pad->data);
	    
	    pad = pad->next;
	  }
	  
	  reset = FALSE;
	}
      }

      if(audio != NULL){
	g_object_ref(G_OBJECT(audio));
	machine->audio = audio;

	if(reset){
	  AgsChannel *input, *output;
	  GList *pad;
	  GList *line;
	  guint i;

	  /* set channel and resize for AgsOutput */
	  if(machine->output_pad_type != G_TYPE_NONE){
	    output = audio->output;
	    pad = gtk_container_get_children(machine->output);

	    i = 0;

	    while(pad != NULL){
	      line = gtk_container_get_children(GTK_CONTAINER(AGS_PAD(pad->data)->expander_set));

	      ags_pad_resize_lines(AGS_PAD(pad->data), machine->output_line_type,
				   audio->audio_channels, g_list_length(line));
	      g_object_set(G_OBJECT(pad->data),
			   "channel\0", output,
			   NULL);

	      output = output->next_pad;
	      pad = pad->next;
	      i++;
	    }

	    if(output != NULL){
	      if(g_list_length(pad) < audio->output_pads){
		AgsPad *pad;

		/* add pad */
		for(; i < audio->output_pads; i++){
		  pad = g_object_new(machine->output_pad_type,
				     "channel\0", output,
				     NULL);
		  gtk_container_add(machine->output,
				    GTK_WIDGET(pad));
	  
		  ags_pad_resize_lines(pad, machine->output_line_type,
				       audio->audio_channels, 0);

		  if(GTK_WIDGET_VISIBLE((GtkWidget *) machine)){
		    ags_connectable_connect(AGS_CONNECTABLE(pad));
		  }

		  output = output->next_pad;
		}
	      }else if(g_list_length(pad) > audio->output_pads){
		/* destroy pad */
		pad = gtk_container_get_children(machine->output);
		pad = g_list_nth(pad, audio->output_pads);

		while(pad != NULL){
		  gtk_widget_destroy(pad->data);

		  pad = pad->next;
		}	      
	      }
	    }
	  }

	  /* set channel and resize for AgsInput */
	  if(machine->input_pad_type != G_TYPE_NONE){
	    input = audio->input;
	    pad = gtk_container_get_children(machine->input);

	    i = 0;

	    while(pad != NULL){
	      line = gtk_container_get_children(GTK_CONTAINER(AGS_PAD(pad->data)->expander_set));

	      ags_pad_resize_lines(AGS_PAD(pad->data), machine->input_line_type,
				   audio->audio_channels, g_list_length(line));
	      g_object_set(G_OBJECT(pad->data),
			   "channel\0", input,
			   NULL);

	      input = input->next_pad;
	      pad = pad->next;
	      i++;
	    }

	    if(input != NULL){
	      if(g_list_length(pad) < audio->input_pads){
		AgsPad *pad;

		/* add pad */
		for(; i < audio->input_pads; i++){
		  pad = g_object_new(machine->input_pad_type,
				     "channel\0", input,
				     NULL);
		  gtk_container_add(machine->input,
				    GTK_WIDGET(pad));

		  ags_pad_resize_lines(pad, machine->input_line_type,
				       audio->audio_channels, 0);

		  if(GTK_WIDGET_VISIBLE((GtkWidget *) machine)){
		    ags_connectable_connect(AGS_CONNECTABLE(pad));
		  }

		  input = input->next_pad;
		}
	      }else if(g_list_length(pad) > audio->input_pads){
		/* destroy pad */
		pad = gtk_container_get_children(machine->input);
		pad = g_list_nth(pad, audio->input_pads);

		while(pad != NULL){
		  gtk_widget_destroy(pad->data);

		  pad = pad->next;
		}	      
	      }
	    }
	  }
	}else{
	  AgsPad *pad;
	  AgsChannel *channel;
	  guint i;

	  /* add pad */
	  if(machine->output_pad_type != G_TYPE_NONE){
	    channel = audio->output;

	    for(i = 0; i < audio->output_pads; i++){
	      pad = g_object_new(machine->output_pad_type,
				 "channel\0", channel,
				 NULL);
	      gtk_container_add(machine->output,
				GTK_WIDGET(pad));
	      ags_connectable_connect(AGS_CONNECTABLE(pad));
	  
	      ags_pad_resize_lines(pad, machine->output_line_type,
				   audio->audio_channels, 0);

	      channel = channel->next_pad;
	    }
	  }

	  if(machine->input_pad_type != G_TYPE_NONE){
	    channel = audio->input;

	    for(i = 0; i < audio->input_pads; i++){
	      pad = g_object_new(machine->input_pad_type,
				 "channel\0", channel,
				 NULL);
	      gtk_container_add(machine->output,
				GTK_WIDGET(pad));
	      ags_connectable_connect(AGS_CONNECTABLE(pad));

	      ags_pad_resize_lines(pad, machine->input_line_type,
				   audio->audio_channels, 0);

	      channel = channel->next_pad;
	    }
	  }
	}
      }else{
	machine->audio = NULL;
      }
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_machine_get_property(GObject *gobject,
			 guint prop_id,
			 GValue *value,
			 GParamSpec *param_spec)
{
  AgsMachine *machine;

  machine = AGS_MACHINE(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    g_value_set_object(value, machine->audio);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_machine_connect(AgsConnectable *connectable)
{
  AgsMachine *machine;
  GList *pad_list;

  /* AgsMachine */
  machine = AGS_MACHINE(connectable);

  /* GtkWidget */
  g_signal_connect(G_OBJECT (machine), "button_press_event\0",
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
  //TODO:JK: implement me
}

gchar*
ags_machine_get_name(AgsPlugin *plugin)
{
  return(AGS_MACHINE(plugin)->name);
}

void
ags_machine_set_name(AgsPlugin *plugin, gchar *name)
{
  //TODO:JK: implement me
}

gchar*
ags_machine_get_version(AgsPlugin *plugin)
{
  return(AGS_MACHINE(plugin)->version);
}

void
ags_machine_set_version(AgsPlugin *plugin, gchar *version)
{
  //TODO:JK: implement me
}

gchar*
ags_machine_get_build_id(AgsPlugin *plugin)
{
  return(AGS_MACHINE(plugin)->build_id);
}

void
ags_machine_set_build_id(AgsPlugin *plugin, gchar *build_id)
{
  //TODO:JK: implement me
}

gchar*
ags_machine_get_xml_type(AgsPlugin *plugin)
{
  //TODO:JK: implement me

  return(NULL);
}

void
ags_machine_set_xml_type(AgsPlugin *plugin, gchar *xml_type)
{
  //TODO:JK: implement me
}

GList*
ags_machine_get_ports(AgsPlugin *plugin)
{
  //TODO:JK: implement me

  return(NULL);
}

void
ags_machine_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  //TODO:JK: implement me
}

xmlNode*
ags_machine_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  //TODO:JK: implement me

  return(NULL);
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
		     0, "NULL\0",
		     1, NULL,
		     -1);

  if(GTK_WIDGET(machine)->parent != NULL){
    list = gtk_container_get_children(GTK_CONTAINER(GTK_WIDGET(machine)->parent));

    while(list != NULL){
      if(list->data != machine){
	gtk_list_store_append(model, &iter);
	gtk_list_store_set(model, &iter,
			   0, g_strdup_printf("%s: %s\0", 
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

  file_chooser = (GtkFileChooserDialog *) gtk_file_chooser_dialog_new(g_strdup("open audio files\0"),
								      (GtkWindow *) gtk_widget_get_toplevel((GtkWidget *) machine),
								      GTK_FILE_CHOOSER_ACTION_OPEN,
								      GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
								      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
								      NULL);
  gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(file_chooser), TRUE);

  check_button = (GtkCheckButton *) gtk_check_button_new_with_label(g_strdup("open in new channel\0"));
  gtk_toggle_button_set_active((GtkToggleButton *) check_button, TRUE);
  gtk_box_pack_start((GtkBox *) GTK_DIALOG(file_chooser)->vbox, (GtkWidget *) check_button, FALSE, FALSE, 0);
  g_object_set_data((GObject *) file_chooser, "create\0", (gpointer) check_button);

  check_button = (GtkCheckButton *) gtk_check_button_new_with_label(g_strdup("overwrite existing links\0"));
  gtk_toggle_button_set_active((GtkToggleButton *) check_button, TRUE);
  gtk_box_pack_start((GtkBox *) GTK_DIALOG(file_chooser)->vbox, (GtkWidget *) check_button, FALSE, FALSE, 0);
  g_object_set_data((GObject *) file_chooser, "overwrite\0", (gpointer) check_button);

  return(file_chooser);
}

void
ags_machine_open_files(AgsMachine *machine,
		       GSList *filenames,
		       gboolean overwrite_channels,
		       gboolean create_channels)
{
  AgsOpenFile *open_file;

  open_file = ags_open_file_new(machine->audio,
				filenames,
				overwrite_channels,
				create_channels);

  ags_task_thread_append_task(AGS_TASK_THREAD(AGS_AUDIO_LOOP(AGS_MAIN(AGS_DEVOUT(machine->audio->devout)->ags_main)->main_loop)->task_thread),
			      AGS_TASK(open_file));

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
			"devout\0", &value);
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

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("move up\0"));
  gtk_menu_shell_append((GtkMenuShell *) popup, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("move down\0"));
  gtk_menu_shell_append((GtkMenuShell *) popup, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("hide\0"));
  gtk_menu_shell_append((GtkMenuShell *) popup, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("show\0"));
  gtk_menu_shell_append((GtkMenuShell *) popup, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("destroy\0"));
  gtk_menu_shell_append((GtkMenuShell *) popup, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("rename\0"));
  gtk_menu_shell_append((GtkMenuShell *) popup, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("properties\0"));
  gtk_menu_shell_append((GtkMenuShell *) popup, (GtkWidget*) item);

  list = gtk_container_get_children((GtkContainer *) popup);

  g_signal_connect((GObject*) list->data, "activate\0",
		   G_CALLBACK(ags_machine_popup_move_up_activate_callback), (gpointer) machine);
  list = list->next;

  g_signal_connect((GObject*) list->data, "activate\0",
		   G_CALLBACK(ags_machine_popup_move_down_activate_callback), (gpointer) machine);
  list = list->next;

  g_signal_connect((GObject*) list->data, "activate\0",
		   G_CALLBACK(ags_machine_popup_hide_activate_callback), (gpointer) machine);
  list = list->next;

  g_signal_connect((GObject*) list->data, "activate\0",
		   G_CALLBACK(ags_machine_popup_show_activate_callback), (gpointer) machine);
  list = list->next;

  g_signal_connect((GObject*) list->data, "activate\0",
		   G_CALLBACK(ags_machine_popup_destroy_activate_callback), (gpointer) machine);
  list = list->next;

  g_signal_connect((GObject*) list->data, "activate\0",
		   G_CALLBACK(ags_machine_popup_rename_activate_callback), (gpointer) machine);
  list = list->next;

  g_signal_connect((GObject*) list->data, "activate\0",
		   G_CALLBACK(ags_machine_popup_properties_activate_callback), (gpointer) machine);

  gtk_widget_show_all((GtkWidget*) popup);

  return(popup);
}

#include <ags/X/editor/ags_notebook_callbacks.h>

#include <ags/X/editor/ags_ruler.h>

gboolean
ags_notebook_destroy_callback(GtkObject *object, AgsNotebook *notebook)
{
  return(TRUE);
}

void
ags_notebook_show_callback(GtkWidget *widget, AgsNotebook *notebook)
{
}


void
ags_notebook_change_machine_callback(AgsEditor *editor, AgsMachine *machine,
				     AgsNotebook *notebook)
{
  AgsMachine *machine_old;
  guint i, stop;
  void ags_notebook_change_machine_shrink(){
    GtkWidget *widget;

    for(; i < stop; i++)
      gtk_notebook_remove_page((GtkNotebook *) notebook, 0);
  }
  void ags_notebook_change_machine_grow(){
    AgsRuler *ruler;

    for(; i < stop; i++){
      ruler = ags_ruler_new();
      gtk_notebook_append_page((GtkNotebook *) notebook, (GtkWidget *) ruler, gtk_label_new(g_strdup_printf("channel %d\0", i)));

      if(GTK_WIDGET_VISIBLE(GTK_WIDGET(notebook)))
	ags_ruler_connect(ruler);
    }
  }

  if(machine == NULL){
    i = 0;
    stop = gtk_notebook_get_n_pages((GtkNotebook *) notebook);
    ags_notebook_change_machine_shrink();
  }else{
    machine_old = AGS_MACHINE(g_object_get_data((GObject *) editor->selected, g_type_name(AGS_TYPE_MACHINE)));

    if(machine_old == NULL){
      i = 0;
      stop = machine->audio->audio_channels;
      ags_notebook_change_machine_grow();
    }else{
      if(machine->audio->audio_channels > machine_old->audio->audio_channels){
	i = machine_old->audio->audio_channels;
	stop = machine->audio->audio_channels;
	ags_notebook_change_machine_grow();
      }else if(machine->audio->audio_channels < machine_old->audio->audio_channels){
	i = machine->audio->audio_channels;
	stop = machine_old->audio->audio_channels;
	ags_notebook_change_machine_shrink();
      }
    }
  }

  g_object_set_data((GObject *) editor->selected, (char *) g_type_name(AGS_TYPE_MACHINE), machine);
  gtk_widget_show_all((GtkWidget *) notebook);
}

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
      ags_notebook_remove_tab((GtkNotebook *) notebook,
			      0);
  }
  void ags_notebook_change_machine_grow(){
    AgsRuler *ruler;

    for(; i < stop; i++){
      ruler = ags_ruler_new();
      ags_notebook_add_tab((GtkNotebook *) notebook);

      if(GTK_WIDGET_VISIBLE(GTK_WIDGET(notebook)))
	ags_ruler_connect(ruler);
    }
  }

  if(machine == NULL){
    i = 0;
    stop = 1;
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

/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/app/ags_quit_dialog_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>

#include <ags/app/file/ags_simple_file.h>

#include <ags/app/machine/ags_audiorec.h>

#define _GNU_SOURCE
#include <locale.h>

#include <ags/i18n.h>

static GMutex locale_mutex;

#if defined(AGS_OSXAPI) || defined(AGS_W32API)
static char *locale_env;
#else
static locale_t c_locale;
#endif

static gboolean locale_initialized = FALSE;

void
ags_quit_dialog_response_callback(GtkDialog *dialog,
				  gint response_id,
				  gpointer user_data)
{
  AgsQuitDialog *quit_dialog;

  gchar *fast_export_question = i18n("Do you want to fast export before quit?");

  quit_dialog = AGS_QUIT_DIALOG(dialog);

#if AGS_DEBUG
  g_message("question %d; nth_machine %d", quit_dialog->current_question,  quit_dialog->nth_wave_export_machine);
#endif
  
  switch(response_id){
  case GTK_RESPONSE_YES:
  {
    AgsWindow *window;
    
    AgsApplicationContext *application_context;

    gboolean accept_all;

    application_context = ags_application_context_get_instance();

    window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

    accept_all = gtk_toggle_button_get_active((GtkToggleButton *) quit_dialog->accept_all);

    if(quit_dialog->current_question == AGS_QUIT_DIALOG_QUESTION_SAVE_FILE){      
      AgsSimpleFile *simple_file;

      GList *start_list, *list;

#if defined(AGS_OSXAPI) || defined(AGS_W32API)
#else
      locale_t current;
#endif

      GError *error;

      g_mutex_lock(&locale_mutex);

      if(!locale_initialized){
#if defined(AGS_OSXAPI) || defined(AGS_W32API)
	locale_env = getenv("LC_ALL");
#else
	c_locale = newlocale(LC_ALL_MASK, "C", (locale_t) 0);
#endif
    
	locale_initialized = TRUE;
      }

      g_mutex_unlock(&locale_mutex);

#if defined(AGS_OSXAPI) || defined(AGS_W32API)
      setlocale(LC_ALL, "C");
#else
      current = uselocale(c_locale);
#endif

      simple_file = (AgsSimpleFile *) g_object_new(AGS_TYPE_SIMPLE_FILE,
						   "filename", window->name,
						   NULL);
      
      error = NULL;
      ags_simple_file_rw_open(simple_file,
			      TRUE,
			      &error);

      if(error != NULL){
	g_message("%s", error->message);

	g_error_free(error);
      }
    
      ags_simple_file_write(simple_file);
      ags_simple_file_close(simple_file);

      g_object_unref(G_OBJECT(simple_file));

#if defined(AGS_OSXAPI) || defined(AGS_W32API)
      setlocale(LC_ALL, locale_env);
#else
      uselocale(current);
#endif

      list = 
	start_list = gtk_container_get_children((GtkContainer *) window->machines);
      
      while(list != NULL){
	if(AGS_IS_AUDIOREC(list->data)){
	  quit_dialog->wave_export_machine = g_list_prepend(quit_dialog->wave_export_machine,
							    list->data);
	}

	list = list->next;
      }

      quit_dialog->wave_export_machine = g_list_reverse(quit_dialog->wave_export_machine);

      g_list_free(start_list);
    }

    if(accept_all ||
       quit_dialog->current_question == AGS_QUIT_DIALOG_QUESTION_EXPORT_WAVE){
      AgsMachine *machine;

      GList *wave_export_machine;
	
      wave_export_machine = g_list_nth(quit_dialog->wave_export_machine,
				       quit_dialog->nth_wave_export_machine);


      if(accept_all){
	while(wave_export_machine != NULL){
	  machine = wave_export_machine->data;

	  if(AGS_IS_AUDIOREC(machine)){
	    ags_quit_dialog_fast_export(quit_dialog,
					machine);
	  }
  
	  wave_export_machine = wave_export_machine->next;
	}
      }else{
	if(wave_export_machine != NULL){
	  machine = wave_export_machine->data;

	  if(AGS_IS_AUDIOREC(machine)){
	    ags_quit_dialog_fast_export(quit_dialog,
					machine);
	  }
	}
      }
      
      quit_dialog->nth_wave_export_machine += 1;

      if(quit_dialog->nth_wave_export_machine < g_list_length(quit_dialog->wave_export_machine)){
	gchar *str;
	gchar *filename;
	gchar *machine_name;

	machine = g_list_nth_data(quit_dialog->wave_export_machine,
				  quit_dialog->nth_wave_export_machine);
      
	filename = NULL;

	if(AGS_IS_AUDIOREC(machine)){
	  filename = gtk_entry_get_text(AGS_AUDIOREC(machine)->filename);
	}

	machine_name = NULL;

	if(machine != NULL){
	  machine_name = machine->machine_name;
	}
	
	str = g_strdup_printf("%s\n\n%s:%s\nfilename: %s",
			      fast_export_question,
			      G_OBJECT_TYPE_NAME(machine),
			      machine_name,
			      filename);

	gtk_label_set_text(quit_dialog->question,
			   str);

	g_free(str);
      }
    }

    if(quit_dialog->current_question == AGS_QUIT_DIALOG_QUESTION_SAVE_FILE){
      if(quit_dialog->wave_export_machine != NULL){
	gchar *str;
	gchar *filename;
	
	quit_dialog->nth_wave_export_machine = 0;

	quit_dialog->current_question = AGS_QUIT_DIALOG_QUESTION_EXPORT_WAVE;

	filename = NULL;

	if(AGS_IS_AUDIOREC(quit_dialog->wave_export_machine->data)){
	  filename = gtk_entry_get_text(AGS_AUDIOREC(quit_dialog->wave_export_machine->data)->filename);
	}

	str = g_strdup_printf("%s\n\nmachine: %s:%s\nfilename: %s",
			      fast_export_question,
			      G_OBJECT_TYPE_NAME(quit_dialog->wave_export_machine->data),
			      AGS_MACHINE(quit_dialog->wave_export_machine->data)->machine_name,
			      filename);
	
	gtk_label_set_text(quit_dialog->question,
			   str);
	
	g_free(str);
      }
    }
    
    if(accept_all ||
       quit_dialog->nth_wave_export_machine >= g_list_length(quit_dialog->wave_export_machine)){
      ags_application_context_quit(AGS_APPLICATION_CONTEXT(application_context));
    }
  }
  break;
  case GTK_RESPONSE_NO:
  { 
    AgsWindow *window;
    
    AgsApplicationContext *application_context;

    application_context = ags_application_context_get_instance();

    window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

    if(quit_dialog->current_question == AGS_QUIT_DIALOG_QUESTION_SAVE_FILE){
      GList *start_list, *list;

      list = 
	start_list = gtk_container_get_children((GtkContainer *) window->machines);

      while(list != NULL){
	if(AGS_IS_AUDIOREC(list->data)){
	  quit_dialog->wave_export_machine = g_list_prepend(quit_dialog->wave_export_machine,
							    list->data);
	}

	list = list->next;
      }

      quit_dialog->wave_export_machine = g_list_reverse(quit_dialog->wave_export_machine);

      g_list_free(start_list);
    }
    
    if(quit_dialog->current_question == AGS_QUIT_DIALOG_QUESTION_EXPORT_WAVE){
      quit_dialog->nth_wave_export_machine += 1;

      if(quit_dialog->nth_wave_export_machine < g_list_length(quit_dialog->wave_export_machine)){
	AgsMachine *machine;
      
	gchar *str;
	gchar *filename;
	gchar *machine_name;
      
	machine = g_list_nth_data(quit_dialog->wave_export_machine,
				  quit_dialog->nth_wave_export_machine);
      
	filename = NULL;

	if(AGS_IS_AUDIOREC(machine)){
	  filename = gtk_entry_get_text(AGS_AUDIOREC(machine)->filename);
	}

	machine_name = NULL;

	if(machine != NULL){
	  machine_name = machine->machine_name;
	}
      
	str = g_strdup_printf("%s\n\n%s:%s\nfilename: %s",
			      fast_export_question,
			      G_OBJECT_TYPE_NAME(machine),
			      machine_name,
			      filename);

	gtk_label_set_text(quit_dialog->question,
			   str);

	g_free(str);
      }
    }

    if(quit_dialog->current_question == AGS_QUIT_DIALOG_QUESTION_SAVE_FILE){
      if(quit_dialog->wave_export_machine != NULL){
	gchar *str;
	gchar *filename;

	quit_dialog->nth_wave_export_machine = 0;

	quit_dialog->current_question = AGS_QUIT_DIALOG_QUESTION_EXPORT_WAVE;

	filename = NULL;

	if(AGS_IS_AUDIOREC(quit_dialog->wave_export_machine->data)){
	  filename = gtk_entry_get_text(AGS_AUDIOREC(quit_dialog->wave_export_machine->data)->filename);
	}
	
	str = g_strdup_printf("%s\n\n%s:%s\nfilename: %s",
			      fast_export_question,
			      G_OBJECT_TYPE_NAME(quit_dialog->wave_export_machine->data),
			      AGS_MACHINE(quit_dialog->wave_export_machine->data)->machine_name,
			      filename);

	gtk_label_set_text(quit_dialog->question,
			   str);

	g_free(str);
      }
    }

    if(quit_dialog->nth_wave_export_machine >= g_list_length(quit_dialog->wave_export_machine)){
      ags_application_context_quit(AGS_APPLICATION_CONTEXT(application_context));
    }    
  }
  break;
  case GTK_RESPONSE_CANCEL:
  {
    gtk_widget_destroy(GTK_WIDGET(dialog));
  }
  }
}

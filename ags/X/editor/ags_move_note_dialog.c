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

#include <ags/X/editor/ags_move_note_dialog.h>
#include <ags/X/editor/ags_move_note_dialog_callbacks.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>

#include <ags/thread/ags_concurrency_provider.h>
#include <ags/thread/ags_mutex_manager.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_notation.h>
#include <ags/audio/ags_note.h>

#include <ags/audio/task/ags_move_note.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_editor.h>
#include <ags/X/ags_machine.h>

#include <ags/X/thread/ags_gui_thread.h>

#include <ags/i18n.h>

void ags_move_note_dialog_class_init(AgsMoveNoteDialogClass *move_note_dialog);
void ags_move_note_dialog_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_move_note_dialog_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_move_note_dialog_init(AgsMoveNoteDialog *move_note_dialog);
void ags_move_note_dialog_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_move_note_dialog_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
void ags_move_note_dialog_finalize(GObject *gobject);
void ags_move_note_dialog_connect(AgsConnectable *connectable);
void ags_move_note_dialog_disconnect(AgsConnectable *connectable);
void ags_move_note_dialog_set_update(AgsApplicable *applicable, gboolean update);
void ags_move_note_dialog_apply(AgsApplicable *applicable);
void ags_move_note_dialog_reset(AgsApplicable *applicable);
gboolean ags_move_note_dialog_delete_event(GtkWidget *widget, GdkEventAny *event);

/**
 * SECTION:ags_move_note_dialog
 * @short_description: move tool
 * @title: AgsMoveNoteDialog
 * @section_id:
 * @include: ags/X/editor/ags_move_note_dialog.h
 *
 * The #AgsMoveNoteDialog lets you move notes.
 */

enum{
  PROP_0,
  PROP_APPLICATION_CONTEXT,
  PROP_MAIN_WINDOW,
};

static gpointer ags_move_note_dialog_parent_class = NULL;

GType
ags_move_note_dialog_get_type(void)
{
  static GType ags_type_move_note_dialog = 0;

  if (!ags_type_move_note_dialog){
    static const GTypeInfo ags_move_note_dialog_info = {
      sizeof (AgsMoveNoteDialogClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_move_note_dialog_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMoveNoteDialog),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_move_note_dialog_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_move_note_dialog_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_move_note_dialog_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_move_note_dialog = g_type_register_static(GTK_TYPE_DIALOG,
						       "AgsMoveNoteDialog", &ags_move_note_dialog_info,
						       0);
    
    g_type_add_interface_static(ags_type_move_note_dialog,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_move_note_dialog,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }

  return (ags_type_move_note_dialog);
}

void
ags_move_note_dialog_class_init(AgsMoveNoteDialogClass *move_note_dialog)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  GParamSpec *param_spec;

  ags_move_note_dialog_parent_class = g_type_class_peek_parent(move_note_dialog);

  /* GObjectClass */
  gobject = (GObjectClass *) move_note_dialog;

  gobject->set_property = ags_move_note_dialog_set_property;
  gobject->get_property = ags_move_note_dialog_get_property;

  gobject->finalize = ags_move_note_dialog_finalize;

  /* properties */
  /**
   * AgsMoveNoteDialog:application-context:
   *
   * The assigned #AgsApplicationContext to give control of application.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("application-context",
				   i18n_pspec("assigned application context"),
				   i18n_pspec("The AgsApplicationContext it is assigned with"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APPLICATION_CONTEXT,
				  param_spec);

  /**
   * AgsMoveNoteDialog:main-window:
   *
   * The assigned #AgsWindow.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("main-window",
				   i18n_pspec("assigned main window"),
				   i18n_pspec("The assigned main window"),
				   AGS_TYPE_WINDOW,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MAIN_WINDOW,
				  param_spec);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) move_note_dialog;

  widget->delete_event = ags_move_note_dialog_delete_event;
}

void
ags_move_note_dialog_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_move_note_dialog_connect;
  connectable->disconnect = ags_move_note_dialog_disconnect;
}

void
ags_move_note_dialog_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_move_note_dialog_set_update;
  applicable->apply = ags_move_note_dialog_apply;
  applicable->reset = ags_move_note_dialog_reset;
}

void
ags_move_note_dialog_init(AgsMoveNoteDialog *move_note_dialog)
{
  GtkVBox *vbox;
  GtkHBox *hbox;
  GtkLabel *label;

  move_note_dialog->flags = 0;

  g_object_set(move_note_dialog,
	       "title", i18n("move notes"),
	       NULL);
  
  vbox = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) move_note_dialog->dialog.vbox,
		     GTK_WIDGET(vbox),
		     FALSE, FALSE,
		     0);  

  /* radio - relative */
  move_note_dialog->relative = (GtkRadioButton *) gtk_radio_button_new_with_label(NULL,
										  i18n("relative"));
  gtk_box_pack_start((GtkBox *) vbox,
		     (GtkWidget *) move_note_dialog->relative,
		     FALSE, FALSE,
		     0);

  /* radio - absolute */
  move_note_dialog->absolute = (GtkRadioButton *) gtk_radio_button_new_with_label(gtk_radio_button_get_group(move_note_dialog->relative),
										  i18n("absolute"));
  gtk_box_pack_start((GtkBox *) vbox,
		     (GtkWidget *) move_note_dialog->absolute,
		     FALSE, FALSE,
		     0);

  /* move x - hbox */
  hbox = (GtkVBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(hbox),
		     FALSE, FALSE,
		     0);

  /* move x - label */
  label = (GtkLabel *) gtk_label_new(i18n("move x"));
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  /* move x - spin button */
  move_note_dialog->move_x = (GtkSpinButton *) gtk_spin_button_new_with_range(-1.0 * AGS_MOVE_NOTE_DIALOG_MAX_X,
									      AGS_MOVE_NOTE_DIALOG_MAX_X,
									      1.0);
  gtk_spin_button_set_value(move_note_dialog->move_x,
			    0.0);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) move_note_dialog->move_x,
		     FALSE, FALSE,
		     0);

  /* move y - hbox */
  hbox = (GtkVBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(hbox),
		     FALSE, FALSE,
		     0);

  /* move y - label */
  label = (GtkLabel *) gtk_label_new(i18n("move y"));
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  /* move y - spin button */
  move_note_dialog->move_y = (GtkSpinButton *) gtk_spin_button_new_with_range(-1.0 * AGS_MOVE_NOTE_DIALOG_MAX_Y,
									      AGS_MOVE_NOTE_DIALOG_MAX_Y,
									      1.0);
  gtk_spin_button_set_value(move_note_dialog->move_y,
			    0.0);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) move_note_dialog->move_y,
		     FALSE, FALSE,
		     0);

  /* dialog buttons */
  gtk_dialog_add_buttons((GtkDialog *) move_note_dialog,
			 GTK_STOCK_APPLY, GTK_RESPONSE_APPLY,
			 GTK_STOCK_OK, GTK_RESPONSE_OK,
			 GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			 NULL);
}

void
ags_move_note_dialog_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsMoveNoteDialog *move_note_dialog;

  move_note_dialog = AGS_MOVE_NOTE_DIALOG(gobject);

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;

      application_context = (AgsApplicationContext *) g_value_get_object(value);

      if((AgsApplicationContext *) move_note_dialog->application_context == application_context){
	return;
      }
      
      if(move_note_dialog->application_context != NULL){
	g_object_unref(move_note_dialog->application_context);
      }

      if(application_context != NULL){
	g_object_ref(application_context);
      }

      move_note_dialog->application_context = (GObject *) application_context;
    }
    break;
  case PROP_MAIN_WINDOW:
    {
      AgsWindow *main_window;

      main_window = (AgsWindow *) g_value_get_object(value);

      if((AgsWindow *) move_note_dialog->main_window == main_window){
	return;
      }

      if(move_note_dialog->main_window != NULL){
	g_object_unref(move_note_dialog->main_window);
      }

      if(main_window != NULL){
	g_object_ref(main_window);
      }

      move_note_dialog->main_window = (GObject *) main_window;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_move_note_dialog_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec)
{
  AgsMoveNoteDialog *move_note_dialog;

  move_note_dialog = AGS_MOVE_NOTE_DIALOG(gobject);

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      g_value_set_object(value, move_note_dialog->application_context);
    }
    break;
  case PROP_MAIN_WINDOW:
    {
      g_value_set_object(value, move_note_dialog->main_window);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_move_note_dialog_connect(AgsConnectable *connectable)
{
  AgsMoveNoteDialog *move_note_dialog;

  move_note_dialog = AGS_MOVE_NOTE_DIALOG(connectable);

  if((AGS_MOVE_NOTE_DIALOG_CONNECTED & (move_note_dialog->flags)) != 0){
    return;
  }

  move_note_dialog->flags |= AGS_MOVE_NOTE_DIALOG_CONNECTED;

  g_signal_connect(move_note_dialog, "response",
		   G_CALLBACK(ags_move_note_dialog_response_callback), move_note_dialog);

  g_signal_connect_after(move_note_dialog->relative, "clicked",
			 G_CALLBACK(ags_move_note_dialog_relative_callback), move_note_dialog);

  g_signal_connect_after(move_note_dialog->absolute, "clicked",
			 G_CALLBACK(ags_move_note_dialog_absolute_callback), move_note_dialog);
}

void
ags_move_note_dialog_disconnect(AgsConnectable *connectable)
{
  AgsMoveNoteDialog *move_note_dialog;

  move_note_dialog = AGS_MOVE_NOTE_DIALOG(connectable);

  if((AGS_MOVE_NOTE_DIALOG_CONNECTED & (move_note_dialog->flags)) == 0){
    return;
  }

  move_note_dialog->flags &= (~AGS_MOVE_NOTE_DIALOG_CONNECTED);

  g_object_disconnect(G_OBJECT(move_note_dialog),
		      "response",
		      G_CALLBACK(ags_move_note_dialog_response_callback),
		      move_note_dialog,
		      NULL);

  g_object_disconnect(G_OBJECT(move_note_dialog->relative),
		      "clicked",
		      G_CALLBACK(ags_move_note_dialog_relative_callback),
		      move_note_dialog,
		      NULL);

  g_object_disconnect(G_OBJECT(move_note_dialog->absolute),
		      "clicked",
		      G_CALLBACK(ags_move_note_dialog_absolute_callback),
		      move_note_dialog,
		      NULL);
}

void
ags_move_note_dialog_finalize(GObject *gobject)
{
  AgsMoveNoteDialog *move_note_dialog;

  move_note_dialog = (AgsMoveNoteDialog *) gobject;

  if(move_note_dialog->application_context != NULL){
    g_object_unref(move_note_dialog->application_context);
  }
  
  G_OBJECT_CLASS(ags_move_note_dialog_parent_class)->finalize(gobject);
}

void
ags_move_note_dialog_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_move_note_dialog_apply(AgsApplicable *applicable)
{
  AgsMoveNoteDialog *move_note_dialog;

  AgsWindow *window;
  AgsEditor *editor;
  AgsMachine *machine;

  AgsMoveNote *move_note;
  
  AgsAudio *audio;

  AgsMutexManager *mutex_manager;
  AgsGuiThread *gui_thread;
  
  AgsApplicationContext *application_context;

  GList *notation;
  GList *selection;
  GList *task;

  guint first_x;
  guint first_y;
  guint move_x;
  guint move_y;
  
  gboolean relative;
  gboolean absolute;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  
  move_note_dialog = AGS_MOVE_NOTE_DIALOG(applicable);

  window = move_note_dialog->main_window;
  editor = window->editor;

  machine = editor->selected_machine;

  if(machine == NULL){
    return;
  }

  audio = machine->audio;

  /* get some values */
  move_x = gtk_spin_button_get_value_as_int(move_note_dialog->move_x);
  move_y = gtk_spin_button_get_value_as_int(move_note_dialog->move_y);

  relative = gtk_toggle_button_get_active(move_note_dialog->relative);
  absolute = gtk_toggle_button_get_active(move_note_dialog->absolute);
  
  /* application context and mutex manager */
  application_context = window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get task thread */
  pthread_mutex_lock(application_mutex);

  gui_thread = (AgsGuiThread *) ags_thread_find_type(application_context->main_loop,
						     AGS_TYPE_GUI_THREAD);
  
  pthread_mutex_unlock(application_mutex);

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* get position and move note */
  pthread_mutex_lock(audio_mutex);

  first_x = 0;
  first_y = 0;
  
  if(absolute){
    notation = audio->notation;

    first_x = G_MAXUINT;
    first_y = G_MAXUINT;
  
    while(notation != NULL){
      selection = AGS_NOTATION(notation->data);

      while(selection != NULL){
	if(AGS_NOTE(selection->data)->x[0] < first_x){
	  first_x = AGS_NOTE(selection->data)->x[0];
	}

	if(AGS_NOTE(selection->data)->y < first_y){
	  first_y = AGS_NOTE(selection->data)->y;
	}

	selection = selection->next;
      }

      notation = notation->next;
    }
  }

  /* create move note task */
  notation = audio->notation;

  task = NULL;
  
  while(notation != NULL){
    selection = AGS_NOTATION(notation->data)->selection;
    
    move_note = ags_move_note_new(notation->data,
				  selection,
				  first_x, first_y,
				  move_x, move_y,
				  relative, absolute);
    task = g_list_prepend(task,
			  move_note);
    
    notation = notation->next;
  }

  pthread_mutex_unlock(audio_mutex);

  /* append tasks */
  ags_gui_thread_schedule_task_list(gui_thread,
				    task);
}

void
ags_move_note_dialog_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

gboolean
ags_move_note_dialog_delete_event(GtkWidget *widget, GdkEventAny *event)
{
  gtk_widget_hide(widget);

  //  GTK_WIDGET_CLASS(ags_move_note_dialog_parent_class)->delete_event(widget, event);

  return(TRUE);
}

/**
 * ags_move_note_dialog_new:
 * @main_window: the #AgsWindow
 *
 * Create a new #AgsMoveNoteDialog.
 *
 * Returns: a new #AgsMoveNoteDialog
 *
 * Since: 1.0.0
 */
AgsMoveNoteDialog*
ags_move_note_dialog_new(GtkWidget *main_window)
{
  AgsMoveNoteDialog *move_note_dialog;

  move_note_dialog = (AgsMoveNoteDialog *) g_object_new(AGS_TYPE_MOVE_NOTE_DIALOG,
							"main-window", main_window,
							NULL);

  return(move_note_dialog);
}

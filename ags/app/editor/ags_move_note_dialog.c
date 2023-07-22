/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#include <ags/app/editor/ags_move_note_dialog.h>
#include <ags/app/editor/ags_move_note_dialog_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_machine.h>

#include <ags/i18n.h>

void ags_move_note_dialog_class_init(AgsMoveNoteDialogClass *move_note_dialog);
void ags_move_note_dialog_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_move_note_dialog_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_move_note_dialog_init(AgsMoveNoteDialog *move_note_dialog);
void ags_move_note_dialog_finalize(GObject *gobject);

void ags_move_note_dialog_connect(AgsConnectable *connectable);
void ags_move_note_dialog_disconnect(AgsConnectable *connectable);

void ags_move_note_dialog_set_update(AgsApplicable *applicable, gboolean update);
void ags_move_note_dialog_apply(AgsApplicable *applicable);
void ags_move_note_dialog_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_move_note_dialog
 * @short_description: move tool
 * @title: AgsMoveNoteDialog
 * @section_id:
 * @include: ags/app/editor/ags_move_note_dialog.h
 *
 * The #AgsMoveNoteDialog lets you move notes.
 */

static gpointer ags_move_note_dialog_parent_class = NULL;

GType
ags_move_note_dialog_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_move_note_dialog = 0;

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

    g_once_init_leave(&g_define_type_id__volatile, ags_type_move_note_dialog);
  }

  return g_define_type_id__volatile;
}

void
ags_move_note_dialog_class_init(AgsMoveNoteDialogClass *move_note_dialog)
{
  GObjectClass *gobject;

  ags_move_note_dialog_parent_class = g_type_class_peek_parent(move_note_dialog);

  /* GObjectClass */
  gobject = (GObjectClass *) move_note_dialog;

  gobject->finalize = ags_move_note_dialog_finalize;
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
  GtkBox *vbox;
  GtkBox *hbox;
  GtkLabel *label;

  move_note_dialog->connectable_flags = 0;

  g_object_set(move_note_dialog,
	       "title", i18n("move notes"),
	       NULL);

  gtk_window_set_hide_on_close((GtkWindow *) move_note_dialog,
			       TRUE);
  
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				0);
  gtk_box_append((GtkBox *) gtk_dialog_get_content_area((GtkDialog *) move_note_dialog),
		 (GtkWidget *) vbox);  

  /* radio - relative */
  move_note_dialog->relative = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("relative"));
  gtk_box_append(vbox,
		 (GtkWidget *) move_note_dialog->relative);

  /* radio - absolute */
  move_note_dialog->absolute = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("absolute"));
  gtk_check_button_set_group(move_note_dialog->absolute,
			     move_note_dialog->relative);
  gtk_box_append(vbox,
		 (GtkWidget *) move_note_dialog->absolute);

  /* move x - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);
  gtk_box_append(vbox,
		 GTK_WIDGET(hbox));

  /* move x - label */
  label = (GtkLabel *) gtk_label_new(i18n("move x"));
  gtk_box_append(hbox,
		 GTK_WIDGET(label));

  /* move x - spin button */
  move_note_dialog->move_x = (GtkSpinButton *) gtk_spin_button_new_with_range(-1.0 * AGS_MOVE_NOTE_DIALOG_MAX_X,
									      AGS_MOVE_NOTE_DIALOG_MAX_X,
									      1.0);
  gtk_spin_button_set_value(move_note_dialog->move_x,
			    0.0);
  gtk_box_append(hbox,
		 (GtkWidget *) move_note_dialog->move_x);

  /* move y - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);
  gtk_box_append(vbox,
		 GTK_WIDGET(hbox));

  /* move y - label */
  label = (GtkLabel *) gtk_label_new(i18n("move y"));
  gtk_box_append(hbox,
		 GTK_WIDGET(label));

  /* move y - spin button */
  move_note_dialog->move_y = (GtkSpinButton *) gtk_spin_button_new_with_range(-1.0 * AGS_MOVE_NOTE_DIALOG_MAX_Y,
									      AGS_MOVE_NOTE_DIALOG_MAX_Y,
									      1.0);
  gtk_spin_button_set_value(move_note_dialog->move_y,
			    0.0);
  gtk_box_append(hbox,
		 (GtkWidget *) move_note_dialog->move_y);

  /* dialog buttons */
  gtk_dialog_add_buttons((GtkDialog *) move_note_dialog,
			 i18n("_Apply"), GTK_RESPONSE_APPLY,
			 i18n("_OK"), GTK_RESPONSE_OK,
			 i18n("_Cancel"), GTK_RESPONSE_CANCEL,
			 NULL);
}

void
ags_move_note_dialog_connect(AgsConnectable *connectable)
{
  AgsMoveNoteDialog *move_note_dialog;

  move_note_dialog = AGS_MOVE_NOTE_DIALOG(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (move_note_dialog->connectable_flags)) != 0){
    return;
  }

  move_note_dialog->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  g_signal_connect(move_note_dialog, "response",
		   G_CALLBACK(ags_move_note_dialog_response_callback), move_note_dialog);

  g_signal_connect_after(move_note_dialog->relative, "toggled",
			 G_CALLBACK(ags_move_note_dialog_relative_callback), move_note_dialog);

  g_signal_connect_after(move_note_dialog->absolute, "toggled",
			 G_CALLBACK(ags_move_note_dialog_absolute_callback), move_note_dialog);
}

void
ags_move_note_dialog_disconnect(AgsConnectable *connectable)
{
  AgsMoveNoteDialog *move_note_dialog;

  move_note_dialog = AGS_MOVE_NOTE_DIALOG(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (move_note_dialog->connectable_flags)) == 0){
    return;
  }

  move_note_dialog->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  g_object_disconnect(G_OBJECT(move_note_dialog),
		      "any_signal::response",
		      G_CALLBACK(ags_move_note_dialog_response_callback),
		      move_note_dialog,
		      NULL);

  g_object_disconnect(G_OBJECT(move_note_dialog->relative),
		      "any_signal::toggled",
		      G_CALLBACK(ags_move_note_dialog_relative_callback),
		      move_note_dialog,
		      NULL);

  g_object_disconnect(G_OBJECT(move_note_dialog->absolute),
		      "any_signal::toggled",
		      G_CALLBACK(ags_move_note_dialog_absolute_callback),
		      move_note_dialog,
		      NULL);
}

void
ags_move_note_dialog_finalize(GObject *gobject)
{
  AgsMoveNoteDialog *move_note_dialog;

  move_note_dialog = (AgsMoveNoteDialog *) gobject;
  
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
  AgsMachine *machine;
  AgsCompositeEditor *composite_editor;

  AgsMoveNote *move_note;
  
  AgsAudio *audio;
  
  AgsApplicationContext *application_context;

  GList *start_notation, *notation;
  GList *task;

  guint first_x;
  guint first_y;
  guint move_x;
  guint move_y;
  
  gboolean relative;
  gboolean absolute;
  
  move_note_dialog = AGS_MOVE_NOTE_DIALOG(applicable);

  /* application context */
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
    
  composite_editor = window->composite_editor;

  machine = composite_editor->selected_machine;
  
  if(machine == NULL){
    return;
  }

  audio = machine->audio;

  /* get some values */
  move_x = gtk_spin_button_get_value_as_int(move_note_dialog->move_x);
  move_y = gtk_spin_button_get_value_as_int(move_note_dialog->move_y);

  relative = gtk_check_button_get_active(move_note_dialog->relative);
  absolute = gtk_check_button_get_active(move_note_dialog->absolute);
  
  /* get position and move note */
  g_object_get(audio,
	       "notation", &start_notation,
	       NULL);

  first_x = 0;
  first_y = 0;
  
  if(absolute){
    notation = start_notation;

    first_x = G_MAXUINT;
    first_y = G_MAXUINT;
  
    while(notation != NULL){
      GList *start_selection, *selection;
      
      GRecMutex *notation_mutex;

      notation_mutex = AGS_NOTATION_GET_OBJ_MUTEX(notation->data);

      /* selection */
      g_rec_mutex_lock(notation_mutex);

      selection =
	start_selection = g_list_copy_deep(AGS_NOTATION(notation->data)->selection,
					   (GCopyFunc) g_object_ref,
					   NULL);

      g_rec_mutex_unlock(notation_mutex);

      while(selection != NULL){
	guint x0, y;

	g_object_get(selection->data,
		     "x0", &x0,
		     "y", &y,
		     NULL);
	
	if(x0 < first_x){
	  first_x = AGS_NOTE(selection->data)->x[0];
	}

	if(y < first_y){
	  first_y = AGS_NOTE(selection->data)->y;
	}

	selection = selection->next;
      }

      g_list_free_full(start_selection,
		       g_object_unref);

      notation = notation->next;
    }
  }

  /* create move note task */
  notation = start_notation;

  task = NULL;
  
  while(notation != NULL){
    GList *start_selection;
    
    GRecMutex *notation_mutex;

    notation_mutex = AGS_NOTATION_GET_OBJ_MUTEX(notation->data);

    /* selection */
    g_rec_mutex_lock(notation_mutex);

    start_selection = g_list_copy_deep(AGS_NOTATION(notation->data)->selection,
				       (GCopyFunc) g_object_ref,
				       NULL);

    g_rec_mutex_unlock(notation_mutex);

    if(start_selection != NULL){
      move_note = ags_move_note_new(audio,
				    notation->data,
				    start_selection,
				    first_x, first_y,
				    move_x, move_y,
				    relative, absolute);

      task = g_list_prepend(task,
			    move_note);

      g_list_free_full(start_selection,
		       g_object_unref);
    }
    
    notation = notation->next;
  }

  g_list_free_full(start_notation,
		   g_object_unref);

  /* append tasks */
  ags_ui_provider_schedule_task_all(AGS_UI_PROVIDER(application_context),
				    task);
}

void
ags_move_note_dialog_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

/**
 * ags_move_note_dialog_new:
 *
 * Create a new #AgsMoveNoteDialog.
 *
 * Returns: a new #AgsMoveNoteDialog
 *
 * Since: 3.0.0
 */
AgsMoveNoteDialog*
ags_move_note_dialog_new()
{
  AgsMoveNoteDialog *move_note_dialog;

  move_note_dialog = (AgsMoveNoteDialog *) g_object_new(AGS_TYPE_MOVE_NOTE_DIALOG,
							NULL);

  return(move_note_dialog);
}

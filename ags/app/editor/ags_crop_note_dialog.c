/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/app/editor/ags_crop_note_dialog.h>
#include <ags/app/editor/ags_crop_note_dialog_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_machine.h>

#include <ags/i18n.h>

void ags_crop_note_dialog_class_init(AgsCropNoteDialogClass *crop_note_dialog);
void ags_crop_note_dialog_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_crop_note_dialog_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_crop_note_dialog_init(AgsCropNoteDialog *crop_note_dialog);
void ags_crop_note_dialog_finalize(GObject *gobject);

gboolean ags_crop_note_dialog_is_connected(AgsConnectable *connectable);
void ags_crop_note_dialog_connect(AgsConnectable *connectable);
void ags_crop_note_dialog_disconnect(AgsConnectable *connectable);

void ags_crop_note_dialog_set_update(AgsApplicable *applicable, gboolean update);
void ags_crop_note_dialog_apply(AgsApplicable *applicable);
void ags_crop_note_dialog_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_crop_note_dialog
 * @short_description: crop tool
 * @title: AgsCropNoteDialog
 * @section_id:
 * @include: ags/app/editor/ags_crop_note_dialog.h
 *
 * The #AgsCropNoteDialog lets you crop notes.
 */

static gpointer ags_crop_note_dialog_parent_class = NULL;

GType
ags_crop_note_dialog_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_crop_note_dialog = 0;

    static const GTypeInfo ags_crop_note_dialog_info = {
      sizeof (AgsCropNoteDialogClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_crop_note_dialog_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCropNoteDialog),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_crop_note_dialog_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_crop_note_dialog_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_crop_note_dialog_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_crop_note_dialog = g_type_register_static(GTK_TYPE_DIALOG,
						       "AgsCropNoteDialog", &ags_crop_note_dialog_info,
						       0);
    
    g_type_add_interface_static(ags_type_crop_note_dialog,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_crop_note_dialog,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_crop_note_dialog);
  }

  return g_define_type_id__volatile;
}

void
ags_crop_note_dialog_class_init(AgsCropNoteDialogClass *crop_note_dialog)
{
  GObjectClass *gobject;

  ags_crop_note_dialog_parent_class = g_type_class_peek_parent(crop_note_dialog);

  /* GObjectClass */
  gobject = (GObjectClass *) crop_note_dialog;

  gobject->finalize = ags_crop_note_dialog_finalize;
}

void
ags_crop_note_dialog_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = NULL;
  connectable->has_resource = NULL;

  connectable->is_ready = NULL;
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;

  connectable->list_resource = NULL;
  connectable->xml_compose = NULL;
  connectable->xml_parse = NULL;

  connectable->is_connected = ags_crop_note_dialog_is_connected;  
  connectable->connect = ags_crop_note_dialog_connect;
  connectable->disconnect = ags_crop_note_dialog_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_crop_note_dialog_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_crop_note_dialog_set_update;
  applicable->apply = ags_crop_note_dialog_apply;
  applicable->reset = ags_crop_note_dialog_reset;
}

void
ags_crop_note_dialog_init(AgsCropNoteDialog *crop_note_dialog)
{
  GtkBox *vbox;
  GtkBox *hbox;
  GtkLabel *label;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();

  crop_note_dialog->connectable_flags = 0;

  g_object_set(crop_note_dialog,
	       "title", i18n("crop notes"),
	       NULL);

  gtk_window_set_hide_on_close((GtkWindow *) crop_note_dialog,
			       TRUE);
  
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append((GtkBox *) gtk_dialog_get_content_area((GtkDialog *) crop_note_dialog),
		 GTK_WIDGET(vbox));  

  /* absolute */
  crop_note_dialog->absolute = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("absolute"));
  gtk_box_append(vbox,
		 GTK_WIDGET(crop_note_dialog->absolute));  

  /* radio - in place */
  crop_note_dialog->in_place = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("in-place"));
  gtk_box_append(vbox,
		 GTK_WIDGET(crop_note_dialog->in_place));  
  
  /* radio - do resize */
  crop_note_dialog->do_resize = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("do resize"));
  gtk_check_button_set_group(crop_note_dialog->do_resize,
			     crop_note_dialog->in_place);
  gtk_box_append(vbox,
		 GTK_WIDGET(crop_note_dialog->do_resize));  

  /* crop note - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(vbox,
		 GTK_WIDGET(hbox));

  /* crop note - label */
  label = (GtkLabel *) gtk_label_new(i18n("crop note"));
  gtk_box_append(hbox,
		 GTK_WIDGET(label));

  /* crop note - spin button */
  crop_note_dialog->crop_note = (GtkSpinButton *) gtk_spin_button_new_with_range(-1.0 * AGS_CROP_NOTE_DIALOG_MAX_WIDTH,
										 AGS_CROP_NOTE_DIALOG_MAX_WIDTH,
										 1.0);
  gtk_spin_button_set_value(crop_note_dialog->crop_note,
			    0.0);
  gtk_box_append(hbox,
		 GTK_WIDGET(crop_note_dialog->crop_note));

  /* padding note - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(vbox,
		 GTK_WIDGET(hbox));

  /* padding note - label */
  label = (GtkLabel *) gtk_label_new(i18n("padding note"));
  gtk_box_append(hbox,
		 GTK_WIDGET(label));

  /* padding note - spin button */
  crop_note_dialog->padding_note = (GtkSpinButton *) gtk_spin_button_new_with_range(-1.0 * AGS_CROP_NOTE_DIALOG_MAX_WIDTH,
										    AGS_CROP_NOTE_DIALOG_MAX_WIDTH,
										    1.0);
  gtk_spin_button_set_value(crop_note_dialog->padding_note,
			    0.0);
  gtk_box_append(hbox,
		 GTK_WIDGET(crop_note_dialog->padding_note));
  
  /* dialog buttons */
  gtk_dialog_add_buttons((GtkDialog *) crop_note_dialog,
			 i18n("_Apply"), GTK_RESPONSE_APPLY,
			 i18n("_OK"), GTK_RESPONSE_OK,
			 i18n("_Cancel"), GTK_RESPONSE_CANCEL,
			 NULL);
}

void
ags_crop_note_dialog_finalize(GObject *gobject)
{
  AgsCropNoteDialog *crop_note_dialog;

  crop_note_dialog = (AgsCropNoteDialog *) gobject;
  
  G_OBJECT_CLASS(ags_crop_note_dialog_parent_class)->finalize(gobject);
}

gboolean
ags_crop_note_dialog_is_connected(AgsConnectable *connectable)
{
  AgsCropNoteDialog *crop_note_dialog;
  
  gboolean is_connected;
  
  crop_note_dialog = AGS_CROP_NOTE_DIALOG(connectable);

  /* check is connected */
  is_connected = ((AGS_CONNECTABLE_CONNECTED & (crop_note_dialog->connectable_flags)) != 0) ? TRUE: FALSE;

  return(is_connected);
}

void
ags_crop_note_dialog_connect(AgsConnectable *connectable)
{
  AgsCropNoteDialog *crop_note_dialog;

  crop_note_dialog = AGS_CROP_NOTE_DIALOG(connectable);

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  crop_note_dialog->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  g_signal_connect(crop_note_dialog, "response",
		   G_CALLBACK(ags_crop_note_dialog_response_callback), crop_note_dialog);

  /* absolute */
  g_signal_connect_after(crop_note_dialog->absolute, "toggled",
			 G_CALLBACK(ags_crop_note_dialog_absolute_callback), crop_note_dialog);
}

void
ags_crop_note_dialog_disconnect(AgsConnectable *connectable)
{
  AgsCropNoteDialog *crop_note_dialog;

  crop_note_dialog = AGS_CROP_NOTE_DIALOG(connectable);

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  crop_note_dialog->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  g_object_disconnect(G_OBJECT(crop_note_dialog),
		      "any_signal::response",
		      G_CALLBACK(ags_crop_note_dialog_response_callback),
		      crop_note_dialog,
		      NULL);

  /* absolute */
  g_object_disconnect(G_OBJECT(crop_note_dialog->absolute),
		      "any_signal::toggled",
		      G_CALLBACK(ags_crop_note_dialog_absolute_callback),
		      crop_note_dialog,
		      NULL);
}

void
ags_crop_note_dialog_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_crop_note_dialog_apply(AgsApplicable *applicable)
{
  AgsCropNoteDialog *crop_note_dialog;

  AgsWindow *window;
  AgsMachine *machine;
  AgsCompositeEditor *composite_editor;

  AgsCropNote *crop_note;  

  AgsAudio *audio;

  AgsApplicationContext *application_context;
  
  GList *start_notation, *notation;
  GList *task;
  
  guint x_padding;
  guint x_crop;
  
  gboolean absolute;
  gboolean in_place;
  gboolean do_resize;
  
  crop_note_dialog = AGS_CROP_NOTE_DIALOG(applicable);

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
  x_crop = gtk_spin_button_get_value_as_int(crop_note_dialog->crop_note);
  x_padding = gtk_spin_button_get_value_as_int(crop_note_dialog->padding_note);

  absolute = gtk_check_button_get_active(crop_note_dialog->absolute);

  in_place = gtk_check_button_get_active(crop_note_dialog->in_place);
  do_resize = gtk_check_button_get_active(crop_note_dialog->do_resize);
    
  /* crop note */
  g_object_get(audio,
	       "notation", &start_notation,
	       NULL);

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
      crop_note = ags_crop_note_new(audio,
				    notation->data,
				    start_selection,
				    x_padding, x_crop,
				    absolute,
				    in_place, do_resize);
      task = g_list_prepend(task,
			    crop_note);

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
ags_crop_note_dialog_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

/**
 * ags_crop_note_dialog_new:
 * @transient_for: the transient #AgsWindow
 *
 * Create a new #AgsCropNoteDialog.
 *
 * Returns: a new #AgsCropNoteDialog
 *
 * Since: 3.0.0
 */
AgsCropNoteDialog*
ags_crop_note_dialog_new(GtkWindow *transient_for)
{
  AgsCropNoteDialog *crop_note_dialog;

  crop_note_dialog = (AgsCropNoteDialog *) g_object_new(AGS_TYPE_CROP_NOTE_DIALOG,
							"transient-for", transient_for,
							NULL);

  return(crop_note_dialog);
}

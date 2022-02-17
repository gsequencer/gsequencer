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

#include <ags/GSequencer/editor/ags_crop_note_dialog.h>
#include <ags/GSequencer/editor/ags_crop_note_dialog_callbacks.h>

#include <ags/GSequencer/ags_ui_provider.h>
#include <ags/GSequencer/ags_window.h>
#include <ags/GSequencer/ags_notation_editor.h>
#include <ags/GSequencer/ags_machine.h>

#include <ags/i18n.h>

void ags_crop_note_dialog_class_init(AgsCropNoteDialogClass *crop_note_dialog);
void ags_crop_note_dialog_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_crop_note_dialog_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_crop_note_dialog_init(AgsCropNoteDialog *crop_note_dialog);
void ags_crop_note_dialog_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_crop_note_dialog_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
void ags_crop_note_dialog_finalize(GObject *gobject);
void ags_crop_note_dialog_connect(AgsConnectable *connectable);
void ags_crop_note_dialog_disconnect(AgsConnectable *connectable);
void ags_crop_note_dialog_set_update(AgsApplicable *applicable, gboolean update);
void ags_crop_note_dialog_apply(AgsApplicable *applicable);
void ags_crop_note_dialog_reset(AgsApplicable *applicable);
gboolean ags_crop_note_dialog_delete_event(GtkWidget *widget, GdkEventAny *event);

/**
 * SECTION:ags_crop_note_dialog
 * @short_description: crop tool
 * @title: AgsCropNoteDialog
 * @section_id:
 * @include: ags/X/editor/ags_crop_note_dialog.h
 *
 * The #AgsCropNoteDialog lets you crop notes.
 */

enum{
  PROP_0,
  PROP_MAIN_WINDOW,
};

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
  GtkWidgetClass *widget;

  GParamSpec *param_spec;

  ags_crop_note_dialog_parent_class = g_type_class_peek_parent(crop_note_dialog);

  /* GObjectClass */
  gobject = (GObjectClass *) crop_note_dialog;

  gobject->set_property = ags_crop_note_dialog_set_property;
  gobject->get_property = ags_crop_note_dialog_get_property;

  gobject->finalize = ags_crop_note_dialog_finalize;

  /* properties */
  /**
   * AgsCropNoteDialog:main-window:
   *
   * The assigned #AgsWindow.
   * 
   * Since: 3.0.0
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
  widget = (GtkWidgetClass *) crop_note_dialog;

  widget->delete_event = ags_crop_note_dialog_delete_event;
}

void
ags_crop_note_dialog_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_crop_note_dialog_connect;
  connectable->disconnect = ags_crop_note_dialog_disconnect;
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
  GtkVBox *vbox;
  GtkHBox *hbox;
  GtkLabel *label;

  crop_note_dialog->flags = 0;

  g_object_set(crop_note_dialog,
	       "title", i18n("crop notes"),
	       NULL);

  vbox = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) gtk_dialog_get_content_area(crop_note_dialog),
		     GTK_WIDGET(vbox),
		     FALSE, FALSE,
		     0);  

  /* absolute */
  crop_note_dialog->absolute = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("absolute"));
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(crop_note_dialog->absolute),
		     FALSE, FALSE,
		     0);  

  /* radio - in place */
  crop_note_dialog->in_place = (GtkRadioButton *) gtk_radio_button_new_with_label(NULL,
										  i18n("in-place"));
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(crop_note_dialog->in_place),
		     FALSE, FALSE,
		     0);  
  
  /* radio - do resize */
  crop_note_dialog->do_resize = (GtkRadioButton *) gtk_radio_button_new_with_label(gtk_radio_button_get_group(crop_note_dialog->in_place),
										   i18n("do resize"));
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(crop_note_dialog->do_resize),
		     FALSE, FALSE,
		     0);  

  /* crop note - hbox */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(hbox),
		     FALSE, FALSE,
		     0);

  /* crop note - label */
  label = (GtkLabel *) gtk_label_new(i18n("crop note"));
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  /* crop note - spin button */
  crop_note_dialog->crop_note = (GtkSpinButton *) gtk_spin_button_new_with_range(-1.0 * AGS_CROP_NOTE_DIALOG_MAX_WIDTH,
										 AGS_CROP_NOTE_DIALOG_MAX_WIDTH,
										 1.0);
  gtk_spin_button_set_value(crop_note_dialog->crop_note,
			    0.0);
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(crop_note_dialog->crop_note),
		     FALSE, FALSE,
		     0);

  /* padding note - hbox */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(hbox),
		     FALSE, FALSE,
		     0);

  /* padding note - label */
  label = (GtkLabel *) gtk_label_new(i18n("padding note"));
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  /* padding note - spin button */
  crop_note_dialog->padding_note = (GtkSpinButton *) gtk_spin_button_new_with_range(-1.0 * AGS_CROP_NOTE_DIALOG_MAX_WIDTH,
										    AGS_CROP_NOTE_DIALOG_MAX_WIDTH,
										    1.0);
  gtk_spin_button_set_value(crop_note_dialog->padding_note,
			    0.0);
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(crop_note_dialog->padding_note),
		     FALSE, FALSE,
		     0);
  
  /* dialog buttons */
  gtk_dialog_add_buttons((GtkDialog *) crop_note_dialog,
			 i18n("_Apply"), GTK_RESPONSE_APPLY,
			 i18n("_OK"), GTK_RESPONSE_OK,
			 i18n("_Cancel"), GTK_RESPONSE_CANCEL,
			 NULL);
}

void
ags_crop_note_dialog_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsCropNoteDialog *crop_note_dialog;

  crop_note_dialog = AGS_CROP_NOTE_DIALOG(gobject);

  switch(prop_id){
  case PROP_MAIN_WINDOW:
    {
      AgsWindow *main_window;

      main_window = (AgsWindow *) g_value_get_object(value);

      if((AgsWindow *) crop_note_dialog->main_window == main_window){
	return;
      }

      if(crop_note_dialog->main_window != NULL){
	g_object_unref(crop_note_dialog->main_window);
      }

      if(main_window != NULL){
	g_object_ref(main_window);
      }

      crop_note_dialog->main_window = (GtkWidget *) main_window;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_crop_note_dialog_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec)
{
  AgsCropNoteDialog *crop_note_dialog;

  crop_note_dialog = AGS_CROP_NOTE_DIALOG(gobject);

  switch(prop_id){
  case PROP_MAIN_WINDOW:
    {
      g_value_set_object(value, crop_note_dialog->main_window);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_crop_note_dialog_connect(AgsConnectable *connectable)
{
  AgsCropNoteDialog *crop_note_dialog;

  crop_note_dialog = AGS_CROP_NOTE_DIALOG(connectable);

  if((AGS_CROP_NOTE_DIALOG_CONNECTED & (crop_note_dialog->flags)) != 0){
    return;
  }

  crop_note_dialog->flags |= AGS_CROP_NOTE_DIALOG_CONNECTED;

  g_signal_connect(crop_note_dialog, "response",
		   G_CALLBACK(ags_crop_note_dialog_response_callback), crop_note_dialog);

  /* absolute */
  g_signal_connect_after(crop_note_dialog->absolute, "clicked",
			 G_CALLBACK(ags_crop_note_dialog_absolute_callback), crop_note_dialog);
}

void
ags_crop_note_dialog_disconnect(AgsConnectable *connectable)
{
  AgsCropNoteDialog *crop_note_dialog;

  crop_note_dialog = AGS_CROP_NOTE_DIALOG(connectable);

  if((AGS_CROP_NOTE_DIALOG_CONNECTED & (crop_note_dialog->flags)) == 0){
    return;
  }

  crop_note_dialog->flags &= (~AGS_CROP_NOTE_DIALOG_CONNECTED);

  g_object_disconnect(G_OBJECT(crop_note_dialog),
		      "any_signal::response",
		      G_CALLBACK(ags_crop_note_dialog_response_callback),
		      crop_note_dialog,
		      NULL);

  /* absolute */
  g_object_disconnect(G_OBJECT(crop_note_dialog->absolute),
		      "any_signal::clicked",
		      G_CALLBACK(ags_crop_note_dialog_absolute_callback),
		      crop_note_dialog,
		      NULL);
}

void
ags_crop_note_dialog_finalize(GObject *gobject)
{
  AgsCropNoteDialog *crop_note_dialog;

  crop_note_dialog = (AgsCropNoteDialog *) gobject;
  
  G_OBJECT_CLASS(ags_crop_note_dialog_parent_class)->finalize(gobject);
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

  AgsCropNote *crop_note;  

  AgsAudio *audio;

  AgsApplicationContext *application_context;
  
  GList *start_notation, *notation;
  GList *task;
  
  guint x_padding;
  guint x_crop;
  
  gboolean use_composite_editor;
  gboolean absolute;
  gboolean in_place;
  gboolean do_resize;
  
  crop_note_dialog = AGS_CROP_NOTE_DIALOG(applicable);

  /* application context */
  application_context = ags_application_context_get_instance();

  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  machine = NULL;

  if(use_composite_editor){
    AgsCompositeEditor *composite_editor;
    
    composite_editor = window->composite_editor;

    machine = composite_editor->selected_machine;
  }else{
    AgsNotationEditor *notation_editor;
    
    notation_editor = window->notation_editor;

    machine = notation_editor->selected_machine;
  }
  
  if(machine == NULL){
    return;
  }

  audio = machine->audio;

  /* get some values */
  x_crop = gtk_spin_button_get_value_as_int(crop_note_dialog->crop_note);
  x_padding = gtk_spin_button_get_value_as_int(crop_note_dialog->padding_note);

  absolute = gtk_toggle_button_get_active((GtkToggleButton *) crop_note_dialog->absolute);

  in_place = gtk_toggle_button_get_active((GtkToggleButton *) crop_note_dialog->in_place);
  do_resize = gtk_toggle_button_get_active((GtkToggleButton *) crop_note_dialog->do_resize);
    
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

gboolean
ags_crop_note_dialog_delete_event(GtkWidget *widget, GdkEventAny *event)
{
  gtk_widget_hide(widget);

  //  GTK_WIDGET_CLASS(ags_crop_note_dialog_parent_class)->delete_event(widget, event);

  return(TRUE);
}

/**
 * ags_crop_note_dialog_new:
 * @main_window: the #AgsWindow
 *
 * Create a new #AgsCropNoteDialog.
 *
 * Returns: a new #AgsCropNoteDialog
 *
 * Since: 3.0.0
 */
AgsCropNoteDialog*
ags_crop_note_dialog_new(GtkWidget *main_window)
{
  AgsCropNoteDialog *crop_note_dialog;

  crop_note_dialog = (AgsCropNoteDialog *) g_object_new(AGS_TYPE_CROP_NOTE_DIALOG,
							"main-window", main_window,
							NULL);

  return(crop_note_dialog);
}

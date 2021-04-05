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

#include <ags/X/editor/ags_select_note_dialog.h>
#include <ags/X/editor/ags_select_note_dialog_callbacks.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_notation_editor.h>
#include <ags/X/ags_machine.h>

#include <ags/i18n.h>

void ags_select_note_dialog_class_init(AgsSelectNoteDialogClass *select_note_dialog);
void ags_select_note_dialog_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_select_note_dialog_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_select_note_dialog_init(AgsSelectNoteDialog *select_note_dialog);
void ags_select_note_dialog_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec);
void ags_select_note_dialog_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec);
void ags_select_note_dialog_finalize(GObject *gobject);
void ags_select_note_dialog_connect(AgsConnectable *connectable);
void ags_select_note_dialog_disconnect(AgsConnectable *connectable);
void ags_select_note_dialog_set_update(AgsApplicable *applicable, gboolean update);
void ags_select_note_dialog_apply(AgsApplicable *applicable);
void ags_select_note_dialog_reset(AgsApplicable *applicable);
gboolean ags_select_note_dialog_delete_event(GtkWidget *widget, GdkEventAny *event);

/**
 * SECTION:ags_select_note_dialog
 * @short_description: select tool
 * @title: AgsSelectNoteDialog
 * @section_id:
 * @include: ags/X/editor/ags_select_note_dialog.h
 *
 * The #AgsSelectNoteDialog lets you select notes.
 */

enum{
  PROP_0,
  PROP_MAIN_WINDOW,
};

static gpointer ags_select_note_dialog_parent_class = NULL;

GType
ags_select_note_dialog_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_select_note_dialog = 0;

    static const GTypeInfo ags_select_note_dialog_info = {
      sizeof (AgsSelectNoteDialogClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_select_note_dialog_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSelectNoteDialog),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_select_note_dialog_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_select_note_dialog_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_select_note_dialog_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_select_note_dialog = g_type_register_static(GTK_TYPE_DIALOG,
							 "AgsSelectNoteDialog", &ags_select_note_dialog_info,
							 0);
    
    g_type_add_interface_static(ags_type_select_note_dialog,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_select_note_dialog,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_select_note_dialog);
  }

  return g_define_type_id__volatile;
}

void
ags_select_note_dialog_class_init(AgsSelectNoteDialogClass *select_note_dialog)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  GParamSpec *param_spec;

  ags_select_note_dialog_parent_class = g_type_class_peek_parent(select_note_dialog);

  /* GObjectClass */
  gobject = (GObjectClass *) select_note_dialog;

  gobject->set_property = ags_select_note_dialog_set_property;
  gobject->get_property = ags_select_note_dialog_get_property;

  gobject->finalize = ags_select_note_dialog_finalize;

  /* properties */
  /**
   * AgsSelectNoteDialog:main-window:
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
  widget = (GtkWidgetClass *) select_note_dialog;

  widget->delete_event = ags_select_note_dialog_delete_event;
}

void
ags_select_note_dialog_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_select_note_dialog_connect;
  connectable->disconnect = ags_select_note_dialog_disconnect;
}

void
ags_select_note_dialog_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_select_note_dialog_set_update;
  applicable->apply = ags_select_note_dialog_apply;
  applicable->reset = ags_select_note_dialog_reset;
}

void
ags_select_note_dialog_init(AgsSelectNoteDialog *select_note_dialog)
{
  GtkVBox *vbox;
  GtkHBox *hbox;
  GtkLabel *label;

  select_note_dialog->flags = 0;

  g_object_set(select_note_dialog,
	       "title", i18n("select notes"),
	       NULL);

  vbox = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) gtk_dialog_get_content_area(select_note_dialog),
		     GTK_WIDGET(vbox),
		     FALSE, FALSE,
		     0);  

  /* copy selection */
  select_note_dialog->copy_selection = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("copy selection"));
  gtk_toggle_button_set_active((GtkToggleButton *) select_note_dialog->copy_selection,
			       TRUE);
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(select_note_dialog->copy_selection),
		     FALSE, FALSE,
		     0);  

  /* select x0 - hbox */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(hbox),
		     FALSE, FALSE,
		     0);

  /* select x0 - label */
  label = (GtkLabel *) gtk_label_new(i18n("select x0"));
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  /* select x0 - spin button */
  select_note_dialog->select_x0 = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
										   AGS_SELECT_NOTE_MAX_BEATS,
										   1.0);
  gtk_spin_button_set_value(select_note_dialog->select_x0,
			    0.0);
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(select_note_dialog->select_x0),
		     FALSE, FALSE,
		     0);
  
  /* select y0 - hbox */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(hbox),
		     FALSE, FALSE,
		     0);

  /* select y0 - label */
  label = (GtkLabel *) gtk_label_new(i18n("select y0"));
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  /* select y0 - spin button */
  select_note_dialog->select_y0 = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
										   AGS_SELECT_NOTE_MAX_KEYS,
										   1.0);
  gtk_spin_button_set_value(select_note_dialog->select_y0,
			    0.0);
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(select_note_dialog->select_y0),
		     FALSE, FALSE,
		     0);

  /* select x1 - hbox */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(hbox),
		     FALSE, FALSE,
		     0);

  /* select x1 - label */
  label = (GtkLabel *) gtk_label_new(i18n("select x1"));
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  /* select x1 - spin button */
  select_note_dialog->select_x1 = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
										   AGS_SELECT_NOTE_MAX_BEATS,
										   1.0);
  gtk_spin_button_set_value(select_note_dialog->select_x1,
			    0.0);
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(select_note_dialog->select_x1),
		     FALSE, FALSE,
		     0);

  /* select y1 - hbox */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(hbox),
		     FALSE, FALSE,
		     0);

  /* select y1 - label */
  label = (GtkLabel *) gtk_label_new(i18n("select y1"));
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  /* select y1 - spin button */
  select_note_dialog->select_y1 = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
										   AGS_SELECT_NOTE_MAX_KEYS,
										   1.0);
  gtk_spin_button_set_value(select_note_dialog->select_y1,
			    0.0);
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(select_note_dialog->select_y1),
		     FALSE, FALSE,
		     0);
  
  /* dialog buttons */
  gtk_dialog_add_buttons((GtkDialog *) select_note_dialog,
			 i18n("_Apply"), GTK_RESPONSE_APPLY,
			 i18n("_OK"), GTK_RESPONSE_OK,
			 i18n("_Cancel"), GTK_RESPONSE_CANCEL,
			 NULL);
}

void
ags_select_note_dialog_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec)
{
  AgsSelectNoteDialog *select_note_dialog;

  select_note_dialog = AGS_SELECT_NOTE_DIALOG(gobject);

  switch(prop_id){
  case PROP_MAIN_WINDOW:
    {
      AgsWindow *main_window;

      main_window = (AgsWindow *) g_value_get_object(value);

      if((AgsWindow *) select_note_dialog->main_window == main_window){
	return;
      }

      if(select_note_dialog->main_window != NULL){
	g_object_unref(select_note_dialog->main_window);
      }

      if(main_window != NULL){
	g_object_ref(main_window);
      }

      select_note_dialog->main_window = (GtkWidget *) main_window;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_select_note_dialog_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec)
{
  AgsSelectNoteDialog *select_note_dialog;

  select_note_dialog = AGS_SELECT_NOTE_DIALOG(gobject);

  switch(prop_id){
  case PROP_MAIN_WINDOW:
    {
      g_value_set_object(value, select_note_dialog->main_window);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_select_note_dialog_connect(AgsConnectable *connectable)
{
  AgsSelectNoteDialog *select_note_dialog;

  select_note_dialog = AGS_SELECT_NOTE_DIALOG(connectable);

  if((AGS_SELECT_NOTE_DIALOG_CONNECTED & (select_note_dialog->flags)) != 0){
    return;
  }

  select_note_dialog->flags |= AGS_SELECT_NOTE_DIALOG_CONNECTED;

  g_signal_connect(select_note_dialog, "response",
		   G_CALLBACK(ags_select_note_dialog_response_callback), select_note_dialog);
}

void
ags_select_note_dialog_disconnect(AgsConnectable *connectable)
{
  AgsSelectNoteDialog *select_note_dialog;

  select_note_dialog = AGS_SELECT_NOTE_DIALOG(connectable);

  if((AGS_SELECT_NOTE_DIALOG_CONNECTED & (select_note_dialog->flags)) == 0){
    return;
  }

  select_note_dialog->flags &= (~AGS_SELECT_NOTE_DIALOG_CONNECTED);

  g_object_disconnect(G_OBJECT(select_note_dialog),
		      "any_signal::response",
		      G_CALLBACK(ags_select_note_dialog_response_callback),
		      select_note_dialog,
		      NULL);
}

void
ags_select_note_dialog_finalize(GObject *gobject)
{
  AgsSelectNoteDialog *select_note_dialog;

  select_note_dialog = (AgsSelectNoteDialog *) gobject;
  
  G_OBJECT_CLASS(ags_select_note_dialog_parent_class)->finalize(gobject);
}

void
ags_select_note_dialog_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_select_note_dialog_apply(AgsApplicable *applicable)
{
  AgsSelectNoteDialog *select_note_dialog;

  AgsWindow *window;
  AgsNotationEditor *notation_editor;
  AgsMachine *machine;

  AgsAudio *audio;

  AgsTimestamp *timestamp;

  xmlDoc *clipboard;
  xmlNode *audio_node, *notation_node;

  GList *start_list_notation, *list_notation;

  xmlChar *buffer;
  
  int size;
  guint x0, y0;
  guint x1, y1;
  gint i;
  
  gboolean copy_selection;
  
  select_note_dialog = AGS_SELECT_NOTE_DIALOG(applicable);

  window = (AgsWindow *) select_note_dialog->main_window;
  notation_editor = window->notation_editor;

  machine = notation_editor->selected_machine;

  if(machine == NULL){
    return;
  }

  audio = machine->audio;

  g_object_get(audio,
	       "notation", &start_list_notation,
	       NULL);

  /* get some values */
  copy_selection = gtk_toggle_button_get_active((GtkToggleButton *) select_note_dialog->copy_selection);

  x0 = gtk_spin_button_get_value_as_int(select_note_dialog->select_x0);
  y0 = gtk_spin_button_get_value_as_int(select_note_dialog->select_y0);

  x1 = gtk_spin_button_get_value_as_int(select_note_dialog->select_x1);
  y1 = gtk_spin_button_get_value_as_int(select_note_dialog->select_y1);
  
  timestamp = ags_timestamp_new();

  timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  timestamp->flags |= AGS_TIMESTAMP_OFFSET;

  /* select note */
  if(copy_selection){
    /* create document */
    clipboard = xmlNewDoc(BAD_CAST XML_DEFAULT_VERSION);
  
    /* create root node */
    audio_node = xmlNewNode(NULL, BAD_CAST "audio");
    xmlDocSetRootElement(clipboard, audio_node);
  }
  
  i = 0;
  
  while((i = ags_notebook_next_active_tab(notation_editor->notebook,
					  i)) != -1){
    list_notation = start_list_notation;
    
    timestamp->timer.ags_offset.offset = 0;
    
    while((list_notation = ags_notation_find_near_timestamp(list_notation, i,
							    timestamp)) != NULL){
      ags_notation_add_region_to_selection(AGS_NOTATION(list_notation->data),
					   x0, y0,
					   x1, y1,
					   TRUE);
    
      
      if(copy_selection){
	notation_node = ags_notation_copy_selection(AGS_NOTATION(list_notation->data));
	xmlAddChild(audio_node, notation_node);      
      }

      /* iterate */
      timestamp->timer.ags_offset.offset += AGS_NOTATION_DEFAULT_OFFSET;

      list_notation = list_notation->next;
    }

    i++;
  }

  g_object_unref(timestamp);
  
  g_list_free_full(start_list_notation,
		   g_object_unref);
  
  /* write to clipboard */
  if(copy_selection){
    xmlDocDumpFormatMemoryEnc(clipboard, &buffer, &size, "UTF-8", TRUE);
    gtk_clipboard_set_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD),
			   buffer, size);
    gtk_clipboard_store(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));
    
    xmlFreeDoc(clipboard);
  }
}

void
ags_select_note_dialog_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

gboolean
ags_select_note_dialog_delete_event(GtkWidget *widget, GdkEventAny *event)
{
  gtk_widget_hide(widget);

  //  GTK_WIDGET_CLASS(ags_select_note_dialog_parent_class)->delete_event(widget, event);

  return(TRUE);
}

/**
 * ags_select_note_dialog_new:
 * @main_window: the #AgsWindow
 *
 * Create a new #AgsSelectNoteDialog.
 *
 * Returns: a new #AgsSelectNoteDialog
 *
 * Since: 3.0.0
 */
AgsSelectNoteDialog*
ags_select_note_dialog_new(GtkWidget *main_window)
{
  AgsSelectNoteDialog *select_note_dialog;

  select_note_dialog = (AgsSelectNoteDialog *) g_object_new(AGS_TYPE_SELECT_NOTE_DIALOG,
							    "main-window", main_window,
							    NULL);

  return(select_note_dialog);
}

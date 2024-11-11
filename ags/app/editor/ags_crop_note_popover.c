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

#include <ags/app/editor/ags_crop_note_popover.h>
#include <ags/app/editor/ags_crop_note_popover_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_composite_editor.h>
#include <ags/app/ags_machine.h>

#include <ags/app/editor/ags_notation_edit.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

#include <ags/i18n.h>

void ags_crop_note_popover_class_init(AgsCropNotePopoverClass *crop_note_popover);
void ags_crop_note_popover_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_crop_note_popover_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_crop_note_popover_init(AgsCropNotePopover *crop_note_popover);
void ags_crop_note_popover_finalize(GObject *gobject);

xmlNode* ags_crop_note_popover_xml_compose(AgsConnectable *connectable);
void ags_crop_note_popover_xml_parse(AgsConnectable *connectable,
				     xmlNode *node);

gboolean ags_crop_note_popover_is_connected(AgsConnectable *connectable);
void ags_crop_note_popover_connect(AgsConnectable *connectable);
void ags_crop_note_popover_disconnect(AgsConnectable *connectable);

void ags_crop_note_popover_set_update(AgsApplicable *applicable, gboolean update);
void ags_crop_note_popover_apply(AgsApplicable *applicable);
void ags_crop_note_popover_reset(AgsApplicable *applicable);

gboolean ags_crop_note_popover_key_pressed_callback(GtkEventControllerKey *event_controller,
						    guint keyval,
						    guint keycode,
						    GdkModifierType state,
						    AgsCropNotePopover *crop_note_popover);
void ags_crop_note_popover_key_released_callback(GtkEventControllerKey *event_controller,
						 guint keyval,
						 guint keycode,
						 GdkModifierType state,
						 AgsCropNotePopover *crop_note_popover);
gboolean ags_crop_note_popover_modifiers_callback(GtkEventControllerKey *event_controller,
						  GdkModifierType keyval,
						  AgsCropNotePopover *crop_note_popover);

void ags_crop_note_popover_activate_button_callback(GtkButton *activate_button,
						    AgsCropNotePopover *crop_note_popover);

/**
 * SECTION:ags_crop_note_popover
 * @short_description: crop tool
 * @title: AgsCropNotePopover
 * @section_id:
 * @include: ags/app/editor/ags_crop_note_popover.h
 *
 * The #AgsCropNotePopover lets you crop notes.
 */

static gpointer ags_crop_note_popover_parent_class = NULL;

GType
ags_crop_note_popover_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_crop_note_popover = 0;

    static const GTypeInfo ags_crop_note_popover_info = {
      sizeof (AgsCropNotePopoverClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_crop_note_popover_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCropNotePopover),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_crop_note_popover_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_crop_note_popover_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_crop_note_popover_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_crop_note_popover = g_type_register_static(GTK_TYPE_POPOVER,
							"AgsCropNotePopover", &ags_crop_note_popover_info,
							0);
    
    g_type_add_interface_static(ags_type_crop_note_popover,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_crop_note_popover,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_crop_note_popover);
  }

  return g_define_type_id__volatile;
}

void
ags_crop_note_popover_class_init(AgsCropNotePopoverClass *crop_note_popover)
{
  GObjectClass *gobject;

  ags_crop_note_popover_parent_class = g_type_class_peek_parent(crop_note_popover);

  /* GObjectClass */
  gobject = (GObjectClass *) crop_note_popover;

  gobject->finalize = ags_crop_note_popover_finalize;
}

void
ags_crop_note_popover_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = NULL;
  connectable->has_resource = NULL;

  connectable->is_ready = NULL;
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;

  connectable->list_resource = NULL;
  connectable->xml_compose = ags_crop_note_popover_xml_compose;
  connectable->xml_parse = ags_crop_note_popover_xml_parse;

  connectable->is_connected = ags_crop_note_popover_is_connected;  
  connectable->connect = ags_crop_note_popover_connect;
  connectable->disconnect = ags_crop_note_popover_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_crop_note_popover_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_crop_note_popover_set_update;
  applicable->apply = ags_crop_note_popover_apply;
  applicable->reset = ags_crop_note_popover_reset;
}

void
ags_crop_note_popover_init(AgsCropNotePopover *crop_note_popover)
{
  GtkBox *vbox;
  GtkBox *hbox;
  GtkLabel *label;
  
  GtkEventController *event_controller;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();

  crop_note_popover->connectable_flags = 0;

  event_controller = gtk_event_controller_key_new();
  gtk_widget_add_controller((GtkWidget *) crop_note_popover,
			    event_controller);

  g_signal_connect(event_controller, "key-pressed",
		   G_CALLBACK(ags_crop_note_popover_key_pressed_callback), crop_note_popover);
  
  g_signal_connect(event_controller, "key-released",
		   G_CALLBACK(ags_crop_note_popover_key_released_callback), crop_note_popover);

  g_signal_connect(event_controller, "modifiers",
		   G_CALLBACK(ags_crop_note_popover_modifiers_callback), crop_note_popover);
  
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_popover_set_child((GtkPopover *) crop_note_popover,
			(GtkWidget *) vbox);

  /* absolute */
  crop_note_popover->absolute = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("absolute"));
  gtk_box_append(vbox,
		 GTK_WIDGET(crop_note_popover->absolute));  

  /* radio - in place */
  crop_note_popover->in_place = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("in-place"));
  gtk_box_append(vbox,
		 GTK_WIDGET(crop_note_popover->in_place));  
  
  /* radio - do resize */
  crop_note_popover->do_resize = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("do resize"));
  gtk_check_button_set_group(crop_note_popover->do_resize,
			     crop_note_popover->in_place);
  gtk_box_append(vbox,
		 GTK_WIDGET(crop_note_popover->do_resize));  

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
  crop_note_popover->crop_note = (GtkSpinButton *) gtk_spin_button_new_with_range(-1.0 * AGS_CROP_NOTE_POPOVER_MAX_WIDTH,
										  AGS_CROP_NOTE_POPOVER_MAX_WIDTH,
										  1.0);
  gtk_spin_button_set_value(crop_note_popover->crop_note,
			    0.0);
  gtk_box_append(hbox,
		 GTK_WIDGET(crop_note_popover->crop_note));

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
  crop_note_popover->padding_note = (GtkSpinButton *) gtk_spin_button_new_with_range(-1.0 * AGS_CROP_NOTE_POPOVER_MAX_WIDTH,
										     AGS_CROP_NOTE_POPOVER_MAX_WIDTH,
										     1.0);
  gtk_spin_button_set_value(crop_note_popover->padding_note,
			    0.0);
  gtk_box_append(hbox,
		 GTK_WIDGET(crop_note_popover->padding_note));
  
  /* buttons */
  crop_note_popover->action_area = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
							  AGS_UI_PROVIDER_DEFAULT_SPACING);
  
  gtk_widget_set_halign((GtkWidget *) crop_note_popover->action_area,
			GTK_ALIGN_END);

  gtk_box_append(vbox,
		 (GtkWidget *) crop_note_popover->action_area);

  crop_note_popover->activate_button = (GtkButton *) gtk_button_new_with_label(i18n("apply"));

  gtk_box_append(crop_note_popover->action_area,
		 (GtkWidget *) crop_note_popover->activate_button);

  g_signal_connect(crop_note_popover->activate_button, "clicked",
		   G_CALLBACK(ags_crop_note_popover_activate_button_callback), crop_note_popover);

  gtk_popover_set_default_widget((GtkPopover *) crop_note_popover,
				 (GtkWidget *) crop_note_popover->activate_button);
}

xmlNode*
ags_crop_note_popover_xml_compose(AgsConnectable *connectable)
{
  AgsCropNotePopover *crop_note_popover;
  
  xmlNode *node;

  gchar *str;
  
  crop_note_popover = AGS_CROP_NOTE_POPOVER(connectable);

  node = xmlNewNode(NULL,
		    BAD_CAST "ags-move-note-popover");

  /* absolute */
  str = g_strdup_printf("%s",
			((gtk_check_button_get_active(crop_note_popover->absolute)) ? "true": "false"));
  
  xmlNewProp(node,
	     BAD_CAST "absolute",
	     BAD_CAST str);
  
  g_free(str);

  /* in place */
  str = g_strdup_printf("%s",
			((gtk_check_button_get_active(crop_note_popover->in_place)) ? "true": "false"));
  
  xmlNewProp(node,
	     BAD_CAST "in-place",
	     BAD_CAST str);
  
  g_free(str);

  /* do resize */
  str = g_strdup_printf("%s",
			((gtk_check_button_get_active(crop_note_popover->do_resize)) ? "true": "false"));
  
  xmlNewProp(node,
	     BAD_CAST "do-resize",
	     BAD_CAST str);
  
  g_free(str);

  /* crop note */
  str = g_strdup_printf("%f",
			gtk_spin_button_get_value(crop_note_popover->crop_note));
  
  xmlNewProp(node,
	     BAD_CAST "crop-note",
	     BAD_CAST str);
  
  g_free(str);

  /* padding note */
  str = g_strdup_printf("%f",
			gtk_spin_button_get_value(crop_note_popover->padding_note));
  
  xmlNewProp(node,
	     BAD_CAST "padding-note",
	     BAD_CAST str);
  
  g_free(str);

  return(node);
}

void
ags_crop_note_popover_xml_parse(AgsConnectable *connectable,
				xmlNode *node)
{
  AgsCropNotePopover *crop_note_popover;
  
  gchar *str;
  
  crop_note_popover = AGS_CROP_NOTE_POPOVER(connectable);

  /* absolute */
  str = xmlGetProp(node,
		   "absolute");

  gtk_check_button_set_active(crop_note_popover->absolute,
			      ((!g_ascii_strncasecmp(str, "false", 6) == FALSE) ? TRUE: FALSE));

  xmlFree(str);

  /* in place */
  str = xmlGetProp(node,
		   "in-place");

  gtk_check_button_set_active(crop_note_popover->in_place,
			      ((!g_ascii_strncasecmp(str, "false", 6) == FALSE) ? TRUE: FALSE));

  xmlFree(str);

  /* do resize */
  str = xmlGetProp(node,
		   "do-resize");

  gtk_check_button_set_active(crop_note_popover->do_resize,
			      ((!g_ascii_strncasecmp(str, "false", 6) == FALSE) ? TRUE: FALSE));

  xmlFree(str);

  /* crop note */
  str = xmlGetProp(node,
		   "crop-note");

  gtk_spin_button_set_value(crop_note_popover->crop_note,
			    g_ascii_strtod(str,
					   NULL));
  
  xmlFree(str);

  /* padding note */
  str = xmlGetProp(node,
		   "padding-note");

  gtk_spin_button_set_value(crop_note_popover->padding_note,
			    g_ascii_strtod(str,
					   NULL));
  
  xmlFree(str);
}

gboolean
ags_crop_note_popover_is_connected(AgsConnectable *connectable)
{
  AgsCropNotePopover *crop_note_popover;
  
  gboolean is_connected;
  
  crop_note_popover = AGS_CROP_NOTE_POPOVER(connectable);

  /* check is connected */
  is_connected = ((AGS_CONNECTABLE_CONNECTED & (crop_note_popover->connectable_flags)) != 0) ? TRUE: FALSE;

  return(is_connected);
}

void
ags_crop_note_popover_connect(AgsConnectable *connectable)
{
  AgsCropNotePopover *crop_note_popover;

  crop_note_popover = AGS_CROP_NOTE_POPOVER(connectable);

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  crop_note_popover->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  /* absolute */
  g_signal_connect_after(crop_note_popover->absolute, "toggled",
			 G_CALLBACK(ags_crop_note_popover_absolute_callback), crop_note_popover);
}

void
ags_crop_note_popover_disconnect(AgsConnectable *connectable)
{
  AgsCropNotePopover *crop_note_popover;

  crop_note_popover = AGS_CROP_NOTE_POPOVER(connectable);

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  crop_note_popover->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  /* absolute */
  g_object_disconnect(G_OBJECT(crop_note_popover->absolute),
		      "any_signal::toggled",
		      G_CALLBACK(ags_crop_note_popover_absolute_callback),
		      crop_note_popover,
		      NULL);
}

void
ags_crop_note_popover_finalize(GObject *gobject)
{
  AgsCropNotePopover *crop_note_popover;

  crop_note_popover = (AgsCropNotePopover *) gobject;
  
  G_OBJECT_CLASS(ags_crop_note_popover_parent_class)->finalize(gobject);
}

void
ags_crop_note_popover_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_crop_note_popover_apply(AgsApplicable *applicable)
{
  AgsCropNotePopover *crop_note_popover;

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
  
  crop_note_popover = AGS_CROP_NOTE_POPOVER(applicable);

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
  x_crop = gtk_spin_button_get_value_as_int(crop_note_popover->crop_note);
  x_padding = gtk_spin_button_get_value_as_int(crop_note_popover->padding_note);

  absolute = gtk_check_button_get_active(crop_note_popover->absolute);

  in_place = gtk_check_button_get_active(crop_note_popover->in_place);
  do_resize = gtk_check_button_get_active(crop_note_popover->do_resize);
    
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
ags_crop_note_popover_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

gboolean
ags_crop_note_popover_key_pressed_callback(GtkEventControllerKey *event_controller,
					   guint keyval,
					   guint keycode,
					   GdkModifierType state,
					   AgsCropNotePopover *crop_note_popover)
{
  gboolean key_handled;

  key_handled = TRUE;

  if(keyval == GDK_KEY_Tab ||
     keyval == GDK_KEY_ISO_Left_Tab ||
     keyval == GDK_KEY_Shift_L ||
     keyval == GDK_KEY_Shift_R ||
     keyval == GDK_KEY_Alt_L ||
     keyval == GDK_KEY_Alt_R ||
     keyval == GDK_KEY_Control_L ||
     keyval == GDK_KEY_Control_R){
    key_handled = FALSE;
  }
  
  return(key_handled);
}

void
ags_crop_note_popover_key_released_callback(GtkEventControllerKey *event_controller,
					    guint keyval,
					    guint keycode,
					    GdkModifierType state,
					    AgsCropNotePopover *crop_note_popover)
{
  gboolean key_handled;

  key_handled = TRUE;

  if(keyval == GDK_KEY_Tab ||
     keyval == GDK_KEY_ISO_Left_Tab ||
     keyval == GDK_KEY_Shift_L ||
     keyval == GDK_KEY_Shift_R ||
     keyval == GDK_KEY_Alt_L ||
     keyval == GDK_KEY_Alt_R ||
     keyval == GDK_KEY_Control_L ||
     keyval == GDK_KEY_Control_R){
    key_handled = FALSE;
  }else{
    switch(keyval){
    case GDK_KEY_Escape:
      {
	gtk_popover_popdown((GtkPopover *) crop_note_popover);	
      }
      break;
    }
  }
}

gboolean
ags_crop_note_popover_modifiers_callback(GtkEventControllerKey *event_controller,
					 GdkModifierType keyval,
					 AgsCropNotePopover *crop_note_popover)
{
  return(FALSE);
}

void
ags_crop_note_popover_activate_button_callback(GtkButton *activate_button,
					       AgsCropNotePopover *crop_note_popover)
{
  ags_applicable_apply(AGS_APPLICABLE(crop_note_popover));
}

/**
 * ags_crop_note_popover_new:
 *
 * Create a new #AgsCropNotePopover.
 *
 * Returns: a new #AgsCropNotePopover
 *
 * Since: 6.11.0
 */
AgsCropNotePopover*
ags_crop_note_popover_new()
{
  AgsCropNotePopover *crop_note_popover;

  crop_note_popover = (AgsCropNotePopover *) g_object_new(AGS_TYPE_CROP_NOTE_POPOVER,
							  NULL);

  return(crop_note_popover);
}

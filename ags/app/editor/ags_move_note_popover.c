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

#include <ags/app/editor/ags_move_note_popover.h>
#include <ags/app/editor/ags_move_note_popover_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_composite_editor.h>
#include <ags/app/ags_machine.h>

#include <ags/app/editor/ags_notation_edit.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

#include <ags/i18n.h>

void ags_move_note_popover_class_init(AgsMoveNotePopoverClass *move_note_popover);
void ags_move_note_popover_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_move_note_popover_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_move_note_popover_init(AgsMoveNotePopover *move_note_popover);
void ags_move_note_popover_finalize(GObject *gobject);

xmlNode* ags_move_note_popover_xml_compose(AgsConnectable *connectable);
void ags_move_note_popover_xml_parse(AgsConnectable *connectable,
				     xmlNode *node);

gboolean ags_move_note_popover_is_connected(AgsConnectable *connectable);
void ags_move_note_popover_connect(AgsConnectable *connectable);
void ags_move_note_popover_disconnect(AgsConnectable *connectable);

void ags_move_note_popover_set_update(AgsApplicable *applicable, gboolean update);
void ags_move_note_popover_apply(AgsApplicable *applicable);
void ags_move_note_popover_reset(AgsApplicable *applicable);

gboolean ags_move_note_popover_key_pressed_callback(GtkEventControllerKey *event_controller,
						    guint keyval,
						    guint keycode,
						    GdkModifierType state,
						    AgsMoveNotePopover *move_note_popover);
void ags_move_note_popover_key_released_callback(GtkEventControllerKey *event_controller,
						 guint keyval,
						 guint keycode,
						 GdkModifierType state,
						 AgsMoveNotePopover *move_note_popover);
gboolean ags_move_note_popover_modifiers_callback(GtkEventControllerKey *event_controller,
						  GdkModifierType keyval,
						  AgsMoveNotePopover *move_note_popover);

void ags_move_note_popover_activate_button_callback(GtkButton *activate_button,
						    AgsMoveNotePopover *move_note_popover);

/**
 * SECTION:ags_move_note_popover
 * @short_description: move tool
 * @title: AgsMoveNotePopover
 * @section_id:
 * @include: ags/app/editor/ags_move_note_popover.h
 *
 * The #AgsMoveNotePopover lets you move notes.
 */

static gpointer ags_move_note_popover_parent_class = NULL;

GType
ags_move_note_popover_get_type(void)
{
  static gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_move_note_popover = 0;

    static const GTypeInfo ags_move_note_popover_info = {
      sizeof (AgsMoveNotePopoverClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_move_note_popover_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMoveNotePopover),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_move_note_popover_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_move_note_popover_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_move_note_popover_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_move_note_popover = g_type_register_static(GTK_TYPE_POPOVER,
							"AgsMoveNotePopover", &ags_move_note_popover_info,
							0);
    
    g_type_add_interface_static(ags_type_move_note_popover,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_move_note_popover,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_move_note_popover);
  }

  return g_define_type_id__volatile;
}

void
ags_move_note_popover_class_init(AgsMoveNotePopoverClass *move_note_popover)
{
  GObjectClass *gobject;

  ags_move_note_popover_parent_class = g_type_class_peek_parent(move_note_popover);

  /* GObjectClass */
  gobject = (GObjectClass *) move_note_popover;

  gobject->finalize = ags_move_note_popover_finalize;
}

void
ags_move_note_popover_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = NULL;
  connectable->has_resource = NULL;

  connectable->is_ready = NULL;
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;

  connectable->list_resource = NULL;
  connectable->xml_compose = ags_move_note_popover_xml_compose;
  connectable->xml_parse = ags_move_note_popover_xml_parse;

  connectable->is_connected = ags_move_note_popover_is_connected;  
  connectable->connect = ags_move_note_popover_connect;
  connectable->disconnect = ags_move_note_popover_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_move_note_popover_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_move_note_popover_set_update;
  applicable->apply = ags_move_note_popover_apply;
  applicable->reset = ags_move_note_popover_reset;
}

void
ags_move_note_popover_init(AgsMoveNotePopover *move_note_popover)
{
  GtkBox *vbox;
  GtkBox *hbox;
  GtkLabel *label;
  
  GtkEventController *event_controller;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();

  move_note_popover->connectable_flags = 0;

  event_controller = gtk_event_controller_key_new();
  gtk_widget_add_controller((GtkWidget *) move_note_popover,
			    event_controller);

  g_signal_connect(event_controller, "key-pressed",
		   G_CALLBACK(ags_move_note_popover_key_pressed_callback), move_note_popover);
  
  g_signal_connect(event_controller, "key-released",
		   G_CALLBACK(ags_move_note_popover_key_released_callback), move_note_popover);

  g_signal_connect(event_controller, "modifiers",
		   G_CALLBACK(ags_move_note_popover_modifiers_callback), move_note_popover);
  
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_popover_set_child((GtkPopover *) move_note_popover,
			(GtkWidget *) vbox);

  /* radio - relative */
  move_note_popover->relative = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("relative"));
  gtk_box_append(vbox,
		 (GtkWidget *) move_note_popover->relative);

  /* radio - absolute */
  move_note_popover->absolute = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("absolute"));
  gtk_check_button_set_group(move_note_popover->absolute,
			     move_note_popover->relative);
  gtk_box_append(vbox,
		 (GtkWidget *) move_note_popover->absolute);

  /* move x - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(vbox,
		 GTK_WIDGET(hbox));

  /* move x - label */
  label = (GtkLabel *) gtk_label_new(i18n("move x"));
  gtk_box_append(hbox,
		 GTK_WIDGET(label));

  /* move x - spin button */
  move_note_popover->move_x = (GtkSpinButton *) gtk_spin_button_new_with_range(-1.0 * AGS_MOVE_NOTE_POPOVER_MAX_X,
									       AGS_MOVE_NOTE_POPOVER_MAX_X,
									       1.0);
  gtk_spin_button_set_value(move_note_popover->move_x,
			    0.0);
  gtk_box_append(hbox,
		 (GtkWidget *) move_note_popover->move_x);

  /* move y - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(vbox,
		 GTK_WIDGET(hbox));

  /* move y - label */
  label = (GtkLabel *) gtk_label_new(i18n("move y"));
  gtk_box_append(hbox,
		 GTK_WIDGET(label));

  /* move y - spin button */
  move_note_popover->move_y = (GtkSpinButton *) gtk_spin_button_new_with_range(-1.0 * AGS_MOVE_NOTE_POPOVER_MAX_Y,
									       AGS_MOVE_NOTE_POPOVER_MAX_Y,
									       1.0);
  gtk_spin_button_set_value(move_note_popover->move_y,
			    0.0);
  gtk_box_append(hbox,
		 (GtkWidget *) move_note_popover->move_y);
  
  /* buttons */
  move_note_popover->action_area = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
							  AGS_UI_PROVIDER_DEFAULT_SPACING);
  
  gtk_widget_set_halign((GtkWidget *) move_note_popover->action_area,
			GTK_ALIGN_END);

  gtk_box_append(vbox,
		 (GtkWidget *) move_note_popover->action_area);

  move_note_popover->activate_button = (GtkButton *) gtk_button_new_with_label(i18n("apply"));

  gtk_box_append(move_note_popover->action_area,
		 (GtkWidget *) move_note_popover->activate_button);

  g_signal_connect(move_note_popover->activate_button, "clicked",
		   G_CALLBACK(ags_move_note_popover_activate_button_callback), move_note_popover);

  gtk_popover_set_default_widget((GtkPopover *) move_note_popover,
				 (GtkWidget *) move_note_popover->activate_button);
}

xmlNode*
ags_move_note_popover_xml_compose(AgsConnectable *connectable)
{
  AgsMoveNotePopover *move_note_popover;
  
  xmlNode *node;

  gchar *str;
  
  move_note_popover = AGS_MOVE_NOTE_POPOVER(connectable);

  node = xmlNewNode(NULL,
		    BAD_CAST "ags-move-note-popover");

  /* relative */
  str = g_strdup_printf("%s",
			((gtk_check_button_get_active(move_note_popover->relative)) ? "true": "false"));
  
  xmlNewProp(node,
	     BAD_CAST "relative",
	     BAD_CAST str);
  
  g_free(str);

  /* absolute */
  str = g_strdup_printf("%s",
			((gtk_check_button_get_active(move_note_popover->absolute)) ? "true": "false"));
  
  xmlNewProp(node,
	     BAD_CAST "absolute",
	     BAD_CAST str);
  
  g_free(str);

  /* move x */
  str = g_strdup_printf("%f",
			gtk_spin_button_get_value(move_note_popover->move_x));
  
  xmlNewProp(node,
	     BAD_CAST "move-x",
	     BAD_CAST str);
  
  g_free(str);

  /* move y */
  str = g_strdup_printf("%f",
			gtk_spin_button_get_value(move_note_popover->move_y));
  
  xmlNewProp(node,
	     BAD_CAST "move-y",
	     BAD_CAST str);
  
  g_free(str);
  
  return(node);
}

void
ags_move_note_popover_xml_parse(AgsConnectable *connectable,
				xmlNode *node)
{
  AgsMoveNotePopover *move_note_popover;
  
  gchar *str;
  
  move_note_popover = AGS_MOVE_NOTE_POPOVER(connectable);

  /* relative */
  str = xmlGetProp(node,
		   "relative");

  gtk_check_button_set_active(move_note_popover->relative,
			      ((!g_ascii_strncasecmp(str, "false", 6) == FALSE) ? TRUE: FALSE));

  xmlFree(str);

  /* absolute */
  str = xmlGetProp(node,
		   "absolute");

  gtk_check_button_set_active(move_note_popover->absolute,
			      ((!g_ascii_strncasecmp(str, "false", 6) == FALSE) ? TRUE: FALSE));

  xmlFree(str);

  /* move x */
  str = xmlGetProp(node,
		   "move-x");

  gtk_spin_button_set_value(move_note_popover->move_x,
			    g_ascii_strtod(str,
					   NULL));
  
  xmlFree(str);

  /* move y */
  str = xmlGetProp(node,
		   "move-y");

  gtk_spin_button_set_value(move_note_popover->move_y,
			    g_ascii_strtod(str,
					   NULL));
  
  xmlFree(str);
}

gboolean
ags_move_note_popover_is_connected(AgsConnectable *connectable)
{
  AgsMoveNotePopover *move_note_popover;
  
  gboolean is_connected;
  
  move_note_popover = AGS_MOVE_NOTE_POPOVER(connectable);

  /* check is connected */
  is_connected = ((AGS_CONNECTABLE_CONNECTED & (move_note_popover->connectable_flags)) != 0) ? TRUE: FALSE;

  return(is_connected);
}

void
ags_move_note_popover_connect(AgsConnectable *connectable)
{
  AgsMoveNotePopover *move_note_popover;

  move_note_popover = AGS_MOVE_NOTE_POPOVER(connectable);

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  move_note_popover->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  g_signal_connect_after(move_note_popover->relative, "toggled",
			 G_CALLBACK(ags_move_note_popover_relative_callback), move_note_popover);

  g_signal_connect_after(move_note_popover->absolute, "toggled",
			 G_CALLBACK(ags_move_note_popover_absolute_callback), move_note_popover);
}

void
ags_move_note_popover_disconnect(AgsConnectable *connectable)
{
  AgsMoveNotePopover *move_note_popover;

  move_note_popover = AGS_MOVE_NOTE_POPOVER(connectable);

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  move_note_popover->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  g_object_disconnect(G_OBJECT(move_note_popover->relative),
		      "any_signal::toggled",
		      G_CALLBACK(ags_move_note_popover_relative_callback),
		      move_note_popover,
		      NULL);

  g_object_disconnect(G_OBJECT(move_note_popover->absolute),
		      "any_signal::toggled",
		      G_CALLBACK(ags_move_note_popover_absolute_callback),
		      move_note_popover,
		      NULL);
}

void
ags_move_note_popover_finalize(GObject *gobject)
{
  AgsMoveNotePopover *move_note_popover;

  move_note_popover = (AgsMoveNotePopover *) gobject;
  
  G_OBJECT_CLASS(ags_move_note_popover_parent_class)->finalize(gobject);
}

void
ags_move_note_popover_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_move_note_popover_apply(AgsApplicable *applicable)
{
  AgsMoveNotePopover *move_note_popover;
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
  
  move_note_popover = AGS_MOVE_NOTE_POPOVER(applicable);

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
  move_x = gtk_spin_button_get_value_as_int(move_note_popover->move_x);
  move_y = gtk_spin_button_get_value_as_int(move_note_popover->move_y);

  relative = gtk_check_button_get_active(move_note_popover->relative);
  absolute = gtk_check_button_get_active(move_note_popover->absolute);
  
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
ags_move_note_popover_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

gboolean
ags_move_note_popover_key_pressed_callback(GtkEventControllerKey *event_controller,
					   guint keyval,
					   guint keycode,
					   GdkModifierType state,
					   AgsMoveNotePopover *move_note_popover)
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
ags_move_note_popover_key_released_callback(GtkEventControllerKey *event_controller,
					    guint keyval,
					    guint keycode,
					    GdkModifierType state,
					    AgsMoveNotePopover *move_note_popover)
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
	gtk_popover_popdown((GtkPopover *) move_note_popover);	
      }
      break;
    }
  }
}

gboolean
ags_move_note_popover_modifiers_callback(GtkEventControllerKey *event_controller,
					 GdkModifierType keyval,
					 AgsMoveNotePopover *move_note_popover)
{
  return(FALSE);
}

void
ags_move_note_popover_activate_button_callback(GtkButton *activate_button,
					       AgsMoveNotePopover *move_note_popover)
{
  ags_applicable_apply(AGS_APPLICABLE(move_note_popover));
}

/**
 * ags_move_note_popover_new:
 *
 * Create a new #AgsMoveNotePopover.
 *
 * Returns: a new #AgsMoveNotePopover
 *
 * Since: 6.11.0
 */
AgsMoveNotePopover*
ags_move_note_popover_new()
{
  AgsMoveNotePopover *move_note_popover;

  move_note_popover = (AgsMoveNotePopover *) g_object_new(AGS_TYPE_MOVE_NOTE_POPOVER,
							  NULL);

  return(move_note_popover);
}

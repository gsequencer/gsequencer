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

#include <ags/app/editor/ags_select_note_popover.h>
#include <ags/app/editor/ags_select_note_popover_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_composite_editor.h>
#include <ags/app/ags_machine.h>

#include <ags/app/editor/ags_notation_edit.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

#include <ags/i18n.h>

void ags_select_note_popover_class_init(AgsSelectNotePopoverClass *select_note_popover);
void ags_select_note_popover_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_select_note_popover_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_select_note_popover_init(AgsSelectNotePopover *select_note_popover);
void ags_select_note_popover_finalize(GObject *gobject);

xmlNode* ags_select_note_popover_xml_compose(AgsConnectable *connectable);
void ags_select_note_popover_xml_parse(AgsConnectable *connectable,
				       xmlNode *node);

gboolean ags_select_note_popover_is_connected(AgsConnectable *connectable);
void ags_select_note_popover_connect(AgsConnectable *connectable);
void ags_select_note_popover_disconnect(AgsConnectable *connectable);

void ags_select_note_popover_set_update(AgsApplicable *applicable, gboolean update);
void ags_select_note_popover_apply(AgsApplicable *applicable);
void ags_select_note_popover_reset(AgsApplicable *applicable);

gboolean ags_select_note_popover_key_pressed_callback(GtkEventControllerKey *event_controller,
						      guint keyval,
						      guint keycode,
						      GdkModifierType state,
						      AgsSelectNotePopover *select_note_popover);
void ags_select_note_popover_key_released_callback(GtkEventControllerKey *event_controller,
						   guint keyval,
						   guint keycode,
						   GdkModifierType state,
						   AgsSelectNotePopover *select_note_popover);
gboolean ags_select_note_popover_modifiers_callback(GtkEventControllerKey *event_controller,
						    GdkModifierType keyval,
						    AgsSelectNotePopover *select_note_popover);

void ags_select_note_popover_activate_button_callback(GtkButton *activate_button,
						      AgsSelectNotePopover *select_note_popover);

/**
 * SECTION:ags_select_note_popover
 * @short_description: select tool
 * @title: AgsSelectNotePopover
 * @section_id:
 * @include: ags/app/editor/ags_select_note_popover.h
 *
 * The #AgsSelectNotePopover lets you select notes.
 */

static gpointer ags_select_note_popover_parent_class = NULL;

GType
ags_select_note_popover_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_select_note_popover = 0;

    static const GTypeInfo ags_select_note_popover_info = {
      sizeof (AgsSelectNotePopoverClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_select_note_popover_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSelectNotePopover),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_select_note_popover_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_select_note_popover_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_select_note_popover_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_select_note_popover = g_type_register_static(GTK_TYPE_POPOVER,
							  "AgsSelectNotePopover", &ags_select_note_popover_info,
							  0);
    
    g_type_add_interface_static(ags_type_select_note_popover,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_select_note_popover,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__static, ags_type_select_note_popover);
  }

  return(g_define_type_id__static);
}

void
ags_select_note_popover_class_init(AgsSelectNotePopoverClass *select_note_popover)
{
  GObjectClass *gobject;

  ags_select_note_popover_parent_class = g_type_class_peek_parent(select_note_popover);

  /* GObjectClass */
  gobject = (GObjectClass *) select_note_popover;

  gobject->finalize = ags_select_note_popover_finalize;
}

void
ags_select_note_popover_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = NULL;
  connectable->has_resource = NULL;

  connectable->is_ready = NULL;
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;

  connectable->list_resource = NULL;
  connectable->xml_compose = ags_select_note_popover_xml_compose;
  connectable->xml_parse = ags_select_note_popover_xml_parse;

  connectable->is_connected = ags_select_note_popover_is_connected;  
  connectable->connect = ags_select_note_popover_connect;
  connectable->disconnect = ags_select_note_popover_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_select_note_popover_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_select_note_popover_set_update;
  applicable->apply = ags_select_note_popover_apply;
  applicable->reset = ags_select_note_popover_reset;
}

void
ags_select_note_popover_init(AgsSelectNotePopover *select_note_popover)
{
  GtkBox *vbox;
  GtkBox *hbox;
  GtkLabel *label;
  
  GtkEventController *event_controller;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();

  select_note_popover->connectable_flags = 0;

  event_controller = gtk_event_controller_key_new();
  gtk_widget_add_controller((GtkWidget *) select_note_popover,
			    event_controller);

  g_signal_connect(event_controller, "key-pressed",
		   G_CALLBACK(ags_select_note_popover_key_pressed_callback), select_note_popover);
  
  g_signal_connect(event_controller, "key-released",
		   G_CALLBACK(ags_select_note_popover_key_released_callback), select_note_popover);

  g_signal_connect(event_controller, "modifiers",
		   G_CALLBACK(ags_select_note_popover_modifiers_callback), select_note_popover);
  
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_popover_set_child((GtkPopover *) select_note_popover,
			(GtkWidget *) vbox);

  /* copy selection */
  select_note_popover->copy_selection = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("copy selection"));
  gtk_check_button_set_active(select_note_popover->copy_selection,
			      TRUE);
  gtk_box_append(vbox,
		 (GtkWidget *) select_note_popover->copy_selection);  

  /* select x0 - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(vbox,
		 (GtkWidget *) hbox);

  /* select x0 - label */
  label = (GtkLabel *) gtk_label_new(i18n("select x0"));
  gtk_box_append(hbox,
		 (GtkWidget *) label);

  /* select x0 - spin button */
  select_note_popover->select_x0 = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
										    AGS_SELECT_NOTE_MAX_BEATS,
										    1.0);
  gtk_spin_button_set_value(select_note_popover->select_x0,
			    0.0);
  gtk_box_append(hbox,
		 (GtkWidget *) select_note_popover->select_x0);
  
  /* select y0 - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(vbox,
		 (GtkWidget *) hbox);

  /* select y0 - label */
  label = (GtkLabel *) gtk_label_new(i18n("select y0"));
  gtk_box_append(hbox,
		 (GtkWidget *) label);

  /* select y0 - spin button */
  select_note_popover->select_y0 = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
										    AGS_SELECT_NOTE_MAX_KEYS,
										    1.0);
  gtk_spin_button_set_value(select_note_popover->select_y0,
			    0.0);
  gtk_box_append(hbox,
		 (GtkWidget *) select_note_popover->select_y0);

  /* select x1 - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(vbox,
		 (GtkWidget *) hbox);

  /* select x1 - label */
  label = (GtkLabel *) gtk_label_new(i18n("select x1"));
  gtk_box_append(hbox,
		 (GtkWidget *) label);

  /* select x1 - spin button */
  select_note_popover->select_x1 = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
										    AGS_SELECT_NOTE_MAX_BEATS,
										    1.0);
  gtk_spin_button_set_value(select_note_popover->select_x1,
			    0.0);
  gtk_box_append(hbox,
		 (GtkWidget *) select_note_popover->select_x1);

  /* select y1 - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(vbox,
		 (GtkWidget *) hbox);

  /* select y1 - label */
  label = (GtkLabel *) gtk_label_new(i18n("select y1"));
  gtk_box_append(hbox,
		 (GtkWidget *) label);

  /* select y1 - spin button */
  select_note_popover->select_y1 = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
										    AGS_SELECT_NOTE_MAX_KEYS,
										    1.0);
  gtk_spin_button_set_value(select_note_popover->select_y1,
			    0.0);
  gtk_box_append(hbox,
		 (GtkWidget *) select_note_popover->select_y1);
  
  /* buttons */
  select_note_popover->action_area = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
							    AGS_UI_PROVIDER_DEFAULT_SPACING);
  
  gtk_widget_set_halign((GtkWidget *) select_note_popover->action_area,
			GTK_ALIGN_END);

  gtk_box_append(vbox,
		 (GtkWidget *) select_note_popover->action_area);

  select_note_popover->activate_button = (GtkButton *) gtk_button_new_with_label(i18n("apply"));

  gtk_box_append(select_note_popover->action_area,
		 (GtkWidget *) select_note_popover->activate_button);

  g_signal_connect(select_note_popover->activate_button, "clicked",
		   G_CALLBACK(ags_select_note_popover_activate_button_callback), select_note_popover);

  gtk_popover_set_default_widget((GtkPopover *) select_note_popover,
				 (GtkWidget *) select_note_popover->activate_button);
}

xmlNode*
ags_select_note_popover_xml_compose(AgsConnectable *connectable)
{
  AgsSelectNotePopover *select_note_popover;
  
  xmlNode *node;

  gchar *str;
  
  select_note_popover = AGS_SELECT_NOTE_POPOVER(connectable);

  node = xmlNewNode(NULL,
		    BAD_CAST "ags-select-note-popover");

  /* absolute */
  str = g_strdup_printf("%s",
			((gtk_check_button_get_active(select_note_popover->copy_selection)) ? "true": "false"));
  
  xmlNewProp(node,
	     BAD_CAST "copy-selection",
	     BAD_CAST str);
  
  g_free(str);

  /* select x0 */
  str = g_strdup_printf("%f",
			gtk_spin_button_get_value(select_note_popover->select_x0));
  
  xmlNewProp(node,
	     BAD_CAST "select-x0",
	     BAD_CAST str);
  
  g_free(str);

  /* select y0 */
  str = g_strdup_printf("%f",
			gtk_spin_button_get_value(select_note_popover->select_y0));
  
  xmlNewProp(node,
	     BAD_CAST "select-y0",
	     BAD_CAST str);
  
  g_free(str);

  /* select x1 */
  str = g_strdup_printf("%f",
			gtk_spin_button_get_value(select_note_popover->select_x1));
  
  xmlNewProp(node,
	     BAD_CAST "select-x1",
	     BAD_CAST str);
  
  g_free(str);

  /* select y1 */
  str = g_strdup_printf("%f",
			gtk_spin_button_get_value(select_note_popover->select_y1));
  
  xmlNewProp(node,
	     BAD_CAST "select-y1",
	     BAD_CAST str);
  
  g_free(str);

  return(node);
}

void
ags_select_note_popover_xml_parse(AgsConnectable *connectable,
				xmlNode *node)
{
  AgsSelectNotePopover *select_note_popover;
  
  gchar *str;
  
  select_note_popover = AGS_SELECT_NOTE_POPOVER(connectable);

  /* absolute */
  str = xmlGetProp(node,
		   "copy-selection");

  gtk_check_button_set_active(select_note_popover->copy_selection,
			      ((!g_ascii_strncasecmp(str, "false", 6) == FALSE) ? TRUE: FALSE));

  xmlFree(str);

  /* select x0 */
  str = xmlGetProp(node,
		   "select-x0");

  gtk_spin_button_set_value(select_note_popover->select_x0,
			    g_ascii_strtod(str,
					   NULL));
  
  xmlFree(str);

  /* select y0 */
  str = xmlGetProp(node,
		   "select-y0");

  gtk_spin_button_set_value(select_note_popover->select_y0,
			    g_ascii_strtod(str,
					   NULL));
  
  xmlFree(str);

  /* select x1 */
  str = xmlGetProp(node,
		   "select-x1");

  gtk_spin_button_set_value(select_note_popover->select_x1,
			    g_ascii_strtod(str,
					   NULL));
  
  xmlFree(str);

  /* select y1 */
  str = xmlGetProp(node,
		   "select-y1");

  gtk_spin_button_set_value(select_note_popover->select_y1,
			    g_ascii_strtod(str,
					   NULL));
  
  xmlFree(str);
}

gboolean
ags_select_note_popover_is_connected(AgsConnectable *connectable)
{
  AgsSelectNotePopover *select_note_popover;
  
  gboolean is_connected;
  
  select_note_popover = AGS_SELECT_NOTE_POPOVER(connectable);

  /* check is connected */
  is_connected = ((AGS_CONNECTABLE_CONNECTED & (select_note_popover->connectable_flags)) != 0) ? TRUE: FALSE;

  return(is_connected);
}

void
ags_select_note_popover_connect(AgsConnectable *connectable)
{
  AgsSelectNotePopover *select_note_popover;

  select_note_popover = AGS_SELECT_NOTE_POPOVER(connectable);

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  select_note_popover->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
}

void
ags_select_note_popover_disconnect(AgsConnectable *connectable)
{
  AgsSelectNotePopover *select_note_popover;

  select_note_popover = AGS_SELECT_NOTE_POPOVER(connectable);

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  select_note_popover->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
}

void
ags_select_note_popover_finalize(GObject *gobject)
{
  AgsSelectNotePopover *select_note_popover;

  select_note_popover = (AgsSelectNotePopover *) gobject;
  
  G_OBJECT_CLASS(ags_select_note_popover_parent_class)->finalize(gobject);
}

void
ags_select_note_popover_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_select_note_popover_apply(AgsApplicable *applicable)
{
  AgsSelectNotePopover *select_note_popover;
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  AgsMachine *machine;
  AgsNotebook *notebook;

  AgsAudio *audio;

  AgsTimestamp *timestamp;

  AgsApplicationContext *application_context;
  
  xmlDoc *clipboard;
  xmlNode *audio_node, *notation_node;

  GList *start_list_notation, *list_notation;

  xmlChar *buffer;
  
  int size;
  guint x0, y0;
  guint x1, y1;
  gint i;
  
  gboolean copy_selection;
  
  select_note_popover = AGS_SELECT_NOTE_POPOVER(applicable);

  /* application context */
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  composite_editor = window->composite_editor;

  machine = composite_editor->selected_machine;

  notebook = composite_editor->notation_edit->channel_selector;
  
  if(machine == NULL){
    return;
  }

  audio = machine->audio;

  g_object_get(audio,
	       "notation", &start_list_notation,
	       NULL);

  /* get some values */
  copy_selection = gtk_check_button_get_active(select_note_popover->copy_selection);

  x0 = gtk_spin_button_get_value_as_int(select_note_popover->select_x0);
  y0 = gtk_spin_button_get_value_as_int(select_note_popover->select_y0);

  x1 = gtk_spin_button_get_value_as_int(select_note_popover->select_x1);
  y1 = gtk_spin_button_get_value_as_int(select_note_popover->select_y1);
  
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
  
  while((i = ags_notebook_next_active_tab(notebook,
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

    gdk_clipboard_set_text(gdk_display_get_clipboard(gdk_display_get_default()),
			   buffer);
    
    xmlFreeDoc(clipboard);
  }
}

void
ags_select_note_popover_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

gboolean
ags_select_note_popover_key_pressed_callback(GtkEventControllerKey *event_controller,
					     guint keyval,
					     guint keycode,
					     GdkModifierType state,
					     AgsSelectNotePopover *select_note_popover)
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
ags_select_note_popover_key_released_callback(GtkEventControllerKey *event_controller,
					      guint keyval,
					      guint keycode,
					      GdkModifierType state,
					      AgsSelectNotePopover *select_note_popover)
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
	gtk_popover_popdown((GtkPopover *) select_note_popover);	
      }
      break;
    }
  }
}

gboolean
ags_select_note_popover_modifiers_callback(GtkEventControllerKey *event_controller,
					   GdkModifierType keyval,
					   AgsSelectNotePopover *select_note_popover)
{
  return(FALSE);
}

void
ags_select_note_popover_activate_button_callback(GtkButton *activate_button,
						 AgsSelectNotePopover *select_note_popover)
{
  ags_applicable_apply(AGS_APPLICABLE(select_note_popover));
}

/**
 * ags_select_note_popover_new:
 *
 * Create a new #AgsSelectNotePopover.
 *
 * Returns: a new #AgsSelectNotePopover
 *
 * Since: 6.11.0
 */
AgsSelectNotePopover*
ags_select_note_popover_new()
{
  AgsSelectNotePopover *select_note_popover;

  select_note_popover = (AgsSelectNotePopover *) g_object_new(AGS_TYPE_SELECT_NOTE_POPOVER,
							      NULL);

  return(select_note_popover);
}

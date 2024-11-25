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

#include <ags/app/editor/ags_position_notation_cursor_popover.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_composite_editor.h>
#include <ags/app/ags_machine.h>

#include <ags/app/editor/ags_notation_edit.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

#include <ags/i18n.h>

void ags_position_notation_cursor_popover_class_init(AgsPositionNotationCursorPopoverClass *position_notation_cursor_popover);
void ags_position_notation_cursor_popover_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_position_notation_cursor_popover_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_position_notation_cursor_popover_init(AgsPositionNotationCursorPopover *position_notation_cursor_popover);
void ags_position_notation_cursor_popover_finalize(GObject *gobject);

xmlNode* ags_position_notation_cursor_popover_xml_compose(AgsConnectable *connectable);
void ags_position_notation_cursor_popover_xml_parse(AgsConnectable *connectable,
						    xmlNode *node);

gboolean ags_position_notation_cursor_popover_is_connected(AgsConnectable *connectable);
void ags_position_notation_cursor_popover_connect(AgsConnectable *connectable);
void ags_position_notation_cursor_popover_disconnect(AgsConnectable *connectable);

void ags_position_notation_cursor_popover_set_update(AgsApplicable *applicable, gboolean update);
void ags_position_notation_cursor_popover_apply(AgsApplicable *applicable);
void ags_position_notation_cursor_popover_reset(AgsApplicable *applicable);

gboolean ags_position_notation_cursor_popover_key_pressed_callback(GtkEventControllerKey *event_controller,
								   guint keyval,
								   guint keycode,
								   GdkModifierType state,
								   AgsPositionNotationCursorPopover *position_notation_cursor_popover);
void ags_position_notation_cursor_popover_key_released_callback(GtkEventControllerKey *event_controller,
								guint keyval,
								guint keycode,
								GdkModifierType state,
								AgsPositionNotationCursorPopover *position_notation_cursor_popover);
gboolean ags_position_notation_cursor_popover_modifiers_callback(GtkEventControllerKey *event_controller,
								 GdkModifierType keyval,
								 AgsPositionNotationCursorPopover *position_notation_cursor_popover);

void ags_position_notation_cursor_popover_activate_button_callback(GtkButton *activate_button,
								   AgsPositionNotationCursorPopover *position_notation_cursor_popover);

/**
 * SECTION:ags_position_notation_cursor_popover
 * @short_description: crop tool
 * @title: AgsPositionNotationCursorPopover
 * @section_id:
 * @include: ags/app/editor/ags_position_notation_cursor_popover.h
 *
 * The #AgsPositionNotationCursorPopover lets you crop notes.
 */

static gpointer ags_position_notation_cursor_popover_parent_class = NULL;

GType
ags_position_notation_cursor_popover_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_position_notation_cursor_popover = 0;

    static const GTypeInfo ags_position_notation_cursor_popover_info = {
      sizeof (AgsPositionNotationCursorPopoverClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_position_notation_cursor_popover_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPositionNotationCursorPopover),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_position_notation_cursor_popover_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_position_notation_cursor_popover_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_position_notation_cursor_popover_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_position_notation_cursor_popover = g_type_register_static(GTK_TYPE_POPOVER,
								       "AgsPositionNotationCursorPopover", &ags_position_notation_cursor_popover_info,
								       0);
    
    g_type_add_interface_static(ags_type_position_notation_cursor_popover,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_position_notation_cursor_popover,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__static, ags_type_position_notation_cursor_popover);
  }

  return g_define_type_id__static;
}

void
ags_position_notation_cursor_popover_class_init(AgsPositionNotationCursorPopoverClass *position_notation_cursor_popover)
{
  GObjectClass *gobject;

  ags_position_notation_cursor_popover_parent_class = g_type_class_peek_parent(position_notation_cursor_popover);

  /* GObjectClass */
  gobject = (GObjectClass *) position_notation_cursor_popover;

  gobject->finalize = ags_position_notation_cursor_popover_finalize;
}

void
ags_position_notation_cursor_popover_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = NULL;
  connectable->has_resource = NULL;

  connectable->is_ready = NULL;
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;

  connectable->list_resource = NULL;
  connectable->xml_compose = ags_position_notation_cursor_popover_xml_compose;
  connectable->xml_parse = ags_position_notation_cursor_popover_xml_parse;

  connectable->is_connected = ags_position_notation_cursor_popover_is_connected;  
  connectable->connect = ags_position_notation_cursor_popover_connect;
  connectable->disconnect = ags_position_notation_cursor_popover_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_position_notation_cursor_popover_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_position_notation_cursor_popover_set_update;
  applicable->apply = ags_position_notation_cursor_popover_apply;
  applicable->reset = ags_position_notation_cursor_popover_reset;
}

void
ags_position_notation_cursor_popover_init(AgsPositionNotationCursorPopover *position_notation_cursor_popover)
{
  GtkBox *vbox;
  GtkBox *hbox;
  GtkLabel *label;
  
  GtkEventController *event_controller;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();

  position_notation_cursor_popover->connectable_flags = 0;

  event_controller = gtk_event_controller_key_new();
  gtk_widget_add_controller((GtkWidget *) position_notation_cursor_popover,
			    event_controller);

  g_signal_connect(event_controller, "key-pressed",
		   G_CALLBACK(ags_position_notation_cursor_popover_key_pressed_callback), position_notation_cursor_popover);
  
  g_signal_connect(event_controller, "key-released",
		   G_CALLBACK(ags_position_notation_cursor_popover_key_released_callback), position_notation_cursor_popover);

  g_signal_connect(event_controller, "modifiers",
		   G_CALLBACK(ags_position_notation_cursor_popover_modifiers_callback), position_notation_cursor_popover);
  
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_popover_set_child((GtkPopover *) position_notation_cursor_popover,
			(GtkWidget *) vbox);

  /* set focus */
  position_notation_cursor_popover->set_focus = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("set focus"));
  gtk_check_button_set_active(position_notation_cursor_popover->set_focus,
			      TRUE);
  gtk_box_append(vbox,
		 (GtkWidget *) position_notation_cursor_popover->set_focus);

  /* position x - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(vbox,
		 (GtkWidget *) hbox);

  /* position x - label */
  label = (GtkLabel *) gtk_label_new(i18n("position x"));
  gtk_box_append(hbox,
		 (GtkWidget *) label);

  /* position x - spin button */
  position_notation_cursor_popover->position_x = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
												  AGS_POSITION_NOTATION_CURSOR_MAX_BEATS,
												  1.0);
  gtk_spin_button_set_value(position_notation_cursor_popover->position_x,
			    0.0);
  gtk_box_append((GtkBox *) hbox,
		 (GtkWidget *) position_notation_cursor_popover->position_x);
  
  /* position y - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(vbox,
		 (GtkWidget *) hbox);

  /* position y - label */
  label = (GtkLabel *) gtk_label_new(i18n("position y"));
  gtk_box_append(hbox,
		 (GtkWidget *) label);

  /* position y - spin button */
  position_notation_cursor_popover->position_y = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
												  AGS_POSITION_NOTATION_CURSOR_MAX_KEYS,
												  1.0);
  gtk_spin_button_set_value(position_notation_cursor_popover->position_y,
			    0.0);
  gtk_box_append(hbox,
		 (GtkWidget *) position_notation_cursor_popover->position_y);
  
  /* buttons */
  position_notation_cursor_popover->action_area = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
									 AGS_UI_PROVIDER_DEFAULT_SPACING);
  
  gtk_widget_set_halign((GtkWidget *) position_notation_cursor_popover->action_area,
			GTK_ALIGN_END);

  gtk_box_append(vbox,
		 (GtkWidget *) position_notation_cursor_popover->action_area);

  position_notation_cursor_popover->activate_button = (GtkButton *) gtk_button_new_with_label(i18n("apply"));

  gtk_box_append(position_notation_cursor_popover->action_area,
		 (GtkWidget *) position_notation_cursor_popover->activate_button);

  g_signal_connect(position_notation_cursor_popover->activate_button, "clicked",
		   G_CALLBACK(ags_position_notation_cursor_popover_activate_button_callback), position_notation_cursor_popover);

  gtk_popover_set_default_widget((GtkPopover *) position_notation_cursor_popover,
				 (GtkWidget *) position_notation_cursor_popover->activate_button);
}

xmlNode*
ags_position_notation_cursor_popover_xml_compose(AgsConnectable *connectable)
{
  AgsPositionNotationCursorPopover *position_notation_cursor_popover;
  
  xmlNode *node;

  gchar *str;
  
  position_notation_cursor_popover = AGS_POSITION_NOTATION_CURSOR_POPOVER(connectable);

  node = xmlNewNode(NULL,
		    BAD_CAST "ags-position-notation-cursor-popover");

  /* set focus */
  str = g_strdup_printf("%s",
			((gtk_check_button_get_active(position_notation_cursor_popover->set_focus)) ? "true": "false"));
  
  xmlNewProp(node,
	     BAD_CAST "set-focus",
	     BAD_CAST str);
  
  g_free(str);

  /* position x */
  str = g_strdup_printf("%f",
			gtk_spin_button_get_value(position_notation_cursor_popover->position_x));
  
  xmlNewProp(node,
	     BAD_CAST "position-x",
	     BAD_CAST str);
  
  g_free(str);

  /* position y */
  str = g_strdup_printf("%f",
			gtk_spin_button_get_value(position_notation_cursor_popover->position_y));
  
  xmlNewProp(node,
	     BAD_CAST "position-y",
	     BAD_CAST str);
  
  g_free(str);
  
  return(node);
}

void
ags_position_notation_cursor_popover_xml_parse(AgsConnectable *connectable,
					       xmlNode *node)
{  
  AgsPositionNotationCursorPopover *position_notation_cursor_popover;

  xmlChar *str;
  
  position_notation_cursor_popover = AGS_POSITION_NOTATION_CURSOR_POPOVER(connectable);

  /* set-focus */
  str = xmlGetProp(node,
		   "set-focus");

  gtk_check_button_set_active(position_notation_cursor_popover->set_focus,
			      ((!g_ascii_strncasecmp(str, "false", 6) == FALSE) ? TRUE: FALSE));

  xmlFree(str);

  /* position x */
  str = xmlGetProp(node,
		   "position-x");

  gtk_spin_button_set_value(position_notation_cursor_popover->position_x,
			    g_ascii_strtod(str,
					   NULL));
  
  xmlFree(str);

  /* position y */
  str = xmlGetProp(node,
		   "position-y");

  gtk_spin_button_set_value(position_notation_cursor_popover->position_y,
			    g_ascii_strtod(str,
					   NULL));
  
  xmlFree(str);
}

gboolean
ags_position_notation_cursor_popover_is_connected(AgsConnectable *connectable)
{
  AgsPositionNotationCursorPopover *position_notation_cursor_popover;
  
  gboolean is_connected;
  
  position_notation_cursor_popover = AGS_POSITION_NOTATION_CURSOR_POPOVER(connectable);

  /* check is connected */
  is_connected = ((AGS_CONNECTABLE_CONNECTED & (position_notation_cursor_popover->connectable_flags)) != 0) ? TRUE: FALSE;

  return(is_connected);
}

void
ags_position_notation_cursor_popover_connect(AgsConnectable *connectable)
{
  AgsPositionNotationCursorPopover *position_notation_cursor_popover;

  position_notation_cursor_popover = AGS_POSITION_NOTATION_CURSOR_POPOVER(connectable);

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  position_notation_cursor_popover->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
}

void
ags_position_notation_cursor_popover_disconnect(AgsConnectable *connectable)
{
  AgsPositionNotationCursorPopover *position_notation_cursor_popover;

  position_notation_cursor_popover = AGS_POSITION_NOTATION_CURSOR_POPOVER(connectable);

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  position_notation_cursor_popover->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
}

void
ags_position_notation_cursor_popover_finalize(GObject *gobject)
{
  AgsPositionNotationCursorPopover *position_notation_cursor_popover;

  position_notation_cursor_popover = (AgsPositionNotationCursorPopover *) gobject;
  
  G_OBJECT_CLASS(ags_position_notation_cursor_popover_parent_class)->finalize(gobject);
}

void
ags_position_notation_cursor_popover_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_position_notation_cursor_popover_apply(AgsApplicable *applicable)
{
  AgsPositionNotationCursorPopover *position_notation_cursor_popover;
  AgsWindow *window;
  AgsMachine *machine;
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *composite_toolbar;
  AgsNotationEdit *notation_edit;
  GtkWidget *widget;
  
  GtkAdjustment *hadjustment;
  
  AgsApplicationContext *application_context;

  gdouble zoom;
  //NOTE:JK: some deco
  //  guint map_height, height;
  guint history;
  guint x; //, y;
  
  position_notation_cursor_popover = AGS_POSITION_NOTATION_CURSOR_POPOVER(applicable);

  /* application context */
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
    
  composite_editor = window->composite_editor;

  composite_toolbar = composite_editor->toolbar;
    
  machine = composite_editor->selected_machine;

  history = gtk_combo_box_get_active(GTK_COMBO_BOX(composite_toolbar->zoom));
  
  if(machine == NULL){
    return;
  }
  
  zoom = exp2((double) history - 2.0);
  
  x = gtk_spin_button_get_value_as_int(position_notation_cursor_popover->position_x);

  notation_edit = (AgsNotationEdit *) composite_editor->notation_edit->edit;

  if(notation_edit != NULL){
    notation_edit->cursor_position_x = 16 * x;
    notation_edit->cursor_position_y = 0.0;
  }

  hadjustment = gtk_range_get_adjustment(GTK_RANGE(notation_edit->hscrollbar));

  widget = (GtkWidget *) notation_edit->drawing_area;
    
  /* make visible */  
  if(hadjustment != NULL){
    gtk_adjustment_set_value(hadjustment,
			     ((x * 16 * 64 / zoom) * (gtk_adjustment_get_upper(hadjustment) / (AGS_NOTATION_DEFAULT_LENGTH / zoom))));
  }

  if(gtk_check_button_get_active(position_notation_cursor_popover->set_focus)){
    gtk_widget_grab_focus(widget);
  }
}

void
ags_position_notation_cursor_popover_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

gboolean
ags_position_notation_cursor_popover_key_pressed_callback(GtkEventControllerKey *event_controller,
							  guint keyval,
							  guint keycode,
							  GdkModifierType state,
							  AgsPositionNotationCursorPopover *position_notation_cursor_popover)
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
ags_position_notation_cursor_popover_key_released_callback(GtkEventControllerKey *event_controller,
							   guint keyval,
							   guint keycode,
							   GdkModifierType state,
							   AgsPositionNotationCursorPopover *position_notation_cursor_popover)
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
	gtk_popover_popdown((GtkPopover *) position_notation_cursor_popover);	
      }
      break;
    }
  }
}

gboolean
ags_position_notation_cursor_popover_modifiers_callback(GtkEventControllerKey *event_controller,
							GdkModifierType keyval,
							AgsPositionNotationCursorPopover *position_notation_cursor_popover)
{
  return(FALSE);
}

void
ags_position_notation_cursor_popover_activate_button_callback(GtkButton *activate_button,
							      AgsPositionNotationCursorPopover *position_notation_cursor_popover)
{
  ags_applicable_apply(AGS_APPLICABLE(position_notation_cursor_popover));
}

/**
 * ags_position_notation_cursor_popover_new:
 *
 * Create a new #AgsPositionNotationCursorPopover.
 *
 * Returns: a new #AgsPositionNotationCursorPopover
 *
 * Since: 6.11.0
 */
AgsPositionNotationCursorPopover*
ags_position_notation_cursor_popover_new()
{
  AgsPositionNotationCursorPopover *position_notation_cursor_popover;

  position_notation_cursor_popover = (AgsPositionNotationCursorPopover *) g_object_new(AGS_TYPE_POSITION_NOTATION_CURSOR_POPOVER,
										       NULL);

  return(position_notation_cursor_popover);
}

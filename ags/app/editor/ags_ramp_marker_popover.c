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

#include <ags/app/editor/ags_ramp_marker_popover.h>
#include <ags/app/editor/ags_ramp_marker_popover_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_composite_editor.h>
#include <ags/app/ags_machine.h>

#include <ags/app/editor/ags_automation_edit.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

#include <ags/i18n.h>

void ags_ramp_marker_popover_class_init(AgsRampMarkerPopoverClass *ramp_marker_popover);
void ags_ramp_marker_popover_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_ramp_marker_popover_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_ramp_marker_popover_init(AgsRampMarkerPopover *ramp_marker_popover);
void ags_ramp_marker_popover_finalize(GObject *gobject);

gboolean ags_ramp_marker_popover_is_connected(AgsConnectable *connectable);
void ags_ramp_marker_popover_connect(AgsConnectable *connectable);
void ags_ramp_marker_popover_disconnect(AgsConnectable *connectable);

void ags_ramp_marker_popover_set_update(AgsApplicable *applicable, gboolean update);
void ags_ramp_marker_popover_apply(AgsApplicable *applicable);
void ags_ramp_marker_popover_reset(AgsApplicable *applicable);

gboolean ags_ramp_marker_popover_key_pressed_callback(GtkEventControllerKey *event_controller,
						      guint keyval,
						      guint keycode,
						      GdkModifierType state,
						      AgsRampMarkerPopover *ramp_marker_popover);
void ags_ramp_marker_popover_key_released_callback(GtkEventControllerKey *event_controller,
						   guint keyval,
						   guint keycode,
						   GdkModifierType state,
						   AgsRampMarkerPopover *ramp_marker_popover);
gboolean ags_ramp_marker_popover_modifiers_callback(GtkEventControllerKey *event_controller,
						    GdkModifierType keyval,
						    AgsRampMarkerPopover *ramp_marker_popover);

void ags_ramp_marker_popover_activate_button_callback(GtkButton *activate_button,
						      AgsRampMarkerPopover *ramp_marker_popover);

/**
 * SECTION:ags_ramp_marker_popover
 * @short_description: crop tool
 * @title: AgsRampMarkerPopover
 * @section_id:
 * @include: ags/app/editor/ags_ramp_marker_popover.h
 *
 * The #AgsRampMarkerPopover lets you crop notes.
 */

static gpointer ags_ramp_marker_popover_parent_class = NULL;

GType
ags_ramp_marker_popover_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_ramp_marker_popover = 0;

    static const GTypeInfo ags_ramp_marker_popover_info = {
      sizeof (AgsRampMarkerPopoverClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_ramp_marker_popover_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRampMarkerPopover),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_ramp_marker_popover_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_ramp_marker_popover_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_ramp_marker_popover_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_ramp_marker_popover = g_type_register_static(GTK_TYPE_POPOVER,
							  "AgsRampMarkerPopover", &ags_ramp_marker_popover_info,
							  0);
    
    g_type_add_interface_static(ags_type_ramp_marker_popover,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_ramp_marker_popover,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__static, ags_type_ramp_marker_popover);
  }

  return g_define_type_id__static;
}

void
ags_ramp_marker_popover_class_init(AgsRampMarkerPopoverClass *ramp_marker_popover)
{
  GObjectClass *gobject;

  ags_ramp_marker_popover_parent_class = g_type_class_peek_parent(ramp_marker_popover);

  /* GObjectClass */
  gobject = (GObjectClass *) ramp_marker_popover;

  gobject->finalize = ags_ramp_marker_popover_finalize;
}

void
ags_ramp_marker_popover_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = NULL;
  connectable->has_resource = NULL;

  connectable->is_ready = NULL;
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;

  connectable->list_resource = NULL;
  connectable->xml_compose = NULL;
  connectable->xml_parse = NULL;

  connectable->is_connected = ags_ramp_marker_popover_is_connected;  
  connectable->connect = ags_ramp_marker_popover_connect;
  connectable->disconnect = ags_ramp_marker_popover_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_ramp_marker_popover_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_ramp_marker_popover_set_update;
  applicable->apply = ags_ramp_marker_popover_apply;
  applicable->reset = ags_ramp_marker_popover_reset;
}

void
ags_ramp_marker_popover_init(AgsRampMarkerPopover *ramp_marker_popover)
{
  GtkBox *vbox;
  GtkBox *hbox;
  GtkLabel *label;
  
  GtkEventController *event_controller;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();

  ramp_marker_popover->connectable_flags = 0;

  event_controller = gtk_event_controller_key_new();
  gtk_widget_add_controller((GtkWidget *) ramp_marker_popover,
			    event_controller);

  g_signal_connect(event_controller, "key-pressed",
		   G_CALLBACK(ags_ramp_marker_popover_key_pressed_callback), ramp_marker_popover);
  
  g_signal_connect(event_controller, "key-released",
		   G_CALLBACK(ags_ramp_marker_popover_key_released_callback), ramp_marker_popover);

  g_signal_connect(event_controller, "modifiers",
		   G_CALLBACK(ags_ramp_marker_popover_modifiers_callback), ramp_marker_popover);
  
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_popover_set_child((GtkPopover *) ramp_marker_popover,
			(GtkWidget *) vbox);

  /* program */
  ramp_marker_popover->control_name = (GtkComboBoxText *) gtk_combo_box_text_new();

  gtk_combo_box_text_insert(ramp_marker_popover->control_name,
			    -1,
			    "tempo",
			    i18n("tempo"));
  
  gtk_box_append(vbox,
		 GTK_WIDGET(ramp_marker_popover->control_name));  
  
  /* ramp x0 - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(vbox,
		 GTK_WIDGET(hbox));

  /* ramp x0 - label */
  label = (GtkLabel *) gtk_label_new(i18n("ramp x0"));
  gtk_box_append(hbox,
		 GTK_WIDGET(label));

  /* ramp x0 - spin button */
  ramp_marker_popover->ramp_x0 = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
										  AGS_RAMP_MARKER_MAX_BEATS,
										  0.25);
  gtk_spin_button_set_digits(ramp_marker_popover->ramp_x0,
			     2);
  gtk_spin_button_set_value(ramp_marker_popover->ramp_x0,
			    0.0);
  gtk_box_append(hbox,
		 GTK_WIDGET(ramp_marker_popover->ramp_x0));

  /* ramp y0 - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(vbox,
		 GTK_WIDGET(hbox));

  /* ramp y0 - label */
  label = (GtkLabel *) gtk_label_new(i18n("ramp y0"));
  gtk_box_append(hbox,
		 GTK_WIDGET(label));

  /* ramp y0 - spin button */
  ramp_marker_popover->ramp_y0 = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
										  240.0,
										  1.0);
  gtk_spin_button_set_value(ramp_marker_popover->ramp_y0,
			    120.0);
  gtk_box_append(hbox,
		 GTK_WIDGET(ramp_marker_popover->ramp_y0));
  
  /* ramp x1 - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(vbox,
		 GTK_WIDGET(hbox));

  /* ramp x1 - label */
  label = (GtkLabel *) gtk_label_new(i18n("ramp x1"));
  gtk_box_append(hbox,
		 GTK_WIDGET(label));

  /* ramp x1 - spin button */
  ramp_marker_popover->ramp_x1 = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
										  AGS_RAMP_MARKER_MAX_BEATS,
										  0.25);
  gtk_spin_button_set_digits(ramp_marker_popover->ramp_x1,
			     2);
  gtk_spin_button_set_value(ramp_marker_popover->ramp_x1,
			    0.0);
  gtk_box_append(hbox,
		 GTK_WIDGET(ramp_marker_popover->ramp_x1));

  /* ramp y1 - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(vbox,
		 GTK_WIDGET(hbox));

  /* ramp y1 - label */
  label = (GtkLabel *) gtk_label_new(i18n("ramp y1"));
  gtk_box_append(hbox,
		 GTK_WIDGET(label));

  /* ramp y1 - spin button */
  ramp_marker_popover->ramp_y1 = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
										  240.0,
										  1.0);
  gtk_spin_button_set_value(ramp_marker_popover->ramp_y1,
			    120.0);
  gtk_box_append(hbox,
		 GTK_WIDGET(ramp_marker_popover->ramp_y1));

  /* ramp step count - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(vbox,
		 GTK_WIDGET(hbox));

  /* ramp step count - label */
  label = (GtkLabel *) gtk_label_new(i18n("ramp step count"));
  gtk_box_append(hbox,
		 GTK_WIDGET(label));

  /* ramp step count - spin button */
  ramp_marker_popover->ramp_step_count = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
											  AGS_RAMP_MARKER_MAX_STEPS,
											  1.0);
  gtk_spin_button_set_value(ramp_marker_popover->ramp_step_count,
			    0.0);
  gtk_box_append(hbox,
		 GTK_WIDGET(ramp_marker_popover->ramp_step_count));

  /* buttons */
  ramp_marker_popover->action_area = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
							    AGS_UI_PROVIDER_DEFAULT_SPACING);
  
  gtk_widget_set_halign((GtkWidget *) ramp_marker_popover->action_area,
			GTK_ALIGN_END);

  gtk_box_append(vbox,
		 (GtkWidget *) ramp_marker_popover->action_area);

  ramp_marker_popover->activate_button = (GtkButton *) gtk_button_new_with_label(i18n("apply"));

  gtk_box_append(ramp_marker_popover->action_area,
		 (GtkWidget *) ramp_marker_popover->activate_button);

  g_signal_connect(ramp_marker_popover->activate_button, "clicked",
		   G_CALLBACK(ags_ramp_marker_popover_activate_button_callback), ramp_marker_popover);

  gtk_popover_set_default_widget((GtkPopover *) ramp_marker_popover,
				 (GtkWidget *) ramp_marker_popover->activate_button);
}

gboolean
ags_ramp_marker_popover_is_connected(AgsConnectable *connectable)
{
  AgsRampMarkerPopover *ramp_marker_popover;
  
  gboolean is_connected;
  
  ramp_marker_popover = AGS_RAMP_MARKER_POPOVER(connectable);

  /* check is connected */
  is_connected = ((AGS_CONNECTABLE_CONNECTED & (ramp_marker_popover->connectable_flags)) != 0) ? TRUE: FALSE;

  return(is_connected);
}

void
ags_ramp_marker_popover_connect(AgsConnectable *connectable)
{
  AgsWindow *window;
  AgsRampMarkerPopover *ramp_marker_popover;

  AgsApplicationContext *application_context;

  ramp_marker_popover = AGS_RAMP_MARKER_POPOVER(connectable);

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  ramp_marker_popover->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  application_context = ags_application_context_get_instance();
  
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  g_signal_connect((GObject *) ramp_marker_popover->control_name, "changed",
		   G_CALLBACK(ags_ramp_marker_popover_control_name_callback), (gpointer) ramp_marker_popover);

  /* machine changed */
  g_signal_connect_after((GObject *) window->composite_editor, "machine-changed",
			 G_CALLBACK(ags_ramp_marker_popover_machine_changed_callback), (gpointer) ramp_marker_popover);
}

void
ags_ramp_marker_popover_disconnect(AgsConnectable *connectable)
{
  AgsWindow *window;
  AgsRampMarkerPopover *ramp_marker_popover;

  AgsApplicationContext *application_context;

  ramp_marker_popover = AGS_RAMP_MARKER_POPOVER(connectable);

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  ramp_marker_popover->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  application_context = ags_application_context_get_instance();
  
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  g_object_disconnect(G_OBJECT(ramp_marker_popover->control_name),
		      "any_signal::changed",
		      G_CALLBACK(ags_ramp_marker_popover_control_name_callback),
		      ramp_marker_popover,
		      NULL);

  g_object_disconnect(G_OBJECT(window->composite_editor),
		      "any_signal::machine-changed",
		      G_CALLBACK(ags_ramp_marker_popover_machine_changed_callback),
		      ramp_marker_popover,
		      NULL);
}

void
ags_ramp_marker_popover_finalize(GObject *gobject)
{
  AgsRampMarkerPopover *ramp_marker_popover;

  ramp_marker_popover = (AgsRampMarkerPopover *) gobject;
  
  G_OBJECT_CLASS(ags_ramp_marker_popover_parent_class)->finalize(gobject);
}

void
ags_ramp_marker_popover_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_ramp_marker_popover_apply(AgsApplicable *applicable)
{
  AgsRampMarkerPopover *ramp_marker_popover;
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  AgsTempoEdit *tempo_edit;

  AgsProgram *current;
  AgsMarker *marker;

  AgsTimestamp *timestamp;

  AgsApplicationContext *application_context;

  GList *start_list_program, *list_program;
  GList *list_marker, *list_marker_start;

  gchar *control_name;
  
  GType channel_type;

  gdouble gui_y;
  gdouble tact;
  
  gdouble c_y0, c_y1;
  gdouble val;
  gdouble upper, lower, range, step;
  gdouble c_upper, c_lower, c_range;

  guint x0, x1;
  guint tmp;
  gdouble y0, y1;
  guint step_count;
  gint line;
  guint i, i_start, i_stop;
  guint nth_match, match_count;

  gboolean success;
  
  ramp_marker_popover = AGS_RAMP_MARKER_POPOVER(applicable);

  /* application context */
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));  
    
  composite_editor = window->composite_editor;

  tempo_edit = (AgsTempoEdit *) composite_editor->tempo_edit;

  if(tempo_edit == NULL){
    return;
  }

  control_name = gtk_combo_box_text_get_active_text(ramp_marker_popover->control_name);
  
  /* get some values */
  x0 = AGS_TEMPO_EDIT_DEFAULT_CONTROL_WIDTH * gtk_spin_button_get_value_as_int(ramp_marker_popover->ramp_x0);
  y0 = gtk_spin_button_get_value(ramp_marker_popover->ramp_y0);

  x1 = AGS_TEMPO_EDIT_DEFAULT_CONTROL_WIDTH * gtk_spin_button_get_value_as_int(ramp_marker_popover->ramp_x1);
  y1 = gtk_spin_button_get_value(ramp_marker_popover->ramp_y1);
  
  step_count = gtk_spin_button_get_value_as_int(ramp_marker_popover->ramp_step_count);

  /* remove marker of region */
  line = 0;

  start_list_program = ags_sound_provider_get_program(AGS_SOUND_PROVIDER(application_context));
  
  list_program = start_list_program;

  while((list_program = ags_program_find_control_name(list_program,
						      "tempo")) != NULL){
    AgsTimestamp *current_timestamp;

    guint steps;
      
    current = AGS_PROGRAM(list_program->data);

    g_object_get(current,
		 "timestamp", &current_timestamp,
		 NULL);

    g_object_unref(current_timestamp);
      
    if(ags_timestamp_get_ags_offset(current_timestamp) < x0){
      list_program = list_program->next;
	
      continue;
    }

    if(ags_timestamp_get_ags_offset(current_timestamp) > x1){
      break;
    }

    upper = 240.0;
    lower = 0.0;
    steps = 240.0;
      
    range = upper - lower;

    c_upper = upper;
    c_lower = lower;
	  
    c_range = range;
      
    if(range == 0.0){
      list_program = list_program->next;
      g_warning("ags_ramp_marker_popover.c - range = 0.0");
	  
      continue;
    }
	
    /* check steps */
    gui_y = steps;

    val = c_lower + (gui_y * (c_range / steps));
    c_y0 = val;

    /* check steps */
    gui_y = 0;

    val = c_lower + (gui_y * (c_range / steps));
    c_y1 = val;

    /* find and remove region */
    list_marker =
      list_marker_start = ags_program_find_region(list_program->data,
						  x0,
						  x1,
						  FALSE);
      
    while(list_marker != NULL){
      current->marker = g_list_remove(current->marker,
				      list_marker->data);
	
      list_marker = list_marker->next;
    }

    g_list_free(list_marker_start);
      
    list_program = list_program->next;
  }

  g_list_free_full(start_list_program,
		   g_object_unref);
  
  /* ramp marker */
  line = 0;

  timestamp = ags_timestamp_new();

  timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  timestamp->flags |= AGS_TIMESTAMP_OFFSET;
  
  match_count = 0;
  tmp = x1 - x0;

  if(x0 % (guint) AGS_PROGRAM_DEFAULT_OFFSET != 0 &&
     x0 / (guint) AGS_PROGRAM_DEFAULT_OFFSET != (x1 - AGS_PROGRAM_DEFAULT_OFFSET) / (guint) AGS_PROGRAM_DEFAULT_OFFSET){
    match_count = 1;

    tmp -= (x0 % (guint) AGS_PROGRAM_DEFAULT_OFFSET);
  }

  match_count += (guint) ceil((gdouble) tmp / AGS_PROGRAM_DEFAULT_OFFSET);
    
  success = FALSE;

  timestamp->timer.ags_offset.offset = AGS_PROGRAM_DEFAULT_OFFSET * floor(x0 / AGS_PROGRAM_DEFAULT_OFFSET);

  for(nth_match = 0; nth_match < match_count; nth_match++){
    list_program = start_list_program;
    list_program = ags_program_find_near_timestamp(list_program,
						   timestamp);

    if(list_program == NULL){
      current = ags_program_new("tempo");

      current->timestamp->timer.ags_offset.offset = timestamp->timer.ags_offset.offset;

      start_list_program = g_list_prepend(start_list_program,
					  current);

      ags_sound_provider_set_program(AGS_SOUND_PROVIDER(application_context),
				     start_list_program);
    }else{
      current = list_program->data;
    }

    upper = 240.0;
    lower = 0.0;
	
    range = upper - lower;
	
    if(range == 0.0){
      g_warning("ags_ramp_marker_popover.c - range = 0.0");
	  
      break;
    }
	    
    /* ramp value and move offset */
    if(!success){
      i = 0;

      if(floor(x0 / (guint) AGS_PROGRAM_DEFAULT_OFFSET) == floor(x1 / (guint) AGS_PROGRAM_DEFAULT_OFFSET)){
	i_stop = step_count;

	tmp += (x1 - x0);
      }else{
	i_stop = tmp / (x1 - x0) * step_count;

	if(tmp + AGS_PROGRAM_DEFAULT_OFFSET < x1 - x0){
	  tmp += AGS_PROGRAM_DEFAULT_OFFSET;
	}else{
	  tmp = x1 - x0;
	}
      }
	
      success = TRUE;
    }else{
      i_stop = (x1 - tmp) / (x1 - x0) * step_count;
	
      if(tmp + AGS_PROGRAM_DEFAULT_OFFSET < x1 - x0){
	tmp += AGS_PROGRAM_DEFAULT_OFFSET;
      }else{
	tmp = x1 - x0;
      }
    }
	
    for(; i < step_count && i < i_stop; i++){
      marker = ags_marker_new();
      marker->x = ((gdouble) x0 + (gdouble) ((gdouble) (x1 - x0) * (gdouble) ((gdouble) (i) / ((gdouble) step_count))));
      marker->y = ((gdouble) y0 + ((gdouble) (y1 - y0) * (gdouble) ((gdouble) (i + 1) / ((gdouble) step_count))));

#ifdef AGS_DEBUG
      g_message("line %d - %d %f", line, marker->x, marker->y);
#endif
	
      ags_program_add_marker(current,
			     marker,
			     FALSE);
	
    }
	
    timestamp->timer.ags_offset.offset += AGS_PROGRAM_DEFAULT_OFFSET;
  }

  match_count = 0;
  tmp = x1 - x0;

  if(x0 % (guint) AGS_PROGRAM_DEFAULT_OFFSET != 0 &&
     x0 / (guint) AGS_PROGRAM_DEFAULT_OFFSET != (x1 - AGS_PROGRAM_DEFAULT_OFFSET) / (guint) AGS_PROGRAM_DEFAULT_OFFSET){
    match_count = 1;

    tmp -= (x0 % (guint) AGS_PROGRAM_DEFAULT_OFFSET);
  }
      
  match_count += (guint) ceil((gdouble) tmp / AGS_PROGRAM_DEFAULT_OFFSET);
      
  success = FALSE;

  timestamp->timer.ags_offset.offset = AGS_PROGRAM_DEFAULT_OFFSET * floor(x0 / AGS_PROGRAM_DEFAULT_OFFSET);

  for(nth_match = 0; nth_match < match_count; nth_match++){
    list_program = start_list_program;
    list_program = ags_program_find_near_timestamp(list_program,
						   timestamp);

    if(list_program == NULL){
      current = ags_program_new("tempo");

      current->timestamp->timer.ags_offset.offset = timestamp->timer.ags_offset.offset;

      start_list_program = g_list_prepend(start_list_program,
					  current);

      ags_sound_provider_set_program(AGS_SOUND_PROVIDER(application_context),
				     start_list_program);
    }else{
      current = list_program->data;
    }

    upper = 240.0;
    lower = 0.0;
	
    range = upper - lower;
	
    if(range == 0.0){
      g_warning("ags_ramp_marker_popover.c - range = 0.0");
	  
      break;
    }
	    
    /* ramp value and move offset */
    if(!success){
      i = 0;

      if(floor(x0 / (guint) AGS_PROGRAM_DEFAULT_OFFSET) == floor(x1 / (guint) AGS_PROGRAM_DEFAULT_OFFSET)){
	i_stop = step_count;

	tmp += (x1 - x0);
      }else{
	i_stop = tmp / (x1 - x0) * step_count;

	if(tmp + AGS_PROGRAM_DEFAULT_OFFSET < x1 - x0){
	  tmp += AGS_PROGRAM_DEFAULT_OFFSET;
	}else{
	  tmp = x1 - x0;
	}
      }
	
      success = TRUE;
    }else{
      i_stop = (x1 - tmp) / (x1 - x0) * step_count;
	
      if(tmp + AGS_PROGRAM_DEFAULT_OFFSET < x1 - x0){
	tmp += AGS_PROGRAM_DEFAULT_OFFSET;
      }else{
	tmp = x1 - x0;
      }
    }
	
    for(; i < step_count && i < i_stop; i++){
      marker = ags_marker_new();
      marker->x = ((gdouble) x0 + (gdouble) ((gdouble) (x1 - x0) * (gdouble) ((gdouble) (i) / ((gdouble) step_count))));
      marker->y = ((gdouble) y0 + ((gdouble) (y1 - y0) * (gdouble) ((gdouble) (i + 1) / ((gdouble) step_count))));

#ifdef AGS_DEBUG
      g_message("line %d - %d %f", line, marker->x, marker->y);
#endif
	
      ags_program_add_marker(current,
			     marker,
			     FALSE);
	
    }

    timestamp->timer.ags_offset.offset += AGS_PROGRAM_DEFAULT_OFFSET;
  }

  gtk_widget_queue_draw((GtkWidget *) composite_editor->tempo_edit->drawing_area);  
}

void
ags_ramp_marker_popover_reset(AgsApplicable *applicable)
{
}

gboolean
ags_ramp_marker_popover_key_pressed_callback(GtkEventControllerKey *event_controller,
					     guint keyval,
					     guint keycode,
					     GdkModifierType state,
					     AgsRampMarkerPopover *ramp_marker_popover)
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
ags_ramp_marker_popover_key_released_callback(GtkEventControllerKey *event_controller,
					      guint keyval,
					      guint keycode,
					      GdkModifierType state,
					      AgsRampMarkerPopover *ramp_marker_popover)
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
	gtk_popover_popdown((GtkPopover *) ramp_marker_popover);	
      }
      break;
    }
  }
}

gboolean
ags_ramp_marker_popover_modifiers_callback(GtkEventControllerKey *event_controller,
					   GdkModifierType keyval,
					   AgsRampMarkerPopover *ramp_marker_popover)
{
  return(FALSE);
}

void
ags_ramp_marker_popover_activate_button_callback(GtkButton *activate_button,
						 AgsRampMarkerPopover *ramp_marker_popover)
{
  ags_applicable_apply(AGS_APPLICABLE(ramp_marker_popover));
}

/**
 * ags_ramp_marker_popover_new:
 *
 * Create a new #AgsRampMarkerPopover.
 *
 * Returns: a new #AgsRampMarkerPopover
 *
 * Since: 6.11.0
 */
AgsRampMarkerPopover*
ags_ramp_marker_popover_new()
{
  AgsRampMarkerPopover *ramp_marker_popover;

  ramp_marker_popover = (AgsRampMarkerPopover *) g_object_new(AGS_TYPE_RAMP_MARKER_POPOVER,
							      NULL);

  return(ramp_marker_popover);
}

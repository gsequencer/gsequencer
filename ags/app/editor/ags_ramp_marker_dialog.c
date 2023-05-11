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

#include <ags/app/editor/ags_ramp_marker_dialog.h>
#include <ags/app/editor/ags_ramp_marker_dialog_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_machine.h>

#include <ags/app/editor/ags_tempo_edit.h>

#include <ags/i18n.h>

void ags_ramp_marker_dialog_class_init(AgsRampMarkerDialogClass *ramp_marker_dialog);
void ags_ramp_marker_dialog_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_ramp_marker_dialog_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_ramp_marker_dialog_init(AgsRampMarkerDialog *ramp_marker_dialog);
void ags_ramp_marker_dialog_finalize(GObject *gobject);

void ags_ramp_marker_dialog_connect(AgsConnectable *connectable);
void ags_ramp_marker_dialog_disconnect(AgsConnectable *connectable);

void ags_ramp_marker_dialog_set_update(AgsApplicable *applicable, gboolean update);
void ags_ramp_marker_dialog_apply(AgsApplicable *applicable);
void ags_ramp_marker_dialog_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_ramp_marker_dialog
 * @short_description: ramp tool
 * @title: AgsRampMarkerDialog
 * @section_id:
 * @include: ags/app/editor/ags_ramp_marker_dialog.h
 *
 * The #AgsRampMarkerDialog lets you ramp markers.
 */

static gpointer ags_ramp_marker_dialog_parent_class = NULL;

GType
ags_ramp_marker_dialog_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_ramp_marker_dialog = 0;

    static const GTypeInfo ags_ramp_marker_dialog_info = {
      sizeof (AgsRampMarkerDialogClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_ramp_marker_dialog_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRampMarkerDialog),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_ramp_marker_dialog_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_ramp_marker_dialog_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_ramp_marker_dialog_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_ramp_marker_dialog = g_type_register_static(GTK_TYPE_DIALOG,
							 "AgsRampMarkerDialog", &ags_ramp_marker_dialog_info,
							 0);
    
    g_type_add_interface_static(ags_type_ramp_marker_dialog,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_ramp_marker_dialog,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_ramp_marker_dialog);
  }

  return g_define_type_id__volatile;
}

void
ags_ramp_marker_dialog_class_init(AgsRampMarkerDialogClass *ramp_marker_dialog)
{
  GObjectClass *gobject;

  ags_ramp_marker_dialog_parent_class = g_type_class_peek_parent(ramp_marker_dialog);

  /* GObjectClass */
  gobject = (GObjectClass *) ramp_marker_dialog;

  gobject->finalize = ags_ramp_marker_dialog_finalize;
}

void
ags_ramp_marker_dialog_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_ramp_marker_dialog_connect;
  connectable->disconnect = ags_ramp_marker_dialog_disconnect;
}

void
ags_ramp_marker_dialog_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_ramp_marker_dialog_set_update;
  applicable->apply = ags_ramp_marker_dialog_apply;
  applicable->reset = ags_ramp_marker_dialog_reset;
}

void
ags_ramp_marker_dialog_init(AgsRampMarkerDialog *ramp_marker_dialog)
{
  GtkBox *vbox;
  GtkBox *hbox;
  GtkLabel *label;

  ramp_marker_dialog->connectable_flags = 0;

  g_object_set(ramp_marker_dialog,
	       "title", i18n("ramp markers"),
	       NULL);

  gtk_window_set_hide_on_close((GtkWindow *) ramp_marker_dialog,
			       TRUE);
  
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				0);
  gtk_box_append((GtkBox *) gtk_dialog_get_content_area((GtkDialog *) ramp_marker_dialog),
		 GTK_WIDGET(vbox));  

  /* program */
  ramp_marker_dialog->control_name = (GtkComboBoxText *) gtk_combo_box_text_new();

  gtk_combo_box_text_insert(ramp_marker_dialog->control_name,
			    -1,
			    "tempo",
			    i18n("tempo"));
  
  gtk_box_append(vbox,
		 GTK_WIDGET(ramp_marker_dialog->control_name));  
  
  /* ramp x0 - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);
  gtk_box_append(vbox,
		 GTK_WIDGET(hbox));

  /* ramp x0 - label */
  label = (GtkLabel *) gtk_label_new(i18n("ramp x0"));
  gtk_box_append(hbox,
		 GTK_WIDGET(label));

  /* ramp x0 - spin button */
  ramp_marker_dialog->ramp_x0 = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
										 AGS_RAMP_MARKER_MAX_BEATS,
										 0.25);
  gtk_spin_button_set_digits(ramp_marker_dialog->ramp_x0,
			     2);
  gtk_spin_button_set_value(ramp_marker_dialog->ramp_x0,
			    0.0);
  gtk_box_append(hbox,
		 GTK_WIDGET(ramp_marker_dialog->ramp_x0));

  /* ramp y0 - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);
  gtk_box_append(vbox,
		 GTK_WIDGET(hbox));

  /* ramp y0 - label */
  label = (GtkLabel *) gtk_label_new(i18n("ramp y0"));
  gtk_box_append(hbox,
		 GTK_WIDGET(label));

  /* ramp y0 - spin button */
  ramp_marker_dialog->ramp_y0 = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
										 240.0,
										 1.0);
  gtk_spin_button_set_value(ramp_marker_dialog->ramp_y0,
			    120.0);
  gtk_box_append(hbox,
		 GTK_WIDGET(ramp_marker_dialog->ramp_y0));
  
  /* ramp x1 - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);
  gtk_box_append(vbox,
		 GTK_WIDGET(hbox));

  /* ramp x1 - label */
  label = (GtkLabel *) gtk_label_new(i18n("ramp x1"));
  gtk_box_append(hbox,
		 GTK_WIDGET(label));

  /* ramp x1 - spin button */
  ramp_marker_dialog->ramp_x1 = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
										 AGS_RAMP_MARKER_MAX_BEATS,
										 0.25);
  gtk_spin_button_set_digits(ramp_marker_dialog->ramp_x1,
			     2);
  gtk_spin_button_set_value(ramp_marker_dialog->ramp_x1,
			    0.0);
  gtk_box_append(hbox,
		 GTK_WIDGET(ramp_marker_dialog->ramp_x1));

  /* ramp y1 - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);
  gtk_box_append(vbox,
		 GTK_WIDGET(hbox));

  /* ramp y1 - label */
  label = (GtkLabel *) gtk_label_new(i18n("ramp y1"));
  gtk_box_append(hbox,
		 GTK_WIDGET(label));

  /* ramp y1 - spin button */
  ramp_marker_dialog->ramp_y1 = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
										 240.0,
										 1.0);
  gtk_spin_button_set_value(ramp_marker_dialog->ramp_y1,
			    120.0);
  gtk_box_append(hbox,
		 GTK_WIDGET(ramp_marker_dialog->ramp_y1));

  /* ramp step count - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);
  gtk_box_append(vbox,
		 GTK_WIDGET(hbox));

  /* ramp step count - label */
  label = (GtkLabel *) gtk_label_new(i18n("ramp step count"));
  gtk_box_append(hbox,
		 GTK_WIDGET(label));

  /* ramp step count - spin button */
  ramp_marker_dialog->ramp_step_count = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
											 AGS_RAMP_MARKER_MAX_STEPS,
											 1.0);
  gtk_spin_button_set_value(ramp_marker_dialog->ramp_step_count,
			    0.0);
  gtk_box_append(hbox,
		 GTK_WIDGET(ramp_marker_dialog->ramp_step_count));

  /* dialog buttons */
  gtk_dialog_add_buttons((GtkDialog *) ramp_marker_dialog,
			 i18n("_Apply"), GTK_RESPONSE_APPLY,
			 i18n("_OK"), GTK_RESPONSE_OK,
			 i18n("_Cancel"), GTK_RESPONSE_CANCEL,
			 NULL);
}

void
ags_ramp_marker_dialog_connect(AgsConnectable *connectable)
{
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;  
  AgsRampMarkerDialog *ramp_marker_dialog;

  AgsApplicationContext *application_context;
  
  ramp_marker_dialog = AGS_RAMP_MARKER_DIALOG(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (ramp_marker_dialog->connectable_flags)) != 0){
    return;
  }

  ramp_marker_dialog->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  /* application context */
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  g_signal_connect((GObject *) ramp_marker_dialog, "response",
		   G_CALLBACK(ags_ramp_marker_dialog_response_callback), (gpointer) ramp_marker_dialog);

  g_signal_connect((GObject *) ramp_marker_dialog->control_name, "changed",
		   G_CALLBACK(ags_ramp_marker_dialog_control_name_callback), (gpointer) ramp_marker_dialog);

  /* machine changed */
  composite_editor = window->composite_editor;
  
  g_signal_connect_after(composite_editor, "machine-changed",
			 G_CALLBACK(ags_ramp_marker_dialog_machine_changed_callback), ramp_marker_dialog);
}

void
ags_ramp_marker_dialog_disconnect(AgsConnectable *connectable)
{
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  AgsRampMarkerDialog *ramp_marker_dialog;

  AgsApplicationContext *application_context;

  ramp_marker_dialog = AGS_RAMP_MARKER_DIALOG(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (ramp_marker_dialog->connectable_flags)) == 0){
    return;
  }

  ramp_marker_dialog->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  /* application context */
  application_context = ags_application_context_get_instance();
  
  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  g_object_disconnect(G_OBJECT(ramp_marker_dialog),
		      "any_signal::response",
		      G_CALLBACK(ags_ramp_marker_dialog_response_callback),
		      ramp_marker_dialog,
		      NULL);

  g_object_disconnect(G_OBJECT(ramp_marker_dialog->control_name),
		      "any_signal::changed",
		      G_CALLBACK(ags_ramp_marker_dialog_control_name_callback),
		      ramp_marker_dialog,
		      NULL);
}

void
ags_ramp_marker_dialog_finalize(GObject *gobject)
{
  AgsRampMarkerDialog *ramp_marker_dialog;

  ramp_marker_dialog = (AgsRampMarkerDialog *) gobject;
  
  G_OBJECT_CLASS(ags_ramp_marker_dialog_parent_class)->finalize(gobject);
}

void
ags_ramp_marker_dialog_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_ramp_marker_dialog_apply(AgsApplicable *applicable)
{
  AgsRampMarkerDialog *ramp_marker_dialog;
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
  
  ramp_marker_dialog = AGS_RAMP_MARKER_DIALOG(applicable);

  /* application context */
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));  
    
  composite_editor = window->composite_editor;

  tempo_edit = (AgsTempoEdit *) composite_editor->tempo_edit;

  if(tempo_edit == NULL){
    return;
  }

  control_name = gtk_combo_box_text_get_active_text(ramp_marker_dialog->control_name);
  
  /* get some values */
  x0 = AGS_TEMPO_EDIT_DEFAULT_CONTROL_WIDTH * gtk_spin_button_get_value_as_int(ramp_marker_dialog->ramp_x0);
  y0 = gtk_spin_button_get_value(ramp_marker_dialog->ramp_y0);

  x1 = AGS_TEMPO_EDIT_DEFAULT_CONTROL_WIDTH * gtk_spin_button_get_value_as_int(ramp_marker_dialog->ramp_x1);
  y1 = gtk_spin_button_get_value(ramp_marker_dialog->ramp_y1);
  
  step_count = gtk_spin_button_get_value_as_int(ramp_marker_dialog->ramp_step_count);

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
      g_warning("ags_ramp_marker_dialog.c - range = 0.0");
	  
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
      g_warning("ags_ramp_marker_dialog.c - range = 0.0");
	  
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
      g_warning("ags_ramp_marker_dialog.c - range = 0.0");
	  
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

  gtk_widget_queue_draw(composite_editor->tempo_edit->drawing_area);  
}

void
ags_ramp_marker_dialog_reset(AgsApplicable *applicable)
{  
}

/**
 * ags_ramp_marker_dialog_new:
 *
 * Create a new instance of #AgsRampMarkerDialog.
 *
 * Returns: the new #AgsRampMarkerDialog
 *
 * Since: 5.1.0
 */
AgsRampMarkerDialog*
ags_ramp_marker_dialog_new()
{
  AgsRampMarkerDialog *ramp_marker_dialog;

  ramp_marker_dialog = (AgsRampMarkerDialog *) g_object_new(AGS_TYPE_RAMP_MARKER_DIALOG,
							    NULL);

  return(ramp_marker_dialog);
}

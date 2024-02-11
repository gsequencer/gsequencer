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

#include <ags/app/editor/ags_envelope_editor.h>
#include <ags/app/editor/ags_envelope_editor_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_machine.h>

#include <ags/app/editor/ags_envelope_dialog.h>

#include <math.h>
#include <complex.h>

#include <ags/i18n.h>

void ags_envelope_editor_class_init(AgsEnvelopeEditorClass *envelope_editor);
void ags_envelope_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_envelope_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_envelope_editor_init(AgsEnvelopeEditor *envelope_editor);

gboolean ags_envelope_editor_is_connected(AgsConnectable *connectable);
void ags_envelope_editor_connect(AgsConnectable *connectable);
void ags_envelope_editor_disconnect(AgsConnectable *connectable);

void ags_envelope_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_envelope_editor_apply(AgsApplicable *applicable);
void ags_envelope_editor_reset(AgsApplicable *applicable);

gchar* ags_envelope_editor_x_label_func(gdouble value,
					gpointer data);
gchar* ags_envelope_editor_y_label_func(gdouble value,
					gpointer data);

/**
 * SECTION:ags_envelope_editor
 * @short_description: Edit envelope of notes
 * @title: AgsEnvelopeEditor
 * @section_id:
 * @include: ags/app/ags_envelope_editor.h
 *
 * #AgsEnvelopeEditor is a composite widget to edit envelope controls
 * of selected #AgsNote.
 */

static gpointer ags_envelope_editor_parent_class = NULL;

GType
ags_envelope_editor_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_envelope_editor = 0;

    static const GTypeInfo ags_envelope_editor_info = {
      sizeof (AgsEnvelopeEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_envelope_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsEnvelopeEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_envelope_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_envelope_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_envelope_editor_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_envelope_editor = g_type_register_static(GTK_TYPE_BOX,
						      "AgsEnvelopeEditor", &ags_envelope_editor_info,
						      0);

    g_type_add_interface_static(ags_type_envelope_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_envelope_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_envelope_editor);
  }

  return g_define_type_id__volatile;
}

void
ags_envelope_editor_class_init(AgsEnvelopeEditorClass *envelope_editor)
{
  ags_envelope_editor_parent_class = g_type_class_peek_parent(envelope_editor);
}

void
ags_envelope_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = NULL;
  connectable->has_resource = NULL;

  connectable->is_ready = NULL;
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;

  connectable->list_resource = NULL;
  connectable->xml_compose = NULL;
  connectable->xml_parse = NULL;

  connectable->is_connected = ags_envelope_editor_is_connected;  
  connectable->connect = ags_envelope_editor_connect;
  connectable->disconnect = ags_envelope_editor_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_envelope_editor_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_envelope_editor_set_update;
  applicable->apply = ags_envelope_editor_apply;
  applicable->reset = ags_envelope_editor_reset;
}

void
ags_envelope_editor_init(AgsEnvelopeEditor *envelope_editor)
{
  GtkFrame *frame;
  GtkBox *vbox;
  GtkBox *hbox;
  GtkBox *control;
  GtkGrid *grid;
  GtkLabel *label;
  AgsCartesian *cartesian;

  AgsApplicationContext *application_context;   
  
  AgsPlot *plot;

  gdouble gui_scale_factor;
  gdouble width, height;
  gdouble default_width, default_height;
  gdouble offset;
  
  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  gtk_orientable_set_orientation(GTK_ORIENTABLE(envelope_editor),
				 GTK_ORIENTATION_VERTICAL);

  gtk_box_set_spacing((GtkBox *) envelope_editor,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_widget_set_vexpand((GtkWidget *) envelope_editor,
			 TRUE);
  gtk_widget_set_hexpand((GtkWidget *) envelope_editor,
			 TRUE);

  gtk_widget_set_valign((GtkWidget *) envelope_editor,
			GTK_ALIGN_START);

  envelope_editor->flags = 0;
  envelope_editor->connectable_flags = 0;

  envelope_editor->version = AGS_ENVELOPE_EDITOR_DEFAULT_VERSION;
  envelope_editor->build_id = AGS_ENVELOPE_EDITOR_DEFAULT_BUILD_ID;
  
  /* enabled */
  envelope_editor->enabled = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("enabled"));
  gtk_box_append((GtkBox *) envelope_editor,
		 (GtkWidget *) envelope_editor->enabled);

  /* rename dialog */
  envelope_editor->rename = NULL;

  /* frame - preset */
  frame = (GtkFrame *) gtk_frame_new(i18n("preset"));
  gtk_box_append((GtkBox *) envelope_editor,
		 (GtkWidget *) frame);

  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_frame_set_child(frame,
		      (GtkWidget *) hbox);
  
  envelope_editor->preset = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_box_append((GtkBox *) hbox,
		 (GtkWidget *) envelope_editor->preset);

  envelope_editor->add = (GtkButton *) gtk_button_new_with_mnemonic(i18n("_Add"));
  gtk_box_append((GtkBox *) hbox,
		 (GtkWidget *) envelope_editor->add);

  envelope_editor->remove = (GtkButton *) gtk_button_new_with_mnemonic(i18n("_Remove"));
  gtk_box_append((GtkBox *) hbox,
		 (GtkWidget *) envelope_editor->remove);

  /* cartesian */
  cartesian = 
    envelope_editor->cartesian = ags_cartesian_new();

  cartesian->x_label_func = ags_envelope_editor_x_label_func;
  cartesian->y_label_func = ags_envelope_editor_y_label_func;

  ags_cartesian_fill_label(cartesian,
			   TRUE);
  ags_cartesian_fill_label(cartesian,
			   FALSE);  

  /* cartesian - plot */
  plot = ags_plot_alloc(5, 0, 0);
  plot->join_points = TRUE;

  plot->point_color[0][0] = 0.125;
  plot->point_color[0][1] = 0.5;
  plot->point_color[0][2] = 1.0;

  plot->point_color[1][0] = 0.125;
  plot->point_color[1][1] = 0.5;
  plot->point_color[1][2] = 1.0;

  plot->point_color[2][0] = 0.125;
  plot->point_color[2][1] = 0.5;
  plot->point_color[2][2] = 1.0;

  plot->point_color[3][0] = 0.125;
  plot->point_color[3][1] = 0.5;
  plot->point_color[3][2] = 1.0;

  plot->point_color[4][0] = 0.125;
  plot->point_color[4][1] = 0.5;
  plot->point_color[4][2] = 1.0;

  width = cartesian->x_end - cartesian->x_start;
  height = cartesian->y_end - cartesian->y_start;
  
  default_width = cartesian->x_step_width * cartesian->x_scale_step_width;
  default_height = cartesian->y_step_height * cartesian->y_scale_step_height;

  plot->point[0][0] = 0.0;
  plot->point[0][1] = default_height * 1.0;

  plot->point[1][0] = default_width * 0.25;
  plot->point[1][1] = default_height * 1.0;

  offset = default_width * 0.25;
  
  plot->point[2][0] = offset + default_width * 0.25;
  plot->point[2][1] = default_height * 1.0;

  offset += default_width * 0.25;

  plot->point[3][0] = offset + default_width * 0.25;
  plot->point[3][1] = default_height * 1.0;

  offset += default_width * 0.25;

  plot->point[4][0] = offset + default_width * 0.25;
  plot->point[4][1] = default_height * 1.0;

  ags_cartesian_add_plot(cartesian,
			 plot);

  /* cartesian - size, pack and redraw */
  gtk_widget_set_valign((GtkWidget *) cartesian,
			GTK_ALIGN_START);
  gtk_widget_set_halign((GtkWidget *) cartesian,
			GTK_ALIGN_START);

  gtk_widget_set_size_request((GtkWidget *) cartesian,
			      (gint) width + 2.0 * cartesian->x_margin, (gint) height + 2.0 * cartesian->y_margin);

  gtk_box_append((GtkBox *) envelope_editor,
		 (GtkWidget *) cartesian);

  gtk_widget_queue_draw((GtkWidget *) cartesian);

  /* vbox */
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  
  gtk_widget_set_vexpand((GtkWidget *) vbox,
			 TRUE);
  gtk_widget_set_hexpand((GtkWidget *) vbox,
			 TRUE);

  gtk_widget_set_valign((GtkWidget *) vbox,
			GTK_ALIGN_START);

  gtk_box_append((GtkBox *) envelope_editor,
		 (GtkWidget *) vbox);

  /* grid */
  grid = (GtkGrid *) gtk_grid_new();

  gtk_widget_set_vexpand((GtkWidget *) grid,
			 FALSE);
  gtk_widget_set_hexpand((GtkWidget *) grid,
			 TRUE);

  gtk_widget_set_halign((GtkWidget *) grid,
			GTK_ALIGN_START);
  gtk_widget_set_valign((GtkWidget *) grid,
			GTK_ALIGN_START);

  gtk_grid_set_column_spacing(grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_box_append((GtkBox *) vbox,
		 (GtkWidget *) grid);

  /* attack */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("attack"),
				    "xalign", 0.0,
				    "yalign", 1.0,
				    NULL);
  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);

  control = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				   AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_widget_set_hexpand((GtkWidget *) control,
			 TRUE);
  gtk_widget_set_vexpand((GtkWidget *) control,
			 TRUE);

  gtk_widget_set_halign((GtkWidget *) control,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign((GtkWidget *) control,
			GTK_ALIGN_FILL);

  gtk_box_set_spacing(control,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);
  
  gtk_grid_attach(grid,
		  (GtkWidget *) control,
		  1, 0,
		  1, 1);

  envelope_editor->attack_x = ags_dial_new();

  gtk_adjustment_set_lower(envelope_editor->attack_x->adjustment,
			   0.0);
  gtk_adjustment_set_upper(envelope_editor->attack_x->adjustment,
			   1.0);
  
  gtk_adjustment_set_step_increment(envelope_editor->attack_x->adjustment,
				    0.001);
  gtk_adjustment_set_page_increment(envelope_editor->attack_x->adjustment,
				    0.01);

  gtk_adjustment_set_value(envelope_editor->attack_x->adjustment,
			   0.25);

  gtk_box_append(control,
		 (GtkWidget *) envelope_editor->attack_x);

  envelope_editor->attack_y = ags_dial_new();

  gtk_adjustment_set_lower(envelope_editor->attack_y->adjustment,
			   -1.0);
  gtk_adjustment_set_upper(envelope_editor->attack_y->adjustment,
			   1.0);
  
  gtk_adjustment_set_step_increment(envelope_editor->attack_y->adjustment,
				    0.001);
  gtk_adjustment_set_page_increment(envelope_editor->attack_y->adjustment,
				    0.01);

  gtk_adjustment_set_value(envelope_editor->attack_y->adjustment,
			   0.0);

  gtk_box_append(control,
		 (GtkWidget *) envelope_editor->attack_y);

  /* decay */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("decay"),
				    "xalign", 0.0,
				    "yalign", 1.0,
				    NULL);
  gtk_grid_attach(grid,
		  GTK_WIDGET(label),
		  0, 1,
		  1, 1);

  control = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				   0);
  gtk_grid_attach(grid,
		  GTK_WIDGET(control),
		  1, 1,
		  1, 1);

  envelope_editor->decay_x = ags_dial_new();

  gtk_adjustment_set_lower(envelope_editor->decay_x->adjustment,
			   0.0);
  gtk_adjustment_set_upper(envelope_editor->decay_x->adjustment,
			   1.0);
  
  gtk_adjustment_set_step_increment(envelope_editor->decay_x->adjustment,
				    0.001);
  gtk_adjustment_set_page_increment(envelope_editor->decay_x->adjustment,
				    0.01);

  gtk_adjustment_set_value(envelope_editor->decay_x->adjustment,
			   0.25);

  gtk_box_append((GtkBox *) control,
		 (GtkWidget *) envelope_editor->decay_x);

  envelope_editor->decay_y = ags_dial_new();

  gtk_adjustment_set_lower(envelope_editor->decay_y->adjustment,
			   -1.0);
  gtk_adjustment_set_upper(envelope_editor->decay_y->adjustment,
			   1.0);
  
  gtk_adjustment_set_step_increment(envelope_editor->decay_y->adjustment,
				    0.001);
  gtk_adjustment_set_page_increment(envelope_editor->decay_y->adjustment,
				    0.01);

  gtk_adjustment_set_value(envelope_editor->decay_y->adjustment,
			   0.0);

  gtk_box_append((GtkBox *) control,
		 (GtkWidget *) envelope_editor->decay_y);

  /* sustain */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("sustain"),
				    "xalign", 0.0,
				    "yalign", 1.0,
				    NULL);
  gtk_grid_attach(grid,
		  GTK_WIDGET(label),
		  0, 2,
		  1, 1);

  control = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				   0);
  gtk_grid_attach(grid,
		  GTK_WIDGET(control),
		  1, 2,
		  1, 1);

  envelope_editor->sustain_x = ags_dial_new();

  gtk_adjustment_set_lower(envelope_editor->sustain_x->adjustment,
			   0.0);
  gtk_adjustment_set_upper(envelope_editor->sustain_x->adjustment,
			   1.0);
  
  gtk_adjustment_set_step_increment(envelope_editor->sustain_x->adjustment,
				    0.001);
  gtk_adjustment_set_page_increment(envelope_editor->sustain_x->adjustment,
				    0.01);

  gtk_adjustment_set_value(envelope_editor->sustain_x->adjustment,
			   0.25);

  gtk_box_append(control,
		 (GtkWidget *) envelope_editor->sustain_x);

  envelope_editor->sustain_y = ags_dial_new();

  gtk_adjustment_set_lower(envelope_editor->sustain_y->adjustment,
			   -1.0);
  gtk_adjustment_set_upper(envelope_editor->sustain_y->adjustment,
			   1.0);
  
  gtk_adjustment_set_step_increment(envelope_editor->sustain_y->adjustment,
				    0.001);
  gtk_adjustment_set_page_increment(envelope_editor->sustain_y->adjustment,
				    0.01);

  gtk_adjustment_set_value(envelope_editor->sustain_y->adjustment,
			   0.0);

  gtk_box_append(control,
		 (GtkWidget *) envelope_editor->sustain_y);

  /* release */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("release"),
				    "xalign", 0.0,
				    "yalign", 1.0,
				    NULL);
  gtk_grid_attach(grid,
		  GTK_WIDGET(label),
		  0, 3,
		  1, 1);

  control = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				   0);
  gtk_grid_attach(grid,
		  GTK_WIDGET(control),
		  1, 3,
		  1, 1);

  envelope_editor->release_x = ags_dial_new();

  gtk_adjustment_set_lower(envelope_editor->release_x->adjustment,
			   0.0);
  gtk_adjustment_set_upper(envelope_editor->release_x->adjustment,
			   1.0);
  
  gtk_adjustment_set_step_increment(envelope_editor->release_x->adjustment,
				    0.001);
  gtk_adjustment_set_page_increment(envelope_editor->release_x->adjustment,
				    0.01);

  gtk_adjustment_set_value(envelope_editor->release_x->adjustment,
			   0.25);

  gtk_box_append(control,
		 (GtkWidget *) envelope_editor->release_x);

  envelope_editor->release_y = ags_dial_new();

  gtk_adjustment_set_lower(envelope_editor->release_y->adjustment,
			   -1.0);
  gtk_adjustment_set_upper(envelope_editor->release_y->adjustment,
			   1.0);
  
  gtk_adjustment_set_step_increment(envelope_editor->release_y->adjustment,
				    0.001);
  gtk_adjustment_set_page_increment(envelope_editor->release_y->adjustment,
				    0.01);

  gtk_adjustment_set_value(envelope_editor->release_y->adjustment,
			   0.0);

  gtk_box_append(control,
		 (GtkWidget *) envelope_editor->release_y);

  /* ratio */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("ratio"),
				    "xalign", 0.0,
				    "yalign", 1.0,
				    NULL);
  gtk_grid_attach(grid,
		  GTK_WIDGET(label),
		  0, 4,
		  1, 1);

  control = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				   AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_widget_set_hexpand((GtkWidget *) control,
			 TRUE);
  gtk_widget_set_vexpand((GtkWidget *) control,
			 TRUE);

  gtk_widget_set_halign((GtkWidget *) control,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign((GtkWidget *) control,
			GTK_ALIGN_FILL);

  gtk_box_set_spacing(control,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);
  
  gtk_grid_attach(grid,
		  (GtkWidget *) control,
		  1, 4,
		  1, 1);

  envelope_editor->ratio = ags_dial_new();

  gtk_adjustment_set_lower(envelope_editor->ratio->adjustment,
			   -1.0);
  gtk_adjustment_set_upper(envelope_editor->ratio->adjustment,
			   1.0);
  
  gtk_adjustment_set_step_increment(envelope_editor->ratio->adjustment,
				    0.001);
  gtk_adjustment_set_page_increment(envelope_editor->ratio->adjustment,
				    0.01);

  gtk_adjustment_set_value(envelope_editor->ratio->adjustment,
			   1.0);

  gtk_box_append(control,
		 (GtkWidget *) envelope_editor->ratio);
}

gboolean
ags_envelope_editor_is_connected(AgsConnectable *connectable)
{
  AgsEnvelopeEditor *envelope_editor;
  
  gboolean is_connected;
  
  envelope_editor = AGS_ENVELOPE_EDITOR(connectable);

  /* check is connected */
  is_connected = ((AGS_CONNECTABLE_CONNECTED & (envelope_editor->connectable_flags)) != 0) ? TRUE: FALSE;

  return(is_connected);
}

void
ags_envelope_editor_connect(AgsConnectable *connectable)
{
  AgsEnvelopeEditor *envelope_editor;

  envelope_editor = AGS_ENVELOPE_EDITOR(connectable);

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  envelope_editor->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  /* preset */
  g_signal_connect((GObject *) envelope_editor->preset, "changed",
		   G_CALLBACK(ags_envelope_editor_preset_callback), (gpointer) envelope_editor);

  g_signal_connect((GObject *) envelope_editor->add, "clicked",
		   G_CALLBACK(ags_envelope_editor_preset_add_callback), (gpointer) envelope_editor);

  g_signal_connect((GObject *) envelope_editor->remove, "clicked",
		   G_CALLBACK(ags_envelope_editor_preset_remove_callback), (gpointer) envelope_editor);

  /* attack x,y */
  g_signal_connect((GObject *) envelope_editor->attack_x, "value-changed",
		   G_CALLBACK(ags_envelope_editor_attack_x_callback), (gpointer) envelope_editor);

  g_signal_connect((GObject *) envelope_editor->attack_y, "value-changed",
		   G_CALLBACK(ags_envelope_editor_attack_y_callback), (gpointer) envelope_editor);

  /* decay x,y */
  g_signal_connect((GObject *) envelope_editor->decay_x, "value-changed",
		   G_CALLBACK(ags_envelope_editor_decay_x_callback), (gpointer) envelope_editor);

  g_signal_connect((GObject *) envelope_editor->decay_y, "value-changed",
		   G_CALLBACK(ags_envelope_editor_decay_y_callback), (gpointer) envelope_editor);

  /* sustain x,y */
  g_signal_connect((GObject *) envelope_editor->sustain_x, "value-changed",
		   G_CALLBACK(ags_envelope_editor_sustain_x_callback), (gpointer) envelope_editor);

  g_signal_connect((GObject *) envelope_editor->sustain_y, "value-changed",
		   G_CALLBACK(ags_envelope_editor_sustain_y_callback), (gpointer) envelope_editor);

  /* release x,y */
  g_signal_connect((GObject *) envelope_editor->release_x, "value-changed",
		   G_CALLBACK(ags_envelope_editor_release_x_callback), (gpointer) envelope_editor);

  g_signal_connect((GObject *) envelope_editor->release_y, "value-changed",
		   G_CALLBACK(ags_envelope_editor_release_y_callback), (gpointer) envelope_editor);

  /* ratio */
  g_signal_connect((GObject *) envelope_editor->ratio, "value-changed",
		   G_CALLBACK(ags_envelope_editor_ratio_callback), (gpointer) envelope_editor);
}

void
ags_envelope_editor_disconnect(AgsConnectable *connectable)
{
  AgsEnvelopeEditor *envelope_editor;

  envelope_editor = AGS_ENVELOPE_EDITOR(connectable);

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  envelope_editor->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  /* preset */
  g_object_disconnect((GObject *) envelope_editor->preset,
		      "any_signal::changed",
		      G_CALLBACK(ags_envelope_editor_preset_callback),
		      (gpointer) envelope_editor,
		      NULL);

  g_object_disconnect((GObject *) envelope_editor->add,
		      "any_signal::clicked",
		      G_CALLBACK(ags_envelope_editor_preset_add_callback),
		      (gpointer) envelope_editor,
		      NULL);

  g_object_disconnect((GObject *) envelope_editor->remove,
		      "any_signal::clicked",
		      G_CALLBACK(ags_envelope_editor_preset_remove_callback),
		      (gpointer) envelope_editor,
		      NULL);

  /* attack x,y */
  g_object_disconnect((GObject *) envelope_editor->attack_x,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_envelope_editor_attack_x_callback),
		      (gpointer) envelope_editor,
		      NULL);

  g_object_disconnect((GObject *) envelope_editor->attack_y,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_envelope_editor_attack_y_callback),
		      (gpointer) envelope_editor,
		      NULL);

  /* decay x,y */
  g_object_disconnect((GObject *) envelope_editor->decay_x,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_envelope_editor_decay_x_callback),
		      (gpointer) envelope_editor,
		      NULL);

  g_object_disconnect((GObject *) envelope_editor->decay_y,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_envelope_editor_decay_y_callback),
		      (gpointer) envelope_editor,
		      NULL);

  /* sustain x,y */
  g_object_disconnect((GObject *) envelope_editor->sustain_x,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_envelope_editor_sustain_x_callback),
		      (gpointer) envelope_editor,
		      NULL);

  g_object_disconnect((GObject *) envelope_editor->sustain_y,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_envelope_editor_sustain_y_callback),
		      (gpointer) envelope_editor,
		      NULL);

  /* release x,y */
  g_object_disconnect((GObject *) envelope_editor->release_x,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_envelope_editor_release_x_callback),
		      (gpointer) envelope_editor,
		      NULL);

  g_object_disconnect((GObject *) envelope_editor->release_y,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_envelope_editor_release_y_callback),
		      (gpointer) envelope_editor,
		      NULL);

  /* ratio */
  g_object_disconnect((GObject *) envelope_editor->ratio,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_envelope_editor_ratio_callback),
		      (gpointer) envelope_editor,
		      NULL);
}

void
ags_envelope_editor_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_envelope_editor_apply(AgsApplicable *applicable)
{
  AgsEnvelopeDialog *envelope_dialog;
  AgsEnvelopeEditor *envelope_editor;
  
  AgsMachine *machine;

  AgsAudio *audio;
  
  GList *start_notation, *notation;
  GList *start_selection, *selection;

  double attack_x, attack_y;
  double decay_x, decay_y;
  double sustain_x, sustain_y;
  double release_x, release_y;
  double ratio;
  
  double _Complex z;
  
  envelope_editor = AGS_ENVELOPE_EDITOR(applicable);
  envelope_dialog = (AgsEnvelopeDialog *) gtk_widget_get_ancestor((GtkWidget *) envelope_editor,
								  AGS_TYPE_ENVELOPE_DIALOG);

  machine = envelope_dialog->machine;

  audio = machine->audio;

  /* get z */
  attack_x = ags_dial_get_value(AGS_DIAL(envelope_editor->attack_x));
  attack_y = ags_dial_get_value(AGS_DIAL(envelope_editor->attack_y));

  decay_x = ags_dial_get_value(AGS_DIAL(envelope_editor->decay_x));
  decay_y = ags_dial_get_value(AGS_DIAL(envelope_editor->decay_y));

  sustain_x = ags_dial_get_value(AGS_DIAL(envelope_editor->sustain_x));
  sustain_y = ags_dial_get_value(AGS_DIAL(envelope_editor->sustain_y));

  release_x = ags_dial_get_value(AGS_DIAL(envelope_editor->release_x));
  release_y = ags_dial_get_value(AGS_DIAL(envelope_editor->release_y));

  ratio = ags_dial_get_value(AGS_DIAL(envelope_editor->ratio));

  /* notation */
  g_object_get(audio,
	       "notation", &start_notation,
	       NULL);

  notation = start_notation;

  /* set attack, decay, sustain and release */
  while(notation != NULL){
    GRecMutex *notation_mutex;

    /* get notation mutex */
    notation_mutex = AGS_NOTATION_GET_OBJ_MUTEX(notation->data);

    /**/
    g_rec_mutex_lock(notation_mutex);

    selection =
      start_selection = g_list_copy_deep(AGS_NOTATION(notation->data)->selection,
					 (GCopyFunc) g_object_ref,
					 NULL);

    g_rec_mutex_unlock(notation_mutex);

    while(selection != NULL){
      AgsNote *current_note;

      GRecMutex *note_mutex;
      
      current_note = AGS_NOTE(selection->data);

      /* get note mutex */
      note_mutex = AGS_NOTE_GET_OBJ_MUTEX(current_note);

      /* apply */
      g_rec_mutex_lock(note_mutex);

      current_note->flags |= AGS_NOTE_ENVELOPE;
      
      z = attack_x + I * attack_y;
      ags_complex_set(&(current_note->attack),
		      z);

      z = decay_x + I * decay_y;
      ags_complex_set(&(current_note->decay),
		      z);

      z = sustain_x + I * sustain_y;
      ags_complex_set(&(current_note->sustain),
		      z);

      z = release_x + I * release_y;
      ags_complex_set(&(current_note->release),
		      z);

      z = 0.0 + I * ratio;
      ags_complex_set(&(current_note->ratio),
		      z);
      
      g_rec_mutex_unlock(note_mutex);

      /* iterate */
      selection = selection->next;
    }

    g_list_free_full(start_selection,
		     g_object_unref);
    
    notation = notation->next;
  }

  g_list_free_full(start_notation,
		   g_object_unref);
}

void
ags_envelope_editor_reset(AgsApplicable *applicable)
{
  AgsEnvelopeEditor *envelope_editor;

  envelope_editor = AGS_ENVELOPE_EDITOR(applicable);
  
  ags_envelope_editor_load_preset(envelope_editor);
}

gchar*
ags_envelope_editor_x_label_func(gdouble value,
				 gpointer data)
{
  gchar *format;
  gchar *str;
  
  format = g_strdup_printf("%%.%df",
			   (guint) ceil(AGS_CARTESIAN(data)->y_label_precision));

  str = g_strdup_printf(format,
			value / 10.0);
  g_free(format);

  return(str);
}

gchar*
ags_envelope_editor_y_label_func(gdouble value,
				 gpointer data)
{
  gchar *format;
  gchar *str;
  
  format = g_strdup_printf("%%.%df",
			   (guint) ceil(AGS_CARTESIAN(data)->y_label_precision));

  str = g_strdup_printf(format,
			value / 10.0);
  g_free(format);

  return(str);
}

/**
 * ags_envelope_editor_get_active_preset:
 * @envelope_editor: the #AgsEnvelopeEditor
 * 
 * Get active preset.
 * 
 * Returns: the matching #AgsPreset, if none selected %NULL
 * 
 * Since: 3.0.0
 */
AgsPreset*
ags_envelope_editor_get_active_preset(AgsEnvelopeEditor *envelope_editor)
{
  AgsEnvelopeDialog *envelope_dialog;

  AgsMachine *machine;

  AgsAudio *audio;
  AgsPreset *current;

  GList *start_preset, *preset;

  gchar *preset_name;
  
  if(!AGS_IS_ENVELOPE_EDITOR(envelope_editor)){
    return(NULL);
  }
    
  envelope_dialog = (AgsEnvelopeDialog *) gtk_widget_get_ancestor((GtkWidget *) envelope_editor,
								  AGS_TYPE_ENVELOPE_DIALOG);

  machine = envelope_dialog->machine;

  audio = machine->audio;

  /* preset name */
  preset_name = gtk_combo_box_text_get_active_text(envelope_editor->preset);
  
  /* find preset */
  g_object_get(audio,
	       "preset", &start_preset,
	       NULL);
  
  preset = start_preset;  
  current = NULL;
  
  preset = ags_preset_find_name(preset,
				preset_name);

  g_free(preset_name);
  
  if(preset != NULL){
    current = preset->data;
  }

  g_list_free_full(start_preset,
		   g_object_unref);
  
  return(current);
}

/**
 * ags_envelope_editor_load_preset:
 * @envelope_editor: the #AgsPatternEnvelope
 *
 * Load preset.
 * 
 * Since: 3.0.0
 */
void
ags_envelope_editor_load_preset(AgsEnvelopeEditor *envelope_editor)
{
  AgsEnvelopeDialog *envelope_dialog;

  AgsMachine *machine;

  GtkTreeModel *model;

  AgsAudio *audio;
    
  GList *start_preset, *preset;
  
  if(!AGS_IS_ENVELOPE_EDITOR(envelope_editor)){
    return;
  }

  envelope_dialog = (AgsEnvelopeDialog *) gtk_widget_get_ancestor((GtkWidget *) envelope_editor,
								  AGS_TYPE_ENVELOPE_DIALOG);

  machine = envelope_dialog->machine;

  audio = machine->audio;

  /* get model */
  model = GTK_TREE_MODEL(gtk_combo_box_get_model(GTK_COMBO_BOX(envelope_editor->preset)));

  /* clear old */
  gtk_list_store_clear(GTK_LIST_STORE(model));

  /* create new */
  g_object_get(audio,
	       "preset", &start_preset,
	       NULL);
  
  preset = start_preset;

  while(preset != NULL){
    if(AGS_PRESET(preset->data)->preset_name != NULL){
      gtk_combo_box_text_append_text(envelope_editor->preset,
				     AGS_PRESET(preset->data)->preset_name);
    }

    preset = preset->next;
  }

  g_list_free_full(start_preset,
		   g_object_unref);
}

/**
 * ags_envelope_editor_add_preset:
 * @envelope_editor: the #AgsPatternEnvelope
 * @preset_name: the preset name
 *
 * Add preset.
 * 
 * Since: 3.0.0
 */
void
ags_envelope_editor_add_preset(AgsEnvelopeEditor *envelope_editor,
			       gchar *preset_name)
{
  AgsEnvelopeDialog *envelope_dialog;

  AgsMachine *machine;

  AgsAudio *audio;
  AgsPreset *preset;

  GList *start_preset;
  
  AgsComplex *val;
  
  GValue value = G_VALUE_INIT;
  
  if(!AGS_IS_ENVELOPE_EDITOR(envelope_editor) ||
     preset_name == NULL){
    return;
  }
  
  envelope_dialog = (AgsEnvelopeDialog *) gtk_widget_get_ancestor((GtkWidget *) envelope_editor,
								  AGS_TYPE_ENVELOPE_DIALOG);

  machine = envelope_dialog->machine;

  audio = machine->audio;

  g_object_get(audio,
	       "preset", &start_preset,
	       NULL);
  
  /* check if already present */
  if(ags_preset_find_name(start_preset,
			  preset_name) != NULL){    
    g_list_free_full(start_preset,
		     g_object_unref);
    
    return;
  }

  /* create preset */
  preset = g_object_new(AGS_TYPE_PRESET,
			"scope", "ags-envelope",
			"preset-name", preset_name,
			NULL);
  ags_audio_add_preset(audio,
		       (GObject *) preset);

  /* preset - attack */
  val = ags_complex_alloc();
  ags_complex_set(val,
		  0.25 + I * 0.0);
  
  g_value_init(&value,
	       AGS_TYPE_COMPLEX);
  g_value_set_boxed(&value,
		    val);

  ags_preset_add_parameter(preset,
			   "attack", &value);

  /* preset - decay */
  val = ags_complex_alloc();
  ags_complex_set(val,
		  0.25 + I * 0.0);
  
  g_value_reset(&value);
  g_value_set_boxed(&value,
		    val);

  ags_preset_add_parameter(preset,
			   "decay", &value);

  /* preset - sustain */
  val = ags_complex_alloc();
  ags_complex_set(val,
		  0.25 + I * 0.0);
  
  g_value_reset(&value);
  g_value_set_boxed(&value,
		    val);

  ags_preset_add_parameter(preset,
			   "sustain", &value);

  /* preset - release */
  val = ags_complex_alloc();
  ags_complex_set(val,
		  0.25 + I * 0.0);
  
  g_value_reset(&value);
  g_value_set_boxed(&value,
		    val);

  ags_preset_add_parameter(preset,
			   "release", &value);

  /* preset - ratio */
  val = ags_complex_alloc();
  ags_complex_set(val,
		  0.0 + I * 1.0);
  
  g_value_reset(&value);
  g_value_set_boxed(&value,
		    val);

  ags_preset_add_parameter(preset,
			   "ratio", &value);

  g_list_free(start_preset);
}

/**
 * ags_envelope_editor_remove_preset:
 * @envelope_editor: the #AgsPatternEnvelope
 * @nth: the nth preset to remove
 *
 * Remove preset.
 * 
 * Since: 3.0.0
 */
void
ags_envelope_editor_remove_preset(AgsEnvelopeEditor *envelope_editor,
				  guint nth)
{
  AgsEnvelopeDialog *envelope_dialog;

  AgsMachine *machine;

  AgsAudio *audio;
  AgsPreset *preset;
  
  GList *start_preset;
  
  if(!AGS_IS_ENVELOPE_EDITOR(envelope_editor)){
    return;
  }
  
  envelope_dialog = (AgsEnvelopeDialog *) gtk_widget_get_ancestor((GtkWidget *) envelope_editor,
								  AGS_TYPE_ENVELOPE_DIALOG);

  machine = envelope_dialog->machine;

  audio = machine->audio;
  
  /* remove preset */
  g_object_get(audio,
	       "preset", &start_preset,
	       NULL);
  preset = g_list_nth_data(start_preset,
			   nth);

  g_list_free_full(start_preset,
		   g_object_unref);

  ags_audio_remove_preset(audio,
			  (GObject *) preset);
}

/**
 * ags_envelope_editor_reset_control:
 * @envelope_editor: the #AgsEnvelopeEditor
 * 
 * Reset controls.
 * 
 * Since: 3.0.0
 */
void
ags_envelope_editor_reset_control(AgsEnvelopeEditor *envelope_editor)
{
  AgsPreset *preset;
  
  AgsComplex *val;
  
  double _Complex z;
  
  GValue value = G_VALUE_INIT;

  GError *error;
  
  if(!AGS_IS_ENVELOPE_EDITOR(envelope_editor)){
    return;
  }

  /* disable update */
  envelope_editor->flags |= AGS_ENVELOPE_EDITOR_NO_UPDATE;
  
  /* check preset */
  preset = ags_envelope_editor_get_active_preset(envelope_editor);
  
  if(preset == NULL){
    envelope_editor->flags &= (~AGS_ENVELOPE_EDITOR_NO_UPDATE);
    
    return;
  }

  /* attack */
  g_value_init(&value,
	       AGS_TYPE_COMPLEX);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "attack", &value,
			   &error);

  if(error != NULL){
    g_warning("%s", error->message);

    envelope_editor->flags &= (~AGS_ENVELOPE_EDITOR_NO_UPDATE);

    g_error_free(error);
    
    return;
  }

  val = (AgsComplex *) g_value_get_boxed(&value);  
  z = ags_complex_get(val);
  
  ags_dial_set_value(envelope_editor->attack_x,
		     creal(z));
  ags_dial_set_value(envelope_editor->attack_y,
		     cimag(z));

  /* decay */
  g_value_reset(&value);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "decay", &value,
			   &error);

  if(error != NULL){
    g_warning("%s", error->message);

    envelope_editor->flags &= (~AGS_ENVELOPE_EDITOR_NO_UPDATE);

    g_error_free(error);

    return;
  }

  val = (AgsComplex *) g_value_get_boxed(&value);  
  z = ags_complex_get(val);
  
  ags_dial_set_value(envelope_editor->decay_x,
		     creal(z));
  ags_dial_set_value(envelope_editor->decay_y,
		     cimag(z));

  /* sustain */
  g_value_reset(&value);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "sustain", &value,
			   &error);

  if(error != NULL){
    g_warning("%s", error->message);

    envelope_editor->flags &= (~AGS_ENVELOPE_EDITOR_NO_UPDATE);

    g_error_free(error);

    return;
  }

  val = (AgsComplex *) g_value_get_boxed(&value);  
  z = ags_complex_get(val);
  
  ags_dial_set_value(envelope_editor->sustain_x,
		     creal(z));
  ags_dial_set_value(envelope_editor->sustain_y,
		     cimag(z));

  /* release */
  g_value_reset(&value);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "release", &value,
			   &error);

  if(error != NULL){
    g_warning("%s", error->message);

    envelope_editor->flags &= (~AGS_ENVELOPE_EDITOR_NO_UPDATE);

    g_error_free(error);
   
    return;
  }

  val = (AgsComplex *) g_value_get_boxed(&value);  
  z = ags_complex_get(val);
  
  ags_dial_set_value(envelope_editor->release_x,
		     creal(z));
  ags_dial_set_value(envelope_editor->release_y,
		     cimag(z));

  /* ratio */
  g_value_reset(&value);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "ratio", &value,
			   &error);

  if(error != NULL){
    g_warning("%s", error->message);

    envelope_editor->flags &= (~AGS_ENVELOPE_EDITOR_NO_UPDATE);

    g_error_free(error);

    return;
  }

  val = (AgsComplex *) g_value_get_boxed(&value);  
  z = ags_complex_get(val);
  
  ags_dial_set_value(envelope_editor->ratio,
		     cimag(z));

  /* unset no update */
  envelope_editor->flags &= (~AGS_ENVELOPE_EDITOR_NO_UPDATE);
}

/**
 * ags_envelope_editor_plot:
 * @envelope_editor: the #AgsEnvelopeEditor
 * 
 * Plot envelope.
 * 
 * Since: 3.0.0
 */
void
ags_envelope_editor_plot(AgsEnvelopeEditor *envelope_editor)
{
  AgsCartesian *cartesian;
  
  AgsPlot *plot;

  gdouble default_width, default_height;
  gdouble attack_x, attack_y;
  gdouble decay_x, decay_y;
  gdouble sustain_x, sustain_y;
  gdouble release_x, release_y;
  gdouble ratio;
  gdouble offset;
  
  if(!AGS_IS_ENVELOPE_EDITOR(envelope_editor)){
    return;
  }
  
  cartesian = envelope_editor->cartesian;

  plot = cartesian->plot->data;
  
  default_width = cartesian->x_step_width * cartesian->x_scale_step_width;
  default_height = cartesian->y_step_height * cartesian->y_scale_step_height;

  attack_x = ags_dial_get_value(envelope_editor->attack_x);
  attack_y = ags_dial_get_value(envelope_editor->attack_y);

  decay_x = ags_dial_get_value(envelope_editor->decay_x);
  decay_y = ags_dial_get_value(envelope_editor->decay_y);

  sustain_x = ags_dial_get_value(envelope_editor->sustain_x);
  sustain_y = ags_dial_get_value(envelope_editor->sustain_y);

  release_x = ags_dial_get_value(envelope_editor->release_x);
  release_y = ags_dial_get_value(envelope_editor->release_y);

  ratio = ags_dial_get_value(envelope_editor->ratio);

  /* reset plot points */
  plot->point[0][0] = 0.0;
  plot->point[0][1] = default_height * ratio;

  plot->point[1][0] = default_width * attack_x;
  plot->point[1][1] = default_height * (attack_y + ratio);
    
  offset = default_width * attack_x;
  
  plot->point[2][0] = offset + default_width * decay_x;
  plot->point[2][1] = default_height * (decay_y + ratio);
  
  offset += default_width * decay_x;
  
  plot->point[3][0] = offset + default_width * sustain_x;
  plot->point[3][1] = default_height * (sustain_y + ratio);
  
  offset += default_width * sustain_x;

  plot->point[4][0] = offset + default_width * release_x;
  plot->point[4][1] = default_height * (release_y + ratio);
  
  /* redraw */
  gtk_widget_queue_draw((GtkWidget *) cartesian);
}

/**
 * ags_envelope_editor_new:
 *
 * Create a new instance of #AgsEnvelopeEditor
 *
 * Returns: the new #AgsEnvelopeEditor
 *
 * Since: 3.0.0
 */
AgsEnvelopeEditor*
ags_envelope_editor_new()
{
  AgsEnvelopeEditor *envelope_editor;

  envelope_editor = (AgsEnvelopeEditor *) g_object_new(AGS_TYPE_ENVELOPE_EDITOR,
						       NULL);

  return(envelope_editor);
}

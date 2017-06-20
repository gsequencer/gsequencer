/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/X/editor/ags_pattern_envelope.h>
#include <ags/X/editor/ags_pattern_envelope_callbacks.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>

#include <ags/audio/ags_audio.h>

#include <ags/X/editor/ags_envelope_dialog.h>

#include <math.h>

#include <ags/i18n.h>

void ags_pattern_envelope_class_init(AgsPatternEnvelopeClass *pattern_envelope);
void ags_pattern_envelope_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_pattern_envelope_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_pattern_envelope_init(AgsPatternEnvelope *pattern_envelope);
void ags_pattern_envelope_connect(AgsConnectable *connectable);
void ags_pattern_envelope_disconnect(AgsConnectable *connectable);
void ags_pattern_envelope_finalize(GObject *gobject);
void ags_pattern_envelope_set_update(AgsApplicable *applicable, gboolean update);
void ags_pattern_envelope_apply(AgsApplicable *applicable);
void ags_pattern_envelope_reset(AgsApplicable *applicable);

gchar* ags_pattern_envelope_x_label_func(gdouble value,
					 gpointer data);
gchar* ags_pattern_envelope_y_label_func(gdouble value,
					 gpointer data);

/**
 * SECTION:ags_pattern_envelope
 * @short_description: pack pad editors.
 * @title: AgsPatternEnvelope
 * @section_id:
 * @include: ags/X/ags_pattern_envelope.h
 *
 * #AgsPatternEnvelope is a composite widget to show envelope controls
 * of selected AgsNote.
 */

static gpointer ags_pattern_envelope_parent_class = NULL;

GType
ags_pattern_envelope_get_type(void)
{
  static GType ags_type_pattern_envelope = 0;

  if(!ags_type_pattern_envelope){
    static const GTypeInfo ags_pattern_envelope_info = {
      sizeof (AgsPatternEnvelopeClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_pattern_envelope_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPatternEnvelope),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_pattern_envelope_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_pattern_envelope_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_pattern_envelope_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_pattern_envelope = g_type_register_static(GTK_TYPE_VBOX,
						       "AgsPatternEnvelope", &ags_pattern_envelope_info,
						       0);

    g_type_add_interface_static(ags_type_pattern_envelope,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_pattern_envelope,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }
  
  return(ags_type_pattern_envelope);
}

void
ags_pattern_envelope_class_init(AgsPatternEnvelopeClass *pattern_envelope)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GParamSpec *param_spec;

  ags_pattern_envelope_parent_class = g_type_class_peek_parent(pattern_envelope);

  /* GObjectClass */
  gobject = (GObjectClass *) pattern_envelope;

  gobject->finalize = ags_pattern_envelope_finalize;
  
  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) pattern_envelope;
}

void
ags_pattern_envelope_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_pattern_envelope_connect;
  connectable->disconnect = ags_pattern_envelope_disconnect;
}

void
ags_pattern_envelope_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_pattern_envelope_set_update;
  applicable->apply = ags_pattern_envelope_apply;
  applicable->reset = ags_pattern_envelope_reset;
}

void
ags_pattern_envelope_init(AgsPatternEnvelope *pattern_envelope)
{
  GtkTable *table;
  GtkHBox *hbox;
  GtkVBox *control;
  AgsCartesian *cartesian;
  GtkLabel *label;
  
  GtkCellRenderer *edit_renderer;
  GtkCellRenderer *plot_renderer;
  GtkCellRenderer *renderer;

  GtkListStore  *model;

  gdouble width, height;

  pattern_envelope->flags = 0;

  pattern_envelope->version = AGS_PATTERN_ENVELOPE_DEFAULT_VERSION;
  pattern_envelope->build_id = AGS_PATTERN_ENVELOPE_DEFAULT_BUILD_ID;

  /* enabled */
  //NOTE:JK: it is edited in place since only preset is used
  pattern_envelope->enabled = NULL; // gtk_check_button_new_with_label(i18n("enabled"));
    //  gtk_box_pack_start((GtkBox *) pattern_envelope,
    //		     pattern_envelope->enabled,
    //		     FALSE, FALSE,
    //		     0);

  /* cartesian */
  cartesian = 
    pattern_envelope->cartesian = ags_cartesian_new();

  cartesian->x_label_func = ags_pattern_envelope_x_label_func;
  cartesian->y_label_func = ags_pattern_envelope_y_label_func;

  ags_cartesian_fill_label(cartesian,
			   TRUE);
  ags_cartesian_fill_label(cartesian,
			   FALSE);  

  /* cartesian - width and height */
  width = cartesian->x_end - cartesian->x_start;
  height = cartesian->y_end - cartesian->y_start;

  /* cartesian - size, pack and redraw */
  gtk_widget_set_size_request(cartesian,
			      (gint) width + 2.0 * cartesian->x_margin, (gint) height + 2.0 * cartesian->y_margin);
  gtk_box_pack_start((GtkBox *) pattern_envelope,
		     (GtkWidget *) cartesian,
		     FALSE, FALSE,
		     0);

  gtk_widget_queue_draw(cartesian);

  /* tree view */
  pattern_envelope->tree_view = (GtkTreeView *) gtk_tree_view_new();

  model = gtk_list_store_new(AGS_PATTERN_ENVELOPE_COLUMN_LAST,
			     G_TYPE_BOOLEAN,
			     G_TYPE_BOOLEAN,
			     G_TYPE_STRING,
			     G_TYPE_UINT,
			     G_TYPE_UINT,
			     G_TYPE_UINT,
			     G_TYPE_UINT,
			     G_TYPE_UINT,
			     G_TYPE_UINT);
  gtk_tree_view_set_model(pattern_envelope->tree_view,
			  model);
  
  edit_renderer = gtk_cell_renderer_toggle_new();
  plot_renderer = gtk_cell_renderer_toggle_new();
  renderer = gtk_cell_renderer_text_new();

  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(pattern_envelope->tree_view),
					      -1,
					      i18n("edit"),
					      edit_renderer,
					      "active", AGS_PATTERN_ENVELOPE_COLUMN_EDIT,
					      NULL);

  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(pattern_envelope->tree_view),
					      -1,
					      i18n("plot"),
					      plot_renderer,
					      "active", AGS_PATTERN_ENVELOPE_COLUMN_PLOT,
					      NULL);

  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(pattern_envelope->tree_view),
					      -1,
					      i18n("preset"),
					      renderer,
					      "text", AGS_PATTERN_ENVELOPE_COLUMN_PRESET_NAME,
					      NULL);

  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(pattern_envelope->tree_view),
					      -1,
					      i18n("audio channel start"),
					      renderer,
					      "text", AGS_PATTERN_ENVELOPE_COLUMN_AUDIO_CHANNEL_START,
					      NULL);

  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(pattern_envelope->tree_view),
					      -1,
					      i18n("audio channel end"),
					      renderer,
					      "text", AGS_PATTERN_ENVELOPE_COLUMN_AUDIO_CHANNEL_END,
					      NULL);

  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(pattern_envelope->tree_view),
					      -1,
					      i18n("pad start"),
					      renderer,
					      "text", AGS_PATTERN_ENVELOPE_COLUMN_PAD_START,
					      NULL);

  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(pattern_envelope->tree_view),
					      -1,
					      i18n("pad end"),
					      renderer,
					      "text", AGS_PATTERN_ENVELOPE_COLUMN_PAD_END,
					      NULL);

  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(pattern_envelope->tree_view),
					      -1,
					      i18n("x start"),
					      renderer,
					      "text", AGS_PATTERN_ENVELOPE_COLUMN_X_START,
					      NULL);

  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(pattern_envelope->tree_view),
					      -1,
					      i18n("x end"),
					      renderer,
					      "text", AGS_PATTERN_ENVELOPE_COLUMN_X_END,
					      NULL);

  gtk_box_pack_start((GtkBox *) pattern_envelope,
		     (GtkWidget *) pattern_envelope->tree_view,
		     FALSE, FALSE,
		     0);

  g_signal_connect(G_OBJECT(edit_renderer), "toggled\0",
		   G_CALLBACK(ags_pattern_envelope_edit_callback), pattern_envelope);

  g_signal_connect(G_OBJECT(plot_renderer), "toggled\0",
		   G_CALLBACK(ags_pattern_envelope_plot_callback), pattern_envelope);

  /* table */
  table = (GtkTable *) gtk_table_new(6, 2,
				     FALSE);
  gtk_box_pack_start((GtkBox *) pattern_envelope,
		     GTK_WIDGET(table),
		     FALSE, FALSE,
		     0);

  /* audio channel - start */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("audio channel start"),
				    "xalign", 0.0,
				    "yalign", 1.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  pattern_envelope->audio_channel_start = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
											   0.0,
											   1.0);
  gtk_table_attach(table,
		   GTK_WIDGET(pattern_envelope->audio_channel_start),
		   1, 2,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* audio channel - end */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("audio channel end"),
				    "xalign", 0.0,
				    "yalign", 1.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  pattern_envelope->audio_channel_end = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
											 65535.0,
											 1.0);
  gtk_table_attach(table,
		   GTK_WIDGET(pattern_envelope->audio_channel_end),
		   1, 2,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* pad - start */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("pad start"),
				    "xalign", 0.0,
				    "yalign", 1.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  pattern_envelope->pad_start = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
										 65535.0,
										 1.0);
  gtk_table_attach(table,
		   GTK_WIDGET(pattern_envelope->pad_start),
		   1, 2,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* pad - end */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("pad end"),
				    "xalign", 0.0,
				    "yalign", 1.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   3, 4,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  pattern_envelope->pad_end = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
									       65535.0,
									       1.0);
  gtk_table_attach(table,
		   GTK_WIDGET(pattern_envelope->pad_end),
		   1, 2,
		   3, 4,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* x - start */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("x start"),
				    "xalign", 0.0,
				    "yalign", 1.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   4, 5,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  pattern_envelope->x_start = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
									       65535.0,
									       1.0);
  gtk_table_attach(table,
		   GTK_WIDGET(pattern_envelope->x_start),
		   1, 2,
		   4, 5,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* x - end */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("x end"),
				    "xalign", 0.0,
				    "yalign", 1.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   5, 6,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  pattern_envelope->x_end = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
									     65535.0,
									     1.0);
  gtk_table_attach(table,
		   GTK_WIDGET(pattern_envelope->x_end),
		   1, 2,
		   5, 6,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* table */
  table = (GtkTable *) gtk_table_new(5, 2,
				     FALSE);
  gtk_box_pack_start((GtkBox *) pattern_envelope,
		     GTK_WIDGET(table),
		     FALSE, FALSE,
		     0);

  /* attack */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("attack"),
				    "xalign", 0.0,
				    "yalign", 1.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  control = (GtkVBox *) gtk_vbox_new(FALSE,
				     0);
  gtk_table_attach(table,
		   GTK_WIDGET(control),
		   1, 2,
		   0, 1,
		   GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND,
		   0, 0);
  
  pattern_envelope->attack_x = (GtkHScale *) gtk_hscale_new_with_range(0.0, 1.0, 0.001);
  gtk_scale_set_draw_value(pattern_envelope->attack_x,
			   TRUE);
  gtk_range_set_value((GtkRange *) pattern_envelope->attack_x,
		      0.25);
  gtk_box_pack_start((GtkBox *) control,
		     (GtkWidget *) pattern_envelope->attack_x,
		     FALSE, FALSE,
		     0);

  pattern_envelope->attack_y = (GtkHScale *) gtk_hscale_new_with_range(-1.0, 1.0, 0.001);
  gtk_scale_set_draw_value(pattern_envelope->attack_y,
			   TRUE);
  gtk_range_set_value((GtkRange *) pattern_envelope->attack_y,
		      1.0);
  gtk_box_pack_start((GtkBox *) control,
		     (GtkWidget *) pattern_envelope->attack_y,
		     FALSE, FALSE,
		     0);

  /* decay */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("decay"),
				    "xalign", 0.0,
				    "yalign", 1.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  control = (GtkVBox *) gtk_vbox_new(FALSE,
				     0);
  gtk_table_attach(table,
		   GTK_WIDGET(control),
		   1, 2,
		   1, 2,
		   GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND,
		   0, 0);

  pattern_envelope->decay_x = (GtkHScale *) gtk_hscale_new_with_range(0.0, 1.0, 0.001);
  gtk_scale_set_draw_value(pattern_envelope->decay_x,
			   TRUE);
  gtk_range_set_value((GtkRange *) pattern_envelope->decay_x,
		      0.25);
  gtk_box_pack_start((GtkBox *) control,
		     (GtkWidget *) pattern_envelope->decay_x,
		     FALSE, FALSE,
		     0);

  pattern_envelope->decay_y = (GtkHScale *) gtk_hscale_new_with_range(-1.0, 1.0, 0.001);
  gtk_scale_set_draw_value(pattern_envelope->decay_y,
			   TRUE);
  gtk_range_set_value((GtkRange *) pattern_envelope->decay_y,
		      1.0);
  gtk_box_pack_start((GtkBox *) control,
		     (GtkWidget *) pattern_envelope->decay_y,
		     FALSE, FALSE,
		     0);

  /* sustain */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("sustain"),
				    "xalign", 0.0,
				    "yalign", 1.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  control = (GtkVBox *) gtk_vbox_new(FALSE,
				     0);
  gtk_table_attach(table,
		   GTK_WIDGET(control),
		   1, 2,
		   2, 3,
		   GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND,
		   0, 0);

  pattern_envelope->sustain_x = (GtkHScale *) gtk_hscale_new_with_range(0.0, 1.0, 0.001);
  gtk_scale_set_draw_value(pattern_envelope->sustain_x,
			   TRUE);
  gtk_range_set_value((GtkRange *) pattern_envelope->sustain_x,
		      0.25);
  gtk_box_pack_start((GtkBox *) control,
		     (GtkWidget *) pattern_envelope->sustain_x,
		     FALSE, FALSE,
		     0);

  pattern_envelope->sustain_y = (GtkHScale *) gtk_hscale_new_with_range(-1.0, 1.0, 0.001);
  gtk_scale_set_draw_value(pattern_envelope->sustain_y,
			   TRUE);
  gtk_range_set_value((GtkRange *) pattern_envelope->sustain_y,
		      1.0);
  gtk_box_pack_start((GtkBox *) control,
		     (GtkWidget *) pattern_envelope->sustain_y,
		     FALSE, FALSE,
		     0);

  /* release */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("release"),
				    "xalign", 0.0,
				    "yalign", 1.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   3, 4,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  control = (GtkVBox *) gtk_vbox_new(FALSE,
				     0);
  gtk_table_attach(table,
		   GTK_WIDGET(control),
		   1, 2,
		   3, 4,
		   GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND,
		   0, 0);

  pattern_envelope->release_x = (GtkHScale *) gtk_hscale_new_with_range(0.0, 1.0, 0.001);
  gtk_scale_set_draw_value(pattern_envelope->release_x,
			   TRUE);
  gtk_range_set_value((GtkRange *) pattern_envelope->release_x,
		      0.25);
  gtk_box_pack_start((GtkBox *) control,
		     (GtkWidget *) pattern_envelope->release_x,
		     FALSE, FALSE,
		     0);

  pattern_envelope->release_y = (GtkHScale *) gtk_hscale_new_with_range(-1.0, 1.0, 0.001);
  gtk_scale_set_draw_value(pattern_envelope->release_y,
			   TRUE);
  gtk_range_set_value((GtkRange *) pattern_envelope->release_y,
		      1.0);
  gtk_box_pack_start((GtkBox *) control,
		     (GtkWidget *) pattern_envelope->release_y,
		     FALSE, FALSE,
		     0);

  /* ratio */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("ratio"),
				    "xalign", 0.0,
				    "yalign", 1.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   4, 5,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  pattern_envelope->ratio = (GtkHScale *) gtk_hscale_new_with_range(0.0, 1.0, 0.001);
  gtk_scale_set_draw_value(pattern_envelope->ratio,
			   TRUE);
  gtk_range_set_value((GtkRange *) pattern_envelope->ratio,
		      0.0);
  gtk_table_attach(table,
		   GTK_WIDGET(pattern_envelope->ratio),
		   1, 2,
		   4, 5,
		   GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND,
		   0, 0);

  /* rename dialog */
  pattern_envelope->rename = NULL;
  
  /* hbox - actions */
  hbox = (GtkBox *) gtk_hbox_new(FALSE,
				 0);
  gtk_box_pack_start((GtkBox *) pattern_envelope,
		     GTK_WIDGET(hbox),
		     FALSE, FALSE,
		     0);

  /* move up */
  pattern_envelope->move_up = (GtkButton *) gtk_button_new_with_label(i18n("move up"));
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(pattern_envelope->move_up),
		     FALSE, FALSE,
		     0);

  /* move down */
  pattern_envelope->move_down = (GtkButton *) gtk_button_new_with_label(i18n("move down"));
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(pattern_envelope->move_down),
		     FALSE, FALSE,
		     0);

  /* add */
  pattern_envelope->add = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_ADD);
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(pattern_envelope->add),
		     FALSE, FALSE,
		     0);

  /* remove */
  pattern_envelope->remove = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_REMOVE);
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(pattern_envelope->remove),
		     FALSE, FALSE,
		     0);
}

void
ags_pattern_envelope_connect(AgsConnectable *connectable)
{
  AgsPatternEnvelope *pattern_envelope;

  pattern_envelope = AGS_PATTERN_ENVELOPE(connectable);

  if((AGS_PATTERN_ENVELOPE_CONNECTED & (pattern_envelope->flags)) != 0){
    return;
  }

  pattern_envelope->flags |= AGS_PATTERN_ENVELOPE_CONNECTED;

  /* audio channel start/end */
  g_signal_connect((GObject *) pattern_envelope->audio_channel_start, "value-changed",
		   G_CALLBACK(ags_pattern_envelope_audio_channel_start_callback), (gpointer) pattern_envelope);

  g_signal_connect((GObject *) pattern_envelope->audio_channel_end, "value-changed",
		   G_CALLBACK(ags_pattern_envelope_audio_channel_end_callback), (gpointer) pattern_envelope);

  /* pad start/end */
  g_signal_connect((GObject *) pattern_envelope->pad_start, "value-changed",
		   G_CALLBACK(ags_pattern_envelope_pad_start_callback), (gpointer) pattern_envelope);

  g_signal_connect((GObject *) pattern_envelope->pad_end, "value-changed",
		   G_CALLBACK(ags_pattern_envelope_pad_end_callback), (gpointer) pattern_envelope);

  /* x start/end */
  g_signal_connect((GObject *) pattern_envelope->x_start, "value-changed",
		   G_CALLBACK(ags_pattern_envelope_x_start_callback), (gpointer) pattern_envelope);

  g_signal_connect((GObject *) pattern_envelope->x_end, "value-changed",
		   G_CALLBACK(ags_pattern_envelope_x_end_callback), (gpointer) pattern_envelope);

  /* attack x,y */
  g_signal_connect((GObject *) pattern_envelope->attack_x, "value-changed",
		   G_CALLBACK(ags_pattern_envelope_attack_x_callback), (gpointer) pattern_envelope);

  g_signal_connect((GObject *) pattern_envelope->attack_y, "value-changed",
		   G_CALLBACK(ags_pattern_envelope_attack_y_callback), (gpointer) pattern_envelope);

  /* decay x,y */
  g_signal_connect((GObject *) pattern_envelope->decay_x, "value-changed",
		   G_CALLBACK(ags_pattern_envelope_decay_x_callback), (gpointer) pattern_envelope);

  g_signal_connect((GObject *) pattern_envelope->decay_y, "value-changed",
		   G_CALLBACK(ags_pattern_envelope_decay_y_callback), (gpointer) pattern_envelope);

  /* sustain x,y */
  g_signal_connect((GObject *) pattern_envelope->sustain_x, "value-changed",
		   G_CALLBACK(ags_pattern_envelope_sustain_x_callback), (gpointer) pattern_envelope);

  g_signal_connect((GObject *) pattern_envelope->sustain_y, "value-changed",
		   G_CALLBACK(ags_pattern_envelope_sustain_y_callback), (gpointer) pattern_envelope);

  /* release x,y */
  g_signal_connect((GObject *) pattern_envelope->release_x, "value-changed",
		   G_CALLBACK(ags_pattern_envelope_release_x_callback), (gpointer) pattern_envelope);

  g_signal_connect((GObject *) pattern_envelope->release_y, "value-changed",
		   G_CALLBACK(ags_pattern_envelope_release_y_callback), (gpointer) pattern_envelope);

  /* ratio y */
  g_signal_connect((GObject *) pattern_envelope->ratio, "value-changed",
		   G_CALLBACK(ags_pattern_envelope_ratio_callback), (gpointer) pattern_envelope);
  
  /* move up/down preset */
  g_signal_connect((GObject *) pattern_envelope->move_up, "clicked",
		   G_CALLBACK(ags_pattern_envelope_preset_move_up_callback), (gpointer) pattern_envelope);
  g_signal_connect((GObject *) pattern_envelope->move_down, "clicked",
		   G_CALLBACK(ags_pattern_envelope_preset_move_down_callback), (gpointer) pattern_envelope);

  /* add/remove preset */
  g_signal_connect((GObject *) pattern_envelope->add, "clicked",
		   G_CALLBACK(ags_pattern_envelope_preset_add_callback), (gpointer) pattern_envelope);
  g_signal_connect((GObject *) pattern_envelope->remove, "clicked",
		   G_CALLBACK(ags_pattern_envelope_preset_remove_callback), (gpointer) pattern_envelope);

}

void
ags_pattern_envelope_disconnect(AgsConnectable *connectable)
{
  AgsPatternEnvelope *pattern_envelope;

  pattern_envelope = AGS_PATTERN_ENVELOPE(connectable);

  if((AGS_PATTERN_ENVELOPE_CONNECTED & (pattern_envelope->flags)) == 0){
    return;
  }

  pattern_envelope->flags &= (~AGS_PATTERN_ENVELOPE_CONNECTED);

  /* audio channel start/end */
  g_object_disconnect((GObject *) pattern_envelope->audio_channel_start,
		      "value-changed",
		      G_CALLBACK(ags_pattern_envelope_audio_channel_start_callback),
		      (gpointer) pattern_envelope,
		      NULL);

  g_object_disconnect((GObject *) pattern_envelope->audio_channel_end,
		      "value-changed",
		      G_CALLBACK(ags_pattern_envelope_audio_channel_end_callback),
		      (gpointer) pattern_envelope,
		      NULL);

  /* pad start/end */
  g_object_disconnect((GObject *) pattern_envelope->pad_start,
		      "value-changed",
		      G_CALLBACK(ags_pattern_envelope_pad_start_callback),
		      (gpointer) pattern_envelope,
		      NULL);

  g_object_disconnect((GObject *) pattern_envelope->pad_end,
		      "value-changed",
		      G_CALLBACK(ags_pattern_envelope_pad_end_callback),
		      (gpointer) pattern_envelope,
		      NULL);

  /* x start/end */
  g_object_disconnect((GObject *) pattern_envelope->x_start,
		      "value-changed",
		      G_CALLBACK(ags_pattern_envelope_x_start_callback),
		      (gpointer) pattern_envelope,
		      NULL);

  g_object_disconnect((GObject *) pattern_envelope->x_end,
		      "value-changed",
		      G_CALLBACK(ags_pattern_envelope_x_end_callback),
		      (gpointer) pattern_envelope,
		      NULL);

  /* attack x,y */
  g_object_disconnect((GObject *) pattern_envelope->attack_x,
		      "value-changed",
		      G_CALLBACK(ags_pattern_envelope_attack_x_callback),
		      (gpointer) pattern_envelope,
		      NULL);

  g_object_disconnect((GObject *) pattern_envelope->attack_y,
		      "value-changed",
		      G_CALLBACK(ags_pattern_envelope_attack_y_callback),
		      (gpointer) pattern_envelope,
		      NULL);

  /* decay x,y */
  g_object_disconnect((GObject *) pattern_envelope->decay_x,
		      "value-changed",
		      G_CALLBACK(ags_pattern_envelope_decay_x_callback),
		      (gpointer) pattern_envelope,
		      NULL);

  g_object_disconnect((GObject *) pattern_envelope->decay_y,
		      "value-changed",
		      G_CALLBACK(ags_pattern_envelope_decay_y_callback),
		      (gpointer) pattern_envelope,
		      NULL);

  /* sustain x,y */
  g_object_disconnect((GObject *) pattern_envelope->sustain_x,
		      "value-changed",
		      G_CALLBACK(ags_pattern_envelope_sustain_x_callback),
		      (gpointer) pattern_envelope,
		      NULL);

  g_object_disconnect((GObject *) pattern_envelope->sustain_y,
		      "value-changed",
		      G_CALLBACK(ags_pattern_envelope_sustain_y_callback),
		      (gpointer) pattern_envelope,
		      NULL);

  /* release x,y */
  g_object_disconnect((GObject *) pattern_envelope->release_x,
		      "value-changed",
		      G_CALLBACK(ags_pattern_envelope_release_x_callback),
		      (gpointer) pattern_envelope,
		      NULL);

  g_object_disconnect((GObject *) pattern_envelope->release_y,
		      "value-changed",
		      G_CALLBACK(ags_pattern_envelope_release_y_callback),
		      (gpointer) pattern_envelope,
		      NULL);

  /* ratio */
  g_object_disconnect((GObject *) pattern_envelope->ratio,
		      "value-changed",
		      G_CALLBACK(ags_pattern_envelope_ratio_callback),
		      (gpointer) pattern_envelope,
		      NULL);

  /* move up/down preset */
  g_object_disconnect((GObject *) pattern_envelope->move_up,
		      "clicked",
		      G_CALLBACK(ags_pattern_envelope_preset_move_up_callback),
		      (gpointer) pattern_envelope,
		      NULL);
  g_object_disconnect((GObject *) pattern_envelope->move_down,
		      "clicked",
		      G_CALLBACK(ags_pattern_envelope_preset_move_down_callback),
		      (gpointer) pattern_envelope,
		      NULL);

  /* add/remove preset */
  g_object_disconnect((GObject *) pattern_envelope->add,
		      "clicked",
		      G_CALLBACK(ags_pattern_envelope_preset_add_callback),
		      (gpointer) pattern_envelope,
		      NULL);
  g_object_disconnect((GObject *) pattern_envelope->remove,
		      "clicked",
		      G_CALLBACK(ags_pattern_envelope_preset_remove_callback),
		      (gpointer) pattern_envelope,
		      NULL);
}

void
ags_pattern_envelope_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_pattern_envelope_parent_class)->finalize(gobject);
}

void
ags_pattern_envelope_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_pattern_envelope_apply(AgsApplicable *applicable)
{
  /* empty */
}

void
ags_pattern_envelope_reset(AgsApplicable *applicable)
{
  AgsPatternEnvelope *pattern_envelope;

  pattern_envelope = AGS_PATTERN_ENVELOPE(applicable);
  
  ags_pattern_envelope_load_preset(pattern_envelope);
}

gchar*
ags_pattern_envelope_x_label_func(gdouble value,
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
ags_pattern_envelope_y_label_func(gdouble value,
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
 * ags_pattern_envelope_get_active_preset:
 * @pattern_envelope: the #AgsPatternEnvelope
 * 
 * Get active preset.
 * 
 * Returns: the matching #AgsPreset, if none selected %NULL
 * 
 * Since: 0.8.5
 */
AgsPreset*
ags_pattern_envelope_get_active_preset(AgsPatternEnvelope *pattern_envelope)
{
  AgsEnvelopeDialog *envelope_dialog;
  AgsMachine *machine;

  GtkTreeModel *model;
  GtkTreeIter iter;

  AgsAudio *audio;

  GList *preset;

  gchar *preset_name;
  
  gboolean do_edit;
  
  envelope_dialog = (AgsEnvelopeDialog *) gtk_widget_get_ancestor(pattern_envelope,
								  AGS_TYPE_ENVELOPE_DIALOG);

  machine = envelope_dialog->machine;
  audio = machine->audio;

  /* get model */
  model = gtk_tree_view_get_model(pattern_envelope->tree_view);

  /* get active */
  do_edit = FALSE;

  if(gtk_tree_model_get_iter_first(model, &iter)){
    do{
      gtk_tree_model_get(model, &iter,
			 AGS_PATTERN_ENVELOPE_COLUMN_EDIT, &do_edit,
			 -1);
      
      if(do_edit){
	break;
      }
    }while(gtk_tree_model_iter_next(model, &iter));
  }

  if(!do_edit){
    return(NULL);
  }
  
  /* preset name */
  gtk_tree_model_get(model, &iter,
		     AGS_PATTERN_ENVELOPE_COLUMN_PRESET_NAME, &preset_name,
		     -1);

  /* find preset */
  preset = ags_preset_find_name(audio->preset,
				preset_name);

  if(preset != NULL){
    return(preset->data);
  }

  return(NULL);
}

/**
 * ags_pattern_envelope_load_preset:
 * @pattern_envelope: the #AgsPatternEnvelope
 *
 * Load preset.
 * 
 * Since: 0.8.5
 */
void
ags_pattern_envelope_load_preset(AgsPatternEnvelope *pattern_envelope)
{
  AgsEnvelopeDialog *envelope_dialog;
  AgsMachine *machine;

  GtkListStore *model;
  GtkTreeIter iter;

  AgsAudio *audio;
  
  GList *preset;
  
  if(!AGS_IS_PATTERN_ENVELOPE(pattern_envelope)){
    return;
  }

  envelope_dialog = (AgsEnvelopeDialog *) gtk_widget_get_ancestor(pattern_envelope,
								  AGS_TYPE_ENVELOPE_DIALOG);

  machine = envelope_dialog->machine;
  audio = machine->audio;
  
  /* get model */
  model = GTK_LIST_STORE(gtk_tree_view_get_model(pattern_envelope->tree_view));

  /* clear old */
  gtk_list_store_clear(model);

  /* create new */
  preset = audio->preset;

  while(preset != NULL){
    gtk_list_store_append(model,
			  &iter);
    gtk_list_store_set(model, &iter,
		       AGS_PATTERN_ENVELOPE_COLUMN_PRESET_NAME, AGS_PRESET(preset->data)->preset_name,
		       AGS_PATTERN_ENVELOPE_COLUMN_AUDIO_CHANNEL_START, AGS_PRESET(preset->data)->audio_channel_start,
		       AGS_PATTERN_ENVELOPE_COLUMN_AUDIO_CHANNEL_END, AGS_PRESET(preset->data)->audio_channel_end,
		       AGS_PATTERN_ENVELOPE_COLUMN_PAD_START, AGS_PRESET(preset->data)->pad_start,
		       AGS_PATTERN_ENVELOPE_COLUMN_PAD_END, AGS_PRESET(preset->data)->pad_end,
		       AGS_PATTERN_ENVELOPE_COLUMN_X_START, AGS_PRESET(preset->data)->x_start,
		       AGS_PATTERN_ENVELOPE_COLUMN_X_END, AGS_PRESET(preset->data)->x_end,
		       -1);

    preset = preset->next;
  }
}

/**
 * ags_pattern_envelope_add_preset:
 * @pattern_envelope: the #AgsPatternEnvelope
 * @preset_name: the preset name
 *
 * Add preset.
 * 
 * Since: 0.8.5
 */
void
ags_pattern_envelope_add_preset(AgsPatternEnvelope *pattern_envelope,
				gchar *preset_name)
{
  AgsEnvelopeDialog *envelope_dialog;
  AgsMachine *machine;

  AgsAudio *audio;
  AgsPreset *preset;

  AgsComplex *val;
  
  GValue value = {0,};
  
  if(!AGS_IS_PATTERN_ENVELOPE(pattern_envelope) ||
     preset_name == NULL){
    return;
  }
  
  envelope_dialog = (AgsEnvelopeDialog *) gtk_widget_get_ancestor(pattern_envelope,
								  AGS_TYPE_ENVELOPE_DIALOG);

  machine = envelope_dialog->machine;
  audio = machine->audio;

  if(ags_preset_find_name(audio->preset,
			  preset_name) != NULL){
    return;
  }
  
  /* create preset */
  preset = g_object_new(AGS_TYPE_PRESET,
			"preset-name", preset_name,
			NULL);
  ags_audio_add_preset(audio,
		       preset);

  /* preset - ratio */
  val = ags_complex_alloc();
  ags_complex_set(val,
		  0.0);
  
  g_value_init(&value,
	       AGS_TYPE_COMPLEX);
  g_value_set_boxed(&value,
		    val);

  ags_preset_add_parameter(preset,
			   "ratio", &value);

  /* preset - attack */
  val = ags_complex_alloc();
  ags_complex_set(val,
		  0.25 + 1.0 * I);
  
  g_value_reset(&value);
  g_value_set_boxed(&value,
		    val);

  ags_preset_add_parameter(preset,
			   "attack", &value);

  /* preset - decay */
  val = ags_complex_alloc();
  ags_complex_set(val,
		  0.25 + 1.0 * I);
  
  g_value_reset(&value);
  g_value_set_boxed(&value,
		    val);

  ags_preset_add_parameter(preset,
			   "decay", &value);

  /* preset - sustain */
  val = ags_complex_alloc();
  ags_complex_set(val,
		  0.25 + 1.0 * I);
  
  g_value_reset(&value);
  g_value_set_boxed(&value,
		    val);

  ags_preset_add_parameter(preset,
			   "sustain", &value);

  /* preset - release */
  val = ags_complex_alloc();
  ags_complex_set(val,
		  0.25 + 1.0 * I);
  
  g_value_reset(&value);
  g_value_set_boxed(&value,
		    val);

  ags_preset_add_parameter(preset,
			   "release", &value);
}

/**
 * ags_pattern_envelope_remove_preset:
 * @pattern_envelope: the #AgsPatternEnvelope
 * @nth: the nth preset to remove
 *
 * Remove preset.
 * 
 * Since: 0.8.5
 */
void
ags_pattern_envelope_remove_preset(AgsPatternEnvelope *pattern_envelope,
				   guint nth)
{
  AgsEnvelopeDialog *envelope_dialog;
  AgsMachine *machine;

  AgsAudio *audio;
  AgsPreset *preset;
  
  if(!AGS_IS_PATTERN_ENVELOPE(pattern_envelope)){
    return;
  }
  
  envelope_dialog = (AgsEnvelopeDialog *) gtk_widget_get_ancestor(pattern_envelope,
								  AGS_TYPE_ENVELOPE_DIALOG);

  machine = envelope_dialog->machine;
  audio = machine->audio;

  /* create preset */
  preset = g_list_nth_data(audio->preset,
			   nth);
  ags_audio_remove_preset(audio,
			  preset);
}

/**
 * ags_pattern_envelope_reset_control:
 * @pattern_envelope: the #AgsPatternEnvelope
 * 
 * Reset controls.
 * 
 * Since: 0.8.5
 */
void
ags_pattern_envelope_reset_control(AgsPatternEnvelope *pattern_envelope)
{
  AgsPreset *preset;

  AgsComplex *val;
  
  guint audio_channel_start, audio_channel_end;
  guint pad_start, pad_end;
  guint x_start, x_end;
  
  GValue value = {0,};

  GError *error;
  
  if(!AGS_IS_PATTERN_ENVELOPE(pattern_envelope)){
    return;
  }

  pattern_envelope->flags |= AGS_PATTERN_ENVELOPE_NO_UPDATE;
  
  /* get preset */
  preset = ags_pattern_envelope_get_active_preset(pattern_envelope);
  
  if(preset == NULL){
    pattern_envelope->flags &= (~AGS_PATTERN_ENVELOPE_NO_UPDATE);
    
    return;
  }

  g_object_get(preset,
	       "audio-channel-start", &audio_channel_start,
	       "audio-channel-end", &audio_channel_end,
	       "pad-start", &pad_start,
	       "pad-end", &pad_end,
	       "x-start", &x_start,
	       "x-end", &x_end,
	       NULL);
  
  gtk_spin_button_set_value(pattern_envelope->audio_channel_start,
			    audio_channel_start);
  gtk_spin_button_set_value(pattern_envelope->audio_channel_end,
			    audio_channel_end);

  gtk_spin_button_set_value(pattern_envelope->pad_start,
			    pad_start);
  gtk_spin_button_set_value(pattern_envelope->pad_end,
			    pad_end);

  gtk_spin_button_set_value(pattern_envelope->x_start,
			    x_start);
  gtk_spin_button_set_value(pattern_envelope->x_end,
			    x_end);

  /* attack */
  g_value_init(&value,
	       AGS_TYPE_COMPLEX);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "attack", &value,
			   &error);

  if(error != NULL){
    g_warning("%s", error->message);

    pattern_envelope->flags &= (~AGS_PATTERN_ENVELOPE_NO_UPDATE);
    
    return;
  }

  val = (AgsComplex *) g_value_get_boxed(&value);  
  
  gtk_range_set_value(pattern_envelope->attack_x,
		      val[0][0]);
  gtk_range_set_value(pattern_envelope->attack_y,
		      val[0][1]);

  /* decay */
  g_value_reset(&value);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "decay", &value,
			   &error);

  if(error != NULL){
    g_warning("%s", error->message);

    pattern_envelope->flags &= (~AGS_PATTERN_ENVELOPE_NO_UPDATE);

    return;
  }

  val = (AgsComplex *) g_value_get_boxed(&value);  
  
  gtk_range_set_value(pattern_envelope->decay_x,
		      val[0][0]);
  gtk_range_set_value(pattern_envelope->decay_y,
		      val[0][1]);

  /* sustain */
  g_value_reset(&value);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "sustain", &value,
			   &error);

  if(error != NULL){
    g_warning("%s", error->message);

    pattern_envelope->flags &= (~AGS_PATTERN_ENVELOPE_NO_UPDATE);

    return;
  }

  val = (AgsComplex *) g_value_get_boxed(&value);  
  
  gtk_range_set_value(pattern_envelope->sustain_x,
		      val[0][0]);
  gtk_range_set_value(pattern_envelope->sustain_y,
		      val[0][1]);

  /* release */
  g_value_reset(&value);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "release", &value,
			   &error);

  if(error != NULL){
    g_warning("%s", error->message);

    pattern_envelope->flags &= (~AGS_PATTERN_ENVELOPE_NO_UPDATE);

    return;
  }

  val = (AgsComplex *) g_value_get_boxed(&value);  
  
  gtk_range_set_value(pattern_envelope->release_x,
		      val[0][0]);
  gtk_range_set_value(pattern_envelope->release_y,
		      val[0][1]);

  /* ratio */
  g_value_reset(&value);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "ratio", &value,
			   &error);

  if(error != NULL){
    g_warning("%s", error->message);

    pattern_envelope->flags &= (~AGS_PATTERN_ENVELOPE_NO_UPDATE);

    return;
  }

  val = (AgsComplex *) g_value_get_boxed(&value);  
  
  gtk_range_set_value(pattern_envelope->ratio,
		      val[0][1]);

  /* unset no update */
  pattern_envelope->flags &= (~AGS_PATTERN_ENVELOPE_NO_UPDATE);
}

/**
 * ags_pattern_envelope_reset_tree_view:
 * @pattern_envelope: the #AgsPatternEnvelope
 * 
 * Reset tree view.
 * 
 * Since: 0.8.5
 */
void
ags_pattern_envelope_reset_tree_view(AgsPatternEnvelope *pattern_envelope)
{
  GtkTreeModel *model;
  GtkTreeIter iter;

  AgsPreset *preset;

  gboolean do_edit;

  if(!AGS_IS_PATTERN_ENVELOPE(pattern_envelope)){
    return;
  }

  pattern_envelope->flags |= AGS_PATTERN_ENVELOPE_NO_UPDATE;
  
  /* get preset */
  preset = ags_pattern_envelope_get_active_preset(pattern_envelope);
  
  if(preset == NULL){
    pattern_envelope->flags &= (~AGS_PATTERN_ENVELOPE_NO_UPDATE);
    
    return;
  }

  /* get model */
  model = gtk_tree_view_get_model(pattern_envelope->tree_view);

  do_edit = FALSE;
  
  if(gtk_tree_model_get_iter_first(model, &iter)){
    do{
      gtk_tree_model_get(GTK_TREE_MODEL(model), &iter,
			 AGS_PATTERN_ENVELOPE_COLUMN_EDIT, &do_edit,
			 -1);

      if(do_edit){
	break;
      }
    }while(gtk_tree_model_iter_next(model, &iter));
  }

  /* update row */
  if(do_edit){
    guint audio_channel_start, audio_channel_end;
    guint pad_start, pad_end;
    guint x_start, x_end;
    
    g_object_get(preset,
		 "audio-channel-start", &audio_channel_start,
		 "audio-channel-end", &audio_channel_end,
		 "pad-start", &pad_start,
		 "pad-end", &pad_end,
		 "x-start", &x_start,
		 "x-end", &x_end,
		 NULL);
    
    gtk_list_store_set(GTK_LIST_STORE(model), &iter,
		       AGS_PATTERN_ENVELOPE_COLUMN_AUDIO_CHANNEL_START, audio_channel_start,
		       AGS_PATTERN_ENVELOPE_COLUMN_AUDIO_CHANNEL_END, audio_channel_end,
		       AGS_PATTERN_ENVELOPE_COLUMN_PAD_START, pad_start,
		       AGS_PATTERN_ENVELOPE_COLUMN_PAD_END, pad_end,
		       AGS_PATTERN_ENVELOPE_COLUMN_X_START, x_start,
		       AGS_PATTERN_ENVELOPE_COLUMN_X_END, x_end,
		       -1);
  }
  
  /* unset no update */
  pattern_envelope->flags &= (~AGS_PATTERN_ENVELOPE_NO_UPDATE);
}

/**
 * ags_pattern_envelope_plot:
 * @pattern_envelope: the #AgsPatternEnvelope
 * 
 * Plot envelope.
 * 
 * Since: 0.8.5
 */
void
ags_pattern_envelope_plot(AgsPatternEnvelope *pattern_envelope)
{
  AgsEnvelopeDialog *envelope_dialog;
  AgsMachine *machine;

  AgsCartesian *cartesian;
  AgsPlot *plot;

  GtkTreeModel *model;
  GtkTreeIter iter;

  AgsAudio *audio;

  GList *preset;

  AgsComplex *val;
  
  gchar *preset_name;
  
  gdouble default_width, default_height;
  gdouble offset;
  gboolean do_plot;

  GValue value = {0,};

  GError *error;
  
  if(!AGS_IS_PATTERN_ENVELOPE(pattern_envelope)){
    return;
  }

  envelope_dialog = (AgsEnvelopeDialog *) gtk_widget_get_ancestor(pattern_envelope,
								  AGS_TYPE_ENVELOPE_DIALOG);

  machine = envelope_dialog->machine;
  audio = machine->audio;

  /* get dimension of cartesian */
  cartesian = pattern_envelope->cartesian;

  default_width = cartesian->x_step_width * cartesian->x_scale_step_width;
  default_height = cartesian->y_step_height * cartesian->y_scale_step_height;

  /* get model */
  model = GTK_TREE_MODEL(gtk_tree_view_get_model(pattern_envelope->tree_view));

  /* clear old plot */
  if(cartesian->plot != NULL){
    g_list_free_full(cartesian->plot,
		     ags_plot_free);
    
    cartesian->plot = NULL;
  }

  /* plot */
  if(gtk_tree_model_get_iter_first(model,
				   &iter)){
    g_value_init(&value,
		 AGS_TYPE_COMPLEX);
    
    do{
      gtk_tree_model_get(model,
			 &iter,
			 AGS_PATTERN_ENVELOPE_COLUMN_PLOT, &do_plot,
			 AGS_PATTERN_ENVELOPE_COLUMN_PRESET_NAME, &preset_name,
			 -1);

      if(do_plot){
	preset = ags_preset_find_name(audio->preset,
				      preset_name);

	/* AgsPlot struct */
	plot = ags_plot_alloc(5, 0, 0);
	plot->join_points = TRUE;

	plot->point_color[0][0] = 1.0;
	plot->point_color[1][0] = 1.0;
	plot->point_color[2][0] = 1.0;
	plot->point_color[3][0] = 1.0;
	plot->point_color[4][0] = 1.0;

	/* add plot */
	ags_cartesian_add_plot(cartesian,
			       plot);

	/* set plot points - ratio */
	g_value_reset(&value);

	error = NULL;
	ags_preset_get_parameter(preset->data,
				 "ratio", &value,
				 &error);

	if(error != NULL){
	  g_warning("%s", error->message);
	  
	  continue;
	}
	
	val = (AgsComplex *) g_value_get_boxed(&value);
	
	plot->point[0][0] = 0.0;
	plot->point[0][1] = default_height * val[0][1];

	/* set plot points - attack */
	g_value_reset(&value);

	error = NULL;
	ags_preset_get_parameter(preset->data,
				 "attack", &value,
				 &error);

	if(error != NULL){
	  g_warning("%s", error->message);
	  
	  continue;
	}
	
	val = (AgsComplex *) g_value_get_boxed(&value);

	g_message("- %f", val[0][0]);
	
	plot->point[1][0] = default_width * val[0][0];
	plot->point[1][1] = default_height * val[0][1];

	offset = default_width * val[0][0];
	
	/* set plot points - decay */
	g_value_reset(&value);

	error = NULL;
	ags_preset_get_parameter(preset->data,
				 "decay", &value,
				 &error);

	if(error != NULL){
	  g_warning("%s", error->message);
	  
	  continue;
	}
	
	val = (AgsComplex *) g_value_get_boxed(&value);

	plot->point[2][0] = offset + default_width * val[0][0];
	plot->point[2][1] = default_height * val[0][1];

	offset += default_width * val[0][0];

	/* set plot points - sustain */
	g_value_reset(&value);

	error = NULL;
	ags_preset_get_parameter(preset->data,
				 "sustain", &value,
				 &error);

	if(error != NULL){
	  g_warning("%s", error->message);
	  
	  continue;
	}
	
	val = (AgsComplex *) g_value_get_boxed(&value);

	plot->point[3][0] = offset + default_width * val[0][0];
	plot->point[3][1] = default_height * val[0][1];

	offset += default_width * val[0][0];

	/* set plot points - release */
	g_value_reset(&value);

	error = NULL;
	ags_preset_get_parameter(preset->data,
				 "release", &value,
				 &error);

	if(error != NULL){
	  g_warning("%s", error->message);
	  
	  continue;
	}
	
	val = (AgsComplex *) g_value_get_boxed(&value);

	plot->point[4][0] = offset + default_width * val[0][0];
	plot->point[4][1] = default_height * val[0][1];
      }
    }while(gtk_tree_model_iter_next(model,
				    &iter));
  }

  /* queue draw */
  gtk_widget_queue_draw(cartesian);
}

/**
 * ags_pattern_envelope_new:
 *
 * Creates an #AgsPatternEnvelope
 *
 * Returns: a new #AgsPatternEnvelope
 *
 * Since: 0.8.5
 */
AgsPatternEnvelope*
ags_pattern_envelope_new()
{
  AgsPatternEnvelope *pattern_envelope;

  pattern_envelope = (AgsPatternEnvelope *) g_object_new(AGS_TYPE_PATTERN_ENVELOPE,
							 NULL);

  return(pattern_envelope);
}

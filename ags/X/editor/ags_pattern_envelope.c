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

#include <ags/X/editor/ags_envelope_dialog.h>

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
  AgsCartesian *cartesian;
  GtkLabel *label;
  
  GtkCellRenderer *toggle_renderer;
  GtkCellRenderer *renderer;

  GtkListStore  *model;

  gdouble width, height;

  pattern_envelope->flags = 0;

  pattern_envelope->version = AGS_PATTERN_ENVELOPE_DEFAULT_VERSION;
  pattern_envelope->build_id = AGS_PATTERN_ENVELOPE_DEFAULT_BUILD_ID;

  /* enabled */
  pattern_envelope->enabled = gtk_check_button_new_with_label(i18n("enabled"));
  gtk_box_pack_start((GtkBox *) pattern_envelope,
		     pattern_envelope->enabled,
		     FALSE, FALSE,
		     0);

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
			     G_TYPE_UINT,
			     G_TYPE_UINT,
			     G_TYPE_UINT,
			     G_TYPE_UINT,
			     G_TYPE_UINT,
			     G_TYPE_UINT);
  gtk_tree_view_set_model(pattern_envelope->tree_view,
			  model);

  toggle_renderer = gtk_cell_renderer_toggle_new();
  renderer = gtk_cell_renderer_text_new();

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
				    "yalign", 0.0,
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
		   0, 1,
		   0, 1,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);

  /* audio channel - end */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("audio channel end"),
				    "xalign", 0.0,
				    "yalign", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  pattern_envelope->audio_channel_end = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
											 0.0,
											 1.0);
  gtk_table_attach(table,
		   GTK_WIDGET(pattern_envelope->audio_channel_start),
		   0, 1,
		   0, 1,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);

  /* x - start */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("x start"),
				    "xalign", 0.0,
				    "yalign", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  pattern_envelope->x_start = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
									       0.0,
									       1.0);
  gtk_table_attach(table,
		   GTK_WIDGET(pattern_envelope->x_start),
		   0, 1,
		   0, 1,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);

  /* x - end */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("x end"),
				    "xalign", 0.0,
				    "yalign", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  pattern_envelope->x_end = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
									     0.0,
									     1.0);
  gtk_table_attach(table,
		   GTK_WIDGET(pattern_envelope->x_start),
		   0, 1,
		   0, 1,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);

  /* y - start */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("y start"),
				    "xalign", 0.0,
				    "yalign", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  pattern_envelope->y_start = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
									       0.0,
									       1.0);
  gtk_table_attach(table,
		   GTK_WIDGET(pattern_envelope->y_start),
		   0, 1,
		   0, 1,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);

  /* y - end */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("y end"),
				    "xalign", 0.0,
				    "yalign", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  pattern_envelope->y_end = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
									     0.0,
									     1.0);
  gtk_table_attach(table,
		   GTK_WIDGET(pattern_envelope->y_start),
		   0, 1,
		   0, 1,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);
  
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

void
ags_pattern_envelope_plot(AgsPatternEnvelope *pattern_envelope)
{
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

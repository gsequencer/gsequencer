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

#include <ags/X/editor/ags_envelope_info.h>
#include <ags/X/editor/ags_envelope_info_callbacks.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>

#include <ags/thread/ags_mutex_manager.h>

#include <ags/X/ags_window.h>

#include <ags/X/editor/ags_envelope_dialog.h>

#include <complex.h>

#include <ags/i18n.h>

void ags_envelope_info_class_init(AgsEnvelopeInfoClass *envelope_info);
void ags_envelope_info_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_envelope_info_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_envelope_info_init(AgsEnvelopeInfo *envelope_info);
void ags_envelope_info_connect(AgsConnectable *connectable);
void ags_envelope_info_disconnect(AgsConnectable *connectable);
void ags_envelope_info_set_update(AgsApplicable *applicable, gboolean update);
void ags_envelope_info_apply(AgsApplicable *applicable);
void ags_envelope_info_reset(AgsApplicable *applicable);

gchar* ags_envelope_info_x_label_func(gdouble value,
				      gpointer data);
gchar* ags_envelope_info_y_label_func(gdouble value,
				      gpointer data);

/**
 * SECTION:ags_envelope_info
 * @short_description: pack pad editors.
 * @title: AgsEnvelopeInfo
 * @section_id:
 * @include: ags/X/ags_envelope_info.h
 *
 * #AgsEnvelopeInfo is a composite widget to show envelope controls
 * of selected AgsNote.
 */

static gpointer ags_envelope_info_parent_class = NULL;

GType
ags_envelope_info_get_type(void)
{
  static GType ags_type_envelope_info = 0;

  if(!ags_type_envelope_info){
    static const GTypeInfo ags_envelope_info_info = {
      sizeof (AgsEnvelopeInfoClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_envelope_info_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsEnvelopeInfo),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_envelope_info_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_envelope_info_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_envelope_info_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_envelope_info = g_type_register_static(GTK_TYPE_VBOX,
						    "AgsEnvelopeInfo", &ags_envelope_info_info,
						    0);

    g_type_add_interface_static(ags_type_envelope_info,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_envelope_info,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }
  
  return(ags_type_envelope_info);
}

void
ags_envelope_info_class_init(AgsEnvelopeInfoClass *envelope_info)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GParamSpec *param_spec;

  ags_envelope_info_parent_class = g_type_class_peek_parent(envelope_info);

  /* GObjectClass */
  gobject = (GObjectClass *) envelope_info;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) envelope_info;
}

void
ags_envelope_info_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_envelope_info_connect;
  connectable->disconnect = ags_envelope_info_disconnect;
}

void
ags_envelope_info_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_envelope_info_set_update;
  applicable->apply = ags_envelope_info_apply;
  applicable->reset = ags_envelope_info_reset;
}

void
ags_envelope_info_init(AgsEnvelopeInfo *envelope_info)
{
  AgsCartesian *cartesian;

  GtkCellRenderer *renderer;
  GtkCellRenderer *toggle_renderer;
  
  GtkListStore  *model;
  GtkTreeIter iter;
  
  gdouble width, height;

  envelope_info->flags = 0;

  envelope_info->version = AGS_ENVELOPE_INFO_DEFAULT_VERSION;
  envelope_info->build_id = AGS_ENVELOPE_INFO_DEFAULT_BUILD_ID;

  /* cartesian */
  cartesian = 
    envelope_info->cartesian = ags_cartesian_new();

  cartesian->x_label_func = ags_envelope_info_x_label_func;
  cartesian->y_label_func = ags_envelope_info_y_label_func;

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
  gtk_box_pack_start((GtkBox *) envelope_info,
		     (GtkWidget *) cartesian,
		     FALSE, FALSE,
		     0);

  gtk_widget_queue_draw(cartesian);

  /* tree view */
  envelope_info->tree_view = (GtkTreeView *) gtk_tree_view_new();

  model = gtk_list_store_new(AGS_ENVELOPE_INFO_COLUMN_LAST,
			     G_TYPE_BOOLEAN,
			     G_TYPE_UINT,
			     G_TYPE_UINT,
			     G_TYPE_UINT);
  gtk_tree_view_set_model(envelope_info->tree_view,
			  model);

  renderer = gtk_cell_renderer_text_new();
  toggle_renderer = gtk_cell_renderer_toggle_new();
  
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(envelope_info->tree_view),
					      -1,
					      i18n("plot"),
					      toggle_renderer,
					      "active", AGS_ENVELOPE_INFO_COLUMN_PLOT,
					      NULL);

  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(envelope_info->tree_view),
					      -1,
					      i18n("audio channel"),
					      renderer,
					      "text", AGS_ENVELOPE_INFO_COLUMN_AUDIO_CHANNEL,
					      NULL);

  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(envelope_info->tree_view),
					      -1,
					      i18n("Note:x0"),
					      renderer,
					      "text", AGS_ENVELOPE_INFO_COLUMN_NOTE_X0,
					      NULL);

  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(envelope_info->tree_view),
					      -1,
					      i18n("Note:x1"),
					      renderer,
					      "text", AGS_ENVELOPE_INFO_COLUMN_NOTE_X1,
					      NULL);

  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(envelope_info->tree_view),
					      -1,
					      i18n("Note:y"),
					      renderer,
					      "text", AGS_ENVELOPE_INFO_COLUMN_NOTE_Y,
					      NULL);
  
  gtk_box_pack_start((GtkBox *) envelope_info,
		     (GtkWidget *) envelope_info->tree_view,
		     FALSE, FALSE,
		     0);
  
  g_signal_connect(G_OBJECT(toggle_renderer), "toggled\0",
		   G_CALLBACK(ags_envelope_info_plot_callback), envelope_info);
}

void
ags_envelope_info_connect(AgsConnectable *connectable)
{
  AgsEnvelopeInfo *envelope_info;

  envelope_info = AGS_ENVELOPE_INFO(connectable);

  if((AGS_ENVELOPE_INFO_CONNECTED & (envelope_info->flags)) != 0){
    return;
  }

  envelope_info->flags |= AGS_ENVELOPE_INFO_CONNECTED;
}

void
ags_envelope_info_disconnect(AgsConnectable *connectable)
{
  AgsEnvelopeInfo *envelope_info;

  envelope_info = AGS_ENVELOPE_INFO(connectable);

  if((AGS_ENVELOPE_INFO_CONNECTED & (envelope_info->flags)) == 0){
    return;
  }

  envelope_info->flags &= (~AGS_ENVELOPE_INFO_CONNECTED);
}

void
ags_envelope_info_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_envelope_info_apply(AgsApplicable *applicable)
{
  /* empty */
}

void
ags_envelope_info_reset(AgsApplicable *applicable)
{
  AgsEnvelopeDialog *envelope_dialog;
  AgsEnvelopeInfo *envelope_info;

  AgsMachine *machine;

  GtkListStore *model;
  GtkTreeIter iter;
  
  AgsAudio *audio;

  GList *notation;
  GList *selection;

  envelope_info = AGS_ENVELOPE_INFO(applicable);
  envelope_dialog = gtk_widget_get_ancestor(envelope_info,
					    AGS_TYPE_ENVELOPE_DIALOG);
  
  machine = envelope_dialog->machine;

  audio = machine->audio;

  model = GTK_LIST_STORE(gtk_tree_view_get_model(envelope_info->tree_view));

  /* fill tree view */
  notation = audio->notation;

  while(notation != NULL){
    selection = AGS_NOTATION(notation->data)->selection;

    while(selection != NULL){
      gtk_list_store_append(model,
			    &iter);
      gtk_list_store_set(model, &iter,
			 AGS_ENVELOPE_INFO_COLUMN_AUDIO_CHANNEL, AGS_NOTATION(notation->data)->audio_channel,
			 AGS_ENVELOPE_INFO_COLUMN_NOTE_X0, AGS_NOTE(selection->data)->x[0],
			 AGS_ENVELOPE_INFO_COLUMN_NOTE_X1, AGS_NOTE(selection->data)->x[1],
			 AGS_ENVELOPE_INFO_COLUMN_NOTE_Y, AGS_NOTE(selection->data)->y,
			 -1);
      
      selection = selection->next;
    }

    notation = notation->next;
  }
}

gchar*
ags_envelope_info_x_label_func(gdouble value,
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
ags_envelope_info_y_label_func(gdouble value,
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
ags_envelope_info_plot(AgsEnvelopeInfo *envelope_info)
{
  //TODO:JK: implement me
}

/**
 * ags_envelope_info_new:
 *
 * Creates an #AgsEnvelopeInfo
 *
 * Returns: a new #AgsEnvelopeInfo
 *
 * Since: 0.8.1
 */
AgsEnvelopeInfo*
ags_envelope_info_new()
{
  AgsEnvelopeInfo *envelope_info;

  envelope_info = (AgsEnvelopeInfo *) g_object_new(AGS_TYPE_ENVELOPE_INFO,
						   NULL);

  return(envelope_info);
}

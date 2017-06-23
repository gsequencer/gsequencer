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

#include <ags/X/editor/ags_envelope_editor.h>
#include <ags/X/editor/ags_envelope_editor_callbacks.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>

#include <ags/thread/ags_mutex_manager.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_preset.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>

#include <ags/X/editor/ags_envelope_dialog.h>

#include <complex.h>

#include <ags/i18n.h>

void ags_envelope_editor_class_init(AgsEnvelopeEditorClass *envelope_editor);
void ags_envelope_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_envelope_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_envelope_editor_init(AgsEnvelopeEditor *envelope_editor);
void ags_envelope_editor_connect(AgsConnectable *connectable);
void ags_envelope_editor_disconnect(AgsConnectable *connectable);
void ags_envelope_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_envelope_editor_apply(AgsApplicable *applicable);
void ags_envelope_editor_reset(AgsApplicable *applicable);
gboolean ags_envelope_editor_delete_event(GtkWidget *widget, GdkEventAny *event);

gchar* ags_envelope_editor_x_label_func(gdouble value,
					gpointer data);
gchar* ags_envelope_editor_y_label_func(gdouble value,
					gpointer data);

/**
 * SECTION:ags_envelope_editor
 * @short_description: pack pad editors.
 * @title: AgsEnvelopeEditor
 * @section_id:
 * @include: ags/X/ags_envelope_editor.h
 *
 * #AgsEnvelopeEditor is a composite widget to edit envelope controls
 * of selected AgsNote.
 */

static gpointer ags_envelope_editor_parent_class = NULL;

GType
ags_envelope_editor_get_type(void)
{
  static GType ags_type_envelope_editor = 0;

  if(!ags_type_envelope_editor){
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

    ags_type_envelope_editor = g_type_register_static(GTK_TYPE_VBOX,
						      "AgsEnvelopeEditor", &ags_envelope_editor_info,
						      0);

    g_type_add_interface_static(ags_type_envelope_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_envelope_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }
  
  return(ags_type_envelope_editor);
}

void
ags_envelope_editor_class_init(AgsEnvelopeEditorClass *envelope_editor)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GParamSpec *param_spec;

  ags_envelope_editor_parent_class = g_type_class_peek_parent(envelope_editor);

  /* GObjectClass */
  gobject = (GObjectClass *) envelope_editor;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) envelope_editor;
}

void
ags_envelope_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_envelope_editor_connect;
  connectable->disconnect = ags_envelope_editor_disconnect;
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
  GtkHBox *hbox;
  GtkVBox *control;
  GtkTable *table;
  GtkLabel *label;
  AgsCartesian *cartesian;
  
  AgsPlot *plot;

  gdouble width, height;
  gdouble default_width, default_height;
  gdouble offset;
  
  envelope_editor->flags = 0;

  envelope_editor->version = AGS_ENVELOPE_EDITOR_DEFAULT_VERSION;
  envelope_editor->build_id = AGS_ENVELOPE_EDITOR_DEFAULT_BUILD_ID;

  /* enabled */
  envelope_editor->enabled = gtk_check_button_new_with_label(i18n("enabled"));
  gtk_box_pack_start((GtkBox *) envelope_editor,
		     (GtkWidget *) envelope_editor->enabled,
		     FALSE, FALSE,
		     0);

  /* rename dialog */
  envelope_editor->rename = NULL;

  /* frame - preset */
  frame = (GtkFrame *) gtk_frame_new(i18n("preset"));
  gtk_box_pack_start((GtkBox *) envelope_editor,
		     (GtkWidget *) frame,
		     FALSE, FALSE,
		     0);

  hbox = (GtkHBox *) gtk_hbox_new(FALSE,
				  0);
  gtk_container_add((GtkContainer *) frame,
		    (GtkWidget *) hbox);
  
  envelope_editor->preset = gtk_combo_box_text_new();
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) envelope_editor->preset,
		     FALSE, FALSE,
		     0);

  envelope_editor->add = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_ADD);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) envelope_editor->add,
		     FALSE, FALSE,
		     0);

  envelope_editor->remove = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_REMOVE);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) envelope_editor->remove,
		     FALSE, FALSE,
		     0);

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

  plot->point_color[0][0] = 1.0;
  plot->point_color[1][0] = 1.0;
  plot->point_color[2][0] = 1.0;
  plot->point_color[3][0] = 1.0;
  plot->point_color[4][0] = 1.0;

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
  gtk_widget_set_size_request(cartesian,
			      (gint) width + 2.0 * cartesian->x_margin, (gint) height + 2.0 * cartesian->y_margin);
  gtk_box_pack_start((GtkBox *) envelope_editor,
		     GTK_WIDGET(cartesian),
		     FALSE, FALSE,
		     0);

  gtk_widget_queue_draw(cartesian);

  /* table */
  table = (GtkTable *) gtk_table_new(5, 2,
				     FALSE);
  gtk_box_pack_start((GtkBox *) envelope_editor,
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
  
  envelope_editor->attack_x = (GtkHScale *) gtk_hscale_new_with_range(0.0, 1.0, 0.001);
  gtk_scale_set_draw_value(envelope_editor->attack_x,
			   TRUE);
  gtk_range_set_value((GtkRange *) envelope_editor->attack_x,
		      0.25);
  gtk_box_pack_start((GtkBox *) control,
		     (GtkWidget *) envelope_editor->attack_x,
		     FALSE, FALSE,
		     0);

  envelope_editor->attack_y = (GtkHScale *) gtk_hscale_new_with_range(-1.0, 1.0, 0.001);
  gtk_scale_set_draw_value(envelope_editor->attack_y,
			   TRUE);
  gtk_range_set_value((GtkRange *) envelope_editor->attack_y,
		      1.0);
  gtk_box_pack_start((GtkBox *) control,
		     (GtkWidget *) envelope_editor->attack_y,
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

  envelope_editor->decay_x = (GtkHScale *) gtk_hscale_new_with_range(0.0, 1.0, 0.001);
  gtk_scale_set_draw_value(envelope_editor->decay_x,
			   TRUE);
  gtk_range_set_value((GtkRange *) envelope_editor->decay_x,
		      0.25);
  gtk_box_pack_start((GtkBox *) control,
		     (GtkWidget *) envelope_editor->decay_x,
		     FALSE, FALSE,
		     0);

  envelope_editor->decay_y = (GtkHScale *) gtk_hscale_new_with_range(-1.0, 1.0, 0.001);
  gtk_scale_set_draw_value(envelope_editor->decay_y,
			   TRUE);
  gtk_range_set_value((GtkRange *) envelope_editor->decay_y,
		      1.0);
  gtk_box_pack_start((GtkBox *) control,
		     (GtkWidget *) envelope_editor->decay_y,
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

  envelope_editor->sustain_x = (GtkHScale *) gtk_hscale_new_with_range(0.0, 1.0, 0.001);
  gtk_scale_set_draw_value(envelope_editor->sustain_x,
			   TRUE);
  gtk_range_set_value((GtkRange *) envelope_editor->sustain_x,
		      0.25);
  gtk_box_pack_start((GtkBox *) control,
		     (GtkWidget *) envelope_editor->sustain_x,
		     FALSE, FALSE,
		     0);

  envelope_editor->sustain_y = (GtkHScale *) gtk_hscale_new_with_range(-1.0, 1.0, 0.001);
  gtk_scale_set_draw_value(envelope_editor->sustain_y,
			   TRUE);
  gtk_range_set_value((GtkRange *) envelope_editor->sustain_y,
		      1.0);
  gtk_box_pack_start((GtkBox *) control,
		     (GtkWidget *) envelope_editor->sustain_y,
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

  envelope_editor->release_x = (GtkHScale *) gtk_hscale_new_with_range(0.0, 1.0, 0.001);
  gtk_scale_set_draw_value(envelope_editor->release_x,
			   TRUE);
  gtk_range_set_value((GtkRange *) envelope_editor->release_x,
		      0.25);
  gtk_box_pack_start((GtkBox *) control,
		     (GtkWidget *) envelope_editor->release_x,
		     FALSE, FALSE,
		     0);

  envelope_editor->release_y = (GtkHScale *) gtk_hscale_new_with_range(-1.0, 1.0, 0.001);
  gtk_scale_set_draw_value(envelope_editor->release_y,
			   TRUE);
  gtk_range_set_value((GtkRange *) envelope_editor->release_y,
		      1.0);
  gtk_box_pack_start((GtkBox *) control,
		     (GtkWidget *) envelope_editor->release_y,
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

  envelope_editor->ratio = (GtkHScale *) gtk_hscale_new_with_range(0.0, 1.0, 0.001);
  gtk_scale_set_draw_value(envelope_editor->ratio,
			   TRUE);
  gtk_range_set_value((GtkRange *) envelope_editor->ratio,
		      0.0);
  gtk_table_attach(table,
		   GTK_WIDGET(envelope_editor->ratio),
		   1, 2,
		   4, 5,
		   GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND,
		   0, 0);
}

void
ags_envelope_editor_connect(AgsConnectable *connectable)
{
  AgsEnvelopeEditor *envelope_editor;

  envelope_editor = AGS_ENVELOPE_EDITOR(connectable);

  if((AGS_ENVELOPE_EDITOR_CONNECTED & (envelope_editor->flags)) != 0){
    return;
  }

  envelope_editor->flags |= AGS_ENVELOPE_EDITOR_CONNECTED;

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

  if((AGS_ENVELOPE_EDITOR_CONNECTED & (envelope_editor->flags)) == 0){
    return;
  }

  envelope_editor->flags &= (~AGS_ENVELOPE_EDITOR_CONNECTED);

  /* preset */
  g_object_disconnect((GObject *) envelope_editor->preset,
		      "changed",
		      G_CALLBACK(ags_envelope_editor_preset_callback),
		      (gpointer) envelope_editor,
		      NULL);

  g_object_disconnect((GObject *) envelope_editor->add,
		      "clicked",
		      G_CALLBACK(ags_envelope_editor_preset_add_callback),
		      (gpointer) envelope_editor,
		      NULL);

  g_object_disconnect((GObject *) envelope_editor->remove,
		      "clicked",
		      G_CALLBACK(ags_envelope_editor_preset_remove_callback),
		      (gpointer) envelope_editor,
		      NULL);

  /* attack x,y */
  g_object_disconnect((GObject *) envelope_editor->attack_x,
		      "value-changed",
		      G_CALLBACK(ags_envelope_editor_attack_x_callback),
		      (gpointer) envelope_editor,
		      NULL);

  g_object_disconnect((GObject *) envelope_editor->attack_y,
		      "value-changed",
		      G_CALLBACK(ags_envelope_editor_attack_y_callback),
		      (gpointer) envelope_editor,
		      NULL);

  /* decay x,y */
  g_object_disconnect((GObject *) envelope_editor->decay_x,
		      "value-changed",
		      G_CALLBACK(ags_envelope_editor_decay_x_callback),
		      (gpointer) envelope_editor,
		      NULL);

  g_object_disconnect((GObject *) envelope_editor->decay_y,
		      "value-changed",
		      G_CALLBACK(ags_envelope_editor_decay_y_callback),
		      (gpointer) envelope_editor,
		      NULL);

  /* sustain x,y */
  g_object_disconnect((GObject *) envelope_editor->sustain_x,
		      "value-changed",
		      G_CALLBACK(ags_envelope_editor_sustain_x_callback),
		      (gpointer) envelope_editor,
		      NULL);

  g_object_disconnect((GObject *) envelope_editor->sustain_y,
		      "value-changed",
		      G_CALLBACK(ags_envelope_editor_sustain_y_callback),
		      (gpointer) envelope_editor,
		      NULL);

  /* release x,y */
  g_object_disconnect((GObject *) envelope_editor->release_x,
		      "value-changed",
		      G_CALLBACK(ags_envelope_editor_release_x_callback),
		      (gpointer) envelope_editor,
		      NULL);

  g_object_disconnect((GObject *) envelope_editor->release_y,
		      "value-changed",
		      G_CALLBACK(ags_envelope_editor_release_y_callback),
		      (gpointer) envelope_editor,
		      NULL);

  /* ratio */
  g_object_disconnect((GObject *) envelope_editor->ratio,
		      "value-changed",
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
  
  AgsWindow *window;
  AgsMachine *machine;

  AgsAudio *audio;

  AgsMutexManager *mutex_manager;
  
  AgsApplicationContext *application_context;

  GList *notation;
  GList *selection;

  double attack_x, attack_y;
  double decay_x, decay_y;
  double sustain_x, sustain_y;
  double release_x, release_y;
  double ratio;
  
  complex z;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  
  envelope_editor = AGS_ENVELOPE_EDITOR(applicable);
  envelope_dialog = gtk_widget_get_ancestor(envelope_editor,
					    AGS_TYPE_ENVELOPE_DIALOG);

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) envelope_dialog->machine,
						 AGS_TYPE_WINDOW);
  machine = envelope_dialog->machine;

  audio = machine->audio;

  /* application context and mutex manager */
  application_context = window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* get z */
  attack_x = gtk_range_get_value(GTK_RANGE(envelope_editor->attack_x));
  attack_y = gtk_range_get_value(GTK_RANGE(envelope_editor->attack_y));

  decay_x = gtk_range_get_value(GTK_RANGE(envelope_editor->decay_x));
  decay_y = gtk_range_get_value(GTK_RANGE(envelope_editor->decay_y));

  sustain_x = gtk_range_get_value(GTK_RANGE(envelope_editor->sustain_x));
  sustain_y = gtk_range_get_value(GTK_RANGE(envelope_editor->sustain_y));

  release_x = gtk_range_get_value(GTK_RANGE(envelope_editor->release_x));
  release_y = gtk_range_get_value(GTK_RANGE(envelope_editor->release_y));

  ratio = gtk_range_get_value(GTK_RANGE(envelope_editor->ratio));

  /* notation */
  pthread_mutex_lock(audio_mutex);

  notation = audio->notation;

  /* set attack, decay, sustain and release */
  while(notation != NULL){
    selection = AGS_NOTATION(notation->data)->selection;

    while(selection != NULL){
      AGS_NOTE(selection->data)->flags |= AGS_NOTE_ENVELOPE;
      
      z = attack_x + I * attack_y;
      ags_complex_set(&(AGS_NOTE(selection->data)->attack),
		      z);

      z = decay_x + I * decay_y;
      ags_complex_set(&(AGS_NOTE(selection->data)->decay),
		      z);

      z = sustain_x + I * sustain_y;
      ags_complex_set(&(AGS_NOTE(selection->data)->sustain),
		      z);

      z = release_x + I * release_y;
      ags_complex_set(&(AGS_NOTE(selection->data)->release),
		      z);

      z = I * ratio;
      ags_complex_set(&(AGS_NOTE(selection->data)->ratio),
		      z);

      selection = selection->next;
    }
    
    notation = notation->next;
  }

  pthread_mutex_unlock(audio_mutex);
}

void
ags_envelope_editor_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
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
 * Since: 0.8.5
 */
AgsPreset*
ags_envelope_editor_get_active_preset(AgsEnvelopeEditor *envelope_editor)
{
  AgsEnvelopeDialog *envelope_dialog;

  AgsWindow *window;
  AgsMachine *machine;

  AgsAudio *audio;
  AgsPreset *current;
  
  AgsMutexManager *mutex_manager;
  
  AgsApplicationContext *application_context;

  GList *preset;

  gchar *preset_name;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  
  if(!AGS_IS_ENVELOPE_EDITOR(envelope_editor)){
    return;
  }
    
  envelope_dialog = (AgsEnvelopeDialog *) gtk_widget_get_ancestor(envelope_editor,
								  AGS_TYPE_ENVELOPE_DIALOG);

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) envelope_dialog->machine,
						 AGS_TYPE_WINDOW);
  machine = envelope_dialog->machine;

  audio = machine->audio;

  /* application context and mutex manager */
  application_context = window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* preset name */
  preset_name = gtk_combo_box_text_get_active_text(envelope_editor->preset);
  
  pthread_mutex_lock(audio_mutex);

  preset = audio->preset;
  
  pthread_mutex_unlock(audio_mutex);

  /* find preset */
  current = NULL;
  preset = ags_preset_find_name(preset,
				preset_name);

  g_free(preset_name);
  
  if(preset != NULL){
    pthread_mutex_lock(audio_mutex);

    current = preset->data;

    pthread_mutex_unlock(audio_mutex);
  }

  return(current);
}

/**
 * ags_envelope_editor_load_preset:
 * @envelope_editor: the #AgsPatternEnvelope
 *
 * Load preset.
 * 
 * Since: 0.8.5
 */
void
ags_envelope_editor_load_preset(AgsEnvelopeEditor *envelope_editor)
{
  AgsEnvelopeDialog *envelope_dialog;

  AgsWindow *window;
  AgsMachine *machine;

  GtkTreeModel *model;

  AgsAudio *audio;

  AgsMutexManager *mutex_manager;
  
  AgsApplicationContext *application_context;
  
  GList *preset;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  
  if(!AGS_IS_ENVELOPE_EDITOR(envelope_editor)){
    return;
  }

  envelope_dialog = (AgsEnvelopeDialog *) gtk_widget_get_ancestor(envelope_editor,
								  AGS_TYPE_ENVELOPE_DIALOG);

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) envelope_dialog->machine,
						 AGS_TYPE_WINDOW);
  machine = envelope_dialog->machine;

  audio = machine->audio;

  /* application context and mutex manager */
  application_context = window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* get model */
  model = GTK_TREE_MODEL(gtk_combo_box_get_model(envelope_editor->preset));

  /* clear old */
  gtk_list_store_clear(GTK_LIST_STORE(model));

  /* create new */
  pthread_mutex_lock(audio_mutex);

  preset = audio->preset;

  while(preset != NULL){
    gtk_combo_box_text_append_text(envelope_editor->preset,
				   AGS_PRESET(preset->data)->preset_name);

    preset = preset->next;
  }

  pthread_mutex_unlock(audio_mutex);
}

/**
 * ags_envelope_editor_add_preset:
 * @envelope_editor: the #AgsPatternEnvelope
 * @preset_name: the preset name
 *
 * Add preset.
 * 
 * Since: 0.8.5
 */
void
ags_envelope_editor_add_preset(AgsEnvelopeEditor *envelope_editor,
			       gchar *preset_name)
{
  AgsEnvelopeDialog *envelope_dialog;

  AgsWindow *window;
  AgsMachine *machine;

  AgsAudio *audio;
  AgsPreset *preset;

  AgsMutexManager *mutex_manager;
  
  AgsApplicationContext *application_context;

  AgsComplex *val;
  
  GValue value = {0,};

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  
  if(!AGS_IS_ENVELOPE_EDITOR(envelope_editor) ||
     preset_name == NULL){
    return;
  }
  
  envelope_dialog = (AgsEnvelopeDialog *) gtk_widget_get_ancestor(envelope_editor,
								  AGS_TYPE_ENVELOPE_DIALOG);

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) envelope_dialog->machine,
						 AGS_TYPE_WINDOW);
  machine = envelope_dialog->machine;

  audio = machine->audio;

  /* application context and mutex manager */
  application_context = window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* check if already present */
  pthread_mutex_lock(audio_mutex);

  if(ags_preset_find_name(audio->preset,
			  preset_name) != NULL){
    pthread_mutex_unlock(audio_mutex);
    
    return;
  }

  /* create preset */
  preset = g_object_new(AGS_TYPE_PRESET,
			"scope", "ags-envelope",
			"preset-name", preset_name,
			NULL);
  ags_audio_add_preset(audio,
		       preset);

  /* preset - attack */
  val = ags_complex_alloc();
  ags_complex_set(val,
		  0.25 + I * 1.0);
  
  g_value_init(&value,
	       AGS_TYPE_COMPLEX);
  g_value_set_boxed(&value,
		    val);

  ags_preset_add_parameter(preset,
			   "attack", &value);

  /* preset - decay */
  val = ags_complex_alloc();
  ags_complex_set(val,
		  0.25 + I * 1.0);
  
  g_value_reset(&value);
  g_value_set_boxed(&value,
		    val);

  ags_preset_add_parameter(preset,
			   "decay", &value);

  /* preset - sustain */
  val = ags_complex_alloc();
  ags_complex_set(val,
		  0.25 + I * 1.0);
  
  g_value_reset(&value);
  g_value_set_boxed(&value,
		    val);

  ags_preset_add_parameter(preset,
			   "sustain", &value);

  /* preset - release */
  val = ags_complex_alloc();
  ags_complex_set(val,
		  0.25 + I * 1.0);
  
  g_value_reset(&value);
  g_value_set_boxed(&value,
		    val);

  ags_preset_add_parameter(preset,
			   "release", &value);

  /* preset - ratio */
  val = ags_complex_alloc();
  ags_complex_set(val,
		  0.0);
  
  g_value_reset(&value);
  g_value_set_boxed(&value,
		    val);

  ags_preset_add_parameter(preset,
			   "ratio", &value);

  /* release mutex */
  pthread_mutex_unlock(audio_mutex);
}

/**
 * ags_envelope_editor_remove_preset:
 * @envelope_editor: the #AgsPatternEnvelope
 * @nth: the nth preset to remove
 *
 * Remove preset.
 * 
 * Since: 0.8.5
 */
void
ags_envelope_editor_remove_preset(AgsEnvelopeEditor *envelope_editor,
				  guint nth)
{
  AgsEnvelopeDialog *envelope_dialog;

  AgsWindow *window;
  AgsMachine *machine;

  AgsAudio *audio;
  AgsPreset *preset;

  AgsMutexManager *mutex_manager;
  
  AgsApplicationContext *application_context;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  
  if(!AGS_IS_ENVELOPE_EDITOR(envelope_editor)){
    return;
  }
  
  envelope_dialog = (AgsEnvelopeDialog *) gtk_widget_get_ancestor(envelope_editor,
								  AGS_TYPE_ENVELOPE_DIALOG);

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) envelope_dialog->machine,
						 AGS_TYPE_WINDOW);
  machine = envelope_dialog->machine;

  audio = machine->audio;
  
  /* application context and mutex manager */
  application_context = window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* remove preset */
  pthread_mutex_lock(audio_mutex);

  preset = g_list_nth_data(audio->preset,
			   nth);

  pthread_mutex_unlock(audio_mutex);

  ags_audio_remove_preset(audio,
			  preset);
}

/**
 * ags_envelope_editor_reset_control:
 * @envelope_editor: the #AgsEnvelopeEditor
 * 
 * Reset controls.
 * 
 * Since: 0.8.5
 */
void
ags_envelope_editor_reset_control(AgsEnvelopeEditor *envelope_editor)
{
  AgsEnvelopeDialog *envelope_dialog;

  AgsWindow *window;
  AgsMachine *machine;

  AgsAudio *audio;
  AgsPreset *preset;

  AgsMutexManager *mutex_manager;
  
  AgsApplicationContext *application_context;

  AgsComplex *val;
  
  complex z;
  
  GValue value = {0,};

  GError *error;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  
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

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) envelope_dialog->machine,
						 AGS_TYPE_WINDOW);
  machine = envelope_dialog->machine;

  audio = machine->audio;
  
  /* application context and mutex manager */
  application_context = window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* attack */
  pthread_mutex_lock(audio_mutex);

  g_value_init(&value,
	       AGS_TYPE_COMPLEX);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "attack", &value,
			   &error);

  if(error != NULL){
    g_warning("%s", error->message);

    envelope_editor->flags &= (~AGS_ENVELOPE_EDITOR_NO_UPDATE);

    pthread_mutex_unlock(audio_mutex);

    return;
  }

  val = (AgsComplex *) g_value_get_boxed(&value);  
  z = ags_complex_get(val);
  
  gtk_range_set_value(envelope_editor->attack_x,
		      creal(z));
  gtk_range_set_value(envelope_editor->attack_y,
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

    pthread_mutex_unlock(audio_mutex);

    return;
  }

  val = (AgsComplex *) g_value_get_boxed(&value);  
  z = ags_complex_get(val);
  
  gtk_range_set_value(envelope_editor->decay_x,
		      creal(z));
  gtk_range_set_value(envelope_editor->decay_y,
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

    pthread_mutex_unlock(audio_mutex);

    return;
  }

  val = (AgsComplex *) g_value_get_boxed(&value);  
  z = ags_complex_get(val);
  
  gtk_range_set_value(envelope_editor->sustain_x,
		      creal(z));
  gtk_range_set_value(envelope_editor->sustain_y,
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

    pthread_mutex_unlock(audio_mutex);
   
    return;
  }

  val = (AgsComplex *) g_value_get_boxed(&value);  
  z = ags_complex_get(val);
  
  gtk_range_set_value(envelope_editor->release_x,
		      creal(z));
  gtk_range_set_value(envelope_editor->release_y,
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

    pthread_mutex_unlock(audio_mutex);

    return;
  }

  val = (AgsComplex *) g_value_get_boxed(&value);  
  z = ags_complex_get(val);
  
  gtk_range_set_value(envelope_editor->ratio,
		      cimag(z));

  /* unset no update */
  envelope_editor->flags &= (~AGS_ENVELOPE_EDITOR_NO_UPDATE);

  pthread_mutex_unlock(audio_mutex);
}

/**
 * ags_envelope_editor_plot:
 * @envelope_editor: the #AgsEnvelopeEditor
 * 
 * Plot envelope.
 * 
 * Since: 0.8.5
 */
void
ags_envelope_editor_plot(AgsEnvelopeEditor *envelope_editor)
{
  AgsCartesian *cartesian;
  
  AgsPlot *plot;

  gdouble width, height;
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

  width = (gdouble) cartesian->x_end - cartesian->x_start;
  height = (gdouble) cartesian->y_end - cartesian->y_start;
  
  default_width = cartesian->x_step_width * cartesian->x_scale_step_width;
  default_height = cartesian->y_step_height * cartesian->y_scale_step_height;

  attack_x = gtk_range_get_value(envelope_editor->attack_x);
  attack_y = gtk_range_get_value(envelope_editor->attack_y);

  decay_x = gtk_range_get_value(envelope_editor->decay_x);
  decay_y = gtk_range_get_value(envelope_editor->decay_y);

  sustain_x = gtk_range_get_value(envelope_editor->sustain_x);
  sustain_y = gtk_range_get_value(envelope_editor->sustain_y);

  release_x = gtk_range_get_value(envelope_editor->release_x);
  release_y = gtk_range_get_value(envelope_editor->release_y);

  ratio = gtk_range_get_value(envelope_editor->ratio);

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
  gtk_widget_queue_draw(cartesian);
}

/**
 * ags_envelope_editor_new:
 *
 * Creates an #AgsEnvelopeEditor
 *
 * Returns: a new #AgsEnvelopeEditor
 *
 * Since: 0.8.1
 */
AgsEnvelopeEditor*
ags_envelope_editor_new()
{
  AgsEnvelopeEditor *envelope_editor;

  envelope_editor = (AgsEnvelopeEditor *) g_object_new(AGS_TYPE_ENVELOPE_EDITOR,
						       NULL);

  return(envelope_editor);
}

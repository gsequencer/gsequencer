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

#include <ags/X/editor/ags_envelope_dialog.h>
#include <ags/X/editor/ags_envelope_dialog_callbacks.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>

#include <ags/thread/ags_mutex_manager.h>

#include <ags/X/ags_window.h>

#include <complex.h>

void ags_envelope_dialog_class_init(AgsEnvelopeDialogClass *envelope_dialog);
void ags_envelope_dialog_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_envelope_dialog_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_envelope_dialog_init(AgsEnvelopeDialog *envelope_dialog);
void ags_envelope_dialog_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_envelope_dialog_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_envelope_dialog_connect(AgsConnectable *connectable);
void ags_envelope_dialog_disconnect(AgsConnectable *connectable);
void ags_envelope_dialog_set_update(AgsApplicable *applicable, gboolean update);
void ags_envelope_dialog_apply(AgsApplicable *applicable);
void ags_envelope_dialog_reset(AgsApplicable *applicable);

gchar* ags_envelope_dialog_x_label_func(gdouble value,
					gpointer data);
gchar* ags_envelope_dialog_y_label_func(gdouble value,
					gpointer data);

/**
 * SECTION:ags_envelope_dialog
 * @short_description: pack pad editors.
 * @title: AgsEnvelopeDialog
 * @section_id:
 * @include: ags/X/ags_envelope_dialog.h
 *
 * #AgsEnvelopeDialog is a composite widget to edit all aspects of #AgsAudio.
 * It consists of multiple child editors.
 */

enum{
  SET_MACHINE,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_MACHINE,
};

static guint envelope_dialog_signals[LAST_SIGNAL];

GType
ags_envelope_dialog_get_type(void)
{
  static GType ags_type_envelope_dialog = 0;

  if(!ags_type_envelope_dialog){
    static const GTypeInfo ags_envelope_dialog_info = {
      sizeof (AgsEnvelopeDialogClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_envelope_dialog_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsEnvelopeDialog),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_envelope_dialog_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_envelope_dialog_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_envelope_dialog_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_envelope_dialog = g_type_register_static(GTK_TYPE_DIALOG,
						      "AgsEnvelopeDialog\0", &ags_envelope_dialog_info,
						      0);

    g_type_add_interface_static(ags_type_envelope_dialog,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_envelope_dialog,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }
  
  return(ags_type_envelope_dialog);
}

void
ags_envelope_dialog_class_init(AgsEnvelopeDialogClass *envelope_dialog)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GParamSpec *param_spec;

  /* GObjectClass */
  gobject = (GObjectClass *) envelope_dialog;

  gobject->set_property = ags_envelope_dialog_set_property;
  gobject->get_property = ags_envelope_dialog_get_property;

  /* properties */
  /**
   * AgsMachine:machine:
   *
   * The #AgsMachine to edit.
   * 
   * Since: 0.8.1
   */
  param_spec = g_param_spec_object("machine\0",
				   "assigned machine\0",
				   "The machine which this machine editor is assigned with\0",
				   AGS_TYPE_MACHINE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MACHINE,
				  param_spec);
}

void
ags_envelope_dialog_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_envelope_dialog_connect;
  connectable->disconnect = ags_envelope_dialog_disconnect;
}

void
ags_envelope_dialog_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_envelope_dialog_set_update;
  applicable->apply = ags_envelope_dialog_apply;
  applicable->reset = ags_envelope_dialog_reset;
}

void
ags_envelope_dialog_init(AgsEnvelopeDialog *envelope_dialog)
{
  GtkVBox *vbox;
  GtkTable *table;
  GtkLabel *label;
  AgsCartesian *cartesian;
  
  AgsPlot *plot;

  gdouble width, height;
  gdouble default_width, default_height;
  gdouble offset;
  
  gtk_window_set_title((GtkWindow *) envelope_dialog,
		       g_strdup("Envelope\0"));

  envelope_dialog->flags = 0;

  envelope_dialog->version = AGS_ENVELOPE_DIALOG_DEFAULT_VERSION;
  envelope_dialog->build_id = AGS_ENVELOPE_DIALOG_DEFAULT_BUILD_ID;

  envelope_dialog->machine = NULL;

  /* vbox */
  vbox = (GtkVBox *) gtk_vbox_new(FALSE,
				  0);
  gtk_box_pack_start((GtkBox *) GTK_DIALOG(envelope_dialog)->vbox,
		     GTK_WIDGET(vbox),
		     FALSE, FALSE,
		     0);

  /* cartesian */
  cartesian = 
    envelope_dialog->cartesian = ags_cartesian_new();

  cartesian->x_label_func = ags_envelope_dialog_x_label_func;
  cartesian->y_label_func = ags_envelope_dialog_y_label_func;

  ags_cartesian_fill_label(cartesian,
			   TRUE);
  ags_cartesian_fill_label(cartesian,
			   FALSE);  

  /* cartesian - plot */
  plot = ags_plot_alloc(4, 0, 0);
  plot->join_points = TRUE;

  plot->point_color[0][0] = 1.0;
  plot->point_color[1][0] = 1.0;
  plot->point_color[2][0] = 1.0;
  plot->point_color[3][0] = 1.0;

  width = cartesian->x_end - cartesian->x_start;
  height = cartesian->y_end - cartesian->y_start;
  
  default_width = cartesian->x_step_width * cartesian->x_scale_step_width;
  default_height = cartesian->y_step_height * cartesian->y_scale_step_height;

  plot->point[0][0] = default_width * (creal(0.25)) * cabs(0.0);
  plot->point[0][1] = -1.0 * default_height * cabs(0.25) + default_height / cabs(0.0);

  plot->point[1][0] = default_width * (creal(0.5) + offset) * cabs(0.0);
  plot->point[1][1] = -1.0 * default_height * cabs(0.5) + default_height / cabs(0.0);

  plot->point[2][0] = default_width * (creal(0.75) + offset) * cabs(0.0);
  plot->point[2][1] = -1.0 * default_height * cabs(0.75) + default_height / cabs(0.0);

  plot->point[3][0] = default_width * (creal(1.0) + offset) * cabs(0.0);
  plot->point[3][1] = -1.0 * default_height * cabs(1.0) + default_height / cabs(0.0);

  ags_cartesian_add_plot(cartesian,
			 plot);

  /* cartesian - size, pack and redraw */
  gtk_widget_set_size_request(cartesian,
			      (gint) width + 2.0 * cartesian->x_margin, (gint) height + 2.0 * cartesian->y_margin);
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(cartesian),
		     FALSE, FALSE,
		     0);

  gtk_widget_queue_draw(cartesian);

  /* table */
  table = (GtkTable *) gtk_table_new(5, 2,
				     FALSE);
  gtk_box_pack_start((GtkBox *) vbox,
		     GTK_WIDGET(table),
		     FALSE, FALSE,
		     0);

  /* attack */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label\0", "attack\0",
				    "xalign\0", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  envelope_dialog->attack = (GtkHScale *) gtk_hscale_new_with_range(0.0, 1.0, 0.001);
  gtk_scale_set_draw_value(envelope_dialog->attack,
			   TRUE);
  gtk_range_set_value((GtkRange *) envelope_dialog->attack,
		      0.25);
  gtk_table_attach(table,
		   GTK_WIDGET(envelope_dialog->attack),
		   1, 2,
		   0, 1,
		   GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND,
		   0, 0);

  /* decay */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label\0", "decay\0",
				    "xalign\0", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  envelope_dialog->decay = (GtkHScale *) gtk_hscale_new_with_range(0.0, 1.0, 0.001);
  gtk_scale_set_draw_value(envelope_dialog->decay,
			   TRUE);
  gtk_range_set_value((GtkRange *) envelope_dialog->decay,
		      0.25);
  gtk_table_attach(table,
		   GTK_WIDGET(envelope_dialog->decay),
		   1, 2,
		   1, 2,
		   GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND,
		   0, 0);

  /* sustain */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label\0", "sustain\0",
				    "xalign\0", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  envelope_dialog->sustain = (GtkHScale *) gtk_hscale_new_with_range(0.0, 1.0, 0.001);
  gtk_scale_set_draw_value(envelope_dialog->sustain,
			   TRUE);
  gtk_range_set_value((GtkRange *) envelope_dialog->sustain,
		      0.25);
  gtk_table_attach(table,
		   GTK_WIDGET(envelope_dialog->sustain),
		   1, 2,
		   2, 3,
		   GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND,
		   0, 0);

  /* release */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label\0", "release\0",
				    "xalign\0", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   3, 4,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  envelope_dialog->release = (GtkHScale *) gtk_hscale_new_with_range(0.0, 1.0, 0.001);
  gtk_scale_set_draw_value(envelope_dialog->release,
			   TRUE);
  gtk_range_set_value((GtkRange *) envelope_dialog->release,
		      0.25);
  gtk_table_attach(table,
		   GTK_WIDGET(envelope_dialog->release),
		   1, 2,
		   3, 4,
		   GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND,
		   0, 0);

  /* ratio */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label\0", "ratio\0",
				    "xalign\0", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   4, 5,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  envelope_dialog->ratio = (GtkHScale *) gtk_hscale_new_with_range(-1.0, 1.0, 0.001);
  gtk_scale_set_draw_value(envelope_dialog->ratio,
			   TRUE);
  gtk_range_set_value((GtkRange *) envelope_dialog->ratio,
		      -1.0);
  gtk_table_attach(table,
		   GTK_WIDGET(envelope_dialog->ratio),
		   1, 2,
		   4, 5,
		   GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND,
		   0, 0);

  /* GtkButton's in GtkDialog->action_area  */
  envelope_dialog->apply = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_APPLY);
  gtk_box_pack_start((GtkBox *) GTK_DIALOG(envelope_dialog)->action_area,
		     (GtkWidget *) envelope_dialog->apply,
		     FALSE, FALSE,
		     0);
  
  envelope_dialog->ok = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_OK);
  gtk_box_pack_start((GtkBox *) GTK_DIALOG(envelope_dialog)->action_area,
		     (GtkWidget *) envelope_dialog->ok,
		     FALSE, FALSE,
		     0);
  
  envelope_dialog->cancel = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_CANCEL);
  gtk_box_pack_start((GtkBox *) GTK_DIALOG(envelope_dialog)->action_area,
		     (GtkWidget *) envelope_dialog->cancel,
		     FALSE, FALSE,
		     0);
}

void
ags_envelope_dialog_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsEnvelopeDialog *envelope_dialog;

  envelope_dialog = AGS_ENVELOPE_DIALOG(gobject);

  switch(prop_id){
  case PROP_MACHINE:
    {
      AgsMachine *machine;

      machine = (AgsMachine *) g_value_get_object(value);

      if(machine == envelope_dialog->machine){
	return;
      }

      if(envelope_dialog->machine != NULL){
	g_object_unref(envelope_dialog->machine);
      }

      if(machine != NULL){
	g_object_ref(machine);
      }

      envelope_dialog->machine = machine;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_envelope_dialog_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsEnvelopeDialog *envelope_dialog;

  envelope_dialog = AGS_ENVELOPE_DIALOG(gobject);

  switch(prop_id){
  case PROP_MACHINE:
    {
      g_value_set_object(value, envelope_dialog->machine);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_envelope_dialog_connect(AgsConnectable *connectable)
{
  AgsEnvelopeDialog *envelope_dialog;

  envelope_dialog = AGS_ENVELOPE_DIALOG(connectable);

  if((AGS_ENVELOPE_DIALOG_CONNECTED & (envelope_dialog->flags)) != 0){
    return;
  }

  envelope_dialog->flags |= AGS_ENVELOPE_DIALOG_CONNECTED;

  g_signal_connect((GObject *) envelope_dialog->attack, "value-changed\0",
		   G_CALLBACK(ags_envelope_dialog_attack_callback), (gpointer) envelope_dialog);

  g_signal_connect((GObject *) envelope_dialog->decay, "value-changed\0",
		   G_CALLBACK(ags_envelope_dialog_decay_callback), (gpointer) envelope_dialog);

  g_signal_connect((GObject *) envelope_dialog->sustain, "value-changed\0",
		   G_CALLBACK(ags_envelope_dialog_sustain_callback), (gpointer) envelope_dialog);

  g_signal_connect((GObject *) envelope_dialog->release, "value-changed\0",
		   G_CALLBACK(ags_envelope_dialog_release_callback), (gpointer) envelope_dialog);

  g_signal_connect((GObject *) envelope_dialog->ratio, "value-changed\0",
		   G_CALLBACK(ags_envelope_dialog_ratio_callback), (gpointer) envelope_dialog);

  /* applicable */
  g_signal_connect((GObject *) envelope_dialog->apply, "clicked\0",
		   G_CALLBACK(ags_envelope_dialog_apply_callback), (gpointer) envelope_dialog);

  g_signal_connect((GObject *) envelope_dialog->ok, "clicked\0",
		   G_CALLBACK(ags_envelope_dialog_ok_callback), (gpointer) envelope_dialog);

  g_signal_connect((GObject *) envelope_dialog->cancel, "clicked\0",
		   G_CALLBACK(ags_envelope_dialog_cancel_callback), (gpointer) envelope_dialog);
}

void
ags_envelope_dialog_disconnect(AgsConnectable *connectable)
{
  AgsEnvelopeDialog *envelope_dialog;

  envelope_dialog = AGS_ENVELOPE_DIALOG(connectable);

  if((AGS_ENVELOPE_DIALOG_CONNECTED & (envelope_dialog->flags)) == 0){
    return;
  }

  envelope_dialog->flags &= (~AGS_ENVELOPE_DIALOG_CONNECTED);

  /* applicable */
  g_object_disconnect((GObject *) envelope_dialog->apply,
		      "clicked\0",
		      G_CALLBACK(ags_envelope_dialog_apply_callback),
		      (gpointer) envelope_dialog,
		      NULL);

  g_object_disconnect((GObject *) envelope_dialog->ok,
		      "clicked\0",
		      G_CALLBACK(ags_envelope_dialog_ok_callback),
		      (gpointer) envelope_dialog,
		      NULL);

  g_object_disconnect((GObject *) envelope_dialog->cancel,
		      "clicked\0",
		      G_CALLBACK(ags_envelope_dialog_cancel_callback),
		      (gpointer) envelope_dialog,
		      NULL);
}

void
ags_envelope_dialog_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_envelope_dialog_apply(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

void
ags_envelope_dialog_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

gchar*
ags_envelope_dialog_x_label_func(gdouble value,
				 gpointer data)
{
  gchar *format;
  gchar *str;
  
  format = g_strdup_printf("%%.%df\0",
			   (guint) ceil(AGS_CARTESIAN(data)->y_label_precision));

  str = g_strdup_printf(format,
			value / 10.0);
  g_free(format);

  return(str);
}

gchar*
ags_envelope_dialog_y_label_func(gdouble value,
				 gpointer data)
{
  gchar *format;
  gchar *str;
  
  format = g_strdup_printf("%%.%df\0",
			   (guint) ceil(AGS_CARTESIAN(data)->y_label_precision));

  str = g_strdup_printf(format,
			value / 10.0);
  g_free(format);

  return(str);
}

void
ags_envelope_dialog_plot(AgsEnvelopeDialog *envelope_dialog)
{
  AgsCartesian *cartesian;
  
  AgsPlot *plot;

  gdouble width, height;
  gdouble default_width, default_height;
  gdouble attack, decay, sustain, release;
  gdouble ratio;
  gdouble offset;
  
  if(!AGS_IS_ENVELOPE_DIALOG(envelope_dialog)){
    return;
  }
  
  cartesian = envelope_dialog->cartesian;

  plot = cartesian->plot->data;

  width = (gdouble) cartesian->x_end - cartesian->x_start;
  height = (gdouble) cartesian->y_end - cartesian->y_start;
  
  default_width = cartesian->x_step_width * cartesian->x_scale_step_width;
  default_height = cartesian->y_step_height * cartesian->y_scale_step_height;

  attack = gtk_range_get_value(envelope_dialog->attack);
  decay = gtk_range_get_value(envelope_dialog->decay);
  sustain = gtk_range_get_value(envelope_dialog->sustain);
  release = gtk_range_get_value(envelope_dialog->release);

  ratio = gtk_range_get_value(envelope_dialog->ratio);

  /* reset plot points */
  plot->point[0][0] = default_width * (creal(attack)) * cabs(ratio);
  plot->point[0][1] = default_height * cabs(attack) - default_height / cabs(ratio);
    
  offset = creal(attack);
  
  plot->point[1][0] = default_width * (creal(decay) + offset) * cabs(ratio);
  plot->point[1][1] = default_height * cabs(decay) - default_height / cabs(ratio);
  
  offset += creal(decay);
  
  plot->point[2][0] = default_width * (creal(sustain) + offset) * cabs(ratio);
  plot->point[2][1] = default_height * cabs(sustain) - default_height / cabs(ratio);
  
  offset += creal(sustain);

  plot->point[3][0] = default_width * (creal(release) + offset) * cabs(ratio);
  plot->point[3][1] = default_height * cabs(release) - default_height / cabs(ratio);
  
  /* redraw */
  gtk_widget_queue_draw(cartesian);
}

/**
 * ags_envelope_dialog_new:
 * @machine: the assigned machine.
 *
 * Creates an #AgsEnvelopeDialog
 *
 * Returns: a new #AgsEnvelopeDialog
 *
 * Since: 0.8.1
 */
AgsEnvelopeDialog*
ags_envelope_dialog_new(AgsMachine *machine)
{
  AgsEnvelopeDialog *envelope_dialog;

  envelope_dialog = (AgsEnvelopeDialog *) g_object_new(AGS_TYPE_ENVELOPE_DIALOG,
						       "machine\0", machine,
						       NULL);

  return(envelope_dialog);
}

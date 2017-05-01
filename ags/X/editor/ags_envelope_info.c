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

#include <complex.h>

void ags_envelope_info_class_init(AgsEnvelopeInfoClass *envelope_info);
void ags_envelope_info_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_envelope_info_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_envelope_info_init(AgsEnvelopeInfo *envelope_info);
void ags_envelope_info_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec);
void ags_envelope_info_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec);
void ags_envelope_info_connect(AgsConnectable *connectable);
void ags_envelope_info_disconnect(AgsConnectable *connectable);
void ags_envelope_info_set_update(AgsApplicable *applicable, gboolean update);
void ags_envelope_info_apply(AgsApplicable *applicable);
void ags_envelope_info_reset(AgsApplicable *applicable);
gboolean ags_envelope_info_delete_event(GtkWidget *widget, GdkEventAny *event);

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

enum{
  PROP_0,
  PROP_MACHINE,
};

static guint envelope_info_signals[LAST_SIGNAL];
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

    ags_type_envelope_info = g_type_register_static(GTK_TYPE_DIALOG,
						    "AgsEnvelopeInfo\0", &ags_envelope_info_info,
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

  gobject->set_property = ags_envelope_info_set_property;
  gobject->get_property = ags_envelope_info_get_property;

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


  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) envelope_info;

  widget->delete_event = ags_envelope_info_delete_event;
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
  gtk_window_set_title((GtkWindow *) envelope_info,
		       g_strdup("Envelope Info\0"));

  envelope_info->flags = 0;

  envelope_info->version = AGS_ENVELOPE_INFO_DEFAULT_VERSION;
  envelope_info->build_id = AGS_ENVELOPE_INFO_DEFAULT_BUILD_ID;

  envelope_info->machine = NULL;

  /* GtkButton's in GtkInfo->action_area  */
  envelope_info->ok = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_OK);
  gtk_box_pack_start((GtkBox *) GTK_INFO(envelope_info)->action_area,
		     (GtkWidget *) envelope_info->ok,
		     FALSE, FALSE,
		     0);
}

void
ags_envelope_info_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsEnvelopeInfo *envelope_info;

  envelope_info = AGS_ENVELOPE_INFO(gobject);

  switch(prop_id){
  case PROP_MACHINE:
    {
      AgsMachine *machine;

      machine = (AgsMachine *) g_value_get_object(value);

      if(machine == envelope_info->machine){
	return;
      }

      if(envelope_info->machine != NULL){
	g_object_unref(envelope_info->machine);
      }

      if(machine != NULL){
	g_object_ref(machine);
      }

      envelope_info->machine = machine;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_envelope_info_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec)
{
  AgsEnvelopeInfo *envelope_info;

  envelope_info = AGS_ENVELOPE_INFO(gobject);

  switch(prop_id){
  case PROP_MACHINE:
    {
      g_value_set_object(value, envelope_info->machine);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
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

  /* applicable */
  g_signal_connect((GObject *) envelope_info->ok, "clicked\0",
		   G_CALLBACK(ags_envelope_info_ok_callback), (gpointer) envelope_info);
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

  /* applicable */
  g_object_disconnect((GObject *) envelope_info->ok,
		      "clicked\0",
		      G_CALLBACK(ags_envelope_info_ok_callback),
		      (gpointer) envelope_info,
		      NULL);
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
  //TODO:JK: implement me
}

gboolean
ags_envelope_info_delete_event(GtkWidget *widget, GdkEventAny *event)
{
  AGS_ENVELOPE_INFO(widget)->machine->envelope_info = NULL;
  
  GTK_WIDGET_CLASS(ags_envelope_info_parent_class)->delete_event(widget, event);

  return(TRUE);
}

gchar*
ags_envelope_info_x_label_func(gdouble value,
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
ags_envelope_info_y_label_func(gdouble value,
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

/**
 * ags_envelope_info_new:
 * @machine: the assigned machine.
 *
 * Creates an #AgsEnvelopeInfo
 *
 * Returns: a new #AgsEnvelopeInfo
 *
 * Since: 0.8.1
 */
AgsEnvelopeInfo*
ags_envelope_info_new(AgsMachine *machine)
{
  AgsEnvelopeInfo *envelope_info;

  envelope_info = (AgsEnvelopeInfo *) g_object_new(AGS_TYPE_ENVELOPE_INFO,
						   "machine\0", machine,
						   NULL);

  return(envelope_info);
}

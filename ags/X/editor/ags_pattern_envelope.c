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

#include <ags/i18n.h>

void ags_pattern_envelope_class_init(AgsEnvelopePatternClass *pattern_envelope);
void ags_pattern_envelope_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_pattern_envelope_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_pattern_envelope_init(AgsEnvelopePattern *pattern_envelope);
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
 * @title: AgsEnvelopePattern
 * @section_id:
 * @include: ags/X/ags_pattern_envelope.h
 *
 * #AgsEnvelopePattern is a composite widget to show envelope controls
 * of selected AgsNote.
 */

static gpointer ags_pattern_envelope_parent_class = NULL;

GType
ags_pattern_envelope_get_type(void)
{
  static GType ags_type_pattern_envelope = 0;

  if(!ags_type_pattern_envelope){
    static const GTypeInfo ags_pattern_envelope_info = {
      sizeof (AgsEnvelopePatternClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_pattern_envelope_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsEnvelopePattern),
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
						       "AgsEnvelopePattern", &ags_pattern_envelope_info,
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
ags_pattern_envelope_class_init(AgsEnvelopePatternClass *pattern_envelope)
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
ags_pattern_envelope_init(AgsEnvelopePattern *pattern_envelope)
{
}

void
ags_pattern_envelope_connect(AgsConnectable *connectable)
{
  AgsEnvelopePattern *pattern_envelope;

  pattern_envelope = AGS_PATTERN_ENVELOPE(connectable);

  if((AGS_PATTERN_ENVELOPE_CONNECTED & (pattern_envelope->flags)) != 0){
    return;
  }

  pattern_envelope->flags |= AGS_PATTERN_ENVELOPE_CONNECTED;
}

void
ags_pattern_envelope_disconnect(AgsConnectable *connectable)
{
  AgsEnvelopePattern *pattern_envelope;

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
ags_pattern_envelope_plot(AgsEnvelopePattern *pattern_envelope)
{
}

/**
 * ags_pattern_envelope_new:
 *
 * Creates an #AgsEnvelopePattern
 *
 * Returns: a new #AgsEnvelopePattern
 *
 * Since: 0.8.5
 */
AgsEnvelopePattern*
ags_pattern_envelope_new()
{
  AgsEnvelopePattern *pattern_envelope;

  pattern_envelope = (AgsEnvelopePattern *) g_object_new(AGS_TYPE_PATTERN_ENVELOPE,
							 NULL);

  return(pattern_envelope);
}

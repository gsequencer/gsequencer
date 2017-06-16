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

#ifndef __AGS_PATTERN_ENVELOPE_H__
#define __AGS_PATTERN_ENVELOPE_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/widget/ags_cartesian.h>

#define AGS_TYPE_PATTERN_ENVELOPE                (ags_pattern_envelope_get_type())
#define AGS_PATTERN_ENVELOPE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PATTERN_ENVELOPE, AgsPatternEnvelope))
#define AGS_PATTERN_ENVELOPE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PATTERN_ENVELOPE, AgsPatternEnvelopeClass))
#define AGS_IS_PATTERN_ENVELOPE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_PATTERN_ENVELOPE))
#define AGS_IS_PATTERN_ENVELOPE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_PATTERN_ENVELOPE))
#define AGS_PATTERN_ENVELOPE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_PATTERN_ENVELOPE, AgsPatternEnvelopeClass))

#define AGS_PATTERN_ENVELOPE_DEFAULT_VERSION "0.8.5\0"
#define AGS_PATTERN_ENVELOPE_DEFAULT_BUILD_ID "CEST 16-06-2017 11:29\0"

typedef struct _AgsPatternEnvelope AgsPatternEnvelope;
typedef struct _AgsPatternEnvelopeClass AgsPatternEnvelopeClass;

typedef enum{
  AGS_PATTERN_ENVELOPE_CONNECTED      = 1,
}AgsPatternEnvelopeFlags;

typedef enum{
  AGS_ENVELOPE_INFO_COLUMN_EDIT,
  AGS_ENVELOPE_INFO_COLUMN_PLOT,
  AGS_ENVELOPE_INFO_COLUMN_AUDIO_CHANNEL_START,
  AGS_ENVELOPE_INFO_COLUMN_AUDIO_CHANNEL_END,
  AGS_ENVELOPE_INFO_COLUMN_PATTERN_X_START,
  AGS_ENVELOPE_INFO_COLUMN_PATTERN_X_END,
  AGS_ENVELOPE_INFO_COLUMN_PATTERN_Y_START,
  AGS_ENVELOPE_INFO_COLUMN_PATTERN_Y_END,
  AGS_ENVELOPE_INFO_COLUMN_LAST,
}AgsEnvelopeInfoColumn;

struct _AgsPatternEnvelope
{
  GtkVBox vbox;

  guint flags;

  gchar *version;
  gchar *build_id;

  GtkCheckButton *enabled;
  
  AgsCartesian *cartesian;

  GtkTreeView *tree_view;

  GtkSpinButton *audio_channel_start;
  GtkSpinButton *audio_channel_end;
  
  GtkSpinButton *x_start;
  GtkSpinButton *x_end;
  
  GtkSpinButton *y_start;
  GtkSpinButton *y_end;

  GtkButton *move_up;
  GtkButton *move_down;
  GtkButton *add;
  GtkButton *remove;
};

struct _AgsPatternEnvelopeClass
{
  GtkVBoxClass vbox;
};

GType ags_pattern_envelope_get_type(void);

void ags_pattern_envelope_plot(AgsPatternEnvelope *pattern_envelope);

AgsPatternEnvelope* ags_pattern_envelope_new();

#endif /*__AGS_PATTERN_ENVELOPE_H__*/

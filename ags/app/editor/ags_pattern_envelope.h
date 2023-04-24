/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_PATTERN_ENVELOPE                (ags_pattern_envelope_get_type())
#define AGS_PATTERN_ENVELOPE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PATTERN_ENVELOPE, AgsPatternEnvelope))
#define AGS_PATTERN_ENVELOPE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PATTERN_ENVELOPE, AgsPatternEnvelopeClass))
#define AGS_IS_PATTERN_ENVELOPE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_PATTERN_ENVELOPE))
#define AGS_IS_PATTERN_ENVELOPE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_PATTERN_ENVELOPE))
#define AGS_PATTERN_ENVELOPE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_PATTERN_ENVELOPE, AgsPatternEnvelopeClass))

#define AGS_PATTERN_ENVELOPE_DEFAULT_VERSION "4.0.0"
#define AGS_PATTERN_ENVELOPE_DEFAULT_BUILD_ID "Tue Mar 01 12:27:51 UTC 2022"

#define AGS_PATTERN_ENVELOPE_SCALE_WIDTH (250)

typedef struct _AgsPatternEnvelope AgsPatternEnvelope;
typedef struct _AgsPatternEnvelopeClass AgsPatternEnvelopeClass;

typedef enum{
  AGS_PATTERN_ENVELOPE_CONNECTED      = 1,
  AGS_PATTERN_ENVELOPE_NO_UPDATE      = 1 <<  1,
}AgsPatternEnvelopeFlags;

typedef enum{
  AGS_PATTERN_ENVELOPE_COLUMN_EDIT,
  AGS_PATTERN_ENVELOPE_COLUMN_PLOT,
  AGS_PATTERN_ENVELOPE_COLUMN_PRESET_NAME,
  AGS_PATTERN_ENVELOPE_COLUMN_AUDIO_CHANNEL_START,
  AGS_PATTERN_ENVELOPE_COLUMN_AUDIO_CHANNEL_END,
  AGS_PATTERN_ENVELOPE_COLUMN_PAD_START,
  AGS_PATTERN_ENVELOPE_COLUMN_PAD_END,
  AGS_PATTERN_ENVELOPE_COLUMN_X_START,
  AGS_PATTERN_ENVELOPE_COLUMN_X_END,
  AGS_PATTERN_ENVELOPE_COLUMN_LAST,
}AgsPatternEnvelopeColumn;

struct _AgsPatternEnvelope
{
  GtkBox box;

  guint flags;

  gchar *version;
  gchar *build_id;

  GtkCheckButton *enabled;
  
  AgsCartesian *cartesian;

  GtkTreeView *tree_view;

  GtkSpinButton *audio_channel_start;
  GtkSpinButton *audio_channel_end;
  
  GtkSpinButton *pad_start;
  GtkSpinButton *pad_end;

  GtkSpinButton *x_start;
  GtkSpinButton *x_end;
  
  GtkScale *attack_x;
  GtkScale *attack_y;
  
  GtkScale *decay_x;
  GtkScale *decay_y;

  GtkScale *sustain_x;
  GtkScale *sustain_y;

  GtkScale *release_x;
  GtkScale *release_y;

  GtkScale *ratio;

  GtkDialog *rename;
  
  GtkButton *move_up;
  GtkButton *move_down;
  GtkButton *add;
  GtkButton *remove;
};

struct _AgsPatternEnvelopeClass
{
  GtkBoxClass box;
};

GType ags_pattern_envelope_get_type(void);

void ags_pattern_envelope_set_preset_property(AgsPatternEnvelope *pattern_envelope,
					      AgsPreset *preset,
					      gchar *property_name, GValue *value);

AgsPreset* ags_pattern_envelope_get_active_preset(AgsPatternEnvelope *pattern_envelope);

void ags_pattern_envelope_load_preset(AgsPatternEnvelope *pattern_envelope);
void ags_pattern_envelope_add_preset(AgsPatternEnvelope *pattern_envelope,
				     gchar *preset_name);
void ags_pattern_envelope_remove_preset(AgsPatternEnvelope *pattern_envelope,
					guint nth);

void ags_pattern_envelope_reset_control(AgsPatternEnvelope *pattern_envelope);
void ags_pattern_envelope_reset_tree_view(AgsPatternEnvelope *pattern_envelope);

void ags_pattern_envelope_plot(AgsPatternEnvelope *pattern_envelope);

AgsPatternEnvelope* ags_pattern_envelope_new();

G_END_DECLS

#endif /*__AGS_PATTERN_ENVELOPE_H__*/

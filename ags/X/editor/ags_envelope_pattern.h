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

#ifndef __AGS_ENVELOPE_PATTERN_H__
#define __AGS_ENVELOPE_PATTERN_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/widget/ags_cartesian.h>

#define AGS_TYPE_ENVELOPE_PATTERN                (ags_envelope_pattern_get_type())
#define AGS_ENVELOPE_PATTERN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_ENVELOPE_PATTERN, AgsEnvelopePattern))
#define AGS_ENVELOPE_PATTERN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_ENVELOPE_PATTERN, AgsEnvelopePatternClass))
#define AGS_IS_ENVELOPE_PATTERN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_ENVELOPE_PATTERN))
#define AGS_IS_ENVELOPE_PATTERN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_ENVELOPE_PATTERN))
#define AGS_ENVELOPE_PATTERN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_ENVELOPE_PATTERN, AgsEnvelopePatternClass))

#define AGS_ENVELOPE_PATTERN_DEFAULT_VERSION "0.8.5\0"
#define AGS_ENVELOPE_PATTERN_DEFAULT_BUILD_ID "CEST 16-06-2017 11:29\0"

typedef struct _AgsEnvelopePattern AgsEnvelopePattern;
typedef struct _AgsEnvelopePatternClass AgsEnvelopePatternClass;

typedef enum{
  AGS_ENVELOPE_PATTERN_CONNECTED      = 1,
}AgsEnvelopePatternFlags;

struct _AgsEnvelopePattern
{
  GtkVBox vbox;

  guint flags;

  gchar *version;
  gchar *build_id;

  GtkCheckButton *enabled;
  
  AgsCartesian *cartesian;
  GtkSpinButton *preset_count;
  GtkTreeView *tree_view;
};

struct _AgsEnvelopePatternClass
{
  GtkVBoxClass vbox;
};

GType ags_envelope_pattern_get_type(void);

void ags_envelope_pattern_plot(AgsEnvelopePattern *envelope_pattern);

AgsEnvelopePattern* ags_envelope_pattern_new();

#endif /*__AGS_ENVELOPE_PATTERN_H__*/

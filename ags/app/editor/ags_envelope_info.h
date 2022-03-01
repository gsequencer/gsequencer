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

#ifndef __AGS_ENVELOPE_INFO_H__
#define __AGS_ENVELOPE_INFO_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_ENVELOPE_INFO                (ags_envelope_info_get_type())
#define AGS_ENVELOPE_INFO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_ENVELOPE_INFO, AgsEnvelopeInfo))
#define AGS_ENVELOPE_INFO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_ENVELOPE_INFO, AgsEnvelopeInfoClass))
#define AGS_IS_ENVELOPE_INFO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_ENVELOPE_INFO))
#define AGS_IS_ENVELOPE_INFO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_ENVELOPE_INFO))
#define AGS_ENVELOPE_INFO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_ENVELOPE_INFO, AgsEnvelopeInfoClass))

#define AGS_ENVELOPE_INFO_DEFAULT_VERSION "4.0.0"
#define AGS_ENVELOPE_INFO_DEFAULT_BUILD_ID "Tue Mar 01 12:27:51 UTC 2022"

typedef struct _AgsEnvelopeInfo AgsEnvelopeInfo;
typedef struct _AgsEnvelopeInfoClass AgsEnvelopeInfoClass;

typedef enum{
  AGS_ENVELOPE_INFO_CONNECTED      = 1,
}AgsEnvelopeInfoFlags;

typedef enum{
  AGS_ENVELOPE_INFO_COLUMN_PLOT,
  AGS_ENVELOPE_INFO_COLUMN_AUDIO_CHANNEL,
  AGS_ENVELOPE_INFO_COLUMN_NOTE_X0,
  AGS_ENVELOPE_INFO_COLUMN_NOTE_X1,
  AGS_ENVELOPE_INFO_COLUMN_NOTE_Y,
  AGS_ENVELOPE_INFO_COLUMN_LAST,
}AgsEnvelopeInfoColumn;

struct _AgsEnvelopeInfo
{
  GtkBox box;

  guint flags;

  gchar *version;
  gchar *build_id;

  GList *selection;
  
  AgsCartesian *cartesian;

  GtkTreeView *tree_view;
};

struct _AgsEnvelopeInfoClass
{
  GtkBoxClass box;
};

GType ags_envelope_info_get_type(void);

void ags_envelope_info_plot(AgsEnvelopeInfo *envelope_info);

AgsEnvelopeInfo* ags_envelope_info_new();

G_END_DECLS

#endif /*__AGS_ENVELOPE_INFO_H__*/

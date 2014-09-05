/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __AGS_RECALL_H__
#define __AGS_RECALL_LADSPA_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall.h>

#define AGS_TYPE_RECALL_LADSPA                (ags_recall_ladspa_get_type())
#define AGS_RECALL_LADSPA(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RECALL_LADSPA, AgsRecallLadspa))
#define AGS_RECALL_LADSPA_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_RECALL_LADSPA, AgsRecallLadspaClass))
#define AGS_IS_RECALL_LADSPA(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_RECALL_LADSPA))
#define AGS_IS_RECALL_LADSPA_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_RECALL_LADSPA))
#define AGS_RECALL_LADSPA_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_RECALL_LADSPA, AgsRecallLadspaClass))

typedef struct _AgsRecallLadspa AgsRecallLadspa;
typedef struct _AgsRecallLadspaClass AgsRecallLadspaClass;

struct _AgsRecallLadspa
{
  AgsRecall recall;

  gchar *filename;
  gchar *effect;
  guint index;
};

struct _AgsRecallLadspaClass
{
  AgsRecallClass recall;
};

GType ags_recall_ladspa_get_type();

AgsRecallLadspa* ags_recall_ladspa_new(gchar *filename,
				       gchar *effect,
				       guint index);

#endif /*__AGS_RECALL_LADSPA_H__*/

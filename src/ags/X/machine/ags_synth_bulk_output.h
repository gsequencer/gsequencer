/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
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

#ifndef __AGS_SYNTH_BULK_OUTPUT_H__
#define __AGS_SYNTH_BULK_OUTPUT_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/audio/ags_channel.h>

#include <ags/X/ags_effect_bulk.h>

#define AGS_TYPE_SYNTH_BULK_OUTPUT                (ags_synth_bulk_output_get_type())
#define AGS_SYNTH_BULK_OUTPUT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SYNTH_BULK_OUTPUT, AgsSynthBulkOutput))
#define AGS_SYNTH_BULK_OUTPUT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SYNTH_BULK_OUTPUT, AgsSynthBulkOutputClass))
#define AGS_IS_SYNTH_BULK_OUTPUT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SYNTH_BULK_OUTPUT))
#define AGS_IS_SYNTH_BULK_OUTPUT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_SYNTH_BULK_OUTPUT))
#define AGS_SYNTH_BULK_OUTPUT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_SYNTH_BULK_OUTPUT, AgsSynthBulkOutputClass))

typedef struct _AgsSynthBulkOutput AgsSynthBulkOutput;
typedef struct _AgsSynthBulkOutputClass AgsSynthBulkOutputClass;

struct _AgsSynthBulkOutput
{
  AgsEffectBulk effect_bulk;
};

struct _AgsSynthBulkOutputClass
{
  AgsEffectBulkClass effect_bulk;
};

GType ags_synth_bulk_output_get_type(void);

AgsSynthBulkOutput* ags_synth_bulk_output_new(AgsChannel *channel);

#endif /*__AGS_SYNTH_BULK_OUTPUT_H__*/

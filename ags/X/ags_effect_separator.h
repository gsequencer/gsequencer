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

#ifndef __AGS_EFFECT_SEPARATOR_H__
#define __AGS_EFFECT_SEPARATOR_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#define AGS_TYPE_EFFECT_SEPARATOR                (ags_effect_separator_get_type())
#define AGS_EFFECT_SEPARATOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_EFFECT_SEPARATOR, AgsEffectSeparator))
#define AGS_EFFECT_SEPARATOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_EFFECT_SEPARATOR, AgsEffectSeparatorClass))
#define AGS_IS_EFFECT_SEPARATOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_EFFECT_SEPARATOR))
#define AGS_IS_EFFECT_SEPARATOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_EFFECT_SEPARATOR))
#define AGS_EFFECT_SEPARATOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_EFFECT_SEPARATOR, AgsEffectSeparatorClass))

typedef struct _AgsEffectSeparator AgsEffectSeparator;
typedef struct _AgsEffectSeparatorClass AgsEffectSeparatorClass;

struct _AgsEffectSeparator
{
  GtkAlignment alignment;

  gchar *filename;
  gchar *effect;

  GtkLabel *label;
};

struct _AgsEffectSeparatorClass
{
  GtkAlignmentClass alignment;
};

GType ags_effect_separator_get_type(void);

AgsEffectSeparator* ags_effect_separator_new();

#endif /*__AGS_EFFECT_SEPARATOR_H__*/

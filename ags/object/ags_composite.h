/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#ifndef __AGS_COMPOSITE_H__
#define __AGS_COMPOSITE_H__

#include <glib.h>
#include <glib-object.h>

#define AGS_TYPE_COMPOSITE                    (ags_composite_get_type())
#define AGS_COMPOSITE(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_COMPOSITE, AgsComposite))
#define AGS_COMPOSITE_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_COMPOSITE, AgsCompositeInterface))
#define AGS_IS_COMPOSITE(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_COMPOSITE))
#define AGS_IS_COMPOSITE_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_COMPOSITE))
#define AGS_COMPOSITE_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_COMPOSITE, AgsCompositeInterface))

typedef void AgsComposite;
typedef struct _AgsCompositeInterface AgsCompositeInterface;

struct _AgsCompositeInterface
{
  GTypeInterface interface;

  void (*set_presets)(AgsComposite *composite,
		      guint audio_channels,
		      guint samplerate,
		      guint buffer_size,
		      guint format);
  void (*get_presets)(AgsComposite *composite,
		      guint *audio_channels,
		      guint *samplerate,
		      guint *buffer_size,
		      guint *format);
  
  void (*presets_changed)(AgsComposite *composite);

  void (*add)(AgsComposite *composite,
	      GObject *child);
  void (*remove)(AgsComposite *composite,
		 GObject *child);

  GObject* (*get_parent)(AgsComposite *composite);
  GList* (*get_children)(AgsComposite *composite,
			 GType child_type);
};

GType ags_composite_get_type();

void ags_composite_set_presets(AgsComposite *composite,
			       guint audio_channels,
			       guint samplerate,
			       guint buffer_size,
			       guint format);
void ags_composite_get_presets(AgsComposite *composite,
			       guint *audio_channels,
			       guint *samplerate,
			       guint *buffer_size,
			       guint *format);

void ags_composite_presets_changed(AgsComposite *composite);

void ags_composite_add(AgsComposite *composite,
		       GObject *child);
void ags_composite_remove(AgsComposite *composite,
			  GObject *child);

GObject* ags_composite_get_parent(AgsComposite *composite);
GList* ags_composite_get_children(AgsComposite *composite,
				  GType child_type);

#endif /*__AGS_COMPOSITE_H__*/

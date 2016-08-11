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

#include <ags/object/ags_composite.h>

void ags_composite_class_init(AgsCompositeInterface *interface);

/**
 * SECTION:ags_composite
 * @short_description: composition interface
 * @title: AgsComposite
 * @section_id:
 * @include: ags/object/ags_composite.h
 *
 * The #AgsComposite interface gives you a unique access to object tree.
 */

enum {
  PRESETS_CHANGED,
  ADD,
  REMOVE,
  LAST_SIGNAL,
};

static guint composite_signals[LAST_SIGNAL];

GType
ags_composite_get_type()
{
  static GType ags_type_composite = 0;

  if(!ags_type_composite){
    ags_type_composite = g_type_register_static_simple(G_TYPE_INTERFACE,
						       "AgsComposite\0",
						       sizeof(AgsCompositeInterface),
						       (GClassInitFunc) ags_composite_class_init,
						       0, NULL, 0);
  }

  return(ags_type_composite);
}

void
ags_composite_class_init(AgsCompositeInterface *interface)
{
  /**
   * AgsComposite::preset-changed:
   * @composite: the #AgsComposite
   *
   * The ::preset-changed signal is emitted as preset was modified.
   */
  composite_signals[PRESETS_CHANGED] =
    g_signal_new("preset-changed\0",
		 G_TYPE_FROM_INTERFACE(interface),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsCompositeInterface, presets_changed),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsComposite::add:
   * @composite: the #AgsComposite
   * @child: the child #GObject
   *
   * The ::add signal notifies about added child.
   */
  composite_signals[ADD] =
    g_signal_new("add\0",
		 G_TYPE_FROM_INTERFACE(interface),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsCompositeInterface, add),
		 NULL, NULL,
		 g_cclosure_user_marshal_OBJECT__OBJECT,
		 G_TYPE_OBJECT, 1,
		 G_TYPE_OBJECT);

  /**
   * AgsComposite::remove:
   * @composite: the #AgsComposite
   * @child: the child #GObject
   *
   * The ::remove signal notifies about removeed child.
   */
  composite_signals[REMOVE] =
    g_signal_new("remove\0",
		 G_TYPE_FROM_INTERFACE(interface),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsCompositeInterface, remove),
		 NULL, NULL,
		 g_cclosure_user_marshal_OBJECT__OBJECT,
		 G_TYPE_OBJECT, 1,
		 G_TYPE_OBJECT);
}

/**
 * ags_composite_set_presets:
 * @composite: the #AgsComposite
 * @channels: the audio channels
 * @samplerate: the samplerate
 * @buffer_size: the buffer size
 * @format: the format
 *
 * Set presets. 
 *
 * Since: 0.7.45
 */
void
ags_composite_set_presets(AgsComposite *composite,
			  guint audio_channels,
			  guint samplerate,
			  guint buffer_size,
			  guint format)
{
  AgsCompositeInterface *composite_interface;

  g_return_if_fail(AGS_IS_COMPOSITE(composite));
  composite_interface = AGS_COMPOSITE_GET_INTERFACE(composite);
  g_return_if_fail(composite_interface->set_presets);
  composite_interface->set_presets(composite,
				   audio_channels,
				   samplerate,
				   buffer_size,
				   format);
}

/**
 * ags_composite_get_presets:
 * @composite: the #AgsComposite
 * @audio_channels: the audio channels
 * @samplerate: the samplerate
 * @buffer_size: the buffer size
 * @format: the format
 *
 * Get presets. 
 *
 * Since: 0.7.45
 */
void
ags_composite_get_presets(AgsComposite *composite,
			  guint *audio_channels,
			  guint *samplerate,
			  guint *buffer_size,
			  guint *format)
{
  AgsCompositeInterface *composite_interface;

  g_return_if_fail(AGS_IS_COMPOSITE(composite));
  composite_interface = AGS_COMPOSITE_GET_INTERFACE(composite);
  g_return_if_fail(composite_interface->get_presets);
  composite_interface->get_presets(composite,
				   audio_channels,
				   samplerate,
				   buffer_size,
				   format);
}

/**
 * ags_composite_presets_changed:
 * @composite: the #AgsComposite
 *
 * Emits the ::presets-changed signal.
 *
 * Since: 0.7.45
 */
void
ags_composite_presets_changed(AgsComposite *composite)
{
  g_signal_emit(composite,
		composite_signals[PRESETS_CHANGED],
		0);
}

/**
 * ags_composite_add:
 * @composite: the #AgsComposite
 * @child: the child
 *
 * Emits the ::add signal intended to add a #GObject to tree.
 *
 * Since: 0.7.45
 */
void
ags_composite_add(AgsComposite *composite,
		  GObject *child)
{
  g_signal_emit(composite,
		composite_signals[ADD],
		0,
		child);
}

/**
 * ags_composite_remove:
 * @composite: the #AgsComposite
 * @child: the child
 *
 * Emits the ::remove signal intended to add a #GObject to tree.
 *
 * Since: 0.7.45
 */
void
ags_composite_remove(AgsComposite *composite,
		     GObject *child)
{
  g_signal_emit(composite,
		composite_signals[REMOVE],
		0,
		child);
}

/**
 * ags_composite_get_parent:
 * @composite: the #AgsComposite
 *
 * Get parent #GObject. 
 *
 * Returns: the parent #GObject
 *
 * Since: 0.7.45
 */
GObject*
ags_composite_get_parent(AgsComposite *composite)
{
  AgsCompositeInterface *composite_interface;

  g_return_val_if_fail(AGS_IS_COMPOSITE(composite), NULL);
  composite_interface = AGS_COMPOSITE_GET_INTERFACE(composite);
  g_return_val_if_fail(composite_interface->get_parent, NULL);

  return(composite_interface->get_parent(composite));  
}

/**
 * ags_composite_get_children:
 * @composite: the #AgsComposite
 * @child_type: the child type to get
 *
 * Get children of @child_type specified #GType.
 *
 * Returns: the matching type of children as #GList
 * 
 * Since: 0.7.45
 */
GList*
ags_composite_get_children(AgsComposite *composite,
			   GType child_type)
{
  AgsCompositeInterface *composite_interface;

  g_return_val_if_fail(AGS_IS_COMPOSITE(composite), NULL);
  composite_interface = AGS_COMPOSITE_GET_INTERFACE(composite);
  g_return_val_if_fail(composite_interface->get_children, NULL);

  return(composite_interface->get_children(composite,
					   child_type));
}

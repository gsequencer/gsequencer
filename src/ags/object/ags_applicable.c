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

#include <ags/object/ags_applicable.h>

#include <stdio.h>

void ags_applicable_base_init(AgsApplicableInterface *interface);

/**
 * SECTION:ags_applicable
 * @short_description: unique access to dialogs
 * @title: AgsApplicable
 * @section_id:
 * @include: ags/object/ags_applicable.h
 *
 * The #AgsApplicable interface gives you a unique access to dialogs.
 */

GType
ags_applicable_get_type()
{
  static GType ags_type_applicable = 0;

  if(!ags_type_applicable){
    static const GTypeInfo ags_applicable_info = {
      sizeof(AgsApplicableInterface),
      (GBaseInitFunc) ags_applicable_base_init,
      NULL, /* base_finalize */
    };

    ags_type_applicable = g_type_register_static(G_TYPE_INTERFACE,
						 "AgsApplicable\0", &ags_applicable_info,
						 0);
  }

  return(ags_type_applicable);
}

void
ags_applicable_base_init(AgsApplicableInterface *interface)
{
  /* empty */
}

/**
 * ags_applicable_set_update:
 * @applicable: an #AgsApplicable
 * @update: if %TRUE do ::reset after ::update
 *
 * Update behaviour.
 *
 * Since: 0.3
 */
void
ags_applicable_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsApplicableInterface *applicable_interface;

  g_return_if_fail(AGS_IS_APPLICABLE(applicable));
  applicable_interface = AGS_APPLICABLE_GET_INTERFACE(applicable);
  g_return_if_fail(applicable_interface->set_update);
  applicable_interface->set_update(applicable, update);
}

/**
 * ags_applicable_apply:
 * @applicable: an #AgsApplicable
 *
 * Apply all changes done so far.
 *
 * Since: 0.3
 */
void
ags_applicable_apply(AgsApplicable *applicable)
{
  AgsApplicableInterface *applicable_interface;

  g_return_if_fail(AGS_IS_APPLICABLE(applicable));
  applicable_interface = AGS_APPLICABLE_GET_INTERFACE(applicable);
  g_return_if_fail(applicable_interface->apply);
  applicable_interface->apply(applicable);
}

/**
 * ags_applicable_reset:
 * @applicable: an #AgsApplicable
 *
 * Reset all changes within the user interface.
 *
 * Since: 0.3
 */
void
ags_applicable_reset(AgsApplicable *applicable)
{
  AgsApplicableInterface *applicable_interface;

  g_return_if_fail(AGS_IS_APPLICABLE(applicable));
  applicable_interface = AGS_APPLICABLE_GET_INTERFACE(applicable);
  g_return_if_fail(applicable_interface->reset);
  applicable_interface->reset(applicable);
}

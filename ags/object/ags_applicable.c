/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

void ags_applicable_base_init(AgsApplicableInterface *ginterface);

/**
 * SECTION:ags_applicable
 * @short_description: unique access to applicable objects
 * @title: AgsApplicable
 * @section_id: AgsApplicable
 * @include: ags/object/ags_applicable.h
 *
 * The #AgsApplicable interface gives you a unique access to applicable objects.
 */

GType
ags_applicable_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_applicable = 0;

    static const GTypeInfo ags_applicable_info = {
      sizeof(AgsApplicableInterface),
      (GBaseInitFunc) ags_applicable_base_init,
      NULL, /* base_finalize */
    };

    ags_type_applicable = g_type_register_static(G_TYPE_INTERFACE,
						 "AgsApplicable", &ags_applicable_info,
						 0);

    g_once_init_leave(&g_define_type_id__static, ags_type_applicable);
  }

  return(g_define_type_id__static);
}

void
ags_applicable_base_init(AgsApplicableInterface *ginterface)
{
  /* empty */
}

/**
 * ags_applicable_set_update:
 * @applicable: an #AgsApplicable
 * @update: if %TRUE do ::reset after ::apply
 *
 * Update behaviour.
 *
 * Since: 3.0.0
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
 * Since: 3.0.0
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
 * Since: 3.0.0
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

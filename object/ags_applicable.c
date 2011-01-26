/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
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

#include <ags/object/ags_applicable.h>

#include <stdio.h>

void ags_applicable_base_init(AgsApplicableInterface *interface);

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

void
ags_applicable_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsApplicableInterface *applicable_interface;

  g_return_if_fail(AGS_IS_APPLICABLE(applicable));
  applicable_interface = AGS_APPLICABLE_GET_INTERFACE(applicable);
  g_return_if_fail(applicable_interface->set_update);
  applicable_interface->set_update(applicable, update);
}

void
ags_applicable_apply(AgsApplicable *applicable)
{
  AgsApplicableInterface *applicable_interface;

  g_return_if_fail(AGS_IS_APPLICABLE(applicable));
  applicable_interface = AGS_APPLICABLE_GET_INTERFACE(applicable);
  g_return_if_fail(applicable_interface->apply);
  applicable_interface->apply(applicable);
}

void
ags_applicable_reset(AgsApplicable *applicable)
{
  AgsApplicableInterface *applicable_interface;

  g_return_if_fail(AGS_IS_APPLICABLE(applicable));
  applicable_interface = AGS_APPLICABLE_GET_INTERFACE(applicable);
  g_return_if_fail(applicable_interface->reset);
  applicable_interface->reset(applicable);
}

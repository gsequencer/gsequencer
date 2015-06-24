/* This file is part of GSequencer.
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
/* This file is part of GSequencer.
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
#include <ags/object/ags_persistable.h>

#include <stdio.h>

void ags_persistable_base_init(AgsPersistableInterface *interface);

GType
ags_persistable_get_type()
{
  static GType ags_type_persistable = 0;

  if(!ags_type_persistable){
    static const GTypeInfo ags_persistable_info = {
      sizeof(AgsPersistableInterface),
      (GBaseInitFunc) ags_persistable_base_init,
      NULL, /* base_finalize */
    };

    ags_type_persistable = g_type_register_static(G_TYPE_INTERFACE,
						 "AgsPersistable\0", &ags_persistable_info,
						 0);
  }

  return(ags_type_persistable);
}

void
ags_persistable_base_init(AgsPersistableInterface *interface)
{
  /* empty */
}

AgsPersistable*
ags_persistable_read(AgsPersistable *persistable, xmlDocPtr doc, xmlNodePtr node)
{
  AgsPersistableInterface *persistable_interface;

  g_return_val_if_fail(AGS_IS_PERSISTABLE(persistable), NULL);
  persistable_interface = AGS_PERSISTABLE_GET_INTERFACE(persistable);
  g_return_val_if_fail(persistable_interface->read, NULL);
  return(persistable_interface->read(persistable, doc, node));
}

void
ags_persistable_read_link(AgsPersistable *persistable, xmlDocPtr doc, xmlNodePtr node)
{
  AgsPersistableInterface *persistable_interface;

  g_return_if_fail(AGS_IS_PERSISTABLE(persistable));
  persistable_interface = AGS_PERSISTABLE_GET_INTERFACE(persistable);
  g_return_if_fail(persistable_interface->read_link);
  persistable_interface->read_link(persistable, doc, node);
}

xmlNodePtr
ags_persistable_write(AgsPersistable *persistable, xmlDocPtr doc)
{
  AgsPersistableInterface *persistable_interface;

  g_return_val_if_fail(AGS_IS_PERSISTABLE(persistable), NULL);
  persistable_interface = AGS_PERSISTABLE_GET_INTERFACE(persistable);
  g_return_val_if_fail(persistable_interface->write, NULL);
  return(persistable_interface->write(persistable, doc));
}

void
ags_persistable_write_link(AgsPersistable *persistable, xmlDocPtr doc, xmlNodePtr node)
{
  AgsPersistableInterface *persistable_interface;

  g_return_if_fail(AGS_IS_PERSISTABLE(persistable));
  persistable_interface = AGS_PERSISTABLE_GET_INTERFACE(persistable);
  g_return_if_fail(persistable_interface->write_link);
  persistable_interface->write_link(persistable, doc, node);
}

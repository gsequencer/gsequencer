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

#ifndef __AGS_PERSISTABLE_H__
#define __AGS_PERSISTABLE_H__

#include <glib-object.h>

#include <libxml/tree.h>

#define AGS_TYPE_PERSISTABLE                    (ags_persistable_get_type())
#define AGS_PERSISTABLE(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PERSISTABLE, AgsPersistable))
#define AGS_PERSISTABLE_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_PERSISTABLE, AgsPersistableInterface))
#define AGS_IS_PERSISTABLE(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_PERSISTABLE))
#define AGS_IS_PERSISTABLE_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_PERSISTABLE))
#define AGS_PERSISTABLE_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_PERSISTABLE, AgsPersistableInterface))

typedef void AgsPersistable;
typedef struct _AgsPersistableInterface AgsPersistableInterface;

struct _AgsPersistableInterface
{
  GTypeInterface interface;
  
  AgsPersistable* (*read)(AgsPersistable *persistable, xmlDocPtr doc, xmlNodePtr node);
  void (*read_link)(AgsPersistable *persistable, xmlDocPtr doc, xmlNodePtr node);

  xmlNodePtr (*write)(AgsPersistable *persistable, xmlDocPtr doc);
  void (*write_link)(AgsPersistable *persistable, xmlDocPtr doc, xmlNodePtr node);
};

GType ags_persistable_get_type();

AgsPersistable* ags_persistable_read(AgsPersistable *persistable, xmlDocPtr doc, xmlNodePtr node);
void ags_persistable_read_link(AgsPersistable *persistable, xmlDocPtr doc, xmlNodePtr node);

xmlNodePtr ags_persistable_write(AgsPersistable *persistable, xmlDocPtr doc);
void ags_persistable_write_link(AgsPersistable *persistable, xmlDocPtr doc, xmlNodePtr node);

#endif /*__AGS_PERSISTABLE_H__*/

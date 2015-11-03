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

#ifndef __AGS_DISTRIBUTED_MANAGER_H__
#define __AGS_DISTRIBUTED_MANAGER_H__

#include <glib-object.h>

#define AGS_TYPE_DISTRIBUTED_MANAGER                    (ags_distributed_manager_get_type())
#define AGS_DISTRIBUTED_MANAGER(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_DISTRIBUTED_MANAGER, AgsDistributedManager))
#define AGS_DISTRIBUTED_MANAGER_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_DISTRIBUTED_MANAGER, AgsDistributedManagerInterface))
#define AGS_IS_DISTRIBUTED_MANAGER(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_DISTRIBUTED_MANAGER))
#define AGS_IS_DISTRIBUTED_MANAGER_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_DISTRIBUTED_MANAGER))
#define AGS_DISTRIBUTED_MANAGER_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_DISTRIBUTED_MANAGER, AgsDistributedManagerInterface))

typedef void AgsDistributedManager;
typedef struct _AgsDistributedManagerInterface AgsDistributedManagerInterface;

struct _AgsDistributedManagerInterface
{
  GTypeInterface interface;

  void (*set_url)(AgsDistributedManager *distributed_manager,
		  gchar *url);
  gchar* (*get_url)(AgsDistributedManager *distributed_manager);

  void (*set_ports)(AgsDistributedManager *distributed_manager,
		    gchar **ports);
  gchar** (*get_ports)(AgsDistributedManager *distributed_manager);

  void (*set_soundcard)(AgsDistributedManager *distributed_manager,
			gchar *uri,
			GObject *soundcard);
  GObject* (*get_soundcard)(AgsDistributedManager *distributed_manager,
			    gchar *uri);

  void (*set_sequencer)(AgsDistributedManager *distributed_manager,
			gchar *uri,
			GObject *sequencer);
  GObject* (*get_sequencer)(AgsDistributedManager *distributed_manager,
			    gchar *uri);

  GObject* (*register_soundcard)(AgsDistributedManager *distributed_manager);
  void (*unregister_soundcard)(AgsDistributedManager *distributed_manager,
			       GObject *soundcard);

  GObject* (*register_sequencer)(AgsDistributedManager *distributed_manager);
  void (*unregister_sequencer)(AgsDistributedManager *distributed_manager,
			       GObject *sequencer);
};

GType ags_distributed_manager_get_type();

void ags_distributed_manager_set_url(AgsDistributedManager *distributed_manager,
				     gchar *url);
gchar* ags_distributed_manager_get_url(AgsDistributedManager *distributed_manager);

void ags_distributed_manager_set_ports(AgsDistributedManager *distributed_manager,
				       gchar **ports);
gchar** ags_distributed_manager_get_ports(AgsDistributedManager *distributed_manager);

void ags_distributed_manager_set_soundcard(AgsDistributedManager *distributed_manager,
					   gchar *uri,
					   GObject *soundcard);
GObject* ags_distributed_manager_get_soundcard(AgsDistributedManager *distributed_manager,
					       gchar *uri);

void ags_distributed_manager_set_sequencer(AgsDistributedManager *distributed_manager,
					   gchar *uri,
					   GObject *sequencer);
GObject* ags_distributed_manager_get_sequencer(AgsDistributedManager *distributed_manager,
					       gchar *uri);

GObject* ags_distributed_manager_register_soundcard(AgsDistributedManager *distributed_manager,
						    gboolean is_output);
void ags_distributed_manager_unregister_soundcard(AgsDistributedManager *distributed_manager,
						  GObject *soundcard);

GObject* ags_distributed_manager_register_sequencer(AgsDistributedManager *distributed_manager,
						    gboolean is_output);
void ags_distributed_manager_unregister_sequencer(AgsDistributedManager *distributed_manager,
						  GObject *sequencer);

#endif /*__AGS_DISTRIBUTED_MANAGER_H__*/

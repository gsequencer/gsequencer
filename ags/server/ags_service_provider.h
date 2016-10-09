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

#ifndef __AGS_SERVICE_PROVIDER_H__
#define __AGS_SERVICE_PROVIDER_H__

#include <glib.h>
#include <glib-object.h>

#define AGS_TYPE_SERVICE_PROVIDER                    (ags_service_provider_get_type())
#define AGS_SERVICE_PROVIDER(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SERVICE_PROVIDER, AgsServiceProvider))
#define AGS_SERVICE_PROVIDER_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_SERVICE_PROVIDER, AgsServiceProviderInterface))
#define AGS_IS_SERVICE_PROVIDER(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SERVICE_PROVIDER))
#define AGS_IS_SERVICE_PROVIDER_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_SERVICE_PROVIDER))
#define AGS_SERVICE_PROVIDER_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_SERVICE_PROVIDER, AgsServiceProviderInterface))

typedef void AgsServiceProvider;
typedef struct _AgsServiceProviderInterface AgsServiceProviderInterface;

struct _AgsServiceProviderInterface
{
  GTypeInterface interface;
  
  void (*set_registry)(AgsServiceProvider *service_provider,
		       GObject *registry);
  GObject* (*get_registry)(AgsServiceProvider *service_provider);
  
  void (*set_server)(AgsServiceProvider *service_provider,
		     GList *server);
  GList* (*get_server)(AgsServiceProvider *service_provider);
};

GType ags_service_provider_get_type();

void ags_service_provider_set_registry(AgsServiceProvider *service_provider,
				       GObject *registry);
GObject* ags_service_provider_get_registry(AgsServiceProvider *service_provider);

void ags_service_provider_set_server(AgsServiceProvider *service_provider,
				     GList *server);
GList* ags_service_provider_get_server(AgsServiceProvider *service_provider);

#endif /*__AGS_SERVICE_PROVIDER_H__*/

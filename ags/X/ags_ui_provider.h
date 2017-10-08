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

#ifndef __AGS_UI_PROVIDER_H__
#define __AGS_UI_PROVIDER_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#define AGS_TYPE_UI_PROVIDER                    (ags_ui_provider_get_type())
#define AGS_UI_PROVIDER(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_UI_PROVIDER, AgsUiProvider))
#define AGS_UI_PROVIDER_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_UI_PROVIDER, AgsUiProviderInterface))
#define AGS_IS_UI_PROVIDER(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_UI_PROVIDER))
#define AGS_IS_UI_PROVIDER_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_UI_PROVIDER))
#define AGS_UI_PROVIDER_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_UI_PROVIDER, AgsUiProviderInterface))

typedef struct _AgsUiProvider AgsUiProvider;
typedef struct _AgsUiProviderInterface AgsUiProviderInterface;

struct _AgsUiProviderInterface
{
  GTypeInterface ginterface;
  
  GtkWidget* (*get_window)(AgsUiProvider *ui_provider);
  void (*set_window)(AgsUiProvider *ui_provider,
		     GtkWidget *window);
};

GType ags_ui_provider_get_type();

GtkWidget* ags_ui_provider_get_window(AgsUiProvider *ui_provider);
void ags_ui_provider_set_window(AgsUiProvider *ui_provider,
				GtkWidget *window);

#endif /*__AGS_UI_PROVIDER_H__*/

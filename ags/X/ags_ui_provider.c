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

#include <ags/X/ags_ui_provider.h>

void ags_ui_provider_class_init(AgsUiProviderInterface *interface);

/**
 * SECTION:ags_ui_provider
 * @short_description: provide accoustics
 * @title: AgsUiProvider
 * @section_id: AgsUiProvider
 * @include: ags/X/ags_ui_provider.h
 *
 * The #AgsUiProvider provides you the window capabilities.
 */

GType
ags_ui_provider_get_type()
{
  static GType ags_type_ui_provider = 0;

  if(!ags_type_ui_provider){
    ags_type_ui_provider = g_type_register_static_simple(G_TYPE_INTERFACE,
							 "AgsUiProvider",
							 sizeof(AgsUiProviderInterface),
							 (GClassInitFunc) ags_ui_provider_class_init,
							 0, NULL, 0);
  }

  return(ags_type_ui_provider);
}

void
ags_ui_provider_class_init(AgsUiProviderInterface *interface)
{
  /* empty */
}

/**
 * ags_ui_provider_get_window:
 * @ui_provider: the #AgsUiProvider
 * 
 * Get window.
 * 
 * Returns: the #AgsWindow
 * 
 * Since: 1.0.0
 */
GtkWidget*
ags_ui_provider_get_window(AgsUiProvider *ui_provider)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_val_if_fail(AGS_IS_UI_PROVIDER(ui_provider), NULL);
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_val_if_fail(ui_provider_interface->get_window, NULL);

  return(ui_provider_interface->get_window(ui_provider));
}

/**
 * ags_ui_provider_get_window:
 * @ui_provider: the #AgsUiProvider
 * @window: the #AgsWindow
 * 
 * Set window.
 * 
 * Since: 1.0.0
 */
void
ags_ui_provider_set_window(AgsUiProvider *ui_provider,
			   GtkWidget *window)
{
  AgsUiProviderInterface *ui_provider_interface;

  g_return_if_fail(AGS_IS_UI_PROVIDER(ui_provider));
  ui_provider_interface = AGS_UI_PROVIDER_GET_INTERFACE(ui_provider);
  g_return_if_fail(ui_provider_interface->set_window);

  ui_provider_interface->set_window(ui_provider,
				    window);
}

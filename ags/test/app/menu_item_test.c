/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2025 Joël Krähemann
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

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

void
app_action_util_add_ladspa_bridge(gchar *filename, gchar *effect)
{
  g_message("action - add_ladspa_bridge [%s,%s]", filename, effect);
}

void
add_ladspa_bridge_callback(GAction *action, GVariant *parameter,
			   GApplication *app)
{
  GVariantIter *iter;
  
  gchar *filename;
  gchar *effect;
  
  iter = NULL;

  filename = NULL;
  effect = NULL;
  
  g_variant_get(parameter, "as", &iter);

  g_variant_iter_loop(iter, "s", &filename);
  g_variant_iter_loop(iter, "s", &effect);
  
  app_action_util_add_ladspa_bridge(filename, effect);
}

int
main(int argc, char **argv)
{
  GApplication *app;
  
  GSimpleAction *add_ladspa_bridge_action;

  GMenu *menu;
  GMenuItem *item;

  GVariantBuilder *builder;

  gchar *filename, *effect;

  app = g_application_new("menu-item-test",
			  (GApplicationFlags) (G_APPLICATION_HANDLES_OPEN |
					       G_APPLICATION_NON_UNIQUE));
  
  add_ladspa_bridge_action = g_simple_action_new("add_ladspa_bridge",
						 G_VARIANT_TYPE("as"));
  g_signal_connect(add_ladspa_bridge_action, "activate",
		   G_CALLBACK(add_ladspa_bridge_callback), app);
  g_action_map_add_action(G_ACTION_MAP(app),
			  G_ACTION(add_ladspa_bridge_action));

  menu = g_menu_new();
  item = g_menu_item_new("LADSPA",
			 NULL);

  filename = NULL;
  effect = NULL;

  //TODO:JK: filename and effect
  
  item = g_menu_item_new(effect,
			 "app.add_ladspa_bridge");

  builder = g_variant_builder_new(G_VARIANT_TYPE("as"));
    
  g_variant_builder_add(builder, "s", filename);
  g_variant_builder_add(builder, "s", effect);

  g_menu_item_set_attribute_value(item,
				  "target",
				  g_variant_new("as", builder));
    
  g_menu_append_item(ladspa_menu,
		     item);

  g_variant_builder_unref(builder);

  return(0);
}

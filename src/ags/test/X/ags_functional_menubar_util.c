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

#include <src/ags/test/X/ags_functional_menubar_util.h>

#include <string.h>

gboolean
ags_functional_menu_bar_util_menu_activate_path(GtkMenuBar *menu_bar,
						gchar *path)
{
  GtkMenu *menu;
  
  GList *list, *list_start;

  gchar *str;
  gboolean has_more;
  gboolean success;  

  if(menu_bar == NULL){
    return(FALSE);
  }
  
  list_start =
    list = gtk_container_get_children(menu_bar);

  success = FALSE;
  
  while(!success &&
	list != NULL){
    if(GTK_IS_MENU_ITEM(list->data)){
      if(index(path + 1,
	       '/') != NULL){
	str = g_strndup(path + 1,
			index(path,
			    '/'));
	has_more = TRUE;
      }else{
	g_strndup(path + 1,
		  strlen(path) - 1);

	has_more = FALSE;
      }

      if(g_ascii_strcasecmp(gtk_menu_item_get_label(GTK_MENU_ITEM(list->data)),
			    str)){
	list = list->next;
	g_free(str);
	
	continue;
      }
      
      menu = gtk_menu_item_get_submenu(GTK_MENU_ITEM(list->data));
      gtk_menu_item_activate(GTK_MENU_ITEM(list->data));
      
      if(menu != NULL){
	success = ags_functional_menu_bar_util_activate_menu_path(menu);
      }else{
	if(!has_more){
	  success = TRUE;
	}
      }

      g_free(str);	    
    }

    list = list->next;
  }
  
  g_list_free(list_start);

  return(success);
}

gboolean
ags_functional_menu_bar_util_activate_menu_path(GtkMenu *menu,
						gchar *path)
{
  gboolean success;
  
  auto gboolean ags_functional_menu_bar_util_activate_menu_path_recursive(GtkMenu *menu);

  gboolean ags_functional_menu_bar_util_activate_menu_path_recursive(GtkMenu *menu){
    GList *list, *list_start;

    gchar *str;
    gboolean success;

    list_start =
      list = gtk_container_get_children(menu_bar);

    success = FALSE;

    while(!success &&
	  list != NULL){
      if(GTK_IS_MENU_ITEM(list->data)){
	if(index(path + 1,
		 '/') != NULL){
	  str = g_strndup(path + 1,
			  index(path,
				'/'));
	  has_more = TRUE;
	}else{
	  g_strndup(path + 1,
		    strlen(path) - 1);

	  has_more = FALSE;
	}

	if(g_ascii_strcasecmp(gtk_menu_item_get_label(GTK_MENU_ITEM(list->data)),
			      str)){
	  list = list->next;
	  g_free(str);
	
	  continue;
	}
      
	menu = gtk_menu_item_get_submenu(GTK_MENU_ITEM(list->data));
	gtk_menu_item_activate(GTK_MENU_ITEM(list->data));
      
	if(menu != NULL){
	  success = ags_functional_menu_bar_util_activate_path_recursive(menu);
	}else{
	  if(!has_more){
	    success = TRUE;
	  }
	}

	g_free(str);	    
      }
      
      list = list->next;
    }

    g_list_free(list_start);

    return(success);
  }

  if(menu == NULL){
    return(FALSE);
  }
  
  success = ags_functional_menu_bar_util_activate_menu_path_recursive(menu);

  return(success);
}


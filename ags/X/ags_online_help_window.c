/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/X/ags_online_help_window.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>

#include <stdlib.h>
#include <string.h>

#include <ags/i18n.h>

void ags_online_help_window_class_init(AgsOnlineHelpWindowClass *online_help_window);
void ags_online_help_window_init(AgsOnlineHelpWindow *online_help_window);

static gpointer ags_online_help_window_parent_class = NULL;

GType
ags_online_help_window_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_online_help_window = 0;

    static const GTypeInfo ags_online_help_window_info = {
      sizeof (AgsOnlineHelpWindowClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_online_help_window_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOnlineHelpWindow),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_online_help_window_init,
    };

    ags_type_online_help_window = g_type_register_static(GTK_TYPE_WINDOW,
							 "AgsOnlineHelpWindow", &ags_online_help_window_info,
							 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_online_help_window);
  }

  return g_define_type_id__volatile;
}

void
ags_online_help_window_class_init(AgsOnlineHelpWindowClass *online_help_window)
{
  GtkWidgetClass *widget;
  
  ags_online_help_window_parent_class = g_type_class_peek_parent(online_help_window);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) online_help_window;
}

void
ags_online_help_window_init(AgsOnlineHelpWindow *online_help_window)
{
  GtkVBox *vbox;
  GtkHBox *navigation_hbox;
  GtkLabel *label;
  
  vbox = (GtkVBox *) gtk_vbox_new(FALSE,
				  0);
  gtk_container_add((GtkContainer *) online_help_window,
		    (GtkWidget *) vbox);

  /* navigation */
  navigation_hbox = (GtkHBox *) gtk_hbox_new(FALSE,
					     0);
  gtk_box_pack_start((GtkBox *) vbox,
		     (GtkWidget *) navigation_hbox,
		     FALSE, FALSE,
		     0);

  online_help_window->home = (GtkButton *) gtk_button_new_from_icon_name("go-home",
									 GTK_ICON_SIZE_BUTTON);
  gtk_box_pack_start((GtkBox *) navigation_hbox,
		     (GtkWidget *) online_help_window->home,
		     FALSE, FALSE,
		     0);  
  
  online_help_window->prev = (GtkButton *) gtk_button_new_from_icon_name("go-previous",
									 GTK_ICON_SIZE_BUTTON);
  gtk_box_pack_start((GtkBox *) navigation_hbox,
		     (GtkWidget *) online_help_window->prev,
		     FALSE, FALSE,
		     0);  
  
  online_help_window->next = (GtkButton *) gtk_button_new_from_icon_name("go-next",
									 GTK_ICON_SIZE_BUTTON);
  gtk_box_pack_start((GtkBox *) navigation_hbox,
		     (GtkWidget *) online_help_window->next,
		     FALSE, FALSE,
		     0);  

  label = (GtkLabel *) gtk_label_new(i18n("Place"));
  gtk_box_pack_start((GtkBox *) navigation_hbox,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);  
  
  online_help_window->location = (GtkEntry *) gtk_entry_new();
  gtk_box_pack_start((GtkBox *) navigation_hbox,
		     (GtkWidget *) online_help_window->location,
		     FALSE, FALSE,
		     0);  
}

/**
 * ags_online_help_window_new:
 *
 * Creates an #AgsOnlineHelpWindow
 *
 * Returns: a new #AgsOnlineHelpWindow
 *
 * Since: 3.5.0
 */
AgsOnlineHelpWindow*
ags_online_help_window_new()
{
  AgsOnlineHelpWindow *online_help_window;

  online_help_window = (AgsOnlineHelpWindow *) g_object_new(AGS_TYPE_ONLINE_HELP_WINDOW,
							    NULL);

  return(online_help_window);
}

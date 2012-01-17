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

#include <ags/X/ags_file_selection.h>
#include <ags/X/ags_file_selection_callbacks.h>

#include <ags/object/ags_connectable.h>

#include <ags/X/ags_window.h>

void ags_file_selection_class_init(AgsFileSelectionClass *file_selection);
void ags_file_selection_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_file_selection_init(AgsFileSelection *file_selection);
void ags_file_selection_connect(AgsConnectable *connectable);
void ags_file_selection_disconnect(AgsConnectable *connectable);
static void ags_file_selection_finalize(GObject *gobject);
void ags_file_selection_show(GtkWidget *widget);

GtkMenu* ags_file_selection_popup_new(AgsFileSelection *file_selection);

extern void ags_file_read_file_selection(AgsFile *file, AgsFileSelection *file_selection);
extern void ags_file_write_file_selection(AgsFile *file, AgsFileSelection *file_selection);

static gpointer ags_file_selection_parent_class = NULL;

GType
ags_file_selection_get_type(void)
{
  static GType ags_type_file_selection = 0;

  if(!ags_type_file_selection){
    static const GTypeInfo ags_file_selection_info = {
      sizeof (AgsFileSelectionClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_file_selection_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsFileSelection),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_file_selection_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_file_selection_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_file_selection = g_type_register_static(GTK_TYPE_VBOX,
						     "AgsFileSelection\0", &ags_file_selection_info,
						     0);
    
    g_type_add_interface_static(ags_type_file_selection,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_file_selection);
}

void
ags_file_selection_class_init(AgsFileSelectionClass *file_selection)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  ags_file_selection_parent_class = g_type_class_peek_parent(file_selection);

  /* GtkObjectClass */
  gobject = (GObjectClass *) file_selection;

  gobject->finalize = ags_file_selection_finalize;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) file_selection;

  widget->show = ags_file_selection_show;
}

void
ags_file_selection_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_file_selection_connect;
  connectable->disconnect = ags_file_selection_disconnect;
}

void
ags_file_selection_init(AgsFileSelection *file_selection)
{

}

void
ags_file_selection_connect(AgsConnectable *connectable)
{
  AgsFileSelection *file_selection;

  /* AgsFileSelection */
  file_selection = AGS_FILE_SELECTION(connectable);

}

void
ags_file_selection_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

static void
ags_file_selection_finalize(GObject *gobject)
{
  AgsFileSelection *file_selection;

  file_selection = (AgsFileSelection *) gobject;

  G_OBJECT_CLASS(ags_file_selection_parent_class)->finalize(gobject);
}

AgsFileSelection*
ags_file_selection_new(GObject *devout)
{
  AgsFileSelection *file_selection;
  GValue value;

  file_selection = (AgsFileSelection *) g_object_new(AGS_TYPE_FILE_SELECTION,
						     NULL);

  return(file_selection);
}

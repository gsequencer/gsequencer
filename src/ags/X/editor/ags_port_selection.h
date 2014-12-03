/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
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

#ifndef __AGS_PORT_SELECTION_H__
#define __AGS_PORT_SELECTION_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define AGS_TYPE_PORT_SELECTION                (ags_port_selection_get_type())
#define AGS_PORT_SELECTION(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PORT_SELECTION, AgsPortSelection))
#define AGS_PORT_SELECTION_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PORT_SELECTION, AgsPortSelectionClass))
#define AGS_IS_PORT_SELECTION(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PORT_SELECTION))
#define AGS_IS_PORT_SELECTION_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PORT_SELECTION))
#define AGS_PORT_SELECTION_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_PORT_SELECTION, AgsPortSelectionClass))

#define AGS_PORT_SELECTION_DATA_CHANNEL "ags-channel\0"
#define AGS_PORT_SELECTION_DATA_PORT "ags-port\0"

typedef struct _AgsPortSelection AgsPortSelection;
typedef struct _AgsPortSelectionClass AgsPortSelectionClass;

struct _AgsPortSelection
{
  GtkMenuToolButton menu_tool_button;
};

struct _AgsPortSelectionClass
{
  GtkMenuToolButtonClass menu_tool_button;

  void (*add_port)(AgsPortSelection *port_selection);
};

GType ags_port_selection_get_type(void);

void ags_port_selection_load_ports(AgsPortSelection *selection);
void ags_port_selection_enable_ports(AgsPortSelection *selection,
				     GList *ports);

void ags_port_selection_add_port(AgsPortSelection *port_selection,
				 AgsPort *port);

AgsPortSelection* ags_port_selection_new();

#endif /*__AGS_PORT_SELECTION_H__*/

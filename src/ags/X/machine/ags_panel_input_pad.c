/* This file is part of GSequencer.
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
/* This file is part of GSequencer.
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
/* This file is part of GSequencer.
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
/* This file is part of GSequencer.
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
/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
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

#include <ags/X/machine/ags_panel_input_pad.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/object/ags_plugin.h>

#include <ags/X/ags_window.h>

#include <ags/X/machine/ags_panel.h>

#include <math.h>

void ags_panel_input_pad_class_init(AgsPanelInputPadClass *panel_input_pad);
void ags_panel_input_pad_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_panel_input_pad_plugin_interface_init(AgsPluginInterface *plugin);
void ags_panel_input_pad_init(AgsPanelInputPad *panel_input_pad);
void ags_panel_input_pad_connect(AgsConnectable *connectable);
void ags_panel_input_pad_disconnect(AgsConnectable *connectable);
void ags_panel_input_pad_finalize(GObject *gobject);
gchar* ags_panel_input_pad_get_name(AgsPlugin *plugin);
void ags_panel_input_pad_set_name(AgsPlugin *plugin, gchar *name);
gchar* ags_panel_input_pad_get_xml_type(AgsPlugin *plugin);
void ags_panel_input_pad_set_xml_type(AgsPlugin *plugin, gchar *xml_type);

void ags_panel_input_pad_show(GtkWidget *pad);

void ags_panel_input_pad_set_channel(AgsPad *pad, AgsChannel *channel);
void ags_panel_input_pad_resize_lines(AgsPad *pad, GType line_type,
				     guint audio_channels, guint audio_channels_old);

/**
 * SECTION:ags_panel_input_pad
 * @short_description: panel input pad
 * @title: AgsPanelInputPad
 * @section_id:
 * @include: ags/X/machine/ags_panel_input_pad.h
 *
 * The #AgsPanelInputPad is a composite widget to act as panel input pad.
 */

static gpointer ags_panel_input_pad_parent_class = NULL;
static AgsConnectableInterface *ags_panel_input_pad_parent_connectable_interface;

GType
ags_panel_input_pad_get_type()
{
  static GType ags_type_panel_input_pad = 0;

  if(!ags_type_panel_input_pad){
    static const GTypeInfo ags_panel_input_pad_info = {
      sizeof(AgsPanelInputPadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_panel_input_pad_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsPanelInputPad),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_panel_input_pad_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_panel_input_pad_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_panel_input_pad_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_panel_input_pad = g_type_register_static(AGS_TYPE_PAD,
						     "AgsPanelInputPad\0", &ags_panel_input_pad_info,
						     0);

    g_type_add_interface_static(ags_type_panel_input_pad,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_panel_input_pad,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_panel_input_pad);
}

void
ags_panel_input_pad_class_init(AgsPanelInputPadClass *panel_input_pad)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  AgsPadClass *pad;

  ags_panel_input_pad_parent_class = g_type_class_peek_parent(panel_input_pad);

  /* GObjectClass */
  gobject = (GObjectClass *) panel_input_pad;

  gobject->finalize = ags_panel_input_pad_finalize;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) panel_input_pad;

  widget->show = ags_panel_input_pad_show;

  /* AgsPadClass */
  pad = (AgsPadClass *) panel_input_pad;

  pad->set_channel = ags_panel_input_pad_set_channel;
  pad->resize_lines = ags_panel_input_pad_resize_lines;
}

void
ags_panel_input_pad_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_panel_input_pad_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_panel_input_pad_connect;
  connectable->disconnect = ags_panel_input_pad_disconnect;
}

void
ags_panel_input_pad_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = ags_panel_input_pad_get_name;
  plugin->set_name = ags_panel_input_pad_set_name;
  plugin->get_xml_type = ags_panel_input_pad_get_xml_type;
  plugin->set_xml_type = ags_panel_input_pad_set_xml_type;
}

void
ags_panel_input_pad_init(AgsPanelInputPad *panel_input_pad)
{
  AgsPad *pad;
  GtkVBox *vbox;

  pad = (AgsPad *) panel_input_pad;
  pad->cols = 1;

  vbox = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) pad, (GtkWidget *) vbox, FALSE, TRUE, 0);
  gtk_box_reorder_child((GtkBox *) pad, (GtkWidget *) vbox, 0);
}

void
ags_panel_input_pad_connect(AgsConnectable *connectable)
{
  AgsPanelInputPad *panel_input_pad;

  panel_input_pad = AGS_PANEL_INPUT_PAD(connectable);

  if((AGS_PAD_CONNECTED & (AGS_PAD(panel_input_pad)->flags)) != 0){
    return;
  }

  ags_panel_input_pad_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_panel_input_pad_disconnect(AgsConnectable *connectable)
{
  ags_panel_input_pad_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_panel_input_pad_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_panel_input_pad_parent_class)->finalize(gobject);

  /* empty */
}

gchar*
ags_panel_input_pad_get_name(AgsPlugin *plugin)
{
  return(AGS_PANEL_INPUT_PAD(plugin)->name);
}

void
ags_panel_input_pad_set_name(AgsPlugin *plugin, gchar *name)
{
  AGS_PANEL_INPUT_PAD(plugin)->name = name;
}

gchar*
ags_panel_input_pad_get_xml_type(AgsPlugin *plugin)
{
  return(AGS_PANEL_INPUT_PAD(plugin)->xml_type);
}

void
ags_panel_input_pad_set_xml_type(AgsPlugin *plugin, gchar *xml_type)
{
  AGS_PANEL_INPUT_PAD(plugin)->xml_type = xml_type;
}

void
ags_panel_input_pad_show(GtkWidget *pad)
{
  GTK_WIDGET_CLASS(ags_panel_input_pad_parent_class)->show(pad);

  gtk_widget_hide(GTK_WIDGET(AGS_PAD(pad)->group));
  gtk_widget_hide(GTK_WIDGET(AGS_PAD(pad)->mute));
  gtk_widget_hide(GTK_WIDGET(AGS_PAD(pad)->solo));
}

void
ags_panel_input_pad_set_channel(AgsPad *pad, AgsChannel *channel)
{
  AGS_PAD_CLASS(ags_panel_input_pad_parent_class)->set_channel(pad, channel);

  /* empty */
}

void
ags_panel_input_pad_resize_lines(AgsPad *pad, GType line_type,
				guint audio_channels, guint audio_channels_old)
{
  AGS_PAD_CLASS(ags_panel_input_pad_parent_class)->resize_lines(pad, line_type,
								audio_channels, audio_channels_old);

  /* empty */
}

/**
 * ags_panel_input_pad_new:
 * @channel: the assigned channel
 *
 * Creates an #AgsPanelInputPad
 *
 * Returns: a new #AgsPanelInputPad
 *
 * Since: 0.4
 */
AgsPanelInputPad*
ags_panel_input_pad_new(AgsChannel *channel)
{
  AgsPanelInputPad *panel_input_pad;

  panel_input_pad = (AgsPanelInputPad *) g_object_new(AGS_TYPE_PANEL_INPUT_PAD,
						      "channel\0", channel,
						      NULL);

  return(panel_input_pad);
}

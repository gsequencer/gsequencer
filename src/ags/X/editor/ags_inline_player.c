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

#include <ags/X/editor/ags_inline_player.h>
#include <ags/X/editor/ags_inline_player_callbacks.h>

#include <ags-lib/object/ags_connectable.h>
#include <ags/object/ags_playable.h>

#include <ags/X/ags_window.h>

void ags_inline_player_class_init(AgsInlinePlayerClass *inline_player);
void ags_inline_player_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_inline_player_init(AgsInlinePlayer *inline_player);
void ags_inline_player_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec);
void ags_inline_player_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec);
void ags_inline_player_finalize(GObject *gobject);
void ags_inline_player_connect(AgsConnectable *connectable);
void ags_inline_player_disconnect(AgsConnectable *connectable);
void ags_inline_player_show(GtkWidget *widget);

void ags_inline_player_update(AgsInlinePlayer *inline_player);

enum{
  PROP_0,
  PROP_PLAYABLE,
};

static gpointer ags_inline_player_parent_class = NULL;

GType
ags_inline_player_get_type(void)
{
  static GType ags_type_inline_player = 0;

  if(!ags_type_inline_player){
    static const GTypeInfo ags_inline_player_info = {
      sizeof (AgsInlinePlayerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_inline_player_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsInlinePlayer),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_inline_player_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_inline_player_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_inline_player = g_type_register_static(GTK_TYPE_VBOX,
						  "AgsInlinePlayer\0", &ags_inline_player_info,
						  0);
    
    g_type_add_interface_static(ags_type_inline_player,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return(ags_type_inline_player);
}

void
ags_inline_player_class_init(AgsInlinePlayerClass *inline_player)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GParamSpec *param_spec;

  ags_inline_player_parent_class = g_type_class_peek_parent(inline_player);

  /* GObjectClass */
  gobject = (GObjectClass *) inline_player;

  gobject->set_property = ags_inline_player_set_property;
  gobject->get_property = ags_inline_player_get_property;

  gobject->finalize = ags_inline_player_finalize;

  /* properties */
  param_spec = g_param_spec_object("playable\0",
				   "playable for player\0",
				   "The AgsPlayable to use within the AgsInlinePlayer\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLAYABLE,
				  param_spec);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) inline_player;

  widget->show = ags_inline_player_show;
}

void
ags_inline_player_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_inline_player_connect;
  connectable->disconnect = ags_inline_player_disconnect;
}

void
ags_inline_player_init(AgsInlinePlayer *inline_player)
{
  GtkHBox *hbox;

  inline_player->playable = NULL;

  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) inline_player,
		     (GtkWidget *) hbox,
		     FALSE, FALSE,
		     0);

  inline_player->play = (GtkToggleButton *) g_object_new(GTK_TYPE_TOGGLE_BUTTON,
							 "image\0", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_MEDIA_PLAY,
													   GTK_ICON_SIZE_LARGE_TOOLBAR),
							 NULL);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) inline_player->play,
		     FALSE, FALSE,
		     0);

  inline_player->position = (GtkHScale *) gtk_hscale_new_with_range(0.0, 1.0, 1.0);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) inline_player->position,
		     TRUE, TRUE,
		     0);

  inline_player->volume = (GtkVolumeButton *) gtk_volume_button_new();
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) inline_player->volume,
		     FALSE, FALSE,
		     0);
}

void
ags_inline_player_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsInlinePlayer *inline_player;

  inline_player = AGS_INLINE_PLAYER(gobject);

  switch(prop_id){
  case PROP_PLAYABLE:
    {
      AgsPlayable *playable;

      playable = (AgsPlayable *) g_value_get_object(value);

      if(inline_player->playable == playable)
	return;

      if(inline_player->playable != NULL){
	g_object_unref(inline_player->playable);
      }
	  
      if(playable != NULL){
	g_object_ref(playable);
      }	

      inline_player->playable = playable;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_inline_player_get_property(GObject *gobject,
		       guint prop_id,
		       GValue *value,
		       GParamSpec *param_spec)
{
  AgsInlinePlayer *inline_player;

  inline_player = AGS_INLINE_PLAYER(gobject);

  switch(prop_id){
  case PROP_PLAYABLE:
    g_value_set_object(value, inline_player->playable);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_inline_player_finalize(GObject *gobject)
{
  AgsInlinePlayer *inline_player;

  inline_player = (AgsInlinePlayer *) gobject;

  //TODO:JK:
  /* implement me */

  G_OBJECT_CLASS(ags_inline_player_parent_class)->finalize(gobject);
}

void
ags_inline_player_connect(AgsConnectable *connectable)
{
  AgsInlinePlayer *inline_player;

  /* AgsInlinePlayer */
  inline_player = AGS_INLINE_PLAYER(connectable);

  //TODO:JK: implement me
  /*
  g_signal_connect(G_OBJECT(inline_player->preset), "changed\0",
		   G_CALLBACK(ags_inline_player_preset_changed), inline_player);

  g_signal_connect(G_OBJECT(inline_player->instrument), "changed\0",
		   G_CALLBACK(ags_inline_player_instrument_changed), inline_player);

  g_signal_connect(G_OBJECT(inline_player->sample), "changed\0",
		   G_CALLBACK(ags_inline_player_sample_changed), inline_player);
  */
}

void
ags_inline_player_disconnect(AgsConnectable *connectable)
{
  //TODO:JK:
  /* implement me */
}

void
ags_inline_player_show(GtkWidget *widget)
{
  AgsInlinePlayer *inline_player = (AgsInlinePlayer *) widget;

  GTK_WIDGET_CLASS(ags_inline_player_parent_class)->show(widget);

  //TODO:JK:
  /* perhaps empty */
}

AgsInlinePlayer*
ags_inline_player_new()
{
  AgsInlinePlayer *inline_player;

  inline_player = (AgsInlinePlayer *) g_object_new(AGS_TYPE_INLINE_PLAYER,
						   NULL);
  
  return(inline_player);
}

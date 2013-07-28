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

#include <ags/audio/task/ags_toggle_pattern_bit.h>

#include <ags-lib/object/ags_connectable.h>

void ags_toggle_pattern_bit_class_init(AgsTogglePatternBitClass *toggle_pattern_bit);
void ags_toggle_pattern_bit_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_toggle_pattern_bit_init(AgsTogglePatternBit *toggle_pattern_bit);
void ags_toggle_pattern_bit_connect(AgsConnectable *connectable);
void ags_toggle_pattern_bit_disconnect(AgsConnectable *connectable);
void ags_toggle_pattern_bit_finalize(GObject *gobject);

void ags_toggle_pattern_bit_launch(AgsTask *task);

static gpointer ags_toggle_pattern_bit_parent_class = NULL;
static AgsConnectableInterface *ags_toggle_pattern_bit_parent_connectable_interface;

GType
ags_toggle_pattern_bit_get_type()
{
  static GType ags_type_toggle_pattern_bit = 0;

  if(!ags_type_toggle_pattern_bit){
    static const GTypeInfo ags_toggle_pattern_bit_info = {
      sizeof (AgsTogglePatternBitClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_toggle_pattern_bit_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsTogglePatternBit),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_toggle_pattern_bit_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_toggle_pattern_bit_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_toggle_pattern_bit = g_type_register_static(AGS_TYPE_TASK,
							 "AgsTogglePatternBit\0",
							 &ags_toggle_pattern_bit_info,
							 0);

    g_type_add_interface_static(ags_type_toggle_pattern_bit,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_toggle_pattern_bit);
}

void
ags_toggle_pattern_bit_class_init(AgsTogglePatternBitClass *toggle_pattern_bit)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_toggle_pattern_bit_parent_class = g_type_class_peek_parent(toggle_pattern_bit);

  /* gobject */
  gobject = (GObjectClass *) toggle_pattern_bit;

  gobject->finalize = ags_toggle_pattern_bit_finalize;

  /* task */
  task = (AgsTaskClass *) toggle_pattern_bit;

  task->launch = ags_toggle_pattern_bit_launch;
}

void
ags_toggle_pattern_bit_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_toggle_pattern_bit_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_toggle_pattern_bit_connect;
  connectable->disconnect = ags_toggle_pattern_bit_disconnect;
}

void
ags_toggle_pattern_bit_init(AgsTogglePatternBit *toggle_pattern_bit)
{
  toggle_pattern_bit->pattern = NULL;

  toggle_pattern_bit->index_i = 0;
  toggle_pattern_bit->index_j = 0;
  toggle_pattern_bit->bit = 0;
}

void
ags_toggle_pattern_bit_connect(AgsConnectable *connectable)
{
  ags_toggle_pattern_bit_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_toggle_pattern_bit_disconnect(AgsConnectable *connectable)
{
  ags_toggle_pattern_bit_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_toggle_pattern_bit_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_toggle_pattern_bit_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_toggle_pattern_bit_launch(AgsTask *task)
{
  AgsTogglePatternBit *toggle_pattern_bit;

  toggle_pattern_bit = AGS_TOGGLE_PATTERN_BIT(task);

  ags_pattern_toggle_bit((AgsPattern *) toggle_pattern_bit->pattern,
			 toggle_pattern_bit->index_i, toggle_pattern_bit->index_j,
			 toggle_pattern_bit->bit);
}

AgsTogglePatternBit*
ags_toggle_pattern_bit_new(AgsPattern *pattern,
			   guint index_i, guint index_j,
			   guint bit)
{
  AgsTogglePatternBit *toggle_pattern_bit;

  toggle_pattern_bit = (AgsTogglePatternBit *) g_object_new(AGS_TYPE_TOGGLE_PATTERN_BIT,
							    NULL);

  toggle_pattern_bit->pattern = pattern;

  toggle_pattern_bit->index_i = index_i;
  toggle_pattern_bit->index_j = index_j;
  toggle_pattern_bit->bit = bit;

  return(toggle_pattern_bit);
}

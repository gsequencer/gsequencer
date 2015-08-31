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

#ifndef __AGS_RADIO_BUTTON_H__
#define __AGS_RADIO_BUTTON_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define AGS_TYPE_RADIO_BUTTON                (ags_radio_button_get_type())
#define AGS_RADIO_BUTTON(obj)                (G_TYPE_RADIO_INSTANCE_CAST((obj), AGS_TYPE_RADIO_BUTTON, AgsRadioButton))
#define AGS_RADIO_BUTTON_CLASS(class)        (G_TYPE_RADIO_CLASS_CAST((class), AGS_TYPE_RADIO_BUTTON, AgsRadioButtonClass))
#define AGS_IS_RADIO_BUTTON(obj)             (G_TYPE_RADIO_INSTANCE_TYPE((obj), AGS_TYPE_RADIO_BUTTON))
#define AGS_IS_RADIO_BUTTON_CLASS(class)     (G_TYPE_RADIO_CLASS_TYPE((class), AGS_TYPE_RADIO_BUTTON))
#define AGS_RADIO_BUTTON_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_RADIO_BUTTON, AgsRadioButtonClass))

struct _AgsRadioButton
{
  GtkRadioButton radio_button;
};

struct _AgsRadioButtonClass
{
  GtkRadioButtonClass radio_button;
};
  
AgsRadioButton* ags_radio_button_new();

#endif /*__AGS_RADIO_BUTTON_H__*/

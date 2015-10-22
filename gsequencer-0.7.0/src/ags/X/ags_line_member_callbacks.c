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

#include <ags/X/ags_line_member_callbacks.h>

int
ags_line_member_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsLineMember *line_member)
{
  if(old_parent == NULL){
    //TODO:JK: implement me
  }
}

void
ags_line_member_dial_changed_callback(GtkWidget *dial, AgsLineMember *line_member)
{
  GtkAdjustment *adjustment;

  g_object_get(dial,
	       "adjustment\0", &adjustment,
	       NULL);
  ags_line_member_change_port(line_member,
			      (gpointer) &(adjustment->value));
}

void
ags_line_member_vscale_changed_callback(GtkWidget *vscale, AgsLineMember *line_member)
{
  GtkAdjustment *adjustment;

  adjustment = gtk_range_get_adjustment(GTK_RANGE(vscale));
  ags_line_member_change_port(line_member,
			      (gpointer) &(adjustment->value));
}

void
ags_line_member_hscale_changed_callback(GtkWidget *hscale, AgsLineMember *line_member)
{
  GtkAdjustment *adjustment;

  adjustment = gtk_range_get_adjustment(GTK_RANGE(hscale));
  ags_line_member_change_port(line_member,
			      &(adjustment->value));
}

void
ags_line_member_spin_button_changed_callback(GtkWidget *spin_button, AgsLineMember *line_member)
{
  GtkAdjustment *adjustment;

  adjustment = gtk_spin_button_get_adjustment((GtkSpinButton *) spin_button);
  ags_line_member_change_port(line_member,
			      &(adjustment->value));
}

void
ags_line_member_check_button_clicked_callback(GtkWidget *check_button, AgsLineMember *line_member)
{
  line_member->active = gtk_toggle_button_get_active((GtkToggleButton *) check_button);
  ags_line_member_change_port(line_member,
			      &(line_member->active));
}

void
ags_line_member_toggle_button_clicked_callback(GtkWidget *toggle_button, AgsLineMember *line_member)
{
  line_member->active = gtk_toggle_button_get_active((GtkToggleButton *) toggle_button);
  ags_line_member_change_port(line_member,
			      &(line_member->active));
}

void
ags_line_member_button_clicked_callback(GtkWidget *button, AgsLineMember *line_member)
{
  ags_line_member_change_port(line_member,
			      &(line_member->active));
}
void
ags_line_member_port_safe_write_callback(AgsPort *port, GValue *value,
					 AgsLineMember *line_member)
{
  //TODO:JK: implement me
}

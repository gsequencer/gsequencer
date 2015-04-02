/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
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

#include <ags/X/ags_bulk_member_callbacks.h>

int
ags_bulk_member_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsBulkMember *bulk_member)
{
  if(old_parent == NULL){
    //TODO:JK: implement me
  }
}

void
ags_bulk_member_dial_changed_callback(GtkWidget *dial, AgsBulkMember *bulk_member)
{
  GtkAdjustment *adjustment;

  g_object_get(dial,
	       "adjustment\0", &adjustment,
	       NULL);
  ags_bulk_member_change_port(bulk_member,
			      (gpointer) &(adjustment->value));
}

void
ags_bulk_member_vscale_changed_callback(GtkWidget *vscale, AgsBulkMember *bulk_member)
{
  GtkAdjustment *adjustment;

  adjustment = gtk_range_get_adjustment(GTK_RANGE(vscale));
  ags_bulk_member_change_port(bulk_member,
			      (gpointer) &(adjustment->value));
}

void
ags_bulk_member_hscale_changed_callback(GtkWidget *hscale, AgsBulkMember *bulk_member)
{
  GtkAdjustment *adjustment;

  adjustment = gtk_range_get_adjustment(GTK_RANGE(hscale));
  ags_bulk_member_change_port(bulk_member,
			      &(adjustment->value));
}

void
ags_bulk_member_spin_button_changed_callback(GtkWidget *spin_button, AgsBulkMember *bulk_member)
{
  GtkAdjustment *adjustment;

  adjustment = gtk_spin_button_get_adjustment(spin_button);
  ags_bulk_member_change_port(bulk_member,
			      &(adjustment->value));
}

void
ags_bulk_member_check_button_clicked_callback(GtkWidget *check_button, AgsBulkMember *bulk_member)
{
  gboolean active;

  active = gtk_toggle_button_get_active((GtkToggleButton *) check_button);
  ags_bulk_member_change_port(bulk_member,
			      &(active));
}

void
ags_bulk_member_toggle_button_clicked_callback(GtkWidget *toggle_button, AgsBulkMember *bulk_member)
{
  gboolean active;

  active = gtk_toggle_button_get_active((GtkToggleButton *) toggle_button);
  ags_bulk_member_change_port(bulk_member,
			      &(active));
}

void
ags_bulk_member_button_clicked_callback(GtkWidget *button, AgsBulkMember *bulk_member)
{
  gboolean active;

  ags_bulk_member_change_port(bulk_member,
			      &(active));
}
void
ags_bulk_member_port_safe_write_callback(AgsPort *port, GValue *value,
					 AgsBulkMember *bulk_member)
{
  //TODO:JK: implement me
}

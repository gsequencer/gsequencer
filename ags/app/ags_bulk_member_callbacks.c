/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/app/ags_bulk_member_callbacks.h>

void
ags_bulk_member_dial_changed_callback(GtkWidget *dial, AgsBulkMember *bulk_member)
{
  GtkAdjustment *adjustment;

  gdouble value;
  
  if((AGS_BULK_MEMBER_NO_UPDATE & (bulk_member->flags)) != 0){
    return;
  }

  g_object_get(dial,
	       "adjustment", &adjustment,
	       NULL);

  value = gtk_adjustment_get_value(adjustment);

  ags_bulk_member_change_port(bulk_member,
			      (gpointer) &value);
}

void
ags_bulk_member_vscale_changed_callback(GtkWidget *vscale, AgsBulkMember *bulk_member)
{
  GtkAdjustment *adjustment;

  gdouble value;

  if((AGS_BULK_MEMBER_NO_UPDATE & (bulk_member->flags)) != 0){
    return;
  }

  adjustment = gtk_range_get_adjustment(GTK_RANGE(vscale));

  value = gtk_adjustment_get_value(adjustment);

  ags_bulk_member_change_port(bulk_member,
			      (gpointer) &value);
}

void
ags_bulk_member_hscale_changed_callback(GtkWidget *hscale, AgsBulkMember *bulk_member)
{
  GtkAdjustment *adjustment;

  gdouble value;

  if((AGS_BULK_MEMBER_NO_UPDATE & (bulk_member->flags)) != 0){
    return;
  }

  adjustment = gtk_range_get_adjustment(GTK_RANGE(hscale));

  value = gtk_adjustment_get_value(adjustment);

  ags_bulk_member_change_port(bulk_member,
			      &value);
}

void
ags_bulk_member_spin_button_changed_callback(GtkWidget *spin_button, AgsBulkMember *bulk_member)
{
  GtkAdjustment *adjustment;

  gdouble value;

  if((AGS_BULK_MEMBER_NO_UPDATE & (bulk_member->flags)) != 0){
    return;
  }

  adjustment = gtk_spin_button_get_adjustment((GtkSpinButton *) spin_button);

  value = gtk_adjustment_get_value(adjustment);

  ags_bulk_member_change_port(bulk_member,
			      &value);
}

void
ags_bulk_member_check_button_toggled_callback(GtkWidget *check_button, AgsBulkMember *bulk_member)
{
  gboolean active;

  if((AGS_BULK_MEMBER_NO_UPDATE & (bulk_member->flags)) != 0){
    return;
  }

  active = gtk_toggle_button_get_active((GtkToggleButton *) check_button);
  ags_bulk_member_change_port(bulk_member,
			      &(active));
}

void
ags_bulk_member_toggle_button_toggled_callback(GtkWidget *toggle_button, AgsBulkMember *bulk_member)
{
  gboolean active;

  if((AGS_BULK_MEMBER_NO_UPDATE & (bulk_member->flags)) != 0){
    return;
  }

  active = gtk_toggle_button_get_active((GtkToggleButton *) toggle_button);
  ags_bulk_member_change_port(bulk_member,
			      &(active));
}

void
ags_bulk_member_button_clicked_callback(GtkWidget *button, AgsBulkMember *bulk_member)
{
  gboolean active;

  if((AGS_BULK_MEMBER_NO_UPDATE & (bulk_member->flags)) != 0){
    return;
  }

  ags_bulk_member_change_port(bulk_member,
			      &(active));
}
void
ags_bulk_member_port_safe_write_callback(AgsPort *port, GValue *value,
					 AgsBulkMember *bulk_member)
{
  //TODO:JK: implement me
}

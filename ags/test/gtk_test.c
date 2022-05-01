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

#include <glib.h>
#include <glib-object.h>

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include <ags/libags-gui.h>

#include <locale.h>

typedef struct {
  AgsExpander *expander;
}AgsExpanderFixture;

void ags_expander_fixture_set_up(AgsExpanderFixture *fixture,
				 gconstpointer user_data);
void ags_expander_fixture_tear_down(AgsExpanderFixture *fixture,
				    gconstpointer user_data);

void ags_expander_test_add(AgsExpanderFixture *fixture,
			   gconstpointer data);
void ags_expander_test_add_and_collapse(AgsExpanderFixture *fixture,
					gconstpointer data);

GtkWindow *window;
GtkBox *vbox;

void
ags_expander_fixture_set_up(AgsExpanderFixture *fixture,
			    gconstpointer user_data)
{
  fixture->expander = ags_expander_new();
  gtk_box_append(vbox,
		 fixture->expander);

  gtk_widget_show(fixture->expander);
}

void
ags_expander_fixture_tear_down(AgsExpanderFixture *fixture,
			       gconstpointer user_data)
{
  gtk_box_remove(vbox,
		 fixture->expander);
}

void
ags_expander_test_add(AgsExpanderFixture *fixture,
		      gconstpointer data)
{
  GtkLabel *label;

  label = gtk_label_new("test add");
  ags_expander_add(fixture->expander,
		   label,
		   0, 0,
		   1, 1);

  gtk_widget_show(label);

  g_usleep(3 * G_USEC_PER_SEC);
}

void
ags_expander_test_add_and_collapse(AgsExpanderFixture *fixture,
				   gconstpointer data)
{
  GtkLabel *label;

  label = gtk_label_new("test add");
  ags_expander_add(fixture->expander,
		   label,
		   0, 0,
		   1, 1);

  gtk_widget_show(label);

  g_signal_emit_by_name(fixture->expander->expander, "activate",
			NULL);

  g_usleep(3 * G_USEC_PER_SEC);
}

int
main(int argc, char **argv)
{
  GtkLabel *label;
  
  setlocale(LC_ALL, "");

  gtk_test_init(&argc, &argv,
		NULL);

  window = gtk_window_new();

  vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,
		     0);
  gtk_window_set_child(window,
		       vbox);
  
  label = gtk_label_new("AgsExpander - test");
  gtk_box_append(vbox,
		 label);
  
  gtk_widget_show(window);
  
  g_test_add("/ags-expander/add", AgsExpanderFixture, "ags-expander-add-test",
	     ags_expander_fixture_set_up,
	     ags_expander_test_add,
	     ags_expander_fixture_tear_down);
  g_test_add("/ags-expander/add-and-collapse", AgsExpanderFixture, "ags-expander-add-and-collapse-test",
	     ags_expander_fixture_set_up,
	     ags_expander_test_add_and_collapse,
	     ags_expander_fixture_tear_down);

  return g_test_run();
}

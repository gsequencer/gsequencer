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

#include <gtk/gtk.h>

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include <ags/libags-gui.h>

#include <locale.h>

gboolean ags_expander_test_timeout_func(gpointer data);

void ags_expander_test_add_test();
int ags_expander_test_init_suite();
int ags_expander_test_clean_suite();

void ags_expander_test_add();
void ags_expander_test_add_and_collapse();

CU_pSuite pSuite = NULL;

GtkWindow *window;
GtkBox *vbox;
AgsExpander *expander;

GtkApplication *app;
GThread *ags_expander_test_runner_thread;

GRecMutex mutex;

gboolean initial_lock = TRUE;

gboolean
ags_expander_test_timeout_func(gpointer data)
{
  if(initial_lock){
    initial_lock = FALSE;
  }else{
    g_rec_mutex_unlock(&mutex);
  }
  
  g_usleep(G_USEC_PER_SEC / 30);

  g_rec_mutex_lock(&mutex);
  
  return(G_SOURCE_CONTINUE);
}

void*
ags_expander_test_do_run_thread(void *ptr)
{
  g_usleep(5 * G_USEC_PER_SEC);

  ags_expander_test_add_test();
  
  g_thread_exit(NULL);

  return(NULL);
}

void
ags_expander_test_add_test()
{  
  /* add the tests to the suite */
  if((CU_add_test(pSuite, "functional test of AgsExpander add", ags_expander_test_add) == NULL) ||
     (CU_add_test(pSuite, "functional test of AgsExpander add and collapse", ags_expander_test_add_and_collapse) == NULL)){
    CU_cleanup_registry();
      
    exit(CU_get_error());
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
    
  CU_cleanup_registry();
  
  exit(CU_get_error());
}

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_expander_test_init_suite()
{    
  GtkLabel *label;

  g_rec_mutex_lock(&mutex);

  window = gtk_window_new();

  vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,
		     0);
  gtk_window_set_child(window,
		       vbox);
  
  label = gtk_label_new("AgsExpander - test");
  gtk_box_append(vbox,
		 label);
  
  gtk_widget_show(window);

  expander = ags_expander_new();
  gtk_box_append(vbox,
		 expander);

  gtk_widget_show(expander);

  gtk_test_widget_wait_for_draw(window);
  gtk_test_widget_wait_for_draw(vbox);
  gtk_test_widget_wait_for_draw(expander);

  g_rec_mutex_unlock(&mutex);
  
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_expander_test_clean_suite()
{  
  g_rec_mutex_lock(&mutex);

  gtk_box_remove(vbox,
		 expander);

  gtk_test_widget_wait_for_draw(vbox);

  g_rec_mutex_unlock(&mutex);

  return(0);
}

void
ags_expander_test_add()
{
  GtkLabel *label;

  guint i;

  g_rec_mutex_lock(&mutex);
    
  label = gtk_label_new("test add");
  ags_expander_add(expander,
		   label,
		   0, 0,
		   1, 1);

  gtk_widget_show(label);

  g_rec_mutex_unlock(&mutex);
  
  g_usleep(3 * G_USEC_PER_SEC);
}

void
ags_expander_test_add_and_collapse()
{
  GtkLabel *label;

  guint i;
    
  g_rec_mutex_lock(&mutex);

  label = gtk_label_new("test add");
  ags_expander_add(expander,
		   label,
		   0, 1,
		   1, 1);

  gtk_widget_show(label);

  g_signal_emit_by_name(expander->expander, "activate",
			NULL);

  gtk_test_widget_wait_for_draw(label);

  g_rec_mutex_unlock(&mutex);

  g_usleep(3 * G_USEC_PER_SEC);
}

int
main(int argc, char **argv)
{
  setlocale(LC_ALL, "");
  
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsExpanderTest", ags_expander_test_init_suite, ags_expander_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  gtk_init();

  g_rec_mutex_init(&mutex);

  g_timeout_add(G_USEC_PER_SEC / 60,
		ags_expander_test_timeout_func,
		NULL);
  
  app = gtk_application_new("org.gsequencer.AgsExpanderTest",
			    G_APPLICATION_FLAGS_NONE);
  
  ags_expander_test_runner_thread = g_thread_new("libags_gui.so - functional test",
						 ags_expander_test_do_run_thread,
						 NULL);
  g_usleep(G_USEC_PER_SEC);

  g_application_run(app,
		    argc, argv);
  
  g_thread_join(ags_expander_test_runner_thread);
  
  return(0);
}

/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/X/ags_online_help_window_callbacks.h>

#include <ags/config.h>

#if defined(AGS_WITH_WEBKIT)
void
ags_online_help_window_load_changed(WebKitWebView *web_view,
				    WebKitLoadEvent load_event,
				    AgsOnlineHelpWindow *online_help_window)
{
  gchar *uri;

  uri = webkit_web_view_get_uri(web_view);

  gtk_entry_set_text(online_help_window->location,
		     uri);
}
#endif

gboolean
ags_online_help_window_pdf_drawing_area_draw_callback(GtkWidget *pdf_drawing_area,
						      cairo_t *cr,
						      AgsOnlineHelpWindow *online_help_window)
{
#if defined(AGS_WITH_POPPLER)
  GtkAdjustment *vadjustment, *hadjustment;

  GtkAllocation allocation;
  
  cairo_t *pdf_cr;
  cairo_surface_t *pdf_surface;

  gdouble vvalue, hvalue;
  gint num_pages, i;
  gdouble width, height;
  gint current_width, current_height;

  vadjustment = gtk_scrolled_window_get_vadjustment(online_help_window->pdf_scrolled_window);
  hadjustment = gtk_scrolled_window_get_hadjustment(online_help_window->pdf_scrolled_window);

  vvalue = gtk_adjustment_get_value(vadjustment);
  hvalue = gtk_adjustment_get_value(hadjustment);
  
  gtk_widget_get_allocation(online_help_window->pdf_drawing_area,
			    &allocation);

  gtk_adjustment_configure(vadjustment,
			   vvalue,
			   0.0,
			   online_help_window->max_height - (double) allocation.height,
			   10.0,
			   100.0,
			   (double) allocation.height);

  gtk_adjustment_configure(hadjustment,
			   hvalue,
			   0.0,
			   online_help_window->max_width - (double) allocation.width,
			   10.0,
			   100.0,
			   (double) allocation.width);
  
  num_pages = poppler_document_get_n_pages(online_help_window->pdf_document);
  
  current_width = 0;
  current_height = 0;

  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
  cairo_rectangle(cr,
		  0.0, 0.0,
		  (double) allocation.width, (double) allocation.height);

  cairo_fill(cr);
  
  for(i = 0; i < num_pages; i++){
    PopplerPage *page;
    
    page = poppler_document_get_page(online_help_window->pdf_document,
				     i);
    
    if(page == NULL) {
      g_warning("poppler fail: page not found");

      break;
    }
    
    poppler_page_get_size(page,
    			  &width, &height);

    if(current_height + height > vvalue &&
       current_height < vvalue + allocation.height){
      pdf_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
					       width,
					       height);
  
      pdf_cr = cairo_create(pdf_surface);
    
      cairo_save(cr);
    
      poppler_page_render(page,
			  pdf_cr);

      cairo_restore(cr);

      cairo_set_source_surface(cr,
			       pdf_surface,
			       gtk_adjustment_get_value(hadjustment),
			       gtk_adjustment_get_value(vadjustment));
      cairo_paint(cr);
    
      cairo_destroy(pdf_cr);

      cairo_surface_finish(pdf_surface);
      cairo_surface_destroy(pdf_surface);
    }

    current_height += height;
    
    g_object_unref(page);
  }
  
#endif

  return(FALSE);
}

void
ags_online_help_window_home_callback(GtkWidget *button,
				  AgsOnlineHelpWindow *online_help_window)
{
#if defined(AGS_WITH_WEBKIT)
  webkit_web_view_load_uri(online_help_window->web_view,
			   online_help_window->start_filename);
#endif
}

void
ags_online_help_window_next_callback(GtkWidget *button,
				     AgsOnlineHelpWindow *online_help_window)
{
#if defined(AGS_WITH_WEBKIT)
  webkit_web_view_go_forward(online_help_window->web_view);
#endif
}


void
ags_online_help_window_prev_callback(GtkWidget *button,
				     AgsOnlineHelpWindow *online_help_window)
{
#if defined(AGS_WITH_WEBKIT)
  webkit_web_view_go_back(online_help_window->web_view);
#endif
}

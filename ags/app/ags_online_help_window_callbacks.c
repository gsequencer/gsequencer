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

#include <ags/app/ags_online_help_window_callbacks.h>

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

void
ags_online_help_window_pdf_zoom_changed_callback(GtkComboBox *combo_box,
						 AgsOnlineHelpWindow *online_help_window)
{
  switch(gtk_combo_box_get_active(combo_box)){
  case 0:
    {
      online_help_window->zoom_x =
	online_help_window->zoom_y = 0.5;
    }
    break;
  case 1:
    {
      online_help_window->zoom_x =
	online_help_window->zoom_y = 0.75;
    }
    break;
  case 2:
    {
      online_help_window->zoom_x =
	online_help_window->zoom_y = 1.0;
    }
    break;
  case 3:
    {
      online_help_window->zoom_x =
	online_help_window->zoom_y = 1.25;
    }
    break;
  case 4:
    {
      online_help_window->zoom_x =
	online_help_window->zoom_y = 1.5;
    }
    break;
  case 5:
    {
      online_help_window->zoom_x =
	online_help_window->zoom_y = 2.0;
    }
    break;
  }

  gtk_widget_queue_draw(online_help_window->pdf_drawing_area);
}

void
ags_online_help_window_pdf_drawing_area_draw_callback(GtkWidget *pdf_drawing_area,
						      cairo_t *cr,
						      int width, int height,
						      AgsOnlineHelpWindow *online_help_window)
{
#if defined(AGS_WITH_POPPLER)
  GtkAdjustment *vadjustment, *hadjustment;

  GtkAllocation allocation;
  
  cairo_t *pdf_cr;
  cairo_surface_t *pdf_surface;

  gdouble vvalue, hvalue;
  gdouble voffset, hoffset;
  gint num_pages, i;
  gint current_width, current_height;

  vadjustment = gtk_scrollbar_get_adjustment(online_help_window->pdf_vscrollbar);
  hadjustment = gtk_scrollbar_get_adjustment(online_help_window->pdf_hscrollbar);

  vvalue = gtk_adjustment_get_value(vadjustment);
  hvalue = gtk_adjustment_get_value(hadjustment);
  
  gtk_widget_get_allocation(online_help_window->pdf_drawing_area,
			    &allocation);
  
  num_pages = poppler_document_get_n_pages(online_help_window->pdf_document);
  
  current_width = 0;
  current_height = 0;

  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
  cairo_rectangle(cr,
		  0.0, 0.0,
		  (double) allocation.width, (double) allocation.height);

  cairo_fill(cr);

  cairo_scale(cr,
	      online_help_window->zoom_x, online_help_window->zoom_y);
  
  voffset = 0.0;
  
  for(i = 0; i < num_pages; i++){
    PopplerPage *page;

    gdouble page_height, page_width;
    
    page = poppler_document_get_page(online_help_window->pdf_document,
				     i);
    
    if(page == NULL) {
      g_warning("poppler fail: page not found");

      break;
    }
    
    poppler_page_get_size(page,
    			  &page_width, &page_height);

    if(current_height + page_height > vvalue &&
       current_height < vvalue + allocation.height){
      pdf_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
					       online_help_window->zoom_x * page_width,
					       online_help_window->zoom_y * page_height);
      
      cairo_surface_set_device_scale(pdf_surface,
				     online_help_window->zoom_x, online_help_window->zoom_y);
      
      pdf_cr = cairo_create(pdf_surface);

      hoffset = (-1.0 * hvalue);
      voffset = (current_height - vvalue);
            
      cairo_save(cr);

      poppler_page_render(page,
			  pdf_cr);

      cairo_restore(cr);
      
      cairo_set_source_surface(cr,
			       pdf_surface,
			       hoffset,
			       voffset);
      cairo_paint(cr);
    
      cairo_destroy(pdf_cr);

      cairo_surface_finish(pdf_surface);
      cairo_surface_destroy(pdf_surface);
    }

    current_height += page_height;

    g_object_unref(page);

    if(current_height > vvalue + allocation.height){
      break;
    }
  }  
#endif
}

void
ags_online_help_window_pdf_drawing_area_resize_callback(GtkWidget *pdf_drawing_area,
							gint width, gint height,
							AgsOnlineHelpWindow *online_help_window)
{
#if defined(AGS_WITH_POPPLER)  
  GtkAdjustment *vadjustment, *hadjustment;
  GtkAllocation allocation;

  vadjustment = gtk_scrollbar_get_adjustment(online_help_window->pdf_vscrollbar);
  hadjustment = gtk_scrollbar_get_adjustment(online_help_window->pdf_hscrollbar);

  gtk_widget_get_allocation(online_help_window->pdf_drawing_area,
			    &allocation);

  gtk_adjustment_configure(vadjustment,
			   gtk_adjustment_get_value(vadjustment),
			   0.0,
			   online_help_window->max_height - (double) allocation.height,
			   5.0,
			   15.0,
			   (double) 10.0);

  gtk_adjustment_configure(hadjustment,
			   gtk_adjustment_get_value(hadjustment),
			   0.0,
			   online_help_window->max_width - (double) allocation.width,
			   5.0,
			   15.0,
			   (double) 10.0);
#endif  
}

void
ags_online_help_window_pdf_vscrollbar_value_changed_callback(GtkAdjustment *vscrollbar,
							     AgsOnlineHelpWindow *online_help_window)
{
#if defined(AGS_WITH_POPPLER)
  gtk_widget_queue_draw(online_help_window->pdf_drawing_area);
#endif  
}

void
ags_online_help_window_pdf_hscrollbar_value_changed_callback(GtkAdjustment *hscrollbar,
							     AgsOnlineHelpWindow *online_help_window)
{
#if defined(AGS_WITH_POPPLER)  
  gtk_widget_queue_draw(online_help_window->pdf_drawing_area);
#endif
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

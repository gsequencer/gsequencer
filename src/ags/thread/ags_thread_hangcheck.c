/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
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

#include <ags/thread/ags_thread_hangcheck.h>

void ags_thread_hangcheck_class_init(AgsThreadHangcheckClass *thread_hangcheck);
void ags_thread_hangcheck_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_thread_hangcheck_init(AgsThreadHangcheck *thread_hangcheck);
void ags_thread_hangcheck_connect(AgsConnectable *connectable);
void ags_thread_hangcheck_disconnect(AgsConnectable *connectable);
void ags_thread_hangcheck_finalize(GObject *gobject);

void ags_thread_hangcheck_realloc_stack(AgsThreadHangcheckClass *thread_hangcheck);
gboolean ags_thread_hangcheck_all_locked(AgsThreadHangcheckClass *thread_hangcheck);

void ags_thread_hangcheck_delegate(AgsThreadHangcheckClass *thread_hangcheck);
void ags_thread_hangcheck_lock_init(AgsThreadHangcheckClass *thread_hangcheck, AgsRXGate *rx_gate);
void ags_thread_hangcheck_lock_interupt(AgsThreadHangcheckClass *thread_hangcheck, AgsRXGate *rx_gate);
void ags_thread_hangcheck_lock_exit(AgsThreadHangcheckClass *thread_hangcheck, AgsRXGate *rx_gate);
void ags_thread_hangcheck_watch_abort(AgsThreadHangcheckClass *thread_hangcheck, AgsRXGate *rx_gate);
void ags_thread_hangcheck_watch_wait(AgsThreadHangcheckClass *thread_hangcheck, AgsRXGate *rx_gate);
void ags_thread_hangcheck_watch_exit(AgsThreadHangcheckClass *thread_hangcheck, AgsRXGate *rx_gate);
void ags_thread_hangcheck_watch_error(AgsThreadHangcheckClass *thread_hangcheck, AgsRXGate *rx_gate);

void ags_thread_hangcheck_quit(AgsThreadHangcheckClass *thread_hangcheck);

static gpointer ags_thread_hangcheck_parent_class = NULL;

GType
ags_thread_hangcheck_get_type()
{
  static GType ags_type_thread = 0;

  if(!ags_type_thread){
    const GTypeInfo ags_thread_hangcheck_info = {
      sizeof (AgsThreadHangcheckClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_thread_hangcheck_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsThreadHangcheck),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_thread_hangcheck_init,
    };

    const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_thread_hangcheck_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_thread = g_type_register_static(G_TYPE_OBJECT,
					     "AgsThreadHangcheck\0",
					     &ags_thread_hangcheck_info,
					     0);
        
    g_type_add_interface_static(ags_type_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return(ags_type_thread);
}

void
ags_thread_hangcheck_class_init(AgsThreadHangcheckClass *thread_hangcheck)
{
  GObjectClass *gobject;

  ags_thread_hangcheck_parent_class = g_type_class_peek_parent(thread_hangcheck);

  /* GObject */
  gobject = (GObjectClass *) thread_hangcheck;

  gobject->finalize = ags_thread_hangcheck_finalize;
}

void
ags_thread_hangcheck_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_thread_hangcheck_connect;
  connectable->disconnect = ags_thread_hangcheck_disconnect;
}

void
ags_thread_hangcheck_init(AgsThreadHangcheck *thread_hangcheck)
{
  thread_hangcheck->flags = 0;
  thread_hangcheck->num_threads = 0;

  g_atomic_int_set(&(thread_hangcheck->lock),
		   0);
  g_atomic_int_set(&(thread_hangcheck->monitor),
		   0);
  g_atomic_pointer_set(&(thread_hangcheck->current),
		       NULL);
}

void
ags_thread_hangcheck_connect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_thread_hangcheck_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_thread_hangcheck_finalize(GObject *gobject)
{
  AgsThreadHangcheck *thread_hangcheck;

  thread_hangcheck = AGS_THREAD_HANGCHECK(gobject);
  
  ags_list_free_and_unref_data(thread_hangcheck->thread);
}

gboolean
ags_thread_hangcheck_all_locked(AgsThreadHangcheckClass *thread_hangcheck)
{
  AgsThread *thread;
  GList *list;
  guint val;

  if(thread_hangcheck->gate == NULL){
    return(FALSE);
  }

  list = thread_hangcheck->gate;

  while(list != NULL){
    thread = AGS_RX_GATE(list->data)->thread;
    val = g_atomic_int_get(&(thread->flags));

    if((AGS_THREAD_WAIT_0 & val) == 0 &&
       (AGS_THREAD_WAIT_1 & val) == 0 &&
       (AGS_THREAD_WAIT_2 & val) == 0){
      return(FALSE);
    }

    list = list->next;
  }

  return(TRUE);
}

void
ags_thread_hangcheck_delegate(AgsThreadHangcheckClass *thread_hangcheck)
{
  AgsRXGate *rx_gate;
  gchar *message;
  gchar *control, *whence;
  guint value;

  if(thread_hangcheck == NULL){
    return;
  }

  message = thread_hangcheck->message;

  sscanf(message, "%s:fx(%x, %s)",
	 &whence, &value, &control);

  /* idle/quit */
  if(!strncmp(control,
	      AGS_RX_GATE_CTRL_IDLE,
	      6)){
    return;
  }else if(!strncmp(control,
		    AGS_RX_GATE_CTRL_QUIT,
		    6)){
    thread_hangcheck->flags &= (~AGS_THREAD_HANGCHECK_RUNNING);
    return;
  }

  /* notify */
  if(!strncmp(control,
	      AGS_RX_GATE_CTRL_UNSAFE,
	      6)){


    g_atomic_int_or(&(thread_hangcheck->monitor),
		    AGS_RX_GATE_SYN_X);

    //TODO:JK: implement me
  }else if(!strncmp(control,
	      AGS_RX_GATE_CTRL_SAFE,
	      6)){
    g_atomic_int_and(&(thread_hangcheck->monitor),
		     (~AGS_RX_GATE_SYN_X));

    //TODO:JK: implement me
  }else if(!strncmp(control,
	      AGS_RX_GATE_CTRL_FIRST,
	      6)){
    //TODO:JK: implement me
  }else if(!strncmp(control,
	      AGS_RX_GATE_CTRL_LAST,
	      6)){
    //TODO:JK: implement me
  }else if(!strncmp(control,
	      AGS_RX_GATE_CTRL_SYNCED,
	      6)){
    //TODO:JK: implement me
  }else if(!strncmp(control,
	      AGS_RX_GATE_CTRL_ASYNC,
	      6)){
    //TODO:JK: implement me
  }

  /* lock */
  if(!strncmp(control,
	      AGS_RX_GATE_CTRL_INIT,
	      6)){
    //TODO:JK: implement me
  }else if(!strncmp(control,
		    AGS_RX_GATE_CTRL_IRQ,
		    5)){
    //TODO:JK: implement me
  }else if(!strncmp(control,
		    AGS_RX_GATE_CTRL_EXIT,
		    6)){
    //TODO:JK: implement me
  }

  /* watch */
  if(!strncmp(control,
	      AGS_RX_GATE_CTRL_ABORT,
	      7)){
    //TODO:JK: implement me    
  }else if(!strncmp(control,
		    AGS_RX_GATE_CTRL_WAIT,
		    6)){
    //TODO:JK: implement me
  }else if(!strncmp(control,
		    AGS_RX_GATE_CTRL_EXIT,
		    6)){
    //TODO:JK: implement me
  }else if(!strncmp(control,
		    AGS_RX_GATE_CTRL_ERROR,
		    7)){
    //TODO:JK: implement me
  }

  /* check deadlock*/
  if((AGS_THREAD_HANGCHECK_DEADLOCK & (thread_hangcheck->flags)) != 0){
    if(ags_thread_hangcheck_all_locked(thread_hangcheck)){
      AgsThread *main_loop;
      GList *list;

      rx_gate = AGS_RX_GATE(thread_hangcheck->gate->data);
      main_loop = rx_gate->thread;

      /* resolve it */
      while(!ags_thread_tree_is_synced(main_loop)){
	list = thread_hangcheck->gate;

	while(list != NULL){
	  thread = AGS_RX_GATE(list->data)->thread;

	  if(!ags_thread_ok(thread)){
	    ags_thread_correct(thread);
	  }

	  if(!ags_thread_is_current_synced(thread)){
	    ags_thread_unlock(thread);
	  }

	  list = list->next;
	}
      }
    }
  }
}

void
ags_thread_hangcheck_lock_init(AgsThreadHangcheckClass *thread_hangcheck, AgsRXGate *rx_gate)
{
  //TODO:JK: implement me
}

void
ags_thread_hangcheck_lock_interupt(AgsThreadHangcheckClass *thread_hangcheck, AgsRXGate *rx_gate)
{
  //TODO:JK: implement me
}

void
ags_thread_hangcheck_lock_exit(AgsThreadHangcheckClass *thread_hangcheck, AgsRXGate *rx_gate)
{
  //TODO:JK: implement me
}

void
ags_thread_hangcheck_watch_abort(AgsThreadHangcheckClass *thread_hangcheck, AgsRXGate *rx_gate)
{
  //TODO:JK: implement me
}

void
ags_thread_hangcheck_watch_wait(AgsThreadHangcheckClass *thread_hangcheck, AgsRXGate *rx_gate)
{
  AgsThread *thread, *bind;
  gboolean parent_wait, sibling_wait, children_wait;
  gboolean monitor;

  thread = rx_gate->thread;

  deadlock = FALSE;
  monitor = TRUE;

  /* check parent */
  bind = thread->parent;
  parent_wait = FALSE;

  if(bind != NULL){
    AgsRXGate *bind_gate;

    if((AGS_THREAD_WAIT_0 & (bind->flags)) != 0 ||
       (AGS_THREAD_WAIT_1 & (bind->flags)) != 0 ||
       (AGS_THREAD_WAIT_2 & (bind->flags)) != 0){
      parent_wait = TRUE;
    }

    bind_gate = ags_rx_gate_find(thread_hangcheck->gate, bind);

    if(!parent_wait &&
       ((AGS_RX_SYN_R & (bind_gate->lock)) != 0 ||
	(AGS_RX_SYN_X & (bind_gate->lock)) != 0)){
      monitor = FALSE;
    }
  }

  /* check sibling */
  bind = ags_thread_first(thread);
  sibling_wait = FALSE;

  while(bind != NULL){
    if(bind == thread){
      bind = bind->next;

      continue;
    }


    if(bind != NULL){
      AgsRXGate *bind_gate;

      if(!((AGS_THREAD_WAIT_0 & (bind->flags)) != 0 ||
	   (AGS_THREAD_WAIT_1 & (bind->flags)) != 0 ||
	   (AGS_THREAD_WAIT_2 & (bind->flags)) != 0)){
	break;
      }

      bind_gate = ags_rx_gate_find(thread_hangcheck->gate, bind);

      if(((AGS_RX_SYN_R & (bind_gate->lock)) != 0 ||
	  (AGS_RX_SYN_X & (bind_gate->lock)) != 0)){
	monitor = FALSE;
      }
    }

    bind = bind->next;
  }

  if(bind == NULL){
    sibling_wait = TRUE;
  }

  /* check children */

  /* check for deadlock */
  if(parent_wait &&
     sibling_wait &&
     children_wait){
    if(!monitor){
      bind = ags_thread_get_toplevel(thread);

      ags_thread_reset_all(bind);
    }
  }
}

void
ags_thread_hangcheck_watch_exit(AgsThreadHangcheckClass *thread_hangcheck, AgsRXGate *rx_gate)
{
  //TODO:JK: implement me
}

void
ags_thread_hangcheck_watch_error(AgsThreadHangcheckClass *thread_hangcheck, AgsRXGate *rx_gate)
{
  //TODO:JK: implement me
}

AgsRXGate*
ags_rx_gate_alloc()
{
  //TODO:JK: implement me

  return(NULL);
}

void
ags_rx_gate_pop(AgsRXGate *rx_gate)
{
  //TODO:JK: implement me
}

void
ags_rx_gate_push(AgsRXGate *rx_gate,
		 gchar *message, guint64 value)
{
  //TODO:JK: implement me
}

AgsRXGate*
ags_rx_gate_find(GList *rx_gate, AgsThread *thread)
{
  while(rx_gate != NULL){
    if(AGS_RX_GATE(rx_gate->data)->thread == thread){
      return(AGS_RX_GATE(rx_gate->data));
    }

    rx_gate = rx_gate->next;
  }

  return(NULL);
}

AgsWatchPoint*
ags_watch_point_alloc()
{
  //TODO:JK: implement me

  return(NULL);
}

void
ags_thread_hangcheck_load(AgsThreadHangcheck *thread_hangcheck,
			  AgsThread *main_loop)
{
  //TODO:JK: implement me
}

gboolean
ags_thread_hangcheck_poll(AgsThreadHangcheck *thread_hangcheck,
			  guint **level,
			  guint mode)
{
  //TODO:JK: implement me

  return(FALSE);
}

gboolean
ags_thread_hangcheck_run(AgsThreadHangcheck *thread_hangcheck,
			 guint **level,
			 guint mode)
{
  //TODO:JK: implement me

  return(FALSE);
}

void
ags_thread_hangcheck_reset_all(AgsThreadHangcheck *thread_hangcheck)
{
  //TODO:JK: implement me
}

AgsThreadHangcheck*
ags_thread_hangcheck_new()
{
  AgsThreadHangcheck *thread_hangcheck;

  thread_hangcheck = (AgsThreadHangcheck *) g_object_new(AGS_TYPE_THREAD_HANGCHECK,
							 NULL);

  return(thread_hangcheck);
}

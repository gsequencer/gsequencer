#include "ags_recall.h"

#include "../object/ags_marshal.h"
#include "../object/ags_connectable.h"
#include "../object/ags_run_connectable.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

GType ags_recall_get_type();
void ags_recall_class_init(AgsRecallClass *recall_class);
void ags_recall_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable);
void ags_recall_init(AgsRecall *recall);
void ags_recall_connect(AgsConnectable *connectable);
void ags_recall_disconnect(AgsConnectable *connectable);
void ags_recall_run_connect(AgsRunConnectable *run_connectable);
void ags_recall_run_disconnect(AgsRunConnectable *run_connectable);
void ags_recall_finalize(GObject *recall);

void ags_recall_real_run_init_pre(AgsRecall *recall);
void ags_recall_real_run_init_inter(AgsRecall *recall);
void ags_recall_real_run_init_post(AgsRecall *recall);

void ags_recall_real_run_pre(AgsRecall *recall);
void ags_recall_real_run_inter(AgsRecall *recall);
void ags_recall_real_run_post(AgsRecall *recall);

void ags_recall_real_done(AgsRecall *recall);
void ags_recall_real_cancel(AgsRecall *recall);
void ags_recall_real_remove(AgsRecall *recall);

AgsRecall* ags_recall_real_duplicate(AgsRecall *reall, AgsRecallID *recall_id);

enum{
  RUN_INIT_PRE,
  RUN_INIT_INTER,
  RUN_INIT_POST,
  RUN_PRE,
  RUN_INTER,
  RUN_POST,
  DONE,
  LOOP,
  CANCEL,
  REMOVE,
  DUPLICATE,
  NOTIFY_DEPENDENCY,
  LAST_SIGNAL,
};

static gpointer ags_recall_parent_class = NULL;
static guint recall_signals[LAST_SIGNAL];

GType
ags_recall_get_type (void)
{
  static GType ags_type_recall = 0;

  if(!ags_type_recall){
    static const GTypeInfo ags_recall_info = {
      sizeof (AgsRecallClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecall),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_run_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recall = g_type_register_static(G_TYPE_OBJECT,
					     "AgsRecall\0", &ags_recall_info,
					     0);

    g_type_add_interface_static(ags_type_recall,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_recall,
				AGS_TYPE_RUN_CONNECTABLE,
				&ags_run_connectable_interface_info);
  }

  return(ags_type_recall);
}

void
ags_recall_class_init(AgsRecallClass *recall)
{
  GObjectClass *gobject;

  ags_recall_parent_class = g_type_class_peek_parent(recall);

  gobject = (GObjectClass *) recall;
  gobject->finalize = ags_recall_finalize;

  recall->run_init_pre = ags_recall_real_run_init_pre;
  recall->run_init_inter = ags_recall_real_run_init_inter;
  recall->run_init_post = ags_recall_real_run_init_post;

  recall->run_pre = ags_recall_real_run_pre;
  recall->run_inter = ags_recall_real_run_inter;
  recall->run_post = ags_recall_real_run_post;

  recall->done = ags_recall_real_done;
  recall->loop = NULL;

  recall->cancel = ags_recall_real_cancel;
  recall->remove = ags_recall_real_remove;

  recall->duplicate = ags_recall_real_duplicate;

  recall->notify_dependency = NULL;

  recall_signals[RUN_INIT_PRE] =
    g_signal_new("run_init_pre\0",
		 G_TYPE_FROM_CLASS (recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsRecallClass, run_init_pre),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  recall_signals[RUN_INIT_INTER] =
    g_signal_new("run_init_inter\0",
		 G_TYPE_FROM_CLASS (recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsRecallClass, run_init_inter),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  recall_signals[RUN_INIT_POST] =
    g_signal_new("run_init_post\0",
		 G_TYPE_FROM_CLASS (recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsRecallClass, run_init_post),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  recall_signals[RUN_PRE] =
    g_signal_new("run_pre\0",
		 G_TYPE_FROM_CLASS (recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsRecallClass, run_pre),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  recall_signals[RUN_INTER] =
    g_signal_new("run_inter\0",
		 G_TYPE_FROM_CLASS (recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsRecallClass, run_inter),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  recall_signals[RUN_POST] =
    g_signal_new("run_post\0",
		 G_TYPE_FROM_CLASS (recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsRecallClass, run_post),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  recall_signals[DONE] =
    g_signal_new("done\0",
		 G_TYPE_FROM_CLASS (recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsRecallClass, done),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  recall_signals[LOOP] =
    g_signal_new("loop\0",
		 G_TYPE_FROM_CLASS (recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsRecallClass, loop),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  recall_signals[CANCEL] =
    g_signal_new("cancel\0",
		 G_TYPE_FROM_CLASS (recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsRecallClass, cancel),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  recall_signals[REMOVE] =
    g_signal_new("remove\0",
		 G_TYPE_FROM_CLASS (recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsRecallClass, remove),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  recall_signals[DUPLICATE] =
    g_signal_new("duplicate\0",
		 G_TYPE_FROM_CLASS (recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsRecallClass, duplicate),
		 NULL, NULL,
		 g_cclosure_user_marshal_OBJECT__OBJECT,
		 G_TYPE_OBJECT, 1,
		 G_TYPE_OBJECT);

  recall_signals[NOTIFY_DEPENDENCY] =
    g_signal_new("notify\0",
		 G_TYPE_FROM_CLASS (recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsRecallClass, notify_dependency),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__UINT_INT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT, G_TYPE_INT);
}

void
ags_recall_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_recall_connect;
  connectable->disconnect = ags_recall_disconnect;
}

void
ags_recall_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable)
{
  run_connectable->connect = ags_recall_run_connect;
  run_connectable->disconnect = ags_recall_run_disconnect;
}

void
ags_recall_init(AgsRecall *recall)
{
  recall->flags = 0;

  recall->name = NULL;
  recall->recall_id = NULL;

  recall->parent = NULL;
  recall->child = NULL;
}

void
ags_recall_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_recall_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_recall_run_connect(AgsRunConnectable *run_connectable)
{
  /* empty */
}

void
ags_recall_run_disconnect(AgsRunConnectable *run_connectable)
{
  /* empty */
}

void
ags_recall_finalize(GObject *gobject)
{
  AgsRecall *recall;
  GList *list, *list_next;

  recall = AGS_RECALL(gobject);

  list = recall->child;

  while(list != NULL){
    list_next = list->next;

    g_object_unref(G_OBJECT(list->data));
    g_list_free1(list);

    list = list_next;
  }

  G_OBJECT_CLASS(ags_recall_parent_class)->finalize(gobject);
}

void
ags_recall_real_run_init_pre(AgsRecall *recall)
{
  GList *list;

  list = recall->child;

  while(list != NULL){
    ags_recall_run_init_pre(AGS_RECALL(list->data));

    list = list->next;
  }
}

void
ags_recall_run_init_pre(AgsRecall *recall)
{
  g_return_if_fail(AGS_IS_RECALL(recall));

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[RUN_INIT_PRE], 0);
  g_object_unref(G_OBJECT(recall));
}

void
ags_recall_real_run_init_inter(AgsRecall *recall)
{
  GList *list;

  list = recall->child;

  while(list != NULL){
    ags_recall_run_init_inter(AGS_RECALL(list->data));

    list = list->next;
  }
}

void
ags_recall_run_init_inter(AgsRecall *recall)
{
  g_return_if_fail(AGS_IS_RECALL(recall));

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[RUN_INIT_INTER], 0);
  g_object_unref(G_OBJECT(recall));
}

void
ags_recall_real_run_init_post(AgsRecall *recall)
{
  GList *list;

  list = recall->child;

  while(list != NULL){
    ags_recall_run_init_post(AGS_RECALL(list->data));

    list = list->next;
  }
}

void
ags_recall_run_init_post(AgsRecall *recall)
{
  g_return_if_fail(AGS_IS_RECALL(recall));

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[RUN_INIT_POST], 0);
  g_object_unref(G_OBJECT(recall));
}

void
ags_recall_real_run_pre(AgsRecall *recall)
{
  GList *list;

  list = recall->child;

  while(list != NULL){
    ags_recall_run_pre(AGS_RECALL(list->data));

    list = list->next;
  }
}

void
ags_recall_run_pre(AgsRecall *recall)
{
  g_return_if_fail(AGS_IS_RECALL(recall));

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[RUN_PRE], 0);
  g_object_unref(G_OBJECT(recall));
}

void
ags_recall_real_run_inter(AgsRecall *recall)
{
  GList *list;

  list = recall->child;

  while(list != NULL){
    ags_recall_run_inter(AGS_RECALL(list->data));

    list = list->next;
  }
}

void
ags_recall_run_inter(AgsRecall *recall)
{
  g_return_if_fail(AGS_IS_RECALL(recall));

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[RUN_INTER], 0);
  g_object_unref(G_OBJECT(recall));
}

void
ags_recall_real_run_post(AgsRecall *recall)
{
  GList *list, *list_next;

  list = recall->child;

  while(list != NULL){
    list_next = list->next;

    ags_recall_run_post(AGS_RECALL(list->data));

    list = list_next;
  }
}

void
ags_recall_run_post(AgsRecall *recall)
{
  g_return_if_fail(AGS_IS_RECALL(recall));

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[RUN_POST], 0);
  g_object_unref(G_OBJECT(recall));
}

void
ags_recall_real_done(AgsRecall *recall)
{
  recall->flags |= AGS_RECALL_DONE;
}

void
ags_recall_done(AgsRecall *recall)
{
  g_return_if_fail(AGS_IS_RECALL(recall));

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[DONE], 0);
  g_object_unref(G_OBJECT(recall));
}

void
ags_recall_loop(AgsRecall *recall)
{
  g_return_if_fail(AGS_IS_RECALL(recall));

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[LOOP], 0);
  g_object_unref(G_OBJECT(recall));
}

void
ags_recall_real_cancel(AgsRecall *recall)
{
  recall->flags |= AGS_RECALL_REMOVE;
}

void
ags_recall_cancel(AgsRecall *recall)
{
  g_return_if_fail(AGS_IS_RECALL(recall));

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[CANCEL], 0);
  g_object_unref(G_OBJECT(recall));
}

void
ags_recall_real_remove(AgsRecall *recall)
{
  AgsRecall *parent;

  fprintf(stdout, "remove: %s\n\0", G_OBJECT_TYPE_NAME(recall));

  if(recall->parent == NULL){
    g_object_unref(recall);
    return;
  }

  parent = AGS_RECALL(recall->parent);
  parent->child = g_list_remove(parent->child, recall);
  g_object_unref(recall);

  if((AGS_RECALL_PROPAGATE_DONE & (parent->flags)) != 0 && parent->child == NULL)
    ags_recall_done(parent);
}

void
ags_recall_remove(AgsRecall *recall)
{
  g_return_if_fail(AGS_IS_RECALL(recall));

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[REMOVE], 0);
  g_object_unref(G_OBJECT(recall));
}

AgsRecall*
ags_recall_real_duplicate(AgsRecall *recall, AgsRecallID *recall_id)
{
  AgsRecall *copy;
  AgsRecallClass *recall_class, *copy_class;
  GList *list;

  copy = g_object_new(G_OBJECT_TYPE(recall), NULL);

  copy->flags = (recall->flags & (~AGS_RECALL_TEMPLATE));
  copy->recall_id = recall_id;
  copy->parent = recall->parent;

  list = g_list_last(recall->child);

  while(list != NULL){
    copy->child = g_list_prepend(copy->child,
				 ags_recall_duplicate(AGS_RECALL(list->data), recall_id));

    list = list->prev;
  }

  recall_class = AGS_RECALL_GET_CLASS(recall);
  copy_class = AGS_RECALL_GET_CLASS(copy);

  copy_class->run_init_pre = recall_class->run_init_pre;
  copy_class->run_init_inter = recall_class->run_init_inter;
  copy_class->run_init_post = recall_class->run_init_post;

  copy_class->run_pre = recall_class->run_pre;
  copy_class->run_inter = recall_class->run_inter;
  copy_class->run_post = recall_class->run_post;

  copy_class->done = recall_class->done;
  copy_class->loop = recall_class->loop;

  copy_class->cancel = recall_class->cancel;
  copy_class->remove = recall_class->remove;

  copy_class->duplicate = recall_class->duplicate;

  return(copy);
}

AgsRecall*
ags_recall_duplicate(AgsRecall *recall, AgsRecallID *recall_id)
{
  AgsRecall *copy;

  g_return_val_if_fail(AGS_IS_RECALL(recall), NULL);

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[DUPLICATE], 0,
		recall_id,
		&copy);
  g_object_unref(G_OBJECT(recall));

  return(copy);
}

void
ags_recall_notify_dependency(AgsRecall *recall, guint flags, gint count)
{
  g_return_if_fail(AGS_IS_RECALL(recall));

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[NOTIFY_DEPENDENCY], 0,
		flags, count);
  g_object_unref(G_OBJECT(recall));
}

void
ags_recall_check_cancel(AgsRecall *recall)
{
  void ags_recall_check_cancel_recursive(AgsRecall *recall){
    GList *list;

    list = recall->child;

    while(list != NULL){
      ags_recall_check_cancel_recursive(AGS_RECALL(list->data));

      list = list->next;
    }

    if((AGS_RECALL_CANCEL & (recall->flags)) != 0)
      ags_recall_cancel(recall);
  }

  ags_recall_check_cancel_recursive(recall);
}

void
ags_recall_child_check_remove(AgsRecall *recall)
{
  GList *list;
  void ags_recall_check_remove_recursive(AgsRecall *recall){
    GList *list;

    list = recall->child;

    while(list != NULL){
      ags_recall_check_remove_recursive(AGS_RECALL(list->data));

      list = list->next;
    }

    if((AGS_RECALL_REMOVE & (recall->flags)) != 0)
      ags_recall_remove(recall);
  }

  list = recall->child;

  while(list != NULL){
    ags_recall_check_remove_recursive(AGS_RECALL(list->data));
    
    list = list->next;
  }
}

GList*
ags_recall_find_by_effect(GList *list, AgsRecallID *recall_id, char *effect)
{
  AgsRecall *recall;

  while(list != NULL){
    recall = AGS_RECALL(list->data);
    
    if(((recall_id != NULL &&
	 recall->recall_id != NULL &&
	 recall_id->group_id == recall->recall_id->group_id) ||
	(recall_id == NULL &&
	 recall->recall_id == NULL)) &&
	!g_strcmp0(G_OBJECT_TYPE_NAME(G_OBJECT(recall)), effect))
      return(list);

    list = list->next;
  }

  return(NULL);
}

GList*
ags_recall_find_type(GList *recall_i, GType type)
{
  AgsRecall *recall;

  while(recall_i != NULL){
    recall = AGS_RECALL(recall_i->data);

    if(G_TYPE_CHECK_INSTANCE_TYPE(recall, type))
      break;

    recall_i = recall_i->next;
  }

  return(recall_i);
}

GList*
ags_recall_find_type_with_group_id(GList *recall_i, GType type, guint group_id)
{
  AgsRecall *recall;

  while(recall_i != NULL){
    recall = AGS_RECALL(recall_i->data);

    if(G_TYPE_CHECK_INSTANCE_TYPE(recall, type) &&
       recall->recall_id != NULL &&
       recall->recall_id->group_id == group_id)
      break;

    recall_i = recall_i->next;
  }

  return(recall_i);
}

void
ags_recall_run_init(AgsRecall *recall, guint stage)
{
  if(stage == 0){
    g_signal_emit_by_name(G_OBJECT(recall), "run_init_pre\0");
  }else if(stage == 1){
    g_signal_emit_by_name(G_OBJECT(recall), "run_init_inter\0");
  }else{
    g_signal_emit_by_name(G_OBJECT(recall), "run_init_post\0");
  }
}

AgsRecall*
ags_recall_new()
{
  AgsRecall *recall;

  recall = (AgsRecall *) g_object_new(AGS_TYPE_RECALL, NULL);

  return(recall);
}

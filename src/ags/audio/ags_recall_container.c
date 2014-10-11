#include <ags/audio/ags_recall_container.h>

#include <ags/lib/ags_list.h>

#include <ags-lib/object/ags_connectable.h>
#include <ags/object/ags_packable.h>

#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_audio_run.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_recall_channel_run.h>

#include <ags/audio/recall/ags_play_notation_audio_run.h>

void ags_recall_container_class_init(AgsRecallContainerClass *recall_class);
void ags_recall_container_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_container_init(AgsRecallContainer *recall);
void ags_recall_container_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_recall_container_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
void ags_recall_container_connect(AgsConnectable *connectable);
void ags_recall_container_disconnect(AgsConnectable *connectable);
void ags_recall_container_finalize(GObject *recall);

/**
 * SECTION:ags_recall_container
 * @short_description: Container to group recalls
 * @title: AgsRecallContainer
 * @section_id:
 * @include: ags/audio/ags_recall_container.h
 *
 * #AgsRecallContainer groups recalls of different context.
 */

enum{
  PROP_0,
  PROP_RECALL_AUDIO_TYPE,
  PROP_RECALL_AUDIO,
  PROP_RECALL_AUDIO_RUN_TYPE,
  PROP_RECALL_AUDIO_RUN,
  PROP_RECALL_CHANNEL_TYPE,
  PROP_RECALL_CHANNEL,
  PROP_RECALL_CHANNEL_RUN_TYPE,
  PROP_RECALL_CHANNEL_RUN,
};

static gpointer ags_recall_container_parent_class = NULL;

GType
ags_recall_container_get_type (void)
{
  static GType ags_type_recall_container = 0;

  if(!ags_type_recall_container){
    static const GTypeInfo ags_recall_container_info = {
      sizeof (AgsRecallContainerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_container_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallContainer),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_container_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_container_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recall_container = g_type_register_static(G_TYPE_OBJECT,
						       "AgsRecallContainer\0",
						       &ags_recall_container_info,
						       0);

    g_type_add_interface_static(ags_type_recall_container,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_recall_container);
}

void
ags_recall_container_class_init(AgsRecallContainerClass *recall_container)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_recall_container_parent_class = g_type_class_peek_parent(recall_container);

  /* GObjectClass */
  gobject = (GObjectClass *) recall_container;

  gobject->finalize = ags_recall_container_finalize;

  gobject->set_property = ags_recall_container_set_property;
  gobject->get_property = ags_recall_container_get_property;

  /* properties */
  /**
   * AgsPort:recall-audio-type:
   *
   * The associated recall type within audio context.
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_gtype("recall-audio-type\0",
				  "audio level recall type\0",
				  "The recall type which this recall container has on audio level\0",
				   G_TYPE_NONE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_AUDIO_TYPE,
				  param_spec);

  /**
   * AgsPort:recall-audio:
   *
   * The associated recall within audio context.
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_object("recall-audio\0",
				   "audio level recall\0",
				   "The recall which this recall container has on audio level\0",
				   AGS_TYPE_RECALL_AUDIO,
				   G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_AUDIO,
				  param_spec);

  /**
   * AgsPort:recall-audio-run-type:
   *
   * The associated recall type within dynamic audio context.
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_gtype("recall-audio-run-type\0",
				  "audio runlevel recall type\0",
				  "The recall type which this recall container has on audio level during a run\0",
				  G_TYPE_NONE,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_AUDIO_RUN_TYPE,
				  param_spec);

  /**
   * AgsPort:recall-audio-run:
   *
   * The associated recall within dynamic audio context.
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_object("recall-audio-run\0",
				   "audio runlevel recall\0",
				   "The recall which this recall container has on audio level during a run\0",
				   AGS_TYPE_RECALL_AUDIO_RUN,
				   G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_AUDIO_RUN,
				  param_spec);

  /**
   * AgsPort:recall-channel-type:
   *
   * The associated recall type within channel context.
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_gtype("recall-channel-type\0",
				  "channel level recall type\0",
				  "The recall type which this recall container has on channel level\0",
				  G_TYPE_NONE,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_CHANNEL_TYPE,
				  param_spec);

  /**
   * AgsPort:recall-channel:
   *
   * The associated recall within channel context.
   * 
   * Since: 0.4.0
   */
 param_spec = g_param_spec_object("recall-channel\0",
				   "channel level recall\0",
				   "The recall which this recall container has on channel level\0",
				   AGS_TYPE_RECALL_CHANNEL,
				   G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_CHANNEL,
				  param_spec);

  /**
   * AgsPort:recall-channel-run-type:
   *
   * The associated recall type within dynamic channel context.
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_gtype("recall_channel_run_type\0",
				  "channel runlevel recall type\0",
				  "The recall type which this recall container has on audio level during a run\0",
				  G_TYPE_NONE,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_CHANNEL_RUN_TYPE,
				  param_spec);

  /**
   * AgsPort:recall-audio-run:
   *
   * The associated recall within dynamic channel context.
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_object("recall_channel_run\0",
				   "channel runlevel recall\0",
				   "The recall which this recall container has on audio level during a run\0",
				   AGS_TYPE_RECALL_CHANNEL_RUN,
				   G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_CHANNEL_RUN,
				  param_spec);
}

void
ags_recall_container_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_recall_container_connect;
  connectable->disconnect = ags_recall_container_disconnect;
}

void
ags_recall_container_init(AgsRecallContainer *recall_container)
{
  recall_container->flags = 0;

  recall_container->recall_audio = NULL;
  recall_container->recall_audio_run = NULL;
  recall_container->recall_channel = NULL;
  recall_container->recall_channel_run = NULL;
}

void
ags_recall_container_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsRecallContainer *recall_container;

  recall_container = AGS_RECALL_CONTAINER(gobject);

  switch(prop_id){
  case PROP_RECALL_AUDIO_TYPE:
    {
      GType recall_audio_type;

      recall_audio_type = (GType) g_value_get_gtype(value);

      recall_container->recall_audio_type = recall_audio_type;
    }
    break;
  case PROP_RECALL_AUDIO:
    {
      AgsRecallAudio *recall_audio;

      recall_audio = (AgsRecallAudio *) g_value_get_object(value);

      if(recall_container->recall_audio == recall_audio)
	return;

      if(recall_container->recall_audio != NULL){
	g_object_unref(G_OBJECT(recall_container->recall_audio));
      }

      if(recall_audio != NULL){
	g_object_ref(G_OBJECT(recall_audio));
      }

      recall_container->recall_audio = recall_audio;
    }
    break;
  case PROP_RECALL_AUDIO_RUN_TYPE:
    {
      GType recall_audio_run_type;

      recall_audio_run_type = g_value_get_gtype(value);

      recall_container->recall_audio_run_type = recall_audio_run_type;
    }
    break;
  case PROP_RECALL_AUDIO_RUN:
    {
      AgsRecallAudioRun *recall_audio_run;

      recall_audio_run = (AgsRecallAudioRun *) g_value_get_object(value);

      if(recall_audio_run == NULL ||
	 g_list_find(recall_container->recall_audio_run, recall_audio_run) != NULL)
	return;

      if(recall_audio_run != NULL){
	g_object_ref(G_OBJECT(recall_audio_run));
      }

      recall_container->recall_audio_run = g_list_prepend(recall_container->recall_audio_run, recall_audio_run);
    }
    break;
  case PROP_RECALL_CHANNEL_TYPE:
    {
      GType recall_channel_type;

      recall_channel_type = (GType) g_value_get_gtype(value);

      recall_container->recall_channel_type = recall_channel_type;
    }
    break;
  case PROP_RECALL_CHANNEL:
    {
      AgsRecallChannel *recall_channel;

      recall_channel = (AgsRecallChannel *) g_value_get_object(value);

      if(recall_channel == NULL ||
	 g_list_find(recall_container->recall_channel, recall_channel) != NULL)
	return;

	g_object_ref(G_OBJECT(recall_channel));

	recall_container->recall_channel = g_list_prepend(recall_container->recall_channel, recall_channel);
    }
    break;
  case PROP_RECALL_CHANNEL_RUN_TYPE:
    {
      GType recall_channel_run_type;

      recall_channel_run_type = (GType) g_value_get_gtype(value);

      recall_container->recall_channel_run_type = recall_channel_run_type;
    }
    break;
  case PROP_RECALL_CHANNEL_RUN:
    {
      AgsRecallChannelRun *recall_channel_run;

      recall_channel_run = (AgsRecallChannelRun *) g_value_get_object(value);

      if(recall_channel_run == NULL ||
	 g_list_find(recall_container->recall_channel_run, recall_channel_run) != NULL)
	return;

      g_object_ref(G_OBJECT(recall_channel_run));

      recall_container->recall_channel_run = g_list_prepend(recall_container->recall_channel_run, recall_channel_run);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recall_container_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec)
{
  AgsRecallContainer *recall_container;

  recall_container = AGS_RECALL_CONTAINER(gobject);

  switch(prop_id){
  case PROP_RECALL_AUDIO_TYPE:
    g_value_set_gtype(value, recall_container->recall_audio_type);
    break;
  case PROP_RECALL_AUDIO_RUN_TYPE:
    g_value_set_gtype(value, recall_container->recall_audio_run_type);
    break;
  case PROP_RECALL_CHANNEL_TYPE:
    g_value_set_gtype(value, recall_container->recall_channel_type);
    break;
  case PROP_RECALL_CHANNEL_RUN_TYPE:
    g_value_set_gtype(value, recall_container->recall_channel_run_type);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recall_container_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_recall_container_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_recall_container_finalize(GObject *gobject)
{
  AgsRecallContainer *container;
  GList *list, *list_next;

  container = AGS_RECALL_CONTAINER(gobject);

  ags_packable_unpack(AGS_PACKABLE(container->recall_audio));

  list = container->recall_audio_run;

  while(list != NULL){
    list_next = list->next;

    ags_packable_unpack(AGS_PACKABLE(container->recall_audio_run));

    list = list_next;
  }

  list = container->recall_channel;

  while(list != NULL){
    list_next = list->next;

    ags_packable_unpack(AGS_PACKABLE(container->recall_channel));

    list = list_next;
  }

  list = container->recall_channel_run;

  while(list != NULL){
    list_next = list->next;

    ags_packable_unpack(AGS_PACKABLE(container->recall_channel_run));

    list = list_next;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_recall_container_parent_class)->finalize(gobject);
}

/**
 * ags_recall_container_get_recall_audio:
 * @container: the #AgsRecallContainer
 *
 * Retrieve recall audio of container.
 *
 * Returns: the #AgsRecallAudio
 *
 * Since: 0.4
 */
AgsRecall*
ags_recall_container_get_recall_audio(AgsRecallContainer *container)
{
  return(container->recall_audio);
}

/**
 * ags_recall_container_get_recall_audio_run:
 * @container: the #AgsRecallContainer
 *
 * Retrieve recall audio run of container.
 *
 * Returns: the #AgsRecallAudioRun as list
 *
 * Since: 0.4
 */
GList*
ags_recall_container_get_recall_audio_run(AgsRecallContainer *container)
{
  return(container->recall_audio_run);
}

/**
 * ags_recall_container_get_recall_channel:
 * @container: the #AgsRecallContainer
 *
 * Retrieve the recall channel of container.
 *
 * Returns: the #AgsRecallChannel
 *
 * Since: 0.4
 */
GList*
ags_recall_container_get_recall_channel(AgsRecallContainer *container)
{
  return(container->recall_channel);
}

/**
 * ags_recall_container_get_recall_channel_run:
 * @container: the #AgsRecallContainer
 *
 * Retrieve the recall channel run of container.
 *
 * Returns: the #AgsRecall
 *
 * Since: 0.4
 */
GList*
ags_recall_container_get_recall_channel_run(AgsRecallContainer *container)
{
  return(container->recall_channel_run);
}

/**
 * ags_recall_container_find:
 * @recall_container: the #AgsRecallContainer
 * @type: recall type
 * @find_flags: search mask
 * @recall_id: an #AgsRecallID
 *
 * Finds #AgsRecall for appropriate search criteria.
 *
 * Returns: the matching recalls
 *
 * Since: 0.4
 */
GList*
ags_recall_container_find(GList *recall_container,
			  GType type,
			  guint find_flags,
			  AgsRecallID *recall_id)
{
  AgsRecallContainer *current;
  AgsRecall *recall;
  guint mode;

  if(g_type_is_a(type, AGS_TYPE_RECALL_AUDIO)){
    mode = 0;
  }else if(g_type_is_a(type, AGS_TYPE_RECALL_AUDIO_RUN)){
    mode = 1;
  }else if(g_type_is_a(type, AGS_TYPE_RECALL_CHANNEL)){
    mode = 2;
  }else if(g_type_is_a(type, AGS_TYPE_RECALL_CHANNEL_RUN)){
    mode = 3;
  }else{
    g_message("ags_recall_container_find: invalid type\n\0");
    return(NULL);
  }

  while(recall_container != NULL){
    current = AGS_RECALL_CONTAINER(recall_container->data);

    if(mode == 0){
      recall = ags_recall_container_get_recall_audio(current);
    }else if(mode == 1){
      GList *list;

      list = ags_recall_container_get_recall_audio_run(current);
	
      if(list == NULL)
	recall = NULL;
      else
	recall = AGS_RECALL(list->data);
    }else if(mode == 2){
      GList *list;

      list = ags_recall_container_get_recall_channel(current);

      if(list == NULL)
	recall = NULL;
      else
	recall = AGS_RECALL(list->data);
    }else if(mode == 3){
      GList *list;

      list = ags_recall_container_get_recall_channel_run(current);

      if(list == NULL)
	recall = NULL;
      else
	recall = AGS_RECALL(list->data);
    }
   
    if(recall != NULL){
      if(((AGS_RECALL_CONTAINER_FIND_TYPE & find_flags) == 0 || G_OBJECT_TYPE(recall) == type) &&
	 ((AGS_RECALL_CONTAINER_FIND_TEMPLATE & find_flags) == 0 || (AGS_RECALL_TEMPLATE & (recall->flags)) != 0) &&
	 ((AGS_RECALL_CONTAINER_FIND_RECALL_ID & find_flags) == 0 || (recall->recall_id != NULL && recall->recall_id == recall_id))){
	break;
      }
    }

    recall_container = recall_container->next;
  }

  return(recall_container);
}

/**
 * ags_recall_container_new:
 * 
 * Creates an #AgsRecallContainer
 *
 * Returns: a new #AgsRecallContainer
 *
 * Since: 0.4
 */
AgsRecallContainer*
ags_recall_container_new()
{
  AgsRecallContainer *recall_container;

  recall_container = (AgsRecallContainer *) g_object_new(AGS_TYPE_RECALL_CONTAINER,
							 NULL);

  return(recall_container);
}

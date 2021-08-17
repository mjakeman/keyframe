#include "keyframe-composition.h"

typedef struct
{
    char *title;
    GSList *layers;

    int width, height;
    float framerate;
} KeyframeCompositionPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (KeyframeComposition, keyframe_composition, G_TYPE_OBJECT)

enum {
    PROP_0,
    PROP_TITLE,
    PROP_WIDTH,
    PROP_HEIGHT,
    PROP_FRAMERATE,
    N_PROPS
};

static GParamSpec *properties [N_PROPS];

enum
{
	CHANGED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL];

/**
 * keyframe_composition_new:
 *
 * Create a new #KeyframeComposition.
 *
 * Returns: (transfer full): a newly created #KeyframeComposition
 */
KeyframeComposition *
keyframe_composition_new (const char *title, int width, int height, float framerate)
{
    return g_object_new (KEYFRAME_TYPE_COMPOSITION,
                         "title", title,
                         "width", width,
                         "height", height,
                         "framerate", framerate,
                         NULL);
}

static void
keyframe_composition_finalize (GObject *object)
{
    KeyframeComposition *self = (KeyframeComposition *)object;
    KeyframeCompositionPrivate *priv = keyframe_composition_get_instance_private (self);

    g_slist_free_full (g_steal_pointer (&priv->layers), g_object_unref);

    G_OBJECT_CLASS (keyframe_composition_parent_class)->finalize (object);
}

static void
keyframe_composition_get_property (GObject    *object,
                                   guint       prop_id,
                                   GValue     *value,
                                   GParamSpec *pspec)
{
    KeyframeComposition *self = KEYFRAME_COMPOSITION (object);
    KeyframeCompositionPrivate *priv = keyframe_composition_get_instance_private (self);

    switch (prop_id)
      {
      case PROP_TITLE:
          g_value_set_string (value, priv->title);
          break;
      case PROP_WIDTH:
          g_value_set_int (value, priv->width);
          break;
      case PROP_HEIGHT:
          g_value_set_int (value, priv->height);
          break;
      case PROP_FRAMERATE:
          g_value_set_float (value, priv->framerate);
          break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      }
}

static void
keyframe_composition_set_property (GObject      *object,
                                   guint         prop_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
    KeyframeComposition *self = KEYFRAME_COMPOSITION (object);
    KeyframeCompositionPrivate *priv = keyframe_composition_get_instance_private (self);

    switch (prop_id)
      {
      case PROP_TITLE:
          priv->title = g_value_dup_string (value);
          break;
      case PROP_WIDTH:
          priv->width = g_value_get_int (value);
          break;
      case PROP_HEIGHT:
          priv->height = g_value_get_int (value);
          break;
      case PROP_FRAMERATE:
          priv->framerate = g_value_get_float (value);
          break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      }
}

void
keyframe_composition_push_layer (KeyframeComposition *self, KeyframeLayer *layer)
{
    KeyframeCompositionPrivate *priv = keyframe_composition_get_instance_private (self);
    priv->layers = g_slist_append (priv->layers, layer); // TODO: Prepend?
    g_signal_emit (self, signals[CHANGED], 0);
}

void
keyframe_composition_delete_layer (KeyframeComposition *self, KeyframeLayer *layer)
{
    KeyframeCompositionPrivate *priv = keyframe_composition_get_instance_private (self);
    priv->layers = g_slist_remove (priv->layers, layer);
    g_signal_emit (self, signals[CHANGED], 0);
}

GSList *
keyframe_composition_get_layers (KeyframeComposition *self)
{
    KeyframeCompositionPrivate *priv = keyframe_composition_get_instance_private (self);
    return priv->layers;
}

static void
keyframe_composition_class_init (KeyframeCompositionClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = keyframe_composition_finalize;
    object_class->get_property = keyframe_composition_get_property;
    object_class->set_property = keyframe_composition_set_property;

    properties [PROP_TITLE] = g_param_spec_string ("title", "Title", "Title", NULL, G_PARAM_READWRITE);
    properties [PROP_WIDTH] = g_param_spec_int ("width", "Width", "Width", 0, G_MAXINT, 0, G_PARAM_READWRITE);
    properties [PROP_HEIGHT] = g_param_spec_int ("height", "Height", "Height", 0, G_MAXINT, 0, G_PARAM_READWRITE);
    properties [PROP_FRAMERATE] = g_param_spec_float ("framerate", "Framerate", "Framerate", 0, 1000, 0, G_PARAM_READWRITE);

    g_object_class_install_properties (object_class, N_PROPS, properties);

    signals[CHANGED] =
        g_signal_newv ("changed",
                 G_TYPE_FROM_CLASS (object_class),
                 G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                 NULL /* closure */,
                 NULL /* accumulator */,
                 NULL /* accumulator data */,
                 NULL /* C marshaller */,
                 G_TYPE_NONE /* return_type */,
                 0     /* n_params */,
                 NULL  /* param_types */);
}

static void
keyframe_composition_init (KeyframeComposition *self)
{
    KeyframeCompositionPrivate *priv = keyframe_composition_get_instance_private (self);

    priv->layers = NULL;
}

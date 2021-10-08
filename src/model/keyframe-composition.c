#include "keyframe-composition.h"

typedef struct
{
    char *title;
    GList *layers;

    int width, height;
    float framerate;

    // TODO: We likely don't want composition controlling the current
    // time. That should either be a feature of composition manager or
    // an auxiliary class e.g. some kind of CompositionObserver.
    float current_time;
} KeyframeCompositionPrivate;

static void keyframe_composition_list_model_init (GListModelInterface *iface);

G_DEFINE_TYPE_WITH_CODE (KeyframeComposition, keyframe_composition, G_TYPE_OBJECT,
                         G_ADD_PRIVATE (KeyframeComposition)
                         G_IMPLEMENT_INTERFACE (G_TYPE_LIST_MODEL,
                                                keyframe_composition_list_model_init))

enum {
    PROP_0,
    PROP_TITLE,
    PROP_WIDTH,
    PROP_HEIGHT,
    PROP_FRAMERATE,
    PROP_CURRENT_TIME,
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

    g_list_free_full (g_steal_pointer (&priv->layers), g_object_unref);

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
      case PROP_CURRENT_TIME:
          g_value_set_float(value, priv->current_time);
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
      case PROP_CURRENT_TIME:
          priv->current_time = g_value_get_float(value);
          break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }

    // Invalidate the composition
    g_signal_emit (self, signals[CHANGED], 0);
}

void
keyframe_composition_push_layer (KeyframeComposition *self, KeyframeLayer *layer)
{
    KeyframeComposition *existing;
    g_object_get (layer, "composition", &existing, NULL);
    if (existing != NULL)
    {
        g_critical ("Layer already has a parent composition");
        return;
    }

    KeyframeCompositionPrivate *priv = keyframe_composition_get_instance_private (self);
    int final_index = g_list_length (priv->layers) - 1;

    g_object_set (layer, "composition", g_object_ref (self), NULL);
    priv->layers = g_list_append (priv->layers, layer); // TODO: Prepend?

    g_list_model_items_changed (G_LIST_MODEL (self), final_index, 0, 1);
    g_signal_emit (self, signals[CHANGED], 0);
}

void
keyframe_composition_delete_layer (KeyframeComposition *self, KeyframeLayer *layer)
{
    KeyframeCompositionPrivate *priv = keyframe_composition_get_instance_private (self);
    guint index = g_list_index (priv->layers, layer);

    priv->layers = g_list_remove (priv->layers, layer);
    g_object_unref (layer);

    g_list_model_items_changed (G_LIST_MODEL (self), index, 1, 0);
    g_signal_emit (self, signals[CHANGED], 0);
}

/**
 * keyframe_composition_get_layers:
 *
 * Retrieve all the layers in the composition. The caller must *not* modify
 * or free the list, and doing so is an error.
 *
 * Returns: The layers in this composition
 */
GList *
keyframe_composition_get_layers (KeyframeComposition *self)
{
    KeyframeCompositionPrivate *priv = keyframe_composition_get_instance_private (self);
    return priv->layers;
}

void
keyframe_composition_invalidate (KeyframeComposition *self)
{
    g_signal_emit (self, signals[CHANGED], 0);
}

static gpointer
list_model_get_item (GListModel* list, guint position)
{
    KeyframeComposition *self = KEYFRAME_COMPOSITION (list);
    KeyframeCompositionPrivate *priv = keyframe_composition_get_instance_private (self);

    // TODO: We should store the beginning and end items
    guint length = g_list_length (priv->layers);
    guint index = (length - 1) - position;
    g_assert (index >= 0 && index < length);

    if (index >= 0 && index < length)
    {
        KeyframeLayer *layer = g_list_nth_data (priv->layers, index);
        g_print ("%d of %d: %s\n", position+1, length, g_type_name_from_instance ((GTypeInstance *)layer));
        g_assert (KEYFRAME_IS_LAYER (layer));

        // Make sure to ref the object!
        return g_object_ref (layer);
    }

    // Invalid, return NULL
    return NULL;
}

static GType
list_model_get_item_type (GListModel*)
{
    g_type_ensure (KEYFRAME_TYPE_LAYER);
    g_type_ensure (KEYFRAME_TYPE_LAYER_COOL);
    g_type_ensure (KEYFRAME_TYPE_LAYER_GEOMETRY);
    g_type_ensure (KEYFRAME_TYPE_LAYER_TEXT);
    return keyframe_layer_get_type ();
}

static guint
list_model_get_n_items (GListModel* list)
{
    KeyframeComposition *self = KEYFRAME_COMPOSITION (list);
    KeyframeCompositionPrivate *priv = keyframe_composition_get_instance_private (self);

    return g_list_length (priv->layers);
}

static void
keyframe_composition_list_model_init (GListModelInterface *iface)
{
    iface->get_n_items = list_model_get_n_items;
    iface->get_item = list_model_get_item;
    iface->get_item_type = list_model_get_item_type;
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
    properties [PROP_CURRENT_TIME] = g_param_spec_float ("current-time", "Current Time", "Current Time", 0, G_MAXFLOAT, 0, G_PARAM_READWRITE);

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

#include "keyframe-composition-manager.h"

#include "model/keyframe-layers.h"

typedef struct
{
    GSList* compositions;
    KeyframeComposition *current;
} KeyframeCompositionManagerPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (KeyframeCompositionManager, keyframe_composition_manager, G_TYPE_OBJECT)

enum {
    PROP_0,
    N_PROPS
};

static GParamSpec *properties [N_PROPS];

enum {
    CURRENT_CHANGED,
    N_SIGNALS
};

static uint signals [N_SIGNALS];

/**
 * keyframe_composition_manager_new:
 *
 * Create a new #KeyframeCompositionManager.
 *
 * Returns: (transfer full): a newly created #KeyframeCompositionManager
 */
KeyframeCompositionManager *
keyframe_composition_manager_new (void)
{
    return g_object_new (KEYFRAME_TYPE_COMPOSITION_MANAGER, NULL);
}

static void
keyframe_composition_manager_finalize (GObject *object)
{
    KeyframeCompositionManager *self = (KeyframeCompositionManager *)object;
    KeyframeCompositionManagerPrivate *priv = keyframe_composition_manager_get_instance_private (self);

    G_OBJECT_CLASS (keyframe_composition_manager_parent_class)->finalize (object);
}

static void
keyframe_composition_manager_get_property (GObject    *object,
                                           guint       prop_id,
                                           GValue     *value,
                                           GParamSpec *pspec)
{
    KeyframeCompositionManager *self = KEYFRAME_COMPOSITION_MANAGER (object);

    switch (prop_id)
      {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      }
}

static void
keyframe_composition_manager_set_property (GObject      *object,
                                           guint         prop_id,
                                           const GValue *value,
                                           GParamSpec   *pspec)
{
    KeyframeCompositionManager *self = KEYFRAME_COMPOSITION_MANAGER (object);

    switch (prop_id)
      {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      }
}

KeyframeComposition *
keyframe_composition_manager_new_composition (KeyframeCompositionManager *self)
{
    static guint count;

    KeyframeCompositionManagerPrivate *priv = keyframe_composition_manager_get_instance_private (self);

    // Default Project Properties (get from gsettings?)
    char *title = g_strdup_printf ("Composition %u", ++count);
    KeyframeComposition *composition = keyframe_composition_new (title, 1920, 1080, 30);

    keyframe_composition_push_layer (composition, KEYFRAME_LAYER (keyframe_layer_geometry_new ("layer1")));
    keyframe_composition_push_layer (composition, KEYFRAME_LAYER (keyframe_layer_text_new ("layer2")));

    priv->compositions = g_slist_append (priv->compositions, g_object_ref (composition));
    priv->current = composition;
    g_signal_emit (self, signals[CURRENT_CHANGED], 0);

    return composition;
}

KeyframeComposition *
keyframe_composition_manager_get_current (KeyframeCompositionManager *self)
{
    KeyframeCompositionManagerPrivate *priv = keyframe_composition_manager_get_instance_private (self);
    return priv->current;
}

static void
keyframe_composition_manager_class_init (KeyframeCompositionManagerClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = keyframe_composition_manager_finalize;
    object_class->get_property = keyframe_composition_manager_get_property;
    object_class->set_property = keyframe_composition_manager_set_property;

    signals [CURRENT_CHANGED] =
        g_signal_new ("current-changed",
                      G_TYPE_FROM_CLASS (klass),
                      G_SIGNAL_RUN_LAST,
                      0,
                      NULL,
                      NULL,
                      g_cclosure_marshal_generic,
                      G_TYPE_NONE,
                      0);
}

static void
keyframe_composition_manager_init (KeyframeCompositionManager *self)
{
    KeyframeCompositionManagerPrivate *priv = keyframe_composition_manager_get_instance_private (self);
    priv->current = NULL;
}

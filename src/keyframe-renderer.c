#include "keyframe-renderer.h"

typedef struct
{
    cairo_surface_t *surface;
    cairo_t *cr;
} KeyframeRendererPrivate;

G_DEFINE_FINAL_TYPE_WITH_PRIVATE (KeyframeRenderer, keyframe_renderer, G_TYPE_OBJECT)

enum {
    PROP_0,
    N_PROPS
};

static GParamSpec *properties [N_PROPS];

/**
 * keyframe_renderer_new:
 *
 * Create a new #KeyframeRenderer.
 *
 * Returns: (transfer full): a newly created #KeyframeRenderer
 */
KeyframeRenderer *
keyframe_renderer_new (void)
{
    return g_object_new (KEYFRAME_TYPE_RENDERER, NULL);
}

static void
keyframe_renderer_finalize (GObject *object)
{
    KeyframeRenderer *self = (KeyframeRenderer *)object;
    KeyframeRendererPrivate *priv = keyframe_renderer_get_instance_private (self);

    G_OBJECT_CLASS (keyframe_renderer_parent_class)->finalize (object);
}

static void
keyframe_renderer_get_property (GObject    *object,
                                guint       prop_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
    KeyframeRenderer *self = KEYFRAME_RENDERER (object);

    switch (prop_id)
      {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      }
}

static void
keyframe_renderer_set_property (GObject      *object,
                                guint         prop_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
    KeyframeRenderer *self = KEYFRAME_RENDERER (object);

    switch (prop_id)
      {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      }
}

static void
keyframe_renderer_class_init (KeyframeRendererClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = keyframe_renderer_finalize;
    object_class->get_property = keyframe_renderer_get_property;
    object_class->set_property = keyframe_renderer_set_property;
}

void
keyframe_renderer_begin_frame (KeyframeRenderer *self)
{
    int width = 800;
    int height = 600;

    KeyframeRendererPrivate *priv = keyframe_renderer_get_instance_private (self);
    priv->surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);
    priv->cr = cairo_create (priv->surface);

    cairo_scale (priv->cr, width, height);
}

cairo_surface_t *
keyframe_renderer_end_frame (KeyframeRenderer *self)
{
    KeyframeRendererPrivate *priv = keyframe_renderer_get_instance_private (self);
    cairo_surface_t *result = priv->surface;

    priv->surface = NULL;
    priv->cr = NULL;

    return result;
}

cairo_t *keyframe_renderer_get_cairo (KeyframeRenderer *self)
{
    KeyframeRendererPrivate *priv = keyframe_renderer_get_instance_private (self);

    if (priv->cr)
        return priv->cr;

    g_critical ("No surface to draw to. Have you called 'keyframe_renderer_begin_frame()'?\n");
}

static void
keyframe_renderer_init (KeyframeRenderer *self)
{

}

#include "keyframe-layer-geometry.h"

typedef struct
{
    float width, height;
} KeyframeLayerGeometryPrivate;

G_DEFINE_FINAL_TYPE_WITH_PRIVATE (KeyframeLayerGeometry, keyframe_layer_geometry, KEYFRAME_TYPE_LAYER)

enum {
    PROP_0,
    PROP_WIDTH,
    PROP_HEIGHT,
    N_PROPS
};

static GParamSpec *properties [N_PROPS];

/**
 * keyframe_layer_geometry_new:
 *
 * Create a new #KeyframeLayerGeometry. A solid coloured rectangle.
 *
 * Returns: (transfer full): a newly created #KeyframeLayer
 */
KeyframeLayer *
keyframe_layer_geometry_new (const char *name, float width, float height)
{
    return g_object_new (KEYFRAME_TYPE_LAYER_GEOMETRY,
                         "name", name,
                         "width", width,
                         "height", height,
                         NULL);
}

static void
keyframe_layer_geometry_finalize (GObject *object)
{
    KeyframeLayerGeometry *self = (KeyframeLayerGeometry *)object;
    KeyframeLayerGeometryPrivate *priv = keyframe_layer_geometry_get_instance_private (self);

    G_OBJECT_CLASS (keyframe_layer_geometry_parent_class)->finalize (object);
}

static void
keyframe_layer_geometry_get_property (GObject    *object,
                                      guint       prop_id,
                                      GValue     *value,
                                      GParamSpec *pspec)
{
    KeyframeLayerGeometry *self = KEYFRAME_LAYER_GEOMETRY (object);
    KeyframeLayerGeometryPrivate *priv = keyframe_layer_geometry_get_instance_private (self);

    switch (prop_id)
    {
    case PROP_WIDTH:
        g_value_set_float (value, priv->width);
        break;
    case PROP_HEIGHT:
        g_value_set_float (value, priv->height);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
keyframe_layer_geometry_set_property (GObject      *object,
                                      guint         prop_id,
                                      const GValue *value,
                                      GParamSpec   *pspec)
{
    KeyframeLayerGeometry *self = KEYFRAME_LAYER_GEOMETRY (object);
    KeyframeLayerGeometryPrivate *priv = keyframe_layer_geometry_get_instance_private (self);

    switch (prop_id)
    {
    case PROP_WIDTH:
        priv->width = g_value_get_float (value);
        break;
    case PROP_HEIGHT:
        priv->height = g_value_get_float (value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
keyframe_layer_geometry_fill_command_buffer (KeyframeLayer *self, KeyframeRenderer *renderer)
{
    cairo_t *cr = keyframe_renderer_get_cairo (renderer);

    cairo_save (cr);

    float x, y, width, height;
    g_object_get (self,
                  "x", &x,
                  "y", &y,
                  "width", &width,
                  "height", &height,
                  NULL);
    cairo_translate (cr, x, y);

    cairo_set_source_rgb (cr, 1, 1, 1);
    cairo_rectangle (cr, 0, 0, width, height);
    cairo_fill (cr);

    cairo_restore (cr);

}

static const char *
keyframe_layer_geometry_type (KeyframeLayer *layer)
{
    return "Geometry";
}

static void
keyframe_layer_geometry_class_init (KeyframeLayerGeometryClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = keyframe_layer_geometry_finalize;
    object_class->get_property = keyframe_layer_geometry_get_property;
    object_class->set_property = keyframe_layer_geometry_set_property;

    KeyframeLayerClass *layer_class = KEYFRAME_LAYER_CLASS (klass);

    layer_class->fill_command_buffer = keyframe_layer_geometry_fill_command_buffer;
    layer_class->type = keyframe_layer_geometry_type;

    properties [PROP_WIDTH] =
        g_param_spec_float ("width",
                            "Width",
                            "Width of the layer in pixels.",
                            0, G_MAXFLOAT, 0.0f,
                            G_PARAM_READWRITE);

    properties [PROP_HEIGHT] =
        g_param_spec_float ("height",
                            "Height",
                            "Height of the layer in pixels.",
                            0, G_MAXFLOAT, 0.0f,
                            G_PARAM_READWRITE);

    g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
keyframe_layer_geometry_init (KeyframeLayerGeometry *self)
{
}

#include "keyframe-layer-cool.h"

typedef struct
{
    int padding;
} KeyframeLayerCoolPrivate;

G_DEFINE_FINAL_TYPE_WITH_PRIVATE (KeyframeLayerCool, keyframe_layer_cool, KEYFRAME_TYPE_LAYER)

enum {
    PROP_0,
    N_PROPS
};

static GParamSpec *properties [N_PROPS];

/**
 * keyframe_layer_cool_new:
 *
 * Create a new #KeyframeLayerCool. It doesn't do anything, but it
 * looks cool.
 *
 * Returns: (transfer full): a newly created #KeyframeLayer
 */
KeyframeLayer *
keyframe_layer_cool_new (const char *name)
{
    return g_object_new (KEYFRAME_TYPE_LAYER_COOL,
                         "name", name,
                         NULL);
}

static void
keyframe_layer_cool_finalize (GObject *object)
{
    KeyframeLayerCool *self = (KeyframeLayerCool *)object;
    KeyframeLayerCoolPrivate *priv = keyframe_layer_cool_get_instance_private (self);

    G_OBJECT_CLASS (keyframe_layer_cool_parent_class)->finalize (object);
}

static void
keyframe_layer_cool_get_property (GObject    *object,
                                      guint       prop_id,
                                      GValue     *value,
                                      GParamSpec *pspec)
{
    KeyframeLayerCool *self = KEYFRAME_LAYER_COOL (object);

    switch (prop_id)
      {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      }
}

static void
keyframe_layer_cool_set_property (GObject      *object,
                                      guint         prop_id,
                                      const GValue *value,
                                      GParamSpec   *pspec)
{
    KeyframeLayerCool *self = KEYFRAME_LAYER_COOL (object);

    switch (prop_id)
      {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      }
}

static void
keyframe_layer_cool_fill_command_buffer (KeyframeLayer *self, KeyframeRenderer *renderer)
{
    cairo_t *cr = keyframe_renderer_get_cairo (renderer);

    int width = keyframe_renderer_get_width (renderer);
    int height = keyframe_renderer_get_height (renderer);

    cairo_save (cr);

    float x, y;
    g_object_get (self, "x", &x, "y", &y, NULL);
    cairo_translate (cr, x, y);

    cairo_scale (cr, width, height);

    int i, j;
    cairo_pattern_t *radpat, *linpat;

    radpat = cairo_pattern_create_radial (0.25, 0.25, 0.1,  0.5, 0.5, 0.5);
    cairo_pattern_add_color_stop_rgb (radpat, 0,  1.0, 0.8, 0.8);
    cairo_pattern_add_color_stop_rgb (radpat, 1,  0.9, 0.0, 0.0);

    for (i=1; i<10; i++)
        for (j=1; j<10; j++)
            cairo_rectangle (cr, i/10.0 - 0.04, j/10.0 - 0.04, 0.08, 0.08);
    cairo_set_source (cr, radpat);
    cairo_fill (cr);

    linpat = cairo_pattern_create_linear (0.25, 0.35, 0.75, 0.65);
    cairo_pattern_add_color_stop_rgba (linpat, 0.00,  1, 1, 1, 0);
    cairo_pattern_add_color_stop_rgba (linpat, 0.25,  0, 1, 0, 0.5);
    cairo_pattern_add_color_stop_rgba (linpat, 0.50,  1, 1, 1, 0);
    cairo_pattern_add_color_stop_rgba (linpat, 0.75,  0, 0, 1, 0.5);
    cairo_pattern_add_color_stop_rgba (linpat, 1.00,  1, 1, 1, 0);

    cairo_rectangle (cr, 0.0, 0.0, 1, 1);
    cairo_set_source (cr, linpat);
    cairo_fill (cr);

    cairo_restore (cr);

}

static const char *
keyframe_layer_cool_type (KeyframeLayer *layer)
{
    return "Cool";
}

static void
keyframe_layer_cool_class_init (KeyframeLayerCoolClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = keyframe_layer_cool_finalize;
    object_class->get_property = keyframe_layer_cool_get_property;
    object_class->set_property = keyframe_layer_cool_set_property;

    KeyframeLayerClass *layer_class = KEYFRAME_LAYER_CLASS (klass);

    layer_class->fill_command_buffer = keyframe_layer_cool_fill_command_buffer;
    layer_class->type = keyframe_layer_cool_type;
}

static void
keyframe_layer_cool_init (KeyframeLayerCool *self)
{
}

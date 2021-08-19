#include "keyframe-layer-text.h"

#include <pango/pangocairo.h>

typedef struct
{
    char *markup;
} KeyframeLayerTextPrivate;

G_DEFINE_FINAL_TYPE_WITH_PRIVATE (KeyframeLayerText, keyframe_layer_text, KEYFRAME_TYPE_LAYER)

enum {
    PROP_0,
    PROP_MARKUP,
    N_PROPS
};

static GParamSpec *properties [N_PROPS];

/**
 * keyframe_layer_text_new:
 *
 * Create a new #KeyframeLayerText.
 *
 * Returns: (transfer full): a newly created #KeyframeLayer
 */
KeyframeLayer *
keyframe_layer_text_new (const char *name, const char *markup)
{
    return g_object_new (KEYFRAME_TYPE_LAYER_TEXT,
                         "name", name,
                         "markup", markup,
                         NULL);
}

static void
keyframe_layer_text_finalize (GObject *object)
{
    KeyframeLayerText *self = (KeyframeLayerText *)object;
    KeyframeLayerTextPrivate *priv = keyframe_layer_text_get_instance_private (self);

    G_OBJECT_CLASS (keyframe_layer_text_parent_class)->finalize (object);
}

static void
keyframe_layer_text_get_property (GObject    *object,
                                      guint       prop_id,
                                      GValue     *value,
                                      GParamSpec *pspec)
{
    KeyframeLayerText *self = KEYFRAME_LAYER_TEXT (object);
    KeyframeLayerTextPrivate *priv = keyframe_layer_text_get_instance_private (self);

    switch (prop_id)
    {
    case PROP_MARKUP:
        g_value_set_string (value, priv->markup);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
keyframe_layer_text_set_property (GObject      *object,
                                      guint         prop_id,
                                      const GValue *value,
                                      GParamSpec   *pspec)
{
    KeyframeLayerText *self = KEYFRAME_LAYER_TEXT (object);
    KeyframeLayerTextPrivate *priv = keyframe_layer_text_get_instance_private (self);

    switch (prop_id)
    {
    case PROP_MARKUP:
        priv->markup = g_value_dup_string (value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
keyframe_layer_text_fill_command_buffer (KeyframeLayer *self, KeyframeRenderer *renderer)
{
    KeyframeLayerTextPrivate *priv = keyframe_layer_text_get_instance_private (KEYFRAME_LAYER_TEXT (self));
    cairo_t *cr = keyframe_renderer_get_cairo (renderer);

    cairo_save (cr);

    float x, y;
    g_object_get (self, "x", &x, "y", &y, NULL);
    cairo_translate (cr, x, y);

    int width = keyframe_renderer_get_width (renderer);
    int height = keyframe_renderer_get_height (renderer);

    // Pango doesn't play well with cairo scaling
    // Scale both axis by the same dimension, otherwise the text
    // spacing will not work properly.
    int scale = (width > height) ? width : height;
    cairo_scale (cr, scale, scale);

    // TODO: Creating a new layout every frame is very bad!
    PangoLayout *layout = pango_cairo_create_layout (cr);
    // pango_layout_set_text (layout, "Hello World!\n", -1);
    pango_layout_set_markup (layout, priv->markup, -1);
    PangoFontDescription *desc = pango_font_description_from_string ("Cantarell");
    pango_font_description_set_size (desc, 72);
    pango_layout_set_font_description (layout, desc);
    pango_font_description_free (desc);

    cairo_set_source_rgb (cr, 0, 0, 0);
    pango_cairo_update_layout (cr, layout);
    pango_cairo_show_layout (cr, layout);

    g_object_unref (layout);

    cairo_restore (cr);
}

static const char *
keyframe_layer_text_type (KeyframeLayer *layer)
{
    return "Text";
}

static void
keyframe_layer_text_class_init (KeyframeLayerTextClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = keyframe_layer_text_finalize;
    object_class->get_property = keyframe_layer_text_get_property;
    object_class->set_property = keyframe_layer_text_set_property;

    KeyframeLayerClass *layer_class = KEYFRAME_LAYER_CLASS (klass);

    layer_class->fill_command_buffer = keyframe_layer_text_fill_command_buffer;
    layer_class->type = keyframe_layer_text_type;

    properties [PROP_MARKUP] =
        g_param_spec_string ("markup",
                             "Markup",
                             "The text to display using Pango Markup.",
                             "Text",
                             G_PARAM_READWRITE);

    g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
keyframe_layer_text_init (KeyframeLayerText *self)
{
}

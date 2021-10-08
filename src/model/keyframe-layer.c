#include "keyframe-layer.h"

typedef struct
{
    char *name;
    char *type;
    float x, y;
    gboolean visible;
} KeyframeLayerPrivate;

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE (KeyframeLayer, keyframe_layer, G_TYPE_OBJECT)

enum {
    PROP_0,
    PROP_NAME,
    PROP_TYPE,
    PROP_VISIBLE,
    PROP_X,
    PROP_Y,
    N_PROPS
};

static GParamSpec *properties [N_PROPS];

static void
keyframe_layer_finalize (GObject *object)
{
    KeyframeLayer *self = (KeyframeLayer *)object;
    KeyframeLayerPrivate *priv = keyframe_layer_get_instance_private (self);

    G_OBJECT_CLASS (keyframe_layer_parent_class)->finalize (object);
}

static void
keyframe_layer_get_property (GObject    *object,
                             guint       prop_id,
                             GValue     *value,
                             GParamSpec *pspec)
{
    KeyframeLayer *self = KEYFRAME_LAYER (object);
    KeyframeLayerPrivate *priv = keyframe_layer_get_instance_private (self);

    switch (prop_id)
    {
        case PROP_NAME:
            g_value_set_string (value, priv->name);
            break;
        case PROP_TYPE:
            g_value_set_string (value, KEYFRAME_LAYER_GET_CLASS (self)->type (self));
            break;
        case PROP_VISIBLE:
            // TODO: Need to propagate to parent composition
            g_value_set_boolean (value, priv->visible);
            break;
        case PROP_X:
            g_value_set_float (value, priv->x);
            break;
        case PROP_Y:
            g_value_set_float (value, priv->y);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
keyframe_layer_set_property (GObject      *object,
                             guint         prop_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
    KeyframeLayer *self = KEYFRAME_LAYER (object);
    KeyframeLayerPrivate *priv = keyframe_layer_get_instance_private (self);

    switch (prop_id)
    {
        case PROP_NAME:
            g_free (priv->name);
            priv->name = g_value_dup_string (value);
            break;
        case PROP_VISIBLE:
            priv->visible = g_value_get_boolean (value);
            g_print ("Set Visible: %d\n", priv->visible);
            break;
        case PROP_X:
            priv->x = g_value_get_float (value);
            break;
        case PROP_Y:
            priv->y = g_value_get_float (value);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
keyframe_layer_fill_command_buffer_default (KeyframeLayer *self, KeyframeRenderer *renderer)
{
    g_warning ("%s does not implement 'fill_command_buffer()' - Nothing will be drawn!\n",
               g_type_name_from_instance ((GTypeInstance *)self));
}

void keyframe_layer_fill_command_buffer (KeyframeLayer *self, KeyframeRenderer *renderer)
{
    KEYFRAME_LAYER_GET_CLASS (self)->fill_command_buffer (self, renderer);
}

static const char *
keyframe_layer_type_default (KeyframeLayer *self)
{
    const char *type_name = g_type_name_from_instance ((GTypeInstance *)self);
    g_warning ("%s does not override print_type(). Printing out class name instead.\n",
               type_name);

    return type_name;
}

static void
keyframe_layer_class_init (KeyframeLayerClass *klass)
{
    klass->fill_command_buffer = keyframe_layer_fill_command_buffer_default;
    klass->type = keyframe_layer_type_default;

    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = keyframe_layer_finalize;
    object_class->get_property = keyframe_layer_get_property;
    object_class->set_property = keyframe_layer_set_property;

    properties [PROP_NAME] =
        g_param_spec_string ("name",
                             "Name",
                             "A name to distinguish the layer.",
                             "Untitled Layer",
                             G_PARAM_READWRITE);

    properties [PROP_TYPE] =
        g_param_spec_string ("type",
                             "Type",
                             "Type of Layer",
                             NULL,
                             G_PARAM_READABLE);

    properties [PROP_X] =
        g_param_spec_float ("x",
                            "X-Coord",
                            "Position of the layer on the x-axis (in pixels).",
                            -G_MAXFLOAT, G_MAXFLOAT, 0,
                            G_PARAM_READWRITE);

    properties [PROP_Y] =
        g_param_spec_float ("y",
                            "Y-Coord",
                            "Position of the layer on the y-axis (in pixels).",
                            -G_MAXFLOAT, G_MAXFLOAT, 0,
                            G_PARAM_READWRITE);

    properties [PROP_VISIBLE] =
        g_param_spec_boolean ("visible",
                              "Visible",
                              "Whether the layer is visible or not.",
                              TRUE,
                              G_PARAM_READWRITE);

    g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
keyframe_layer_init (KeyframeLayer *self)
{
}

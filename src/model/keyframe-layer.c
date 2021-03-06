#include "keyframe-layer.h"
#include "keyframe-composition.h"

typedef struct
{
    char *name;
    char *type;
    float x, y;
    gboolean visible;

    float start_time;
    float end_time;

    KeyframeComposition *parent;
} KeyframeLayerPrivate;

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE (KeyframeLayer, keyframe_layer, G_TYPE_OBJECT)

enum {
    PROP_0,
    PROP_NAME,
    PROP_TYPE,
    PROP_COMPOSITION,
    PROP_VISIBLE,
    PROP_START_TIME,
    PROP_END_TIME,
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

    g_object_unref (priv->parent);

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
        case PROP_COMPOSITION:
            g_value_set_object (value, priv->parent);
            break;
        case PROP_VISIBLE:
            g_value_set_boolean (value, priv->visible);
            break;
        case PROP_START_TIME:
            g_value_set_float (value, priv->start_time);
            break;
        case PROP_END_TIME:
            g_value_set_float (value, priv->end_time);
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
        case PROP_COMPOSITION:
            priv->parent = g_value_get_object (value);
            break;
        case PROP_VISIBLE:
            priv->visible = g_value_get_boolean (value);
            g_print ("Set Visible: %d\n", priv->visible);
            break;
        case PROP_START_TIME:
            priv->start_time = g_value_get_float (value);
            break;
        case PROP_END_TIME:
            priv->end_time = g_value_get_float (value);
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
keyframe_layer_notify (KeyframeLayer *self, GParamSpec *pspec, gpointer)
{
    KeyframeLayerPrivate *priv = keyframe_layer_get_instance_private (self);

    // Invalidate composition
    if (priv->parent != NULL)
        keyframe_composition_invalidate (priv->parent);
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

    properties [PROP_COMPOSITION] =
        g_param_spec_object ("composition",
                             "Composition",
                             "Parent Composition",
                             KEYFRAME_TYPE_COMPOSITION,
                             G_PARAM_READWRITE);

    properties [PROP_START_TIME] =
        g_param_spec_float ("start-time",
                            "Start Time",
                            "Start Time",
                            0, G_MAXFLOAT, 0,
                            G_PARAM_READWRITE);

    properties [PROP_END_TIME] =
        g_param_spec_float ("end-time",
                            "End Time",
                            "End Time",
                            0, G_MAXFLOAT, 0,
                            G_PARAM_READWRITE);

    g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
keyframe_layer_init (KeyframeLayer *self)
{
    g_signal_connect (self, "notify", G_CALLBACK (keyframe_layer_notify), NULL);

    // Default properties
    // TODO: Can these be set automatically?
    KeyframeLayerPrivate *priv = keyframe_layer_get_instance_private (self);
    priv->visible = TRUE;
    priv->start_time = 0;
    priv->end_time = 1000;
}

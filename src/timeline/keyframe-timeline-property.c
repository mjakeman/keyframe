#include "keyframe-timeline-property.h"

#include "../model/keyframe-layers.h"
#include "../model/keyframe-value-float.h"

// As our keyframes are boxed types, create
// a GObject to wrap them for displaying in
// the timeline.
struct _KeyframeTimelineProperty
{
    GObject parent_instance;

    // TODO: Should layer and param spec be here?
    // Probably not, it's a bit messy
    KeyframeLayer *layer;
    GParamSpec *param_spec;
    KeyframeValueFloat *keyframe_set;
};

G_DEFINE_FINAL_TYPE (KeyframeTimelineProperty, keyframe_timeline_property, G_TYPE_OBJECT)

enum {
    PROP_0,
    PROP_PARAM_SPEC,
    N_PROPS
};

static GParamSpec *properties [N_PROPS];

KeyframeTimelineProperty *
keyframe_timeline_property_new (GParamSpec *param_spec)
{
    return g_object_new (KEYFRAME_TYPE_TIMELINE_PROPERTY,
                         "param-spec", param_spec,
                         NULL);
}

static void
keyframe_timeline_property_finalize (GObject *object)
{
    KeyframeTimelineProperty *self = (KeyframeTimelineProperty *)object;

    G_OBJECT_CLASS (keyframe_timeline_property_parent_class)->finalize (object);
}

static void
keyframe_timeline_property_get_property (GObject    *object,
                                         guint       prop_id,
                                         GValue     *value,
                                         GParamSpec *pspec)
{
    KeyframeTimelineProperty *self = KEYFRAME_TIMELINE_PROPERTY (object);

    switch (prop_id)
      {
      case PROP_PARAM_SPEC:
          g_value_set_param (value, self->param_spec);
          break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      }
}

static void
keyframe_timeline_property_set_property (GObject      *object,
                                         guint         prop_id,
                                         const GValue *value,
                                         GParamSpec   *pspec)
{
    KeyframeTimelineProperty *self = KEYFRAME_TIMELINE_PROPERTY (object);

    switch (prop_id)
      {
      case PROP_PARAM_SPEC:
          self->param_spec = g_value_get_param (value);
          break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      }
}

static void
keyframe_timeline_property_class_init (KeyframeTimelinePropertyClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = keyframe_timeline_property_finalize;
    object_class->get_property = keyframe_timeline_property_get_property;
    object_class->set_property = keyframe_timeline_property_set_property;

    properties [PROP_PARAM_SPEC]
        = g_param_spec_param ("param-spec", "Param Spec", "Param Spec", G_TYPE_PARAM, G_PARAM_READWRITE|G_PARAM_CONSTRUCT);

    g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
keyframe_timeline_property_init (KeyframeTimelineProperty *self)
{
}

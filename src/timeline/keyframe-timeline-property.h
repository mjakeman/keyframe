#pragma once

#include <glib-object.h>
#include "../model/keyframe-layers.h"

G_BEGIN_DECLS

#define KEYFRAME_TYPE_TIMELINE_PROPERTY (keyframe_timeline_property_get_type())

G_DECLARE_FINAL_TYPE (KeyframeTimelineProperty, keyframe_timeline_property, KEYFRAME, TIMELINE_PROPERTY, GObject)

KeyframeTimelineProperty *
keyframe_timeline_property_new (GParamSpec *param_spec, KeyframeLayer *layer);

G_END_DECLS

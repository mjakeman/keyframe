#pragma once

#include <glib-2.0/glib-object.h>

#include "keyframe-layer.h"

G_BEGIN_DECLS

#define KEYFRAME_TYPE_LAYER_COOL (keyframe_layer_cool_get_type())

G_DECLARE_DERIVABLE_TYPE (KeyframeLayerCool, keyframe_layer_cool, KEYFRAME, LAYER_COOL, KeyframeLayer)

struct _KeyframeLayerCoolClass
{
  KeyframeLayerClass parent_class;
};

KeyframeLayer *keyframe_layer_cool_new (const char *name);

G_END_DECLS

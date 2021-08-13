#pragma once

#include <glib-object.h>

G_BEGIN_DECLS

#define KEYFRAME_TYPE_LAYER (keyframe_layer_get_type())

G_DECLARE_DERIVABLE_TYPE (KeyframeLayer, keyframe_layer, KEYFRAME, LAYER, GObject)

struct _KeyframeLayerClass
{
  GObjectClass parent_class;
};

KeyframeLayer *keyframe_layer_new (const char *name);

G_END_DECLS

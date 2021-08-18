#pragma once

#include <glib-2.0/glib-object.h>

#include "keyframe-layer.h"

G_BEGIN_DECLS

#define KEYFRAME_TYPE_LAYER_GEOMETRY (keyframe_layer_geometry_get_type())

G_DECLARE_DERIVABLE_TYPE (KeyframeLayerGeometry, keyframe_layer_geometry, KEYFRAME, LAYER_GEOMETRY, KeyframeLayer)

struct _KeyframeLayerGeometryClass
{
  KeyframeLayerClass parent_class;
};

KeyframeLayer *keyframe_layer_geometry_new (const char *name, float width, float height);

G_END_DECLS

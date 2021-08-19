#pragma once

#include <glib-2.0/glib-object.h>

G_BEGIN_DECLS

#define KEYFRAME_TYPE_VALUE_FLOAT (keyframe_value_float_get_type ())

typedef struct KeyframeValueFloat_ KeyframeValueFloat;

KeyframeValueFloat *keyframe_value_float_new (float val);
float keyframe_value_float_get (KeyframeValueFloat *self, float time);
void keyframe_value_float_set_static (KeyframeValueFloat *self, float value);

// Test Function
void keyframe_value_float_set_dynamic_test_data (KeyframeValueFloat *self);
void keyframe_value_float_print (KeyframeValueFloat *self);

GType keyframe_value_float_get_type ();

G_END_DECLS

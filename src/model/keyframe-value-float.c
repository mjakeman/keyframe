#include "keyframe-value-float.h"

typedef struct
{
    float timestamp;
    float value;
} KeyframeValueFloatPair;

struct KeyframeValueFloat_
{
    gboolean use_keyframes;
    grefcount ref_count;

    // Use union?
    float value;
    GSList *keyframes;
};

static gpointer
keyframe_value_copy (KeyframeValueFloat *boxed)
{
    g_ref_count_inc (&boxed->ref_count);
    return boxed;
}

static void
keyframe_value_free (KeyframeValueFloat *boxed)
{
    g_ref_count_dec (&boxed->ref_count);
}

// G_DEFINE_BOXED_TYPE (KeyframeValueFloat, keyframe_value_float, keyframe_value_copy, keyframe_value_free);

GType
keyframe_value_float_get_type ()
{
    static GType typeid = 0;
    if (typeid == 0)
    {
        typeid = g_boxed_type_register_static ("KeyframeValueFloat",
                                               keyframe_value_copy,
                                               keyframe_value_free);
    }
    return typeid;
}

KeyframeValueFloat *keyframe_value_float_new (float val)
{
    KeyframeValueFloat *str = g_new0 (KeyframeValueFloat, 1);
    g_ref_count_init (&str->ref_count);
    str->use_keyframes = FALSE;
    str->value = val;
    return str;
}

static void
get_bounding_pairs (KeyframeValueFloat     *self,
                    float                   time,
                    KeyframeValueFloatPair *before,
                    KeyframeValueFloatPair *after)
{
    KeyframeValueFloatPair *last;
    for (GSList *l = self->keyframes; l != NULL; l = l->next)
    {
        KeyframeValueFloatPair *current = (KeyframeValueFloatPair *)l->data;
        if (current->timestamp > time)
        {
            *before = *last;
            *after = *current;
            return;
        }
        last = current;
    }

    // If we get here, there is no 'after' bounding pair
    // so we only assign the before.
    *before = *last;
}

static int
keyframe_compare (KeyframeValueFloatPair *a,
                  KeyframeValueFloatPair *b)
{
    // TODO: Don't allow having two elements at the same index
    if (a->timestamp < b->timestamp)
        return -1;

    if (a->timestamp > b->timestamp)
        return 1;

    if (a->timestamp == b->timestamp)
    {
        g_warning ("We probably don't want multiple keyframes existing at the\
            same point. Makes things confusing :/\n");
        return 0;
    }
}


static void
push_keyframe (KeyframeValueFloat *self,
               float               timestamp,
               float               value)
{
    KeyframeValueFloatPair *pair = g_new0 (KeyframeValueFloatPair, 1);
    pair->timestamp = timestamp;
    pair->value = value;
    self->keyframes = g_slist_insert_sorted (self->keyframes, pair, (GCompareFunc)keyframe_compare);
}

float
keyframe_value_float_get (KeyframeValueFloat *self, float time)
{
    g_return_val_if_fail (self != NULL, 0.0f);

    if (!self->use_keyframes)
        return self->value;

    // Lerp for now, look at fancy control point manipulation later
    // g_assert (FALSE);

    KeyframeValueFloatPair start = {0};
    KeyframeValueFloatPair end = {0};
    get_bounding_pairs (self, time, &start, &end);

    g_print ("%f %f / %f %f\n", start.timestamp, start.value, end.timestamp, end.value);

    float percent = (time - start.timestamp)/(end.timestamp - start.timestamp);

    return start.value + percent * (end.value - start.value);
}

void
keyframe_value_float_set_static (KeyframeValueFloat *self, float value)
{
    g_return_if_fail (self != NULL);

    self->use_keyframes = FALSE;
    self->value = value;

    // Free keyframes
    g_slist_free_full (g_steal_pointer (&self->keyframes), g_free);
}

void
keyframe_value_float_set_dynamic_test_data (KeyframeValueFloat *self)
{
    // Add some test keyframes
    g_return_if_fail (self != NULL);

    if (self->use_keyframes)
        return;

    self->use_keyframes = TRUE;

    push_keyframe (self, 0.0f, 100.0f);
    push_keyframe (self, 200.0f, 900.0f);
    push_keyframe (self, 400.0f, 300.0f);
    push_keyframe (self, 500.0f, 600.0f);
    push_keyframe (self, 1000.0f, 1920.0f);
}

void
keyframe_value_float_print (KeyframeValueFloat *self)
{
    for (GSList *l = self->keyframes; l != NULL; l = l->next)
        g_print ("Keyframe: %f %f\n",
                 ((KeyframeValueFloatPair *)l->data)->timestamp,
                 ((KeyframeValueFloatPair *)l->data)->value);
}

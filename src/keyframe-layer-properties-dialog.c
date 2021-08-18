#include "keyframe-layer-properties-dialog.h"

#include <adwaita.h>

typedef struct
{
    KeyframeLayer *layer;
} KeyframeLayerPropertiesDialogPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (KeyframeLayerPropertiesDialog, keyframe_layer_properties_dialog, GTK_TYPE_DIALOG)

enum {
    PROP_0,
    PROP_LAYER,
    N_PROPS
};

static GParamSpec *properties [N_PROPS];

/**
 * keyframe_layer_properties_dialog_new:
 *
 * Create a new #KeyframeLayerPropertiesDialog.
 *
 * Returns: (transfer full): a newly created #KeyframeLayerPropertiesDialog
 */
GtkWidget *
keyframe_layer_properties_dialog_new (KeyframeLayer *layer)
{
    return g_object_new (KEYFRAME_TYPE_LAYER_PROPERTIES_DIALOG,
                         "layer", layer,
                         "use-header-bar", 1,
                         NULL);
}

static void
keyframe_layer_properties_dialog_finalize (GObject *object)
{
    KeyframeLayerPropertiesDialog *self = (KeyframeLayerPropertiesDialog *)object;
    KeyframeLayerPropertiesDialogPrivate *priv = keyframe_layer_properties_dialog_get_instance_private (self);

    G_OBJECT_CLASS (keyframe_layer_properties_dialog_parent_class)->finalize (object);
}

static void
keyframe_layer_properties_dialog_get_property (GObject    *object,
                                               guint       prop_id,
                                               GValue     *value,
                                               GParamSpec *pspec)
{
    KeyframeLayerPropertiesDialog *self = KEYFRAME_LAYER_PROPERTIES_DIALOG (object);
    KeyframeLayerPropertiesDialogPrivate *priv = keyframe_layer_properties_dialog_get_instance_private (self);

    switch (prop_id)
      {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      }
}

static void
keyframe_layer_properties_dialog_set_property (GObject      *object,
                                               guint         prop_id,
                                               const GValue *value,
                                               GParamSpec   *pspec)
{
    KeyframeLayerPropertiesDialog *self = KEYFRAME_LAYER_PROPERTIES_DIALOG (object);
    KeyframeLayerPropertiesDialogPrivate *priv = keyframe_layer_properties_dialog_get_instance_private (self);

    switch (prop_id)
    {
    case PROP_LAYER:
        priv->layer = g_value_get_object (value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static GtkWidget *
string_factory (GParamSpec    *prop,
                KeyframeLayer *layer,
                const char    *name,
                gboolean       sensitive)
{
    const char *contents;
    g_object_get (layer, name, &contents, NULL);

    GtkEntryBuffer *buf = gtk_entry_buffer_new (contents, -1);

    if (sensitive)
    {
        // When the user changes the entry's text, we update
        // the relevant property on the layer accordingly.
        g_object_bind_property (buf, "text", layer, name, G_BINDING_DEFAULT);
    }

    GtkWidget *entry = gtk_entry_new_with_buffer (buf);
    gtk_widget_set_valign (entry, GTK_ALIGN_CENTER);

    gtk_widget_set_sensitive (entry, sensitive);

    return entry;
}

static GtkWidget *
float_factory (GParamSpec    *prop,
               KeyframeLayer *layer,
               const char    *name,
               gboolean       sensitive)
{
    float val;
    g_object_get (layer, name, &val, NULL);

    GParamSpecFloat *float_prop = (GParamSpecFloat *)prop;
    GtkWidget *spin_button =
        gtk_spin_button_new_with_range (float_prop->minimum,
                                        float_prop->maximum,
                                        1.0f);

    gtk_spin_button_set_value (GTK_SPIN_BUTTON (spin_button), val);
    gtk_widget_set_valign (spin_button, GTK_ALIGN_CENTER);

    if (sensitive)
    {
        // When the user changes the entry's text, we update
        // the relevant property on the layer accordingly.
        g_object_bind_property (spin_button, "value", layer, name, G_BINDING_DEFAULT);
    }

    gtk_widget_set_sensitive (spin_button, sensitive);

    return spin_button;
}

static GtkWidget *
property_widget_factory (GParamSpec *prop, KeyframeLayer *layer)
{
    gboolean sensitive = TRUE;
    GtkWidget *action_widget;

    if (!(prop->flags & G_PARAM_READABLE))
        return;

    if (!(prop->flags & G_PARAM_WRITABLE) ||
        prop->flags & G_PARAM_CONSTRUCT_ONLY)
        sensitive = FALSE;

    AdwActionRow *row = adw_action_row_new ();
    const char *name = g_param_spec_get_name (prop);
    const char *nick = g_param_spec_get_nick (prop);
    const char *blurb = g_param_spec_get_blurb (prop);

    adw_preferences_row_set_title (ADW_PREFERENCES_ROW (row), nick);
    adw_action_row_set_subtitle (row, blurb);

    if (G_IS_PARAM_SPEC_STRING (prop)) {
        action_widget = string_factory (prop, layer, name, sensitive);
        adw_action_row_add_suffix (row, action_widget);
    }
    else if (G_IS_PARAM_SPEC_FLOAT (prop)) {
        action_widget = float_factory (prop, layer, name, sensitive);
        adw_action_row_add_suffix (row, action_widget);
    }

    return row;
}

static void
keyframe_layer_properties_dialog_constructed (KeyframeLayerPropertiesDialog *self)
{
    KeyframeLayerPropertiesDialogPrivate *priv = keyframe_layer_properties_dialog_get_instance_private (self);

    KeyframeLayer *layer = priv->layer;
    g_return_if_fail (KEYFRAME_IS_LAYER (layer));

    int n_props;
    GParamSpec **params = g_object_class_list_properties (G_OBJECT_GET_CLASS (layer), &n_props);

    GtkWidget *content_area = gtk_dialog_get_content_area (GTK_DIALOG (self));

    GtkWidget *listbox = gtk_list_box_new ();
    gtk_list_box_set_selection_mode (listbox, GTK_SELECTION_NONE);
    gtk_widget_set_vexpand (listbox, TRUE);
    gtk_widget_set_hexpand (listbox, TRUE);

    GtkWidget *scroll = gtk_scrolled_window_new ();
    gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scroll), listbox);
    gtk_scrolled_window_set_min_content_height (scroll, 400);
    gtk_scrolled_window_set_min_content_width (scroll, 400);
    gtk_box_append (GTK_BOX (content_area), scroll);


    for (int i = 0; i < n_props; i++)
    {
        GtkWidget *widget = property_widget_factory (params[i], layer);
        gtk_list_box_append (GTK_LIST_BOX (listbox), widget);
    }

    G_OBJECT_CLASS (keyframe_layer_properties_dialog_parent_class)->constructed (self);
}

static void
keyframe_layer_properties_dialog_class_init (KeyframeLayerPropertiesDialogClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = keyframe_layer_properties_dialog_finalize;
    object_class->get_property = keyframe_layer_properties_dialog_get_property;
    object_class->set_property = keyframe_layer_properties_dialog_set_property;
    object_class->constructed = keyframe_layer_properties_dialog_constructed;

    properties [PROP_LAYER] = g_param_spec_object ("layer", "Layer", "Layer", KEYFRAME_TYPE_LAYER,
                                                   G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY);

    g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
keyframe_layer_properties_dialog_init (KeyframeLayerPropertiesDialog *self)
{
    gtk_window_set_title (GTK_WINDOW (self), "Layer Properties");
}

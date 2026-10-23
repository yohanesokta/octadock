#include <gtk/gtk.h>
#include <gtk-layer-shell/gtk-layer-shell.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    g_setenv("GDK_BACKEND", "wayland", TRUE);
    gtk_init(&argc, &argv);

    int zone = 80;
    if (argc > 1) {
        zone = atoi(argv[1]);
    }
    if (zone <= 0) zone = 80;

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_layer_init_for_window(GTK_WINDOW(window));
    gtk_layer_set_layer(GTK_WINDOW(window), GTK_LAYER_SHELL_LAYER_BOTTOM);
    gtk_layer_set_anchor(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_BOTTOM, TRUE);
    gtk_layer_set_anchor(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_LEFT, TRUE);
    gtk_layer_set_anchor(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_RIGHT, TRUE);
    gtk_layer_set_exclusive_zone(GTK_WINDOW(window), zone);
    gtk_layer_set_keyboard_mode(GTK_WINDOW(window), GTK_LAYER_SHELL_KEYBOARD_MODE_NONE);

    // Transparent to all mouse input so clicks pass through to windows beneath
    cairo_region_t *empty_region = cairo_region_create();
    gtk_widget_input_shape_combine_region(window, empty_region);
    cairo_region_destroy(empty_region);

    gtk_widget_set_app_paintable(window, TRUE);
    gtk_widget_set_size_request(window, -1, 1);
    gtk_widget_show_all(window);

    gtk_main();
    return 0;
}

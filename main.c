#include <gtk/gtk.h>

enum
{
  TYPE_HDMI,
  TYPE_DP
};

// return the type of the monitor input source
static int
get_input_source (void)
{
  FILE *fp = popen ("ddcutil getvcp 60", "r");
  char line[1024];
  int type = -1;

  while (fgets(line, sizeof(line), fp) != NULL) {
    if (strstr(line, "sl=0x05")) {
      type = TYPE_HDMI;
      break;
    } else if (strstr(line, "sl=0x0f")) {
      type = TYPE_DP;
      break;
    }
  }
  pclose (fp);
  return type;
}

// switch the monitor input source to the specified type
static void
set_input_source (GtkWidget *widget, gpointer data)
{
  if (!gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(widget))) {
    return;
  }
  int type = (int) data;
  printf ("Set input source: %d\n", type);
  const char *cmds[] = {"ddcutil setvcp 60 0x05", "ddcutil setvcp 60 0x0f"};
  FILE *fp = popen (cmds[type], "w");
  pclose (fp);
}

static GtkWidget *
create_input_source_list (void)
{
  GtkWidget *box;
  GSList *group = NULL;
  const char *labels[] = {"HDMI", "DisplayPort"};
  int type = get_input_source();

  if (type == -1) {
    return NULL;
    perror ("Failed to get input source type\n");
    exit (EXIT_FAILURE);
  }
  printf ("Input source: %s\n", labels[type]);

  box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
  for (int i = 0; i < sizeof(labels) / sizeof(labels[0]); i++) {
    GtkWidget *radiobutton = gtk_radio_button_new_with_label(group, labels[i]);
    if (i == type) {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(radiobutton), TRUE);
      gtk_widget_set_sensitive (radiobutton, FALSE);
    } else {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(radiobutton), FALSE);
    }
    g_signal_connect (radiobutton, "clicked", G_CALLBACK (set_input_source), (gpointer)i);
    group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton));
    gtk_box_pack_start (GTK_BOX (box), radiobutton, FALSE, FALSE, 0);
  }

  return box;
}

static void
activate (GtkApplication* app,
          gpointer        user_data)
{
  GtkWidget *window;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Monitor Switcher");
  gtk_window_set_default_size (GTK_WINDOW (window), 600, 600);
  gtk_container_add (GTK_CONTAINER (window), create_input_source_list ());
  gtk_widget_show_all (window);
}

int main(int argc, char const *argv[])
{

  GtkApplication *app;
  int status;

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, (char**) argv);
  g_object_unref (app);

  return status;
}

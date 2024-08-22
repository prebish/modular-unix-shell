#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include <unistd.h>
#include <dlfcn.h>
#include <sys/wait.h>

#define MAX_PLUGINS 20
#define MAX_WORD_LEN 45

char input[1001]; // raw input 201
char* args[20]; // separated args 20

// Define the function pointer types
typedef int (*InitializeFunction)(); // was void
typedef int (*RunFunction)(char **);

typedef struct Plugin {
    char* name;
    void* handle;              // Handle for the dynamically loaded plugin_name
    InitializeFunction initialize; // Pointer to the initialize function
    RunFunction run;           // Pointer to the run function
} Plugin;

Plugin plugins[MAX_PLUGINS]; // storing plugins

Plugin null_plugin = {NULL, NULL, NULL, NULL};


void get_args() {
    int i = 0;
    char* token = NULL;

    // Read Input
    printf("> "); fgets(input,sizeof(input),stdin); // size of bc '\0'

    token = strtok(input, "'\n' ");

    // Exit & Load Check
    if (!strcmp(token, "exit")) exit_shell();

    // Load Tokens into Args
    while (token) {
        //printf("Token %d: [%s] \n", i, token); // Visualize the tokens
        args[i] = token; i++;
        token = strtok(NULL,"'\n' ");
    }

}

Plugin getPlugin(char* plugin_name) {
    for(int i = 0; i<MAX_PLUGINS; i++) {
        if (!plugins[i].name) continue;
        if(!strcmp(plugin_name, plugins[i].name)) {
            return plugins[i];
        }
    }
    return null_plugin;
}

// Create, Add, Load Plugin
int loadPlugin(char* plugin_name) {

    // If no plugin_name name
    if(!plugin_name) {
        printf("Error: Plugin %s initialization failed!", plugin_name);
        return false;
    }

    // If plugin exists
    if (getPlugin(plugin_name).name) {
        printf("Error: Plugin %s initialization failed!\n", plugin_name);
        return false;
    }

    // Initialize Plugin plugin
    Plugin plugin = null_plugin; //{NULL, NULL, NULL, NULL};
    plugin.name = malloc(strlen(plugin_name)+1);
    strcpy(plugin.name, plugin_name);

    // Building the path
    char plugin_path[MAX_WORD_LEN]; // 45
    strcpy(plugin_path, "./"); strcat(plugin_path, plugin.name); strcat(plugin_path, ".so");


    // Load the plugin.handle
    plugin.handle = dlopen(plugin_path, RTLD_LAZY);
    if (!plugin.handle) {
        printf("Error: Plugin %s initialization failed!\n", plugin.name);
        return 1;
    }

    // Save Initialize Function from plugin.name
    plugin.initialize = (InitializeFunction)dlsym(plugin.handle, "initialize");
    if (!plugin.initialize) {
        printf("Error: Plugin %s initialization failed!\n", plugin.name);
        dlclose(plugin.handle); return 1;
    }

    // Save Run Function from plugin.name
    plugin.run = (RunFunction)dlsym(plugin.handle, "run");
    if (!plugin.run) {
        printf("Failed to load run()");
        dlclose(plugin.handle); return 1;
    }
    
    // Add plugin to array
    for (int i=0; i<MAX_PLUGINS; i++) {
        if (!plugins[i].name) {
            plugins[i] = plugin;
            if (plugin.initialize() != 0) {
                printf("Error: Plugin %s initialization failed!\n", plugin.name);
            }

            return true;
        }

    }

    printf("Error: Plugin %s initialization failed!\n", plugin.name);
    return false;
}

void clearShellMem() {
    memset(input, '\0', sizeof(input));
    for (int i = 0; i < 20; i++){
        args[i] = NULL;
    }
}

void exit_shell() {
    for (int i = 0; i < MAX_PLUGINS; i++) {
        if (plugins[i].name) {
            free(plugins[i].name);
            if (plugins[i].handle) {
                dlclose(plugins[i].handle);
            }
        }
    }
    exit(0);
}


int main () {
    // Initialize Plugins
    for (int i = 0; i < MAX_PLUGINS; i++) {
        plugins[i] = null_plugin;
    }

    while(1) {
        clearShellMem();

        // Does the work to separate
        // raw input into separated args
        get_args();

        if (!strcmp(args[0], "load")) loadPlugin(args[1]);

        else if (!strcmp(args[0], "exit")) exit_shell();

        else if (getPlugin(args[0]).name) {
            Plugin plugin = getPlugin(args[0]);
            if (plugin.name) plugin.run(args);
        }

        else {
            pid_t pid = fork();
            if (pid == 0) 
                execvp(args[0], args);
            else wait(NULL);
        }
    }
    return false;
}


// compile with WSL
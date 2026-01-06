#include "get_config.h"

struct config *init_config()
{
    struct config *config = malloc(sizeof(struct config));
    config->tracker_port = 0;
    memset(config->tracker_address, 0, MAX_TRACKER_ADDRESS);
    return config;
}

void free_config(struct config *config)
{
    free(config);
}

void set_config(struct config *config)
{
    // Open config file
    FILE *file = fopen("install/tracker/config.ini", "r");
    if (file == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Read config file
    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (line[0] == '#' || line[0] == '\n')
        {
            continue;
        }
        // Parse the line to extract information
        char *key = strtok(line, " =\n");
        char *value = strtok(NULL, " =\n");

        // Check the key and extract the appropriate values
        if (key != NULL && value != NULL)
        {
            if (strcmp(key, "tracker_ip") == 0)
            {
                strcpy(config->tracker_address, value);
            }
            else if (strcmp(key, "tracker_port") == 0)
            {
                config->tracker_port = atoi(value);
            }
        }
    }
    printf("Tracker address: %s\nTracker port: %d\n", config->tracker_address, config->tracker_port);

    // Close the file
    fclose(file);
}



/*
    const char* test_message = "announce listen 2222 leech [330a57722ec8b0bf09669a2b35f88e9e] seed [file_a.dat 2097152 1024 8905e92afeb80fc7722ec89eb0bf0966]";
    Announce_req_t announce_req;

    int result = parse_announce_message(test_message, &announce_req);
    if (result == 0) {
        printf("Port d'écoute : %d\n", announce_req.listening_port);
        printf("Nombre de fichiers en mode seed : %d\n", announce_req.seed_count);
        for (int i = 0; i < announce_req.seed_count; i++) {
            printf("Seed %d : %s, Size: %d, Block size: %d, Key: %s\n",
                   i + 1, announce_req.seeds[i].name, announce_req.seeds[i].size,
                   announce_req.seeds[i].block_size, announce_req.seeds[i].key);
        }
        printf("Nombre de clés en mode leech : %d\n", announce_req.leech_count);
        for (int i = 0; i < announce_req.leech_count; i++) {
            printf("Leech %d : Key: %s\n", i + 1, announce_req.leeches[i]);
        }
    } else {
        errors_announce(result);
    }

    // Libération de la mémoire allouée
    free_announce_req(&announce_req);

    Look_req_t req;
    const char *message = "< look [filename=\"file_a.dat\" filesize>\"1048576\" filesize<\"2097152\"]";

    int result = parse_look_message(message, &req);

    if (result == 0) {
        printf("Filename: %s\n", req.filename);
        printf("Min filesize: %d\n", req.min_filesize);
        printf("Max filesize: %d\n", req.max_filesize);
    } else {
        printf("Error parsing message: %d\n", result);
    }

    return 0;


    return result;
*/
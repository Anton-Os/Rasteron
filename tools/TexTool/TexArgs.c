extern mixCallback mixer;

Rasteron_Image* texArgs(char* args) {
    char buffer[256];
    unsigned b = 0; // buffer offset

    if (strlen(args) > 1) 
        for (unsigned c = 0; c < strlen(args); c++) {
            buffer[b] = args[c];
            if(isspace(args[c])) {
                for (unsigned e = 0; e < b; e++) buffer[e] = '\0'; // clears buffer
                if (buffer[0] == '-') { // starts with --this
                    printf("Parsing arg value: %s", buffer);
                    if (strstr(buffer, "algo")) {
                        puts("Algorithm selected");
                        if (strstr(buffer, "octave")) puts("Octave noise selected");
                        else puts("Perlin noise selected");
                    }
                    else if (strstr(buffer, "arg")) {
                        puts("Arguments selected");
                    }
                    else if (strstr(buffer, "grid")) {
                        puts("Grid selected");
                    }
                    else if (strstr(buffer, "mix")) {
                        puts("Mix selected");
                        if (strstr(buffer, "and")) mixer = bit_colors_and;
                        else if (strstr(buffer, "xor")) mixer = bit_colors_xor;
                        else if (strstr(buffer, "or")) mixer = bit_colors_or;
                        else puts("Mixer unselected");
                    }
                    else perror("Invalid arg selected");
                }
                else _onKeyEvent(buffer[0]);
            }
        }
    return noiseImgOp((ImageSize){ 1024, 1024 }, grid);
}
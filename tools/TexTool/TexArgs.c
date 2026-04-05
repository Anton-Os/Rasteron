extern mixCallback mixer;

static ColorGrid grid = { 2, 2, 0xFF111111, 0xFFEEEEEE };
unsigned octaves = 0;

Rasteron_Image* texArgs(char* args) {
    char buffer[1024];
    unsigned b = 0; // buffer offset

    for (unsigned c = 0; c < strlen(args); c++) {
        buffer[b] = args[c]; 
        b++; // increment the offset
        if(isspace(args[c])) { // fills the buffer until space argument is detected
            if (buffer[0] == '-') { // starts with --this
                printf("Parsing arg value: %s", buffer);
                if (strstr(buffer, "algo")) {
                    puts("Algorithm selected");
                    if (strstr(buffer, "add")) puts("Additive noise selected");
                    else if (strstr(buffer, "diff") || strstr(buffer, "sub")) puts("Subtractive noise selected");
                    else if (strstr(buffer, "hi") || strstr(buffer, "hifi")) puts("Hi-Fi noise selected");
                    else if (strstr(buffer, "low") || strstr(buffer, "lowfi")) puts("Low-Fi noise selected");
                    else puts("Default noise selected");
                }
                if (strstr(buffer, "octaves")) {
                    puts("Octaves selected");
                    for (unsigned s = 0; s < strlen(buffer); s++) {
                        char ch = buffer[s];
                        if(isdigit(ch)) octaves = ch - '0'; // see if octaves needs to go greater beyond 9, which is the limit!
                        printf("Parsing character %c, new octave count is %d", ch, octaves);
                    }
                }
                else if (strstr(buffer, "grid")) {
                    puts("Grid selected");
                    // TODO: Determine new grid dimensions
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

            for (unsigned e = 0; e < b; e++) {
                buffer[e] = '\0'; // clears buffer
                b = 0;
            }
        }
    }

    if (octaves <= 1) {
        return noiseImgOp((ImageSize){ 1024, 1024 }, grid);
    }
    else {
        return noiseImgOp_octave((ImageSize) { 1024, 1024 }, grid, octaves);
    }
    // return noiseImgOp((ImageSize){ 1024, 1024 }, grid);
}
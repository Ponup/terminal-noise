#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#define put_out(x) fputs(x, stdout)

static struct timeval tv = {
	.tv_sec = 0L,
	.tv_usec = 0L
};

static void reset_terminal() {
    put_out("\033[0m"); // reset background
    put_out("\033[2J"); // clear screen
    put_out("\033[0;0H");
    put_out("\033[?25h"); // show cursor
}

static bool input_ready() {
	fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(STDIN_FILENO, &read_fds);

    return select(1, &read_fds, NULL, NULL, &tv) == 1;
}

int main(int argc, char **argv) {
    time_t t;
    time(&t);

    srand(t);

    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    size_t buffer_size = sizeof(char) * 12 * w.ws_row * w.ws_col;

    char *screen_buffer = (char*)malloc(buffer_size);
    memset(screen_buffer, '\0', buffer_size);

    put_out("\033[?25l"); // hide cursor

    struct termios custom_termsettings, original_termsettings;
    tcgetattr(STDIN_FILENO, &custom_termsettings);
    original_termsettings = custom_termsettings;
    custom_termsettings.c_lflag &= ~(ECHO | ICANON | ISIG);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &custom_termsettings);

    setvbuf(stdout, NULL, _IONBF, 0);

    bool is_input_ready = 0;
    int buffer_pos = 0;
    size_t num_terminal_chars = w.ws_row * w.ws_col;
    while(!is_input_ready) {
    	buffer_pos = 0;
        for (int c = 0; c < num_terminal_chars; c++) {
			buffer_pos += sprintf(screen_buffer + buffer_pos, "\x1b[48;5;%dm ", rand() % 256);
		}
		write(STDOUT_FILENO, "\033[0;0H", 6);
		write(STDOUT_FILENO, screen_buffer, buffer_pos);

        is_input_ready = input_ready();
        if(!is_input_ready) sleep(1);
    }

    free(screen_buffer);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termsettings);

    reset_terminal();

    return EXIT_SUCCESS;
}


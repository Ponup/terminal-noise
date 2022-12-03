#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/select.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#define WRITE_OUT(x, y) write(STDOUT_FILENO, x, sizeof(char) * y)

static struct timeval tv = {
	.tv_sec = 0L,
	.tv_usec = 0L
};

static void reset_terminal() {
    WRITE_OUT("\033[0m", 4); // reset background
    WRITE_OUT("\033[2J", 4); // clear screen
    WRITE_OUT("\033[0;0H", 6);
    WRITE_OUT("\033[?25h", 6); // show cursor
}

static bool input_ready() {
	fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(STDIN_FILENO, &read_fds);

    return select(1, &read_fds, NULL, NULL, &tv) == 1;
}

const char *rand_number_char() {
	static char number_buffer[5];
	sprintf(number_buffer, "%u", rand() % 256);
	return number_buffer;
}

int main(int argc, char **argv) {
    time_t t;
    time(&t);

    srand(t);

    struct winsize term_size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &term_size);

    const size_t buffer_size = sizeof(char) * 12 * term_size.ws_row * term_size.ws_col;

    char *screen_buffer = (char*)malloc(buffer_size);
    memset(screen_buffer, '\0', buffer_size);

    WRITE_OUT("\033[?25l", 6); // hide cursor

    struct termios custom_termsettings, original_termsettings;
    tcgetattr(STDIN_FILENO, &custom_termsettings);
    original_termsettings = custom_termsettings;
    custom_termsettings.c_lflag &= ~(ECHO | ICANON | ISIG);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &custom_termsettings);

    setvbuf(stdout, NULL, _IONBF, 0);

    const struct timespec sleep_ts = {
    	.tv_sec = 0,
    	.tv_nsec = 50000000
    };

    bool is_input_ready = false;
    const size_t num_terminal_chars = term_size.ws_row * term_size.ws_col;
    while(!is_input_ready) {
    	memset(screen_buffer, '\0', buffer_size);
		strcat(screen_buffer, "\033[0;0H");
        for (size_t c = 0; c < num_terminal_chars; c++) {
    		strcat(screen_buffer, "\x1b[48;5;");
    		strcat(screen_buffer, rand_number_char());
    		strcat(screen_buffer, "m ");
		}
		WRITE_OUT(screen_buffer, buffer_size);

        is_input_ready = input_ready();
        if(!is_input_ready) {
        	nanosleep(&sleep_ts, NULL);
        }
    }

    free(screen_buffer);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termsettings);

    reset_terminal();

    return EXIT_SUCCESS;
}


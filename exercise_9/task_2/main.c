#include <ncurses.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define MAX_FILES 1000

typedef struct {
    char *path;
    char *files[MAX_FILES];
    int count;
    int selected;
} Panel;

void init_panel(Panel *panel, const char *path) {
    panel->path = strdup(path);
    panel->count = 0;
    panel->selected = 0;
}

void free_panel(Panel *panel) {
    free(panel->path);
    for (int i = 0; i < panel->count; i++) {
        free(panel->files[i]);
    }
}

void update_panel(Panel *panel) {
    DIR *dir = opendir(panel->path);
    if (!dir) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    panel->count = 0;
    struct dirent *entry;

    // Always add ".." at the beginning
    panel->files[panel->count++] = strdup("..");

    while ((entry = readdir(dir)) != NULL) {
        if (panel->count >= MAX_FILES) break;
        if (strcmp(entry->d_name, "..") != 0 && strcmp(entry->d_name, ".") != 0) {
            panel->files[panel->count++] = strdup(entry->d_name);
        }
    }
    closedir(dir);
}

void draw_panel(Panel *panel, WINDOW *win, int y, int x) {
    wclear(win);

    // Get the width of the window
    int win_width = getmaxx(win);

    // Truncate the path if it's too long
    char truncated_path[win_width + 1];
    if (strlen(panel->path) > win_width) {
        snprintf(truncated_path, win_width, "..%s", panel->path + strlen(panel->path) - win_width + 3);
    } else {
        strcpy(truncated_path, panel->path);
    }

    mvwprintw(win, 0, 0, "%s", truncated_path);
    for (int i = 0; i < panel->count; i++) {
        if (i == panel->selected) {
            wattron(win, A_REVERSE);
        }
        mvwprintw(win, i + 1, 0, "%s", panel->files[i]);
        if (i == panel->selected) {
            wattroff(win, A_REVERSE);
        }
    }
    wrefresh(win);
}

int is_directory(const char *path) {
    struct stat statbuf;
    if (stat(path, &statbuf) != 0) return 0;
    return S_ISDIR(statbuf.st_mode);
}

void navigate_panel(Panel *panel, int key) {
    switch (key) {
        case KEY_UP:
            if (panel->selected > 0) panel->selected--;
            break;
        case KEY_DOWN:
            if (panel->selected < panel->count - 1) panel->selected++;
            break;
        case KEY_ENTER:
        case '\n': {
            char new_path[1024];
            snprintf(new_path, sizeof(new_path), "%s/%s", panel->path, panel->files[panel->selected]);
            if (is_directory(new_path)) {
                free(panel->path);
                panel->path = strdup(new_path);
                panel->selected = 0;
                update_panel(panel);
            }
            break;
        }
    }
}

int main() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    int height = LINES - 2;
    int width = COLS / 2;

    WINDOW *left_win = newwin(height, width, 1, 0);
    WINDOW *right_win = newwin(height, width, 1, width);

    Panel left_panel, right_panel;
    init_panel(&left_panel, "/");
    init_panel(&right_panel, "/");

    update_panel(&left_panel);
    update_panel(&right_panel);

    int current_panel = 0; // 0 for left, 1 for right

    while (1) {
        draw_panel(&left_panel, left_win, 1, 0);
        draw_panel(&right_panel, right_win, 1, width);

        int key = getch();
        if (key == 'q') break;

        if (key == '\t') {
            current_panel = !current_panel;
        } else {
            if (current_panel == 0) {
                navigate_panel(&left_panel, key);
            } else {
                navigate_panel(&right_panel, key);
            }
        }
    }

    free_panel(&left_panel);
    free_panel(&right_panel);

    delwin(left_win);
    delwin(right_win);
    endwin();

    return 0;
}
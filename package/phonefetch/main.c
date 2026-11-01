#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/ioctl.h>

#define reset   "\033[0m"
#define bold    "\033[1m"
#define black   "\033[1;30m"
#define white   "\033[1;37m"
#define yellow  "\033[1;33m"

#define lc reset bold
#define nc reset bold
#define ic reset
#define c0 reset black
#define c1 reset white
#define c2 reset yellow

const char* tux_img[7] = { // https://gitlab.com/jschx/ufetch/-/blob/main/ufetch-linux
c0 "      ___   ",
c0 "     (" c1 ".. " c0 "\\  ",
c0 "     (" c2 "<> " c0 "|  ",    
c0 "    /" c1 "/  \\ " c0 "\\ ",   
c0 "   ( " c1 "|  | " c0 "/|",  
c2 "  _" c0 "/\\ " c1 "__)" c0 "/" c2 "_" c0 ")",  
c2 "  \\/" c0 "-____" c2 "\\/ " reset
};

void get_os(char *buf, size_t len) {
    snprintf(buf, len, "Buildroot");
    FILE *fp = fopen("/etc/os-release", "r");
    if (fp) {
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, "PRETTY_NAME=", 12) == 0) {
                char *start = strchr(line, '"');
                if (start) {
                    start++;
                    char *end = strchr(start, '"');
                    if (end) *end = '\0';
                    snprintf(buf, len, "%s", start);
                } else {
                    char *val = line + 12;
                    val[strcspn(val, "\r\n")] = 0;
                    snprintf(buf, len, "%s", val);
                }
                break;
            }
        }
        fclose(fp);
    }
}

void get_cpu_and_model(char *cpu, size_t cpu_len, char *model, size_t model_len) {
    snprintf(cpu, cpu_len, "Unknown");
    snprintf(model, model_len, "Unknown");
    FILE *fp = fopen("/proc/cpuinfo", "r");
    if (fp) {
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, "Hardware", 8) == 0) {
                char *val = strchr(line, ':');
                if (val) {
                    val += 2;
                    val[strcspn(val, "\r\n")] = 0;
                    snprintf(cpu, cpu_len, "%s", val);
                }
            }
            if (strncmp(line, "Model", 5) == 0) {
                char *val = strchr(line, ':');
                if (val) {
                    val += 2;
                    val[strcspn(val, "\r\n")] = 0;
                    snprintf(model, model_len, "%s", val);
                }
            }
        }
        fclose(fp);
    }
}

void get_mem(char *buf, size_t len) {
    int total = 0, free = 0;
    FILE *fp = fopen("/proc/meminfo", "r");
    if (fp) {
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, "MemTotal:", 9) == 0) sscanf(line + 9, "%d", &total);
            if (strncmp(line, "MemFree:", 8) == 0)  sscanf(line + 8, "%d", &free);
        }
        fclose(fp);
    }
    snprintf(buf, len, "%d kB / %d kB", (total - free), total);
}

void get_uptime(char *buf, size_t len) {
    FILE *fp = fopen("/proc/uptime", "r");
    if (fp) {
        char line[64];
        if (fgets(line, sizeof(line), fp)) {
            char *dot = strchr(line, '.');
            if (dot) *dot = '\0';
            int uptime_sec = atoi(line);
            
            int h = uptime_sec / 3600;
            int m = (uptime_sec % 3600) / 60;
            if (h > 0) snprintf(buf, len, "%dh %dm", h, m);
            else snprintf(buf, len, "%dm", m);
        }
        fclose(fp);
    } else {
        snprintf(buf, len, "Unknown");
    }
}

void truncate_str(char *str, int max_len) {
    if (max_len <= 0) {
        str[0] = '\0';
        return;
    }
    if ((int)strlen(str) > max_len) {
        if (max_len > 3) {
            str[max_len - 3] = '.';
            str[max_len - 2] = '.';
            str[max_len - 1] = '.';
        }
        str[max_len] = '\0';
    }
}

int main() {
    char info[7][128] = {0};
    
    char host[64] = "unknown";
    gethostname(host, sizeof(host));
    char *user = getenv("USER");
    if (!user) user = "root";

    char title[128];
    snprintf(title, sizeof(title), "%s@%s", user, host);

    char os_name[64]; get_os(os_name, sizeof(os_name));
    struct utsname uts; uname(&uts);
    char uptime[32]; get_uptime(uptime, sizeof(uptime));
    char cpu[64], model[64]; get_cpu_and_model(cpu, sizeof(cpu), model, sizeof(model));
    char mem[64]; get_mem(mem, sizeof(mem));

    int term_width = 80;
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) != -1 && w.ws_col > 0) {
        term_width = w.ws_col;
    }

    int max_visible = term_width - 14; 
    if (max_visible < 8) max_visible = 8;
    
    truncate_str(title, max_visible);

    int max_val = max_visible - 8;
    
    truncate_str(os_name, max_val);
    truncate_str(uts.release, max_val);
    truncate_str(uptime, max_val);
    truncate_str(model, max_val);
    truncate_str(cpu, max_val);
    truncate_str(mem, max_val);

    snprintf(info[0], sizeof(info[0]), "%s%s%s", lc, title, reset);
    snprintf(info[1], sizeof(info[1]), "%sOS:     %s%s%s", lc, ic, os_name, reset);
    snprintf(info[2], sizeof(info[2]), "%sKERNEL: %s%s%s", lc, ic, uts.release, reset);
    snprintf(info[3], sizeof(info[3]), "%sUPTIME: %s%s%s", lc, ic, uptime, reset);
    snprintf(info[4], sizeof(info[4]), "%sMODEL:  %s%s%s", lc, ic, model, reset);
    snprintf(info[5], sizeof(info[5]), "%sCPU:    %s%s%s", lc, ic, cpu, reset);
    snprintf(info[6], sizeof(info[6]), "%sMEM:    %s%s%s", lc, ic, mem, reset);

    for(int i = 0; i < 7; ++i) {
        printf("%s  %s\n", tux_img[i], info[i]);
    }

    return 0;
}
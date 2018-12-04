#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#define COLOR_YELLOW "\033[93m"
#define COLOR_BLUE "\033[94m"
#define COLOR_GREEN "\033[92m"
#define COLOR_RED "\033[91m"
#define COLOR_ENDC "\033[0m"

#define MAX_PATH_LEN 8192



int checkByte(char byte)
{
    if (byte == 13)
        return 1;
    if (byte == 10)
        return 1;
    if (byte == 9)
        return 1;
    if (byte >= 32 && byte <= 126)
        return 1;

    return 0;
}


int readText(char *filename, char **text)
{
    static char buf[10 * 1024 * 1024];
    char b = 0;
    *text = buf;
    int cnt = 0;

    FILE *f = fopen(filename, "r");
    if (!f)
        return 0;

    for(;;) {
        b = getc(f);
        if (b == EOF) {
            fclose(f);
            buf[cnt] = 0;
            return cnt;
        }

        if (!checkByte(b)) {
            fclose(f);
            return 0;
        }

        if (cnt >= (10 * 1024 * 1024) - 1) {
            fclose(f);
            return 0;
        }
        buf[cnt++] = b;
    }

    buf[cnt] = 0;
    return cnt;
}

int is_directory(char *path)
{
 /*  struct stat statbuf;
   if (stat(path, &statbuf) != 0)
       return 0;
   return S_ISDIR(statbuf.st_mode);*/
    DIR *dp;
    dp = opendir(path);
    if (!dp)
        return 0;
    closedir(dp);
    return 1;
}


int textsplit(char *content, int len, char ***list)
{
    char *end = content + len - 1;
    char *p;
    int str_cnt = 0;
    static char **strings;

    strings = malloc(sizeof(char *) * 1024 * 1024);

    *list = strings;
    int cnt = 0;

    for(p = content; p <= end; p++) {
        if (*p == '\r') {
            *p = 0;
            continue;
        }

        cnt ++;
        if (cnt == 1) {
            strings[str_cnt++] = p;
        }

        if (*p == '\n') {
            cnt = 0;
            *p = 0;
            continue;
        }

    }

    return str_cnt;
}


char *str_replace(char *content, char *subject, char *replace)
{
    static char buf[1 * 1024 * 1024];
    char *src_p, *dst_p, *start, *end;
    dst_p = buf;
    int part_len;

    start = src_p = content;
    end = start + strlen(content);
    for (;;) {
        src_p = strstr(start, subject);
        if (!src_p) {
            memcpy(dst_p, start, end - start);
            dst_p += end - start;
            *dst_p = 0;
            break;
        }

        part_len = src_p - start;
        memcpy(dst_p, start, part_len);
        dst_p += part_len;
        memcpy(dst_p, replace, strlen(replace));
        dst_p += strlen(replace);
        start = src_p + strlen(subject);
    }
    return buf;
}

void serachDir(char *dir, char *serachText) {
    static char buf[1 * 1024 * 1024];
    DIR *dp;
    struct dirent *ep;
    char **strings;
    dp = opendir(dir);
    if (!dp)
        return;

    while (ep = readdir(dp)) {
        char path[MAX_PATH_LEN];
        char *file;
        char *file_content;
        char *found_str;
        int fsize;
        int rows_cnt = 0;
        int i;
        if (strcmp(ep->d_name, ".") == 0)
            continue;
        if (strcmp(ep->d_name, "..") == 0)
            continue;

        sprintf(path, "%s/%s", dir, ep->d_name);

        if (is_directory(path)) {
            serachDir(path, serachText);
            continue;
        }
        file = path;

        fsize = readText(file, &file_content);
        if (!fsize)
            continue;

        rows_cnt = textsplit(file_content, fsize, &strings);
      //  printf("rows_cnt = %d, file = %s\n", rows_cnt, file);
        for (i = 0; i < rows_cnt; i++) {
            char *string = strings[i];
            if (!strstr(string, serachText))
                continue;

            sprintf(buf, "%s%s%s", COLOR_YELLOW, serachText, COLOR_ENDC);
            printf("%s +%d: %s\n", file, i + 1,
                                   str_replace(string, serachText, buf));
        }
    }

    closedir(dp);
}

int main(int argc, char *argv[])
{
    if (argc <= 1)
        return 1;
//    char *result;
  //  result = str_replace("aabbccddee", "ss", "22211");
    //printf("result = %s\n", result);

    serachDir(".", argv[1]);
    /*char **list;
    int len;
    char *text;
    int i;
    len = readText(argv[1], &text);
    len = textsplit(text, len, &list);
    printf("count str = %d\n", len);
    for (i = 0; i < len; i++)
        printf("%d: str = %s\n", i, list[i]);
*/
    return 0;
}
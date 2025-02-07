#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define PRINT_STR(str) print_utf8(str, 0)
#define PRINT_ERR(str) print_utf8(str, 1)

static const char *VERSION = "1.0.0";
static int show_msg = 0;

void print_utf8(const char *text, int error) {
    if (show_msg) return;
    UINT cp = GetConsoleOutputCP();
    if (cp == 936) {
        int wideCharLen = MultiByteToWideChar(CP_UTF8, 0, text, -1, NULL, 0);
        if (wideCharLen == 0) goto fallback;

        wchar_t *wideBuffer = (wchar_t *)malloc(wideCharLen * sizeof(wchar_t));
        if (!wideBuffer) goto fallback;

        if (MultiByteToWideChar(CP_UTF8, 0, text, -1, wideBuffer, wideCharLen) == 0) {
            free(wideBuffer);
            goto fallback;
        }

        int gbkLen = WideCharToMultiByte(936, 0, wideBuffer, -1, NULL, 0, NULL, NULL);
        if (gbkLen == 0) {
            free(wideBuffer);
            goto fallback;
        }

        char *gbkBuffer = (char *)malloc(gbkLen);
        if (!gbkBuffer) {
            free(wideBuffer);
            goto fallback;
        }

        if (WideCharToMultiByte(936, 0, wideBuffer, -1, gbkBuffer, gbkLen, NULL, NULL) == 0) {
            free(wideBuffer);
            free(gbkBuffer);
            goto fallback;
        }

        if (error) {
            fputs(gbkBuffer, stderr);
            fputc('\n', stderr);
        } else {
            fputs(gbkBuffer, stdout);
            fputc('\n', stdout);
        }

        free(wideBuffer);
        free(gbkBuffer);
        return;
    }
fallback:;
    if (cp != CP_UTF8) SetConsoleOutputCP(CP_UTF8);

    if (error) {
        fputs(text, stderr);
        fputc('\n', stderr);
    } else {
        fputs(text, stdout);
        fputc('\n', stdout);
    }

    if (cp != CP_UTF8) SetConsoleOutputCP(cp);
}

void showHelp() {
    PRINT_STR(
        "用法：\n"
        "  codepage [选项] [参数]\n\n"
        "选项：\n"
        "  get            获取当前终端代码页\n"
        "  set [数字]     设置当前终端代码页\n"
        "  -v, --version  显示版本信息\n"
        "  -h, --help     显示帮助信息\n"
        "  -q, --quiet    静默模式，不输出控制台信息\n\n");
}

void showVersion() {
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "codepage 版本 %s", VERSION);
    PRINT_STR(buffer);
}

void getCodePage() {
    UINT cp_input = GetConsoleCP();
    UINT cp_output = GetConsoleOutputCP();
    char buffer[1024];

    if (cp_input == cp_output) {
        snprintf(buffer, sizeof(buffer), "当前终端代码页：%u", cp_input);
    } else {
        snprintf(buffer, sizeof(buffer),
                 "当前终端输入代码页：%u\n当前终端输出代码页：%u",
                 cp_input, cp_output);
    }
    PRINT_STR(buffer);
}

int setCodePage(const char *arg) {
    char *endptr;
    unsigned long newCP = strtoul(arg, &endptr, 10);

    if (*endptr != '\0' || endptr == arg) {
        PRINT_ERR("错误：无效的数字格式");
        return 0;
    }

    if (newCP > 0xFFFF) {
        PRINT_ERR("错误：数值超出有效范围");
        return 0;
    }

    UINT oldInputCP = GetConsoleCP();
    UINT oldOutputCP = GetConsoleOutputCP();
    UINT newCP_uint = (UINT)newCP;

    if (newCP_uint == oldInputCP && newCP_uint == oldOutputCP) {
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "未修改：当前代码已设为：%u", newCP_uint);
        PRINT_STR(buffer);
        return 1;
    }

    BOOL setInputSuccess = SetConsoleCP(newCP_uint);
    BOOL setOutputSuccess = SetConsoleOutputCP(newCP_uint);

    if (setInputSuccess && setOutputSuccess) {
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "成功：设置代码页为：%u", newCP_uint);
        PRINT_STR(buffer);
        return 1;
    } else {
        PRINT_ERR("错误：无法设置代码页（系统可能不支持该编码）");
        SetConsoleCP(oldInputCP);
        SetConsoleOutputCP(oldOutputCP);
        return 0;
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        PRINT_ERR("错误：缺少选项\n");
        showHelp();
        return 1;
    }

    char *option = argv[1];
    int q = 0;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-q") == 0 || strcmp(argv[i], "--quiet") == 0) {
            show_msg = 1;
            if (i == 1) {
                if (argc == 2) return 0;
                option = argv[2];
                q = 1;
            }
            break;
        }
    }

    if (strcmp(option, "get") == 0) {
        if (argc > show_msg + 2) {
            PRINT_ERR("错误：get 选项不需要额外参数\n");
            return 1;
        }
        getCodePage();
    } else if (strcmp(option, "set") == 0) {
        if (argc < q + 3) {
            PRINT_ERR("错误：set 选项需要指定代码页数字\n");
            return 1;
        }
        if (argc > show_msg + 3) {
            PRINT_ERR("错误：set 选项只能接受一个参数\n");
            return 1;
        }
        if (!setCodePage(argv[q + 2])) return 1;
    } else if (strcmp(option, "-v") == 0 || strcmp(option, "--version") == 0) {
        showVersion();
    } else if (strcmp(option, "-h") == 0 || strcmp(option, "--help") == 0) {
        PRINT_STR("终端代码页修改工具\n");
        showHelp();
    } else {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "错误：未知选项: %s\n", option);
        PRINT_ERR(buffer);
        showHelp();
        return 1;
    }

    return 0;
}
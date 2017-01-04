/* test C Language api */

#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

/*
int main(int argc, char **argv)
{
   int c;
   int digit_optind = 0;

   while (1) {
       int this_option_optind = optind ? optind : 1;
       int option_index = 0;
       static struct option long_options[] = {
           {"add",     required_argument, 0,  0 },
           {"append",  no_argument,       0,  0 },
           {"delete",  required_argument, 0,  0 },
           {"verbose", no_argument,       0,  0 },
           {"create",  required_argument, 0, 'c'},
           {"file",    required_argument, 0,  0 },
           {0,         0,                 0,  0 }
       };

       c = getopt_long(argc, argv, "abc:d:012",
                long_options, &option_index);
       if (c == -1)
           break;

       switch (c) {
       case 0:
           printf("option %s", long_options[option_index].name);
           if (optarg)
               printf(" with arg %s", optarg);
           printf("\n");
           break;

       case '0':
       case '1':
       case '2':
           if (digit_optind != 0 && digit_optind != this_option_optind)
             printf("digits occur in two different argv-elements.\n");
           digit_optind = this_option_optind;
           printf("option %c\n", c);
           digit_optind = this_option_optind;
           printf("option %c\n", c);
           break;

       case 'a':
           printf("option a\n");
           break;

       case 'b':
           printf("option b\n");
           break;

       case 'c':
           printf("option c with value '%s'\n", optarg);
           break;

       case 'd':
           printf("option d with value '%s'\n", optarg);
           break;

       case '?':
           break;

       default:
           printf("?? getopt returned character code 0%o ??\n", c);
       }
   }

   if (optind < argc) {
       printf("non-option ARGV-elements: ");
       while (optind < argc)
           printf("%s ", argv[optind++]);
       printf("\n");
   }

   exit(EXIT_SUCCESS);
}*/


typedef enum cmd_type_e {
    MIN = 0,
    CMD_SHOW,
    CMD_DEBUG,
    CMD_ADD,
    CMD_DEL,
    CMD_REPLACE,
    CMD_FIND,
    CMD_HELP,
    MAX,
} cmd_type_t;

static struct option test_option[] = {
        {"show", optional_argument, 0, 's'},
        {"debug", required_argument, 0, 'D'},
        {"add", required_argument, 0, 'a'},
        {"del", required_argument, 0,  'd'},
        {"replace", required_argument, 0, 'r'},
        {"find", required_argument, 0, 'f'},
        {"version", no_argument, 0, 'v'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0},

};

typedef union strong_option_cb {
    int (*requird_argument_has_ret_cb)(char *);
    void (*requird_argument_cb)(char *);
    int (*optional_argument_has_ret_cb)(char *);
    void (*optional_argument_cb)(char *);
    int (*no_argument_has_ret_cb)(void);
    void (*no_argument_cb)(void);
} strong_opt_cb_t;

typedef struct strong_option {
    struct option opt;
    bool has_ret;
    strong_opt_cb_t cb;
} strong_opt_t;

#define test_option_nr (sizeof(test_option)/sizeof(test_option[0]))

static void cmd_show(char *argv);
static int cmd_debug(char *argv);
static void cmd_version(void);
static void test_usage(void);

static strong_opt_t strong_opt_test[] = {
        {{"show", optional_argument, 0, 's'}, false, {.optional_argument_cb = cmd_show}},
        {{"debug", required_argument, 0, 'D'}, true, {.requird_argument_has_ret_cb = cmd_debug}},
        {{"add", required_argument, 0, 'a'}, true, {NULL}},
        {{"del", required_argument, 0,  'd'}, true, {NULL}},
        {{"replace", required_argument, 0, 'r'}, true, {NULL}},
        {{"find", required_argument, 0, 'f'}, true, {NULL}},
        {{"version", no_argument, 0, 'v'}, false, {.no_argument_cb = cmd_version}},
        {{"help", no_argument, 0, 'h'}, false, {.no_argument_cb = test_usage}},
};

#define strong_test_option_nr (sizeof(strong_opt_test)/sizeof(strong_opt_test[0]))

static void cmd_show(char *argv)
{
    printf("show \r\n");
}

static int cmd_debug(char *argv)
{
    printf("debug %s\r\n", argv);

    return 0;
}

static void cmd_version(void)
{
    printf("test verion: 0.01\r\n");
}

static void test_usage(void)
{
    printf("\
            test usage:\r\n \
            base command:\r\n \
            show \r\n \
            debug \r\n \
            add \r\n \
            del \r\n \
            replace \r\n \
            find \r\n \
            help \r\n");
}

static int strong_option_cmp(const void *src, const void *dst)
{
    const strong_opt_t *src_opt, *dst_opt;

    src_opt = src;
    dst_opt = dst;

    return strcmp(src_opt->opt.name, dst_opt->opt.name);
}

static bool opt_is_std_style(char *opt)
{
    if (strchr(opt, '-') != NULL) {
        return true;
    }

    return false;
}

static int do_strong_opt_cb(strong_opt_t *opt, char *argv)
{
    int ret;

    if (opt->cb.no_argument_cb == NULL) {
        return 0;
    }

    switch (opt->opt.has_arg) {
    case required_argument:
        if (opt->has_ret) {
            ret = opt->cb.requird_argument_has_ret_cb(argv);
        } else {
            ret = 0;
            opt->cb.requird_argument_cb(argv);
        }
        break;
    case optional_argument:
        if (opt->has_ret) {
            ret = opt->cb.optional_argument_has_ret_cb(argv);
        } else {
            ret = 0;
            opt->cb.optional_argument_cb(argv);
        }
        break;
    case no_argument:
        if (opt->has_ret) {
            ret = opt->cb.no_argument_has_ret_cb();
        } else {
            ret = 0;
            opt->cb.no_argument_cb();
        }
        break;
    default:
        ret = -1;
        break;
    }

    return ret;
}

/* if parse command have std opt, will call std */
static int parse_unusual_opt(int argc, char **argv)
{
    int opt_count, i, ret;
    char *opt;
    strong_opt_t find, *result;

    if (argc <= 1) {
        return -1;
    }
    opt = argv[0];
    /* ingnore process_name */
    opt_count = argc - 1;
    opt++;
    qsort(strong_opt_test, strong_test_option_nr, sizeof(strong_opt_t), strong_option_cmp);
    for (i = 0; i < opt_count; i++) {
        if (opt_is_std_style(argv[i + 1])) {
            /* TODO */
            continue;
        }
        bzero(&find, sizeof(strong_opt_t));
        find.opt.name = argv[i + 1];
        result = bsearch(&find, strong_opt_test, strong_test_option_nr, sizeof(strong_opt_t), strong_option_cmp);
        if (result != NULL) {
            ret = do_strong_opt_cb(result, argv[i + 2]);
        }
    }

    return ret;
}

int main(int argc, char *argv[])
{
    int ret, opt_index;

    opt_index = 0;
    parse_unusual_opt(argc, argv);

    while (1) {
        ret = getopt_long(argc, argv, "+a:D:r:f:d:hv?s::", test_option, &opt_index);
        if (ret == -1) {
            break;
        }
        printf("getopt_long ret val: %d\r\n", ret);
        switch (ret) {
        case 's':
        case CMD_SHOW:
            cmd_show(optarg);
            break;
        case 'd':
            printf("del \r\n");
            break;
        case 'D':
            cmd_debug(optarg);
            break;
        case 'h':
        case '?':
            test_usage();
            break;
        case 'v':
            printf("%s cmd run\r\n", test_option[opt_index].name);
            printf("test verion: 0.01\r\n");
            break;
        default:
            printf("unknow command flags.\r\n");
            break;
        }
    }

    return 0;
}

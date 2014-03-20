#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <pcre.h>
#include <getopt.h>
#include <stdlib.h>
#include <shell.h>
#include <alloca.h>
#define BUFSIZE 30
#define ERROR_NOMATCH 1
#define ERROR_WRONG_ARG 2
#define ERROR_SYNTAXERR_PATTERN 3

static const char  *usage = "Usage: match [ options ] regexp subject [capture] \n"
"Test if subject can be matched by regexp by using pcre library.\n\n"
"  -A, --anchored pattern is constrained to match only at the first\n"
"                 matching point in the string\n"
"  -a, --dotall   a dot in the pattern matches a char of any value\n"
"  -b, --bufsize  set the output vector size\n"
"  -e, --extended white spaces and comment(#...) is ignoreed unless escaped\n"
"  -i, --caseless letters in the pattern match letters in both cases\n"
"  -v, --version  output version information and exit\n"
"  -u, --utf8     treat regexp as sequence of utf8 chars instead of bytes\n"
"\n"
"EXAMPLES\n"
"  match '\\d+' 123           should return 0(success)\n"
"  match '(\\d+)a' 123a out   '123' is stored in ${out[1]}\n\n"
"EXIT CODE\n"
"0  match successfully\n"
"1  match failed\n"
"2  unrecognized option(s)\n"
"3  syntax error in regexp\n";

int b_match(int argc, char *argv[], struct Shbltin_s *shcontext)
{
    optind = 0;   /* reset getopt arguments to rescan argv
                     forced by gnu-getopt. (instead of traditional 1*/
    struct option long_options[] = {
        {"version", no_argument, NULL, 'v'},
        {"bufsize", required_argument, NULL, 'b'},
        {"dotall", no_argument, NULL, 'a'},
        {"utf8", no_argument, NULL, 'u'},
        {"extended", no_argument, NULL, 'e'},
        {"caseless", no_argument, NULL, 'i'},
        {"anchored", no_argument, NULL, 'A'},
        {0, 0, 0, 0}
    };
    int getopt_ret;
    int retcode;
    int bufsize = BUFSIZE;
    int pcreopt = 0;

    while ((getopt_ret = getopt_long(argc, argv,
                                    "aeiAuvb:", long_options, NULL)) != -1) {
        // break when meet -1 (end of scanning)
        switch (getopt_ret) {
            case '?':
                // unknown option, simply exit
                break;
            case 'b':
                bufsize = atoi(optarg);
                bufsize *= 3;
                if (bufsize <= BUFSIZE) {
                    bufsize = BUFSIZE;
                }
                break;
            case 'a':
                pcreopt |= PCRE_DOTALL;
                break;
            case 'e':
                pcreopt |= PCRE_EXTENDED;
                break;
            case 'i':
                pcreopt |= PCRE_CASELESS;
                break;
            case 'A':
                pcreopt |= PCRE_ANCHORED;
                break;
            case 'u':
                pcreopt |= PCRE_UTF8;
                break;
            case 'v':
                fprintf(stdout, "match, with ksh%d & libpcre %s\n",
                        shcontext->version, pcre_version());
                return 0;
        }
    }

    const char *pattern;
    const char *subject;
    Namval_t   *outarg = NULL;

    if (optind == argc - 2) {
        pattern = argv[optind];
        subject = argv[optind + 1];
    } else if (optind == argc - 3) {
        pattern = argv[optind];
        subject = argv[optind + 1];
        Shscope_t *scope;
        const char *varname = argv[optind+2];

        scope = sh_getscope(0, SEEK_CUR);
        outarg = nv_open(varname, scope->var_tree, NV_NOADD | NV_NOSCOPE);
        if (!outarg) {
            // try the caller's scope if the builtin is wrappeed inside a
            // function named exactly 'match'
            if (scope && strncmp(scope->cmdname, "match", 6) == 0) {
                scope = scope->par_scope;
                outarg = nv_open(varname, scope->var_tree, NV_NOADD | NV_NOSCOPE);
            }

            // otherwise, it failed to find var in local scope(s), create
            // variable in global scope
            if (!outarg) {
                scope = sh_getscope(0, SEEK_SET);
                outarg = nv_open(varname, scope->var_tree, NV_IARRAY);
            }
        }
    } else {
        fprintf(stderr, "%s\n", usage);
        return ERROR_WRONG_ARG;
    }

    pcre  *regexp = NULL;
    const char *error;
    int   erroroffset;
    int   result;
    int   ovector[bufsize];

    regexp = pcre_compile(pattern
            , pcreopt
            , &error        /* error message */
            , &erroroffset
            , NULL);        /* use default character table */
    if (regexp == NULL) {
        fprintf(stderr, "PCRE compilation failed at offset %d: %s\n", erroroffset, error);
        retcode = ERROR_SYNTAXERR_PATTERN;
        goto finish_main;
    }


    result = pcre_exec(regexp
            , NULL     /* no extra data*/
            , subject
            , (int)strlen(subject)
            , 0        /* start at offset 0 of subject string */
            , 0        /* default options*/
            , ovector /* output vector for substring information */
            , bufsize);
    if (result < 0) {
        switch (result) {
            case PCRE_ERROR_NOMATCH:
                //fprintf(stderr, "No Match\n");
                break;
            default:
                fprintf(stderr, "matching error\n");
                break;
        }
        retcode = ERROR_NOMATCH;
        goto finish_main;
    } else if (result == 0) {
        // match success, but ovector is not big enough to hold captures
        retcode = 4;
    } else if (result > 0) {
        /* successfull match*/
        if (outarg != NULL) {
            char *appvec[result];
            for (int i=0; i < result; i++) {
                const char *start = subject + ovector[2*i];
                int substring_length = ovector[2*i+1] - ovector[2*i];
                char *buf = alloca(substring_length + 1);
                snprintf(buf, substring_length+1, "%s", start);
                appvec[i] = buf;
            }
            nv_setvec(outarg, 0, result, appvec);
        }
        retcode = 0;
    }

finish_main:
    if (regexp) {
        pcre_free(regexp);
    }
    if (outarg) {
        nv_close(outarg);
    }
    return retcode;
}

unsigned long plugin_version()
{
    return 20130904L;
}

#ifndef ASS_TAG_H
#define ASS_TAG_H

/*EOL str of ASS.*/
static const char ASS_TAG_HARD_EOL[]  = "\\N";
static const char ASS_TAG_SOFT_EOL[]  = "\\n";
static const char ASS_TAG_BLANK[]     = "\\h";
static const char ASS_TAG_BLANK2[]    = "　";
static const char ASS_TAG_SKIP_FLAG[] = "\\";
static const int ASS_TAG_SKIP_NUM     = 2;

/*KARAOKE tag of ASS.*/
static const char ASS_TAG_KARAOKE_K_LOWER[] = "\\k";
static const char ASS_TAG_KARAOKE_K_UPPER[] = "\\K";
static const char ASS_TAG_KARAOKE_KF[]      = "\\kf";
static const char ASS_TAG_KARAOKE_KO[]      = "\\ko";

/*Ext of ASS.*/
static const char ASS_EXT_TYPE_A[] = "ass";
static const char ASS_EXT_TYPE_B[] = "ssa";
static const char ASS_EXT_TYPE_C[] = "txt";

/*AssTt tool.*/
static const char ASS_TAG_TT[]                    = "\\t";
static const char ASS_TAG_STR_TT[]                = "%1(%2,%3,%4)";

/*Tag of LRC*/
static const char LRC_TAG_TI[]        = "[ti:%1]";
static const char LRC_TAG_AR[]        = "[ar:%1]";
static const char LRC_TAG_AL[]        = "[al:%1]";
static const char LRC_TAG_OFFSET[]    = "[offset:%1]";
static const char LRC_TAG_AUTHOR[]    = "[by:%1]";

/*Other str of ASS.*/
static const char ASS_TAG_EVENT_FORMAT_DIALOGUE[] = "Dialogue";
static const char ASS_TAG_EVENT_FORMAT_COMMENT[]  = "Comment";
static const char ASS_TAG_FORMAT[]                = "Format";
static const char ASS_TAG_STYLE[]                 = "Style";
static const char ASS_TAG_TIMECODE_FORMAT[]       = "h:mm:ss.z";
static const char ASS_TAG_COMMA[]                 = ",";
static const char ASS_TAG_BRACE_REGEXP[]          = "(\\{)[^}]*(\\})";
static const char ASS_TAG_COMMENT_SCRIPT_REGEXP[] = "^(Comment.*)";
static const char ASS_TAG_FORMAT_REGEXP[]         = "^(Format.*)";
static const char ASS_TAG_STYLE_REGEXP[]          = "^(Style.*)";
static const char ASS_TAG_COMMENT_TEXT_REGEXP[]   = "^(;.*)";
static const char ASS_TAG_SECTION_REGEXP[]        = "^(\\[.*\\])";
static const char ASS_TAG_KEYWORD_QSTR[]          = "(%1)";

static const int ASS_FONT_SIZE         = 10;

static const char QT_EMPTY[]          = "";
static const char QT_BLANK[]          = " ";
static const char QT_EXT_SPLITE[]     = ".";
static const char QT_PATH_SPLITE[]    = ";";
static const char QT_META[]           = "\\b";
static const char QT_STR_ARG_1[]      = "%1";
static const char QT_STR_ARG_2[]      = "%1%2";
static const char QT_STR_ARG_3[]      = "%1%2%3";
static const char QT_STR_TAG[]        = "{%1%2}";
static const char QT_STR_TAG_OTR[]    = "{%1}";
static const char QT_STR_TAG_TEXT[]   = "{%1%2}%3";
static const char QT_STR_LRC_TEXT[]   = "[%1]%2";

enum class AssEvents {
    Layer = 0,
    Start,
    End,
    Style,
    Name,
    MarginL,
    MarginR,
    MarginV,
    Effect,
    Text,
    MaxEvent,
};

enum class CallK {
    Average,
    Fixed,
    Other,
    MaxCallK,
};

#endif // ASS_TAG_H

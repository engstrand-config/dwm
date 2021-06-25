#include <X11/XF86keysym.h>
/* general */
static const unsigned int snap      = 32;       /* snap pixel */
static const int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const int vertpad            = 0;        /* vertical padding of bar */
static const int sidepad            = 0;        /* horizontal padding of bar */
static const int statustextsidepad  = 10;       /* right side padding for statustext */
static const int swallowfloating    = 0;        /* 1 means swallow floating windows by default */

/* appearance */
static char font[]                  = "JetBrains Mono NL:style=Bold:size=9:antialias=true:autohint=true";

static unsigned int baralpha        = 225;      /* transparency of bar */
static unsigned int barheight       = 20;       /* height of the bar */
static unsigned int borderpx        = 1;        /* border pixel of windows */
static unsigned int gappih          = 10;       /* horiz inner gap between windows */
static unsigned int gappiv          = 10;       /* vert inner gap between windows */
static unsigned int gappoh          = 10;       /* horiz outer gap between windows and screen edge */
static unsigned int gappov          = 10;       /* vert outer gap between windows and screen edge */
static unsigned int gapsenabled     = 1;        /* enables gaps, used by togglegaps */
static char normbgcolor[]           = "#222222";
static char normbordercolor[]       = "#444444";
static char normfgcolor[]           = "#bbbbbb";
static char selfgcolor[]            = "#eeeeee";
static char selbordercolor[]        = "#005577";
static char selbgcolor[]            = "#005577";
static char titlebgcolor[]          = "#005577";
static char titlefgcolor[]          = "#eeeeee";
static char titlebordercolor[]      = "#005577";
static char successcolor[]          = "#005577";
static char criticalcolor[]          = "#005577";

static char *colors[][3] = {
  [SchemeNorm]        = { normfgcolor,    normbgcolor,    normbordercolor   },
  [SchemeSel]         = { selfgcolor,     selbgcolor,     selbordercolor    },
  [SchemeTitle]       = { titlefgcolor,   titlebgcolor,   titlebordercolor  },
  [SchemeSuccess]     = { successcolor,   normbgcolor,    normbordercolor   },
  [SchemeSuccessBg]   = { selfgcolor,     successcolor,   successcolor      },
  [SchemeCritical]    = { criticalcolor,  normbgcolor,    normbordercolor   },
  [SchemeCriticalBg]  = { selfgcolor,     criticalcolor,  criticalcolor     },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };
static const Rule rules[] = {
  /* xprop(1):
   *	WM_CLASS(STRING) = instance, class
   *	WM_NAME(STRING) = title
   */
	/* class        instance    title                 tags mask  isfloating  isterminal  noswallow  monitor */
	{ NULL,         "ferdi",    NULL,                 1 << 3,    0,          0,          0,          1      },
	{ "firefox",    NULL,       NULL,                 1 << 1,    0,          0,          1,         -1      },
	{ "Gimp",       NULL,       NULL,                 1 << 8,    0,          0,          0,         -1      },
  { "St",         NULL,       NULL,                 0,         0,          1,          0,         -1      },
	{ "spotify",    NULL,       NULL,                 1 << 4,    0,          0,          0,          1      },
	{ "tidal",      NULL,       NULL,                 1 << 4,    0,          0,          0,          1      },
  { NULL,         NULL,       "Event Tester",       0,         1,          0,          1,         -1      },
  { NULL,         NULL,       "Picture-in-Picture", 0,         1,          0,          1,         -1      },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
  /* symbol     arrange function */
  { "[]=",      tile           },
	{ "|M|",      centeredmaster },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
  { MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
  { MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
  { MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
  { MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },
#define STACKKEYS(MOD,ACTION) \
	{ MOD,	XK_j,	ACTION##stack,	{.i = INC(+1) } }, \
	{ MOD,	XK_k,	ACTION##stack,	{.i = INC(-1) } },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, NULL };
static const char *termcmd[]  = { "st", NULL };

static Key keys[] = {
  /* general */
  { MODKEY,                   XK_b,         togglebar,            {0}               },
  { MODKEY,                   XK_g,         togglegaps,           {0}               },
  { MODKEY,                   XK_Page_Up,   incnmaster,           {.i = +1 }        },
  { MODKEY,                   XK_Page_Down, incnmaster,           {.i = -1 }        },
  { MODKEY,                   XK_Home,      setnmaster,           {.i = 1  }   	    },
  { MODKEY,                   XK_s,         togglesticky,         {0}               },
  { MODKEY,                   XK_h,         setmfact,             {.f = -0.05}      },
  { MODKEY,                   XK_l,         setmfact,             {.f = +0.05}      },
  { MODKEY,                   XK_Tab,       view,                 {0}               },
  { MODKEY,                   XK_t,         setlayoutex,          {.i = +1}         },
  { MODKEY,                   XK_space,     zoom,                 {0}               },
  { MODKEY|ShiftMask,         XK_space,     togglefloating,       {0}               },
  { MODKEY,                   XK_f,         togglefullscreen,     {0}               },
  { MODKEY|ShiftMask,         XK_f,         togglefakefullscreen, {0}               },
  { MODKEY,                   XK_0,         view,                 {.ui = ~0 }       },
  { MODKEY|ShiftMask,         XK_0,         tag,                  {.ui = ~0 }       },
  { MODKEY,                   XK_Left,      focusmon,             {.i = -1 }        },
  { MODKEY,                   XK_Right,     focusmon,             {.i = +1 }        },
  { MODKEY,                   XK_Up,        focusmon,             {.i = +1 }        },
  { MODKEY,                   XK_Down,      focusmon,             {.i = -1 }        },
  { MODKEY|ShiftMask,         XK_Left,      tagmon,               {.i = -1 }        },
  { MODKEY|ShiftMask,         XK_Right,     tagmon,               {.i = +1 }        },
  { MODKEY|ShiftMask,         XK_Up,        tagmon,               {.i = +1 }        },
  { MODKEY|ShiftMask,         XK_Down,      tagmon,               {.i = -1 }        },
	{ MODKEY,			              XK_q,		      killclient,	          {0}               },

  /* applications */
	{ MODKEY,			              XK_d,		                  spawn,          {.v = dmenucmd } },
	{ MODKEY,			              XK_Return,                spawn,		      {.v = termcmd } },
	{ MODKEY|ShiftMask,		      XK_Return,                spawn,	        SHCMD("samedir") },
	{ MODKEY|ShiftMask,			    XK_w,		                  spawn,		      SHCMD("$BROWSER") },
  { MODKEY|ShiftMask,		      XK_m,		                  spawn,		      SHCMD("amixer -q set Master mute") },
	{ MODKEY,			              XK_section,	              spawn,	        SHCMD("dmenuunicode") },
	{ MODKEY,		                XK_m,		                  spawn,		      SHCMD("st -e ncmpcpp") },
	{ MODKEY,			              XK_x,		                  spawn,		      SHCMD("betterlockscreen -l dim") },
	{ MODKEY,			              XK_End,	                  spawn,	        SHCMD("pkill -USR1 \"^redshift$\"") },
  { MODKEY,                   XK_v,                     spawn,          SHCMD("st -e nvim") },
	{ MODKEY,			              XK_n,		                  spawn,	      	SHCMD("st -e newsboat-signal-on-exit") },
	{ MODKEY|ShiftMask,		      XK_a,		                  spawn,		      SHCMD("st -e pulsemixer; sigdsblocks 1") },
	{ MODKEY,			              XK_e,		                  spawn,		      SHCMD("st -e aerc") },
	{ MODKEY|ShiftMask,		      XK_c,		                  spawn,		      SHCMD("mpv --no-cache --no-osc --no-input-default-bindings --input-conf=/dev/null --title=mpvfloat $(ls /dev/video[0,2,4,6,8] | tail -n 1)") },

  /* function keys */
	{ MODKEY,			              XK_F2,		                spawn,		      SHCMD("set-system-colorscheme") },
	{ MODKEY,			              XK_F3,		                spawn,		      SHCMD("displayselect") },
	{ MODKEY,			              XK_F4,		                spawn,		      SHCMD("prompt 'Hibernate computer?' 'sudo systemctl suspend'") },
	{ MODKEY,			              XK_F8,		                spawn,		      SHCMD("mailsync") },
	{ MODKEY,			              XK_F9,		                spawn,		      SHCMD("dmenumount") },
	{ MODKEY,			              XK_F10,		                spawn,		      SHCMD("dmenuumount") },
	{ MODKEY,			              XK_F12,		                spawn,		      SHCMD("set-monitor-output toggle") },

  /* special keys (spawners) */
	{ MODKEY,	                  XK_Insert,	              spawn,		      SHCMD("set-kb-layout toggle") },
	{ ShiftMask,	              XK_Print,	                spawn,		      SHCMD("maimpick") },
	{ 0,                        XF86XK_WWW,		            spawn,	        SHCMD("$BROWSER") },
	{ 0,                        XF86XK_TaskPane,		      spawn,	        SHCMD("st -e htop") },
	{ 0,                        XF86XK_MyComputer,		    spawn,	        SHCMD("st -e lf /") },
	{ 0,                        XF86XK_Calculator,		    spawn,	        SHCMD("st -e bc -l") },
	{ MODKEY,			              XK_Print,	                spawn,		      SHCMD("dmenurecord") },
	{ 0,                        XF86XK_AudioMedia,		    spawn,	        SHCMD("st -e ncmpcpp") },
	{ MODKEY|ShiftMask,		      XK_Print,	                spawn,		      SHCMD("dmenurecord kill") },
	{ MODKEY,			              XK_Delete,                spawn,		      SHCMD("dmenurecord kill") },
	{ 0,                        XF86XK_Launch1,		        spawn,	        SHCMD("xset dpms force off") },
	{ 0,                        XF86XK_ScreenSaver,	      spawn,	        SHCMD("betterlockscreen -l dim") },
	{ MODKEY,			              XK_Scroll_Lock,	          spawn,		      SHCMD("killall screenkey || screenkey &") },
	{ 0,                        XF86XK_Mail,		          spawn,	        SHCMD("st -e neomutt ; sigdsblocks 5") },
	{ 0,                        XF86XK_Sleep,		          spawn,	        SHCMD("prompt 'Hibernate computer?' 'sudo -A zzz'") },
	{ 0,				                XK_Print,	                spawn,		      SHCMD("maim -s -u | xclip -selection clipboard -t image/png") },
	{ 0,                        XF86XK_PowerOff,		      spawn,	        SHCMD("prompt 'Shutdown computer?' 'sudo -A shutdown -h now'") },
	{ MODKEY|ShiftMask,		      XK_Escape,                spawn,	        SHCMD("prompt 'Close Xorg?' 'killall Xorg'") },
	{ MODKEY|ShiftMask,		      XK_BackSpace,             spawn,		      SHCMD("prompt 'Reboot computer?' 'sudo -A shutdown -r now'") },
	{ MODKEY|ShiftMask,		      XK_e,	                    spawn,		      SHCMD("prompt 'Shutdown computer?' 'sudo -A shutdown -h now'") },

  /* media */
	{ 0,                        XF86XK_AudioRewind,	      spawn,	        SHCMD("mpc seek -10") },
	{ 0,                        XF86XK_AudioForward,	    spawn,	        SHCMD("mpc seek +10") },
	{ 0,                        XF86XK_AudioStop,		      spawn,	        SHCMD("mpc stop; playerctl stop; sigdsblocks 9") },
	{ 0,                        XF86XK_AudioPrev,		      spawn,	        SHCMD("mpc prev; playerctl previous; sigdsblocks 9") },
	{ 0,                        XF86XK_AudioNext,		      spawn,	        SHCMD("mpc next; playerctl next; sigdsblocks 9") },
	{ 0,                        XF86XK_AudioPause,		    spawn,	        SHCMD("mpc toggle; playerctl play-pause; sigdsblocks 9") },
	{ 0,                        XF86XK_AudioPlay,		      spawn,	        SHCMD("mpc toggle; playerctl play-pause; sigdsblocks 9") },
	{ 0,                        XF86XK_AudioMute,		      spawn,		      SHCMD("pamixer -t; sigdsblocks 1") },
	{ 0,                        XF86XK_AudioRaiseVolume,	spawn,		      SHCMD("pamixer -u -i 3; sigdsblocks 1") },
	{ 0,                        XF86XK_AudioLowerVolume,	spawn,		      SHCMD("pamixer -u -d 3; sigdsblocks 1") },

  /* backlighting */
  	{ 0,                        XF86XK_MonBrightnessUp,	spawn,	        SHCMD("brightnessctl s +10%") },
	{ 0,                        XF86XK_MonBrightnessDown,	spawn,	        SHCMD("brightnessctl s 10%-") },

  /* tags */
  TAGKEYS(                    XK_1,                     0      )
  TAGKEYS(                    XK_2,                     1      )
  TAGKEYS(                    XK_3,                     2      )
  TAGKEYS(                    XK_4,                     3      )
  TAGKEYS(                    XK_5,                     4      )
  TAGKEYS(                    XK_6,                     5      )
  TAGKEYS(                    XK_7,                     6      )
  TAGKEYS(                    XK_8,                     7      )
  TAGKEYS(                    XK_9,                     8      )
	STACKKEYS(                  MODKEY,                   focus  )
	STACKKEYS(                  MODKEY|ShiftMask,         push   )
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
  /* click                event mask      button          function        argument */
  { ClkLtSymbol,          0,              Button1,        setlayout,      {0}                 },
  { ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]}  },
  { ClkWinTitle,          0,              Button2,        zoom,           {0}                 },
  { ClkStatusText,        0,              Button1,        sigdsblocks,    {.i = 1}            },
  { ClkStatusText,        0,              Button2,        sigdsblocks,    {.i = 2}            },
  { ClkStatusText,        0,              Button3,        sigdsblocks,    {.i = 3}            },
  { ClkStatusText,        0,              Button4,        sigdsblocks,    {.i = 4}            },
  { ClkStatusText,        0,              Button5,        sigdsblocks,    {.i = 5}            },
  { ClkClientWin,         MODKEY,         Button1,        movemouse,      {0}                 },
  { ClkClientWin,         MODKEY,         Button2,        togglefloating, {0}                 },
  { ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0}                 },
  { ClkTagBar,            0,              Button1,        view,           {0}                 },
  { ClkTagBar,            0,              Button3,        toggleview,     {0}                 },
  { ClkTagBar,            MODKEY,         Button1,        tag,            {0}                 },
  { ClkTagBar,            MODKEY,         Button3,        toggletag,      {0}                 },
};

/* Xresources preferences to load at startup */
ResourcePref resources[] = {
	{ "font",                    STRING,         &font                },
	{ "barHeight",               INTEGER,        &barheight           },
	{ "barAlpha",                INTEGER,        &baralpha            },
	{ "background",              STRING,         &normbgcolor         },
	{ "foreground",              STRING,         &normfgcolor         },
	{ "borderColor",             STRING,         &normbordercolor     },
	{ "selectedBackground",      STRING,         &selbgcolor          },
	{ "selectedForeground",      STRING,         &selfgcolor          },
	{ "selectedBorderColor",     STRING,         &selbordercolor      },
	{ "titleBackground",         STRING,         &titlebgcolor        },
	{ "titleForeground",         STRING,         &titlefgcolor        },
	{ "titleBorderColor",        STRING,         &titlebordercolor    },
	{ "success",                 STRING,         &successcolor        },
	{ "critical",                STRING,         &criticalcolor       },
	{ "borderSize",          	   INTEGER,        &borderpx            },
	{ "gapsInner",          	   INTEGER,        &gappih              },
	{ "gapsInner",          	   INTEGER,        &gappiv              },
	{ "gapsOuter",          	   INTEGER,        &gappoh              },
	{ "gapsOuter",          	   INTEGER,        &gappov              },
};

/* signal definitions */
/* signum must be greater than 0 */
/* trigger signals using `xsetroot -name "fsignal:<signame> [<type> <value>]"` */
static Signal signals[] = {
	{ "focusstack",     focusstack        },
	{ "setmfact",       setmfact          },
	{ "togglebar",      togglebar         },
	{ "incnmaster",     incnmaster        },
	{ "togglefloating", togglefloating    },
	{ "focusmon",       focusmon          },
	{ "tagmon",         tagmon            },
	{ "zoom",           zoom              },
	{ "view",           view              },
	{ "viewall",        viewall           },
	{ "viewex",         viewex            },
	{ "toggleview",     view              },
	{ "toggleviewex",   toggleviewex      },
	{ "incgaps",        incgaps           },
	{ "togglegaps",     togglegaps        },
	{ "defaultgaps",    defaultgaps       },
	{ "tag",            tag               },
	{ "tagall",         tagall            },
	{ "tagex",          tagex             },
	{ "toggletag",      tag               },
	{ "toggletagex",    toggletagex       },
	{ "killclient",     killclient        },
	{ "setlayout",      setlayout         },
	{ "setlayoutex",    setlayoutex       },
	{ "xresources",     reloadxresources  },
};

//============================================================
//
//  Copyright (c) 1996-2009, Nicola Salmoria and the MAME Team.
//  Visit http://mamedev.org for licensing and usage restrictions.
//
//  MAME4DROID MAME4iOS by David Valdeita (Seleuco)
//
//============================================================


#ifdef ANDROID
#include <android/log.h>
#endif

#include "osdepend.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//#include "render.h"
#include "clifront.h"
#include "emu.h"
#include "emuopts.h"
//#include "options.h"
#include "ui.h"
#include "uimenu.h"
//#include "driver.h"

#include "osdinput.h"
#include "osdsound.h"
#include "osdvideo.h"
#include "myosd.h"

//============================================================
//  GLOBALS
//============================================================

// a single rendering target
static render_target *our_target;
static int g_video_none;

static const options_entry droid_mame_options[] =
{
//	{ "initpath", ".;/mame", 0, "path to ini files" },
	{ NULL, NULL, OPTION_HEADER, "MYOSD OPTIONS" },
    { OPTION_HISCORE, "1", OPTION_BOOLEAN, "Enable HISCORE system" },
    { OPTION_BENCH,   "0", 0, "Benchmark for the given number of emulated seconds; implies -video none -sound none -nothrottle" },

	{ NULL }
};

//============================================================
//  FUNCTION PROTOTYPES
//============================================================

static void osd_exit(running_machine &machine);

//============================================================
//  main
//============================================================

#if defined(ANDROID)
extern "C"
int android_main  (int argc, char **argv)
#elif defined(IOS)
int ios_main(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
	myosd_init();

    int ret = cli_execute(argc, argv, droid_mame_options);
    
	myosd_deinit();
    
	return ret;
}

//============================================================
//  osd_init
//============================================================

void osd_init(running_machine *machine)
{
	machine->add_notifier(MACHINE_NOTIFY_EXIT, osd_exit);

	our_target = render_target_alloc(machine, NULL, 0);
	if (our_target == NULL)
		fatalerror("Error creating render target");

    // handle -bench
    int bench = options_get_int(machine->options(), OPTION_BENCH);
    if (bench > 0)
    {
        options_set_bool(machine->options(), OPTION_THROTTLE, FALSE, OPTION_PRIORITY_MAXIMUM);
        options_set_bool(machine->options(), OPTION_SOUND, FALSE, OPTION_PRIORITY_MAXIMUM);
        options_set_int(machine->options(), OPTION_SECONDS_TO_RUN, bench, OPTION_PRIORITY_MAXIMUM);
        g_video_none = 1;
    }
    else
    {
        g_video_none = 0;
    }
    
    myosd_machine_init(machine);
	droid_ios_init_input(machine);
	droid_ios_init_sound(machine);
	droid_ios_init_video(machine);
}

//void osd_exit(running_machine *machine)
static void osd_exit(running_machine &machine)
{
	if (our_target != NULL)
		render_target_free(our_target);
	our_target = NULL;
    myosd_machine_exit(&machine);
}

void osd_update(running_machine *machine, int skip_redraw)
{
    if (!skip_redraw && our_target!=NULL && g_video_none == 0)
	{
		droid_ios_video_render(our_target);
	}
    
    //attotime current_time = timer_get_time(machine);
    //char m[256];
    //sprintf(m,"fr: %d emutime sec:%d ms: %d\n",fr,current_time.seconds,(int)(current_time.attoseconds / ATTOSECONDS_PER_MILLISECOND));
    //mylog(m);
            
	droid_ios_poll_input(machine);
}

//============================================================
//  osd_wait_for_debugger
//============================================================

void osd_wait_for_debugger(running_device *device, int firststop)
{
	// we don't have a debugger, so we just return here
}



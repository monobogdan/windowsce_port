#ifndef __HELLOWORLD_H__
#define __HELLOWORLD_H__

#ifdef DEMO_DEBUG
#define DBG_PRINTF(a) mrc_printf a
#else
#define DBG_PRINTF(a) 
#endif

#endif

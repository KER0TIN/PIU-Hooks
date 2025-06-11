
#include <dlfcn.h>
#include <GL/glx.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <sched.h>
#include <time.h>
#include <err.h>

static uint64_t get_time_ns(void)
{
   struct timespec ts;
   clock_gettime(CLOCK_MONOTONIC, &ts);
   return (uint64_t)ts.tv_sec * (uint64_t)1e9 + (uint64_t)ts.tv_nsec;
}

extern "C" void glXSwapBuffers(Display *dpy, GLXDrawable drawable)
{
   static auto o_swap_buffers = reinterpret_cast<decltype(&glXSwapBuffers)>(dlsym(RTLD_NEXT, "glXSwapBuffers"));

   o_swap_buffers(dpy, drawable);

   if (60 > 0) {
      static uint64_t last_time;
      const double interval = 1e9 / 60;
      const useconds_t step = interval / 1e6;
      while (last_time > 0 && get_time_ns() - last_time < interval) {
         sched_yield();
         usleep(step);
      }
      last_time = get_time_ns();
   }
}


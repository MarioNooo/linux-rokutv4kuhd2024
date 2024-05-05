/* SPDX-License-Identifier: GPL-2.0 */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM mtk_sched

#if !defined(_TRACE_MTK_SCHED_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_MTK_SCHED_H

#include <linux/tracepoint.h>

/* below event is mstar customized */
/*
 * Tracepoint for showing cpu freq change
 */
TRACE_EVENT(cpufreq_change,

       TP_PROTO(int cpu, unsigned int old, unsigned int new),

       TP_ARGS(cpu, old, new),

       TP_STRUCT__entry(
               __field(int, cpu)
               __field(unsigned int, cpufreq_old)
               __field(unsigned int, cpufreq_new)
       ),

       TP_fast_assign(
               __entry->cpu   = cpu;
               __entry->cpufreq_old = old;
               __entry->cpufreq_new = new;
       ),

       TP_printk("dvfs cpu %d freq change from %u MHZ to %u MHZ",
                       __entry->cpu,
                       __entry->cpufreq_old,
                       __entry->cpufreq_new)
);
#endif

/* This part must be outside protection */
#include <trace/define_trace.h>

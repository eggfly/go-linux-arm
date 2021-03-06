// auto generated by go tool dist
// goos=linux goarch=arm


#include "runtime.h"
#include "arch_GOARCH.h"
#include "type.h"
void
runtime·GOMAXPROCS(intgo n, intgo ret)
{
#line 10 "/home/pi/go_build/hg/go/src/pkg/runtime/runtime1.goc"

	ret = runtime·gomaxprocsfunc(n);
	FLUSH(&ret);
}
void
runtime·NumCPU(intgo ret)
{
#line 14 "/home/pi/go_build/hg/go/src/pkg/runtime/runtime1.goc"

	ret = runtime·ncpu;
	FLUSH(&ret);
}
void
runtime·NumCgoCall(int64 ret)
{
#line 18 "/home/pi/go_build/hg/go/src/pkg/runtime/runtime1.goc"

	M *mp;

	ret = 0;
	for(mp=runtime·atomicloadp(&runtime·allm); mp; mp=mp->alllink)
		ret += mp->ncgocall;
	FLUSH(&ret);
}
void
runtime·newParFor(uint32 nthrmax, ParFor* desc)
{
#line 26 "/home/pi/go_build/hg/go/src/pkg/runtime/runtime1.goc"

	desc = runtime·parforalloc(nthrmax);
	FLUSH(&desc);
}
void
runtime·parForSetup(ParFor* desc, uint32 nthr, uint32 n, byte* ctx, bool wait, byte* body)
{
#line 30 "/home/pi/go_build/hg/go/src/pkg/runtime/runtime1.goc"

	runtime·parforsetup(desc, nthr, n, ctx, wait, *(void(**)(ParFor*, uint32))body);
}
void
runtime·parForDo(ParFor* desc)
{
#line 34 "/home/pi/go_build/hg/go/src/pkg/runtime/runtime1.goc"

	runtime·parfordo(desc);
}
void
runtime·parForIters(ParFor* desc, uintptr tid, uintptr start, uintptr end)
{
#line 38 "/home/pi/go_build/hg/go/src/pkg/runtime/runtime1.goc"

	runtime·parforiters(desc, tid, &start, &end);
	FLUSH(&start);
	FLUSH(&end);
}
void
runtime·gogoBytes(int32 x)
{
#line 42 "/home/pi/go_build/hg/go/src/pkg/runtime/runtime1.goc"

	x = RuntimeGogoBytes;
	FLUSH(&x);
}
void
runtime·typestring(Eface e, String s)
{
#line 46 "/home/pi/go_build/hg/go/src/pkg/runtime/runtime1.goc"

	s = *e.type->string;
	FLUSH(&s);
}
void
runtime·golockedOSThread(bool ret)
{
#line 50 "/home/pi/go_build/hg/go/src/pkg/runtime/runtime1.goc"

	ret = runtime·lockedOSThread();
	FLUSH(&ret);
}
void
runtime·NumGoroutine(intgo ret)
{
#line 54 "/home/pi/go_build/hg/go/src/pkg/runtime/runtime1.goc"

	ret = runtime·gcount();
	FLUSH(&ret);
}
void
runtime·getgoroot(String out)
{
#line 58 "/home/pi/go_build/hg/go/src/pkg/runtime/runtime1.goc"

	byte *p;

	p = runtime·getenv("GOROOT");
	out = runtime·gostringnocopy(p);
	FLUSH(&out);
}

#line 71 "/home/pi/go_build/hg/go/src/pkg/runtime/runtime1.goc"
void runtime·sigpanic ( void ) ; 
void
runtime·Caller(intgo skip, uintptr retpc, String retfile, intgo retline, bool retbool)
{
#line 73 "/home/pi/go_build/hg/go/src/pkg/runtime/runtime1.goc"

	Func *f, *g;
	uintptr pc;
	uintptr rpc[2];

	/*
	 * Ask for two PCs: the one we were asked for
	 * and what it called, so that we can see if it
	 * "called" sigpanic.
	 */
	retpc = 0;
	if(runtime·callers(1+skip-1, rpc, 2) < 2) {
		retfile = runtime·emptystring;
		retline = 0;
		retbool = false;
	} else if((f = runtime·findfunc(rpc[1])) == nil) {
		retfile = runtime·emptystring;
		retline = 0;
		retbool = true;  // have retpc at least
	} else {
		retpc = rpc[1];
		pc = retpc;
		g = runtime·findfunc(rpc[0]);
		if(pc > f->entry && (g == nil || g->entry != (uintptr)runtime·sigpanic))
			pc--;
		retline = runtime·funcline(f, pc, &retfile);
		retbool = true;
	}
	FLUSH(&retpc);
	FLUSH(&retfile);
	FLUSH(&retline);
	FLUSH(&retbool);
}
void
runtime·Callers(intgo skip, Slice pc, intgo retn)
{
#line 103 "/home/pi/go_build/hg/go/src/pkg/runtime/runtime1.goc"

	// runtime.callers uses pc.array==nil as a signal
	// to print a stack trace.  Pick off 0-length pc here
	// so that we don't let a nil pc slice get to it.
	if(pc.len == 0)
		retn = 0;
	else
		retn = runtime·callers(skip, (uintptr*)pc.array, pc.len);
	FLUSH(&retn);
}
void
runtime∕pprof·runtime_cyclesPerSecond(int64 res)
{
#line 113 "/home/pi/go_build/hg/go/src/pkg/runtime/runtime1.goc"

	res = runtime·tickspersecond();
	FLUSH(&res);
}
void
sync·runtime_procPin(intgo p)
{
#line 117 "/home/pi/go_build/hg/go/src/pkg/runtime/runtime1.goc"

	M *mp;

	mp = m;
	// Disable preemption.
	mp->locks++;
	p = mp->p->id;
	FLUSH(&p);
}
void
sync·runtime_procUnpin()
{
#line 126 "/home/pi/go_build/hg/go/src/pkg/runtime/runtime1.goc"

	m->locks--;
}

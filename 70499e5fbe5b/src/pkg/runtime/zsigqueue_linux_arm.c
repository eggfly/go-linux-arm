// auto generated by go tool dist
// goos=linux goarch=arm


#include "runtime.h"
#include "defs_GOOS_GOARCH.h"
#include "os_GOOS.h"
#include "cgocall.h"
#include "../../cmd/ld/textflag.h"

#line 34 "/home/pi/go_build/hg/go/src/pkg/runtime/sigqueue.goc"
static struct { 
Note; 
uint32 mask[ ( NSIG+31 ) /32]; 
uint32 wanted[ ( NSIG+31 ) /32]; 
uint32 state; 
bool inuse; 
} sig; 
#line 42 "/home/pi/go_build/hg/go/src/pkg/runtime/sigqueue.goc"
enum { 
HASWAITER = 1 , 
HASSIGNAL = 2 , 
} ; 
#line 48 "/home/pi/go_build/hg/go/src/pkg/runtime/sigqueue.goc"
bool 
runtime·sigsend ( int32 s ) 
{ 
uint32 bit , mask , old , new; 
#line 53 "/home/pi/go_build/hg/go/src/pkg/runtime/sigqueue.goc"
if ( !sig.inuse || s < 0 || s >= 32*nelem ( sig.wanted ) || ! ( sig.wanted[s/32]& ( 1U<< ( s&31 ) ) ) ) 
return false; 
bit = 1 << ( s&31 ) ; 
for ( ;; ) { 
mask = sig.mask[s/32]; 
if ( mask & bit ) 
break; 
if ( runtime·cas ( &sig.mask[s/32] , mask , mask|bit ) ) { 
#line 63 "/home/pi/go_build/hg/go/src/pkg/runtime/sigqueue.goc"
for ( ;; ) { 
old = runtime·atomicload ( &sig.state ) ; 
if ( old == HASSIGNAL ) 
break; 
if ( old == HASWAITER ) 
new = 0; 
else 
new = HASSIGNAL; 
if ( runtime·cas ( &sig.state , old , new ) ) { 
if ( old == HASWAITER ) 
runtime·notewakeup ( &sig ) ; 
break; 
} 
} 
break; 
} 
} 
return true; 
} 
void
runtime·signal_recv(uint32 m)
{
#line 85 "/home/pi/go_build/hg/go/src/pkg/runtime/sigqueue.goc"

	static uint32 recv[nelem(sig.mask)];
	uint32 i, old, new;
	
	for(;;) {
		// Serve from local copy if there are bits left.
		for(i=0; i<NSIG; i++) {
			if(recv[i/32]&(1U<<(i&31))) {
				recv[i/32] ^= 1U<<(i&31);
				m = i;
				goto done;
			}
		}

		// Check and update sig.state.
		for(;;) {
			old = runtime·atomicload(&sig.state);
			if(old == HASWAITER)
				runtime·throw("inconsistent state in signal_recv");
			if(old == HASSIGNAL)
				new = 0;
			else  // if(old == 0)
				new = HASWAITER;
			if(runtime·cas(&sig.state, old, new)) {
				if (new == HASWAITER) {
					runtime·notetsleepg(&sig, -1);
					runtime·noteclear(&sig);
				}
				break;
			}
		}

		// Get a new local copy.
		for(i=0; i<nelem(sig.mask); i++) {
			for(;;) {
				m = sig.mask[i];
				if(runtime·cas(&sig.mask[i], m, 0))
					break;
			}
			recv[i] = m;
		}
	}

done:;
	// goc requires that we fall off the end of functions
	// that return values instead of using our own return
	// statements.
	FLUSH(&m);
}
void
runtime·signal_enable(uint32 s)
{
#line 135 "/home/pi/go_build/hg/go/src/pkg/runtime/sigqueue.goc"

	if(!sig.inuse) {
		// The first call to signal_enable is for us
		// to use for initialization.  It does not pass
		// signal information in m.
		sig.inuse = true;	// enable reception of signals; cannot disable
		runtime·noteclear(&sig);
		return;
	}
	
	if(s >= nelem(sig.wanted)*32)
		return;
	sig.wanted[s/32] |= 1U<<(s&31);
	runtime·sigenable(s);
}
void
runtime·signal_disable(uint32 s)
{
#line 152 "/home/pi/go_build/hg/go/src/pkg/runtime/sigqueue.goc"

	if(s >= nelem(sig.wanted)*32)
		return;
	sig.wanted[s/32] &= ~(1U<<(s&31));
	runtime·sigdisable(s);
}

#pragma textflag NOSPLIT

#line 161 "/home/pi/go_build/hg/go/src/pkg/runtime/sigqueue.goc"
void 
runtime·badsignal ( uintptr sig ) 
{ 
runtime·cgocallback ( ( void ( * ) ( void ) ) runtime·sigsend , &sig , sizeof ( sig ) ) ; 
} 
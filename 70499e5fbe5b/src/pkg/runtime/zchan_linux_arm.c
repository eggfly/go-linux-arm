// auto generated by go tool dist
// goos=linux goarch=arm


#include "runtime.h"
#include "arch_GOARCH.h"
#include "type.h"
#include "race.h"
#include "malloc.h"
#include "chan.h"
#include "../../cmd/ld/textflag.h"

#line 14 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
uint32 runtime·Hchansize = sizeof ( Hchan ) ; 
#line 16 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
static void dequeueg ( WaitQ* ) ; 
static SudoG* dequeue ( WaitQ* ) ; 
static void enqueue ( WaitQ* , SudoG* ) ; 
static void destroychan ( Hchan* ) ; 
static void racesync ( Hchan* , SudoG* ) ; 
#line 22 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
static Hchan* 
makechan ( ChanType *t , int64 hint ) 
{ 
Hchan *c; 
Type *elem; 
#line 28 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
elem = t->elem; 
#line 31 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( elem->size >= ( 1<<16 ) ) 
runtime·throw ( "makechan: invalid channel element type" ) ; 
if ( ( sizeof ( *c ) %MAXALIGN ) != 0 || elem->align > MAXALIGN ) 
runtime·throw ( "makechan: bad alignment" ) ; 
#line 36 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( hint < 0 || ( intgo ) hint != hint || ( elem->size > 0 && hint > ( MaxMem - sizeof ( *c ) ) / elem->size ) ) 
runtime·panicstring ( "makechan: size out of range" ) ; 
#line 40 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
c = ( Hchan* ) runtime·mallocgc ( sizeof ( *c ) + hint*elem->size , ( uintptr ) t | TypeInfo_Chan , 0 ) ; 
c->elemsize = elem->size; 
c->elemtype = elem; 
c->dataqsiz = hint; 
#line 45 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( debug ) 
runtime·printf ( "makechan: chan=%p; elemsize=%D; elemalg=%p; dataqsiz=%D\n" , 
c , ( int64 ) elem->size , elem->alg , ( int64 ) c->dataqsiz ) ; 
#line 49 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
return c; 
} 
void
reflect·makechan(ChanType* t, uint64 size, Hchan* c)
{
#line 52 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"

	c = makechan(t, size);
	FLUSH(&c);
}
void
runtime·makechan(ChanType* t, int64 size, Hchan* c)
{
#line 56 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"

	c = makechan(t, size);
	FLUSH(&c);
}

#line 74 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
static bool 
chansend ( ChanType *t , Hchan *c , byte *ep , bool block , void *pc ) 
{ 
SudoG *sg; 
SudoG mysg; 
G* gp; 
int64 t0; 
#line 82 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( raceenabled ) 
runtime·racereadobjectpc ( ep , t->elem , runtime·getcallerpc ( &t ) , chansend ) ; 
#line 85 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( c == nil ) { 
USED ( t ) ; 
if ( !block ) 
return false; 
runtime·park ( nil , nil , "chan send (nil chan)" ) ; 
return false; 
} 
#line 93 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( debug ) { 
runtime·printf ( "chansend: chan=%p; elem=" , c ) ; 
c->elemtype->alg->print ( c->elemsize , ep ) ; 
runtime·prints ( "\n" ) ; 
} 
#line 99 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
t0 = 0; 
mysg.releasetime = 0; 
if ( runtime·blockprofilerate > 0 ) { 
t0 = runtime·cputicks ( ) ; 
mysg.releasetime = -1; 
} 
#line 106 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
runtime·lock ( c ) ; 
if ( raceenabled ) 
runtime·racereadpc ( c , pc , chansend ) ; 
if ( c->closed ) 
goto closed; 
#line 112 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( c->dataqsiz > 0 ) 
goto asynch; 
#line 115 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
sg = dequeue ( &c->recvq ) ; 
if ( sg != nil ) { 
if ( raceenabled ) 
racesync ( c , sg ) ; 
runtime·unlock ( c ) ; 
#line 121 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
gp = sg->g; 
gp->param = sg; 
if ( sg->elem != nil ) 
c->elemtype->alg->copy ( c->elemsize , sg->elem , ep ) ; 
if ( sg->releasetime ) 
sg->releasetime = runtime·cputicks ( ) ; 
runtime·ready ( gp ) ; 
return true; 
} 
#line 131 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( !block ) { 
runtime·unlock ( c ) ; 
return false; 
} 
#line 136 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
mysg.elem = ep; 
mysg.g = g; 
mysg.selectdone = nil; 
g->param = nil; 
enqueue ( &c->sendq , &mysg ) ; 
runtime·parkunlock ( c , "chan send" ) ; 
#line 143 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( g->param == nil ) { 
runtime·lock ( c ) ; 
if ( !c->closed ) 
runtime·throw ( "chansend: spurious wakeup" ) ; 
goto closed; 
} 
#line 150 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( mysg.releasetime > 0 ) 
runtime·blockevent ( mysg.releasetime - t0 , 2 ) ; 
#line 153 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
return true; 
#line 155 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
asynch: 
if ( c->closed ) 
goto closed; 
#line 159 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( c->qcount >= c->dataqsiz ) { 
if ( !block ) { 
runtime·unlock ( c ) ; 
return false; 
} 
mysg.g = g; 
mysg.elem = nil; 
mysg.selectdone = nil; 
enqueue ( &c->sendq , &mysg ) ; 
runtime·parkunlock ( c , "chan send" ) ; 
#line 170 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
runtime·lock ( c ) ; 
goto asynch; 
} 
#line 174 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( raceenabled ) 
runtime·racerelease ( chanbuf ( c , c->sendx ) ) ; 
#line 177 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
c->elemtype->alg->copy ( c->elemsize , chanbuf ( c , c->sendx ) , ep ) ; 
if ( ++c->sendx == c->dataqsiz ) 
c->sendx = 0; 
c->qcount++; 
#line 182 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
sg = dequeue ( &c->recvq ) ; 
if ( sg != nil ) { 
gp = sg->g; 
runtime·unlock ( c ) ; 
if ( sg->releasetime ) 
sg->releasetime = runtime·cputicks ( ) ; 
runtime·ready ( gp ) ; 
} else 
runtime·unlock ( c ) ; 
if ( mysg.releasetime > 0 ) 
runtime·blockevent ( mysg.releasetime - t0 , 2 ) ; 
return true; 
#line 195 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
closed: 
runtime·unlock ( c ) ; 
runtime·panicstring ( "send on closed channel" ) ; 
return false; 
} 
#line 202 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
static bool 
chanrecv ( ChanType *t , Hchan* c , byte *ep , bool block , bool *received ) 
{ 
SudoG *sg; 
SudoG mysg; 
G *gp; 
int64 t0; 
#line 212 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( debug ) 
runtime·printf ( "chanrecv: chan=%p\n" , c ) ; 
#line 215 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( c == nil ) { 
USED ( t ) ; 
if ( !block ) 
return false; 
runtime·park ( nil , nil , "chan receive (nil chan)" ) ; 
return false; 
} 
#line 223 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
t0 = 0; 
mysg.releasetime = 0; 
if ( runtime·blockprofilerate > 0 ) { 
t0 = runtime·cputicks ( ) ; 
mysg.releasetime = -1; 
} 
#line 230 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
runtime·lock ( c ) ; 
if ( c->dataqsiz > 0 ) 
goto asynch; 
#line 234 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( c->closed ) 
goto closed; 
#line 237 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
sg = dequeue ( &c->sendq ) ; 
if ( sg != nil ) { 
if ( raceenabled ) 
racesync ( c , sg ) ; 
runtime·unlock ( c ) ; 
#line 243 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( ep != nil ) 
c->elemtype->alg->copy ( c->elemsize , ep , sg->elem ) ; 
gp = sg->g; 
gp->param = sg; 
if ( sg->releasetime ) 
sg->releasetime = runtime·cputicks ( ) ; 
runtime·ready ( gp ) ; 
#line 251 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( received != nil ) 
*received = true; 
return true; 
} 
#line 256 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( !block ) { 
runtime·unlock ( c ) ; 
return false; 
} 
#line 261 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
mysg.elem = ep; 
mysg.g = g; 
mysg.selectdone = nil; 
g->param = nil; 
enqueue ( &c->recvq , &mysg ) ; 
runtime·parkunlock ( c , "chan receive" ) ; 
#line 268 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( g->param == nil ) { 
runtime·lock ( c ) ; 
if ( !c->closed ) 
runtime·throw ( "chanrecv: spurious wakeup" ) ; 
goto closed; 
} 
#line 275 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( received != nil ) 
*received = true; 
if ( mysg.releasetime > 0 ) 
runtime·blockevent ( mysg.releasetime - t0 , 2 ) ; 
return true; 
#line 281 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
asynch: 
if ( c->qcount <= 0 ) { 
if ( c->closed ) 
goto closed; 
#line 286 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( !block ) { 
runtime·unlock ( c ) ; 
if ( received != nil ) 
*received = false; 
return false; 
} 
mysg.g = g; 
mysg.elem = nil; 
mysg.selectdone = nil; 
enqueue ( &c->recvq , &mysg ) ; 
runtime·parkunlock ( c , "chan receive" ) ; 
#line 298 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
runtime·lock ( c ) ; 
goto asynch; 
} 
#line 302 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( raceenabled ) 
runtime·raceacquire ( chanbuf ( c , c->recvx ) ) ; 
#line 305 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( ep != nil ) 
c->elemtype->alg->copy ( c->elemsize , ep , chanbuf ( c , c->recvx ) ) ; 
c->elemtype->alg->copy ( c->elemsize , chanbuf ( c , c->recvx ) , nil ) ; 
if ( ++c->recvx == c->dataqsiz ) 
c->recvx = 0; 
c->qcount--; 
#line 312 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
sg = dequeue ( &c->sendq ) ; 
if ( sg != nil ) { 
gp = sg->g; 
runtime·unlock ( c ) ; 
if ( sg->releasetime ) 
sg->releasetime = runtime·cputicks ( ) ; 
runtime·ready ( gp ) ; 
} else 
runtime·unlock ( c ) ; 
#line 322 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( received != nil ) 
*received = true; 
if ( mysg.releasetime > 0 ) 
runtime·blockevent ( mysg.releasetime - t0 , 2 ) ; 
return true; 
#line 328 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
closed: 
if ( ep != nil ) 
c->elemtype->alg->copy ( c->elemsize , ep , nil ) ; 
if ( received != nil ) 
*received = false; 
if ( raceenabled ) 
runtime·raceacquire ( c ) ; 
runtime·unlock ( c ) ; 
if ( mysg.releasetime > 0 ) 
runtime·blockevent ( mysg.releasetime - t0 , 2 ) ; 
return true; 
} 
#pragma textflag NOSPLIT

void
runtime·chansend1(ChanType* t, Hchan* c, byte* elem)
{
#line 342 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"

	chansend(t, c, elem, true, runtime·getcallerpc(&t));
}

#pragma textflag NOSPLIT
void
runtime·chanrecv1(ChanType* t, Hchan* c, byte* elem)
{
#line 347 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"

	chanrecv(t, c, elem, true, nil);
}

#pragma textflag NOSPLIT
void
runtime·chanrecv2(ChanType* t, Hchan* c, byte* elem, bool received)
{
#line 353 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"

	chanrecv(t, c, elem, true, &received);
	FLUSH(&received);
}

#pragma textflag NOSPLIT
void
runtime·selectnbsend(ChanType* t, Hchan* c, byte* elem, bool selected)
{
#line 375 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"

	selected = chansend(t, c, elem, false, runtime·getcallerpc(&t));
	FLUSH(&selected);
}

#pragma textflag NOSPLIT
void
runtime·selectnbrecv(ChanType* t, byte* elem, Hchan* c, bool selected)
{
#line 397 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"

	selected = chanrecv(t, c, elem, false, nil);
	FLUSH(&selected);
}

#pragma textflag NOSPLIT
void
runtime·selectnbrecv2(ChanType* t, byte* elem, bool* received, Hchan* c, bool selected)
{
#line 419 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"

	selected = chanrecv(t, c, elem, false, received);
	FLUSH(&selected);
}

#pragma textflag NOSPLIT
void
reflect·chansend(ChanType* t, Hchan* c, byte* elem, bool nb, uint8, uint16, bool selected)
{
#line 424 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"

	selected = chansend(t, c, elem, !nb, runtime·getcallerpc(&t));
	FLUSH(&selected);
}
void
reflect·chanrecv(ChanType* t, Hchan* c, bool nb, byte* elem, bool selected, bool received)
{
#line 428 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"

	received = false;
	selected = chanrecv(t, c, elem, !nb, &received);
	FLUSH(&selected);
	FLUSH(&received);
}

#line 433 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
static Select* newselect ( int32 ) ; 
#pragma textflag NOSPLIT

void
runtime·newselect(int32 size, byte* sel)
{
#line 436 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"

	sel = (byte*)newselect(size);
	FLUSH(&sel);
}

#line 440 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
static Select* 
newselect ( int32 size ) 
{ 
int32 n; 
Select *sel; 
#line 446 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
n = 0; 
if ( size > 1 ) 
n = size-1; 
#line 454 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
sel = runtime·mal ( sizeof ( *sel ) + 
n*sizeof ( sel->scase[0] ) + 
size*sizeof ( sel->lockorder[0] ) + 
size*sizeof ( sel->pollorder[0] ) ) ; 
#line 459 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
sel->tcase = size; 
sel->ncase = 0; 
sel->lockorder = ( void* ) ( sel->scase + size ) ; 
sel->pollorder = ( void* ) ( sel->lockorder + size ) ; 
#line 464 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( debug ) 
runtime·printf ( "newselect s=%p size=%d\n" , sel , size ) ; 
return sel; 
} 
#line 470 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
static void selectsend ( Select *sel , Hchan *c , void *pc , void *elem , int32 so ) ; 
#pragma textflag NOSPLIT

void
runtime·selectsend(Select* sel, Hchan* c, byte* elem, bool selected)
{
#line 473 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"

	selected = false;

	// nil cases do not compete
	if(c != nil)
		selectsend(sel, c, runtime·getcallerpc(&sel), elem, (byte*)&selected - (byte*)&sel);
	FLUSH(&selected);
}

#line 481 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
static void 
selectsend ( Select *sel , Hchan *c , void *pc , void *elem , int32 so ) 
{ 
int32 i; 
Scase *cas; 
#line 487 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
i = sel->ncase; 
if ( i >= sel->tcase ) 
runtime·throw ( "selectsend: too many cases" ) ; 
sel->ncase = i+1; 
cas = &sel->scase[i]; 
#line 493 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
cas->pc = pc; 
cas->chan = c; 
cas->so = so; 
cas->kind = CaseSend; 
cas->sg.elem = elem; 
#line 499 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( debug ) 
runtime·printf ( "selectsend s=%p pc=%p chan=%p so=%d\n" , 
sel , cas->pc , cas->chan , cas->so ) ; 
} 
#line 505 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
static void selectrecv ( Select *sel , Hchan *c , void *pc , void *elem , bool* , int32 so ) ; 
#pragma textflag NOSPLIT

void
runtime·selectrecv(Select* sel, Hchan* c, byte* elem, bool selected)
{
#line 508 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"

	selected = false;

	// nil cases do not compete
	if(c != nil)
		selectrecv(sel, c, runtime·getcallerpc(&sel), elem, nil, (byte*)&selected - (byte*)&sel);
	FLUSH(&selected);
}

#pragma textflag NOSPLIT
void
runtime·selectrecv2(Select* sel, Hchan* c, byte* elem, bool* received, bool selected)
{
#line 517 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"

	selected = false;

	// nil cases do not compete
	if(c != nil)
		selectrecv(sel, c, runtime·getcallerpc(&sel), elem, received, (byte*)&selected - (byte*)&sel);
	FLUSH(&selected);
}

#line 525 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
static void 
selectrecv ( Select *sel , Hchan *c , void *pc , void *elem , bool *received , int32 so ) 
{ 
int32 i; 
Scase *cas; 
#line 531 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
i = sel->ncase; 
if ( i >= sel->tcase ) 
runtime·throw ( "selectrecv: too many cases" ) ; 
sel->ncase = i+1; 
cas = &sel->scase[i]; 
cas->pc = pc; 
cas->chan = c; 
#line 539 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
cas->so = so; 
cas->kind = CaseRecv; 
cas->sg.elem = elem; 
cas->receivedp = received; 
#line 544 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( debug ) 
runtime·printf ( "selectrecv s=%p pc=%p chan=%p so=%d\n" , 
sel , cas->pc , cas->chan , cas->so ) ; 
} 
#line 550 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
static void selectdefault ( Select* , void* , int32 ) ; 
#pragma textflag NOSPLIT

void
runtime·selectdefault(Select* sel, bool selected)
{
#line 553 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"

	selected = false;
	selectdefault(sel, runtime·getcallerpc(&sel), (byte*)&selected - (byte*)&sel);
	FLUSH(&selected);
}

#line 558 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
static void 
selectdefault ( Select *sel , void *callerpc , int32 so ) 
{ 
int32 i; 
Scase *cas; 
#line 564 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
i = sel->ncase; 
if ( i >= sel->tcase ) 
runtime·throw ( "selectdefault: too many cases" ) ; 
sel->ncase = i+1; 
cas = &sel->scase[i]; 
cas->pc = callerpc; 
cas->chan = nil; 
#line 572 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
cas->so = so; 
cas->kind = CaseDefault; 
#line 575 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( debug ) 
runtime·printf ( "selectdefault s=%p pc=%p so=%d\n" , 
sel , cas->pc , cas->so ) ; 
} 
#line 580 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
static void 
sellock ( Select *sel ) 
{ 
uint32 i; 
Hchan *c , *c0; 
#line 586 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
c = nil; 
for ( i=0; i<sel->ncase; i++ ) { 
c0 = sel->lockorder[i]; 
if ( c0 && c0 != c ) { 
c = sel->lockorder[i]; 
runtime·lock ( c ) ; 
} 
} 
} 
#line 596 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
static void 
selunlock ( Select *sel ) 
{ 
int32 i , n , r; 
Hchan *c; 
#line 610 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
n = ( int32 ) sel->ncase; 
r = 0; 
#line 613 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( n>0 && sel->lockorder[0] == nil ) 
r = 1; 
for ( i = n-1; i >= r; i-- ) { 
c = sel->lockorder[i]; 
if ( i>0 && sel->lockorder[i-1] == c ) 
continue; 
runtime·unlock ( c ) ; 
} 
} 
#line 623 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
static bool 
selparkcommit ( G *gp , void *sel ) 
{ 
USED ( gp ) ; 
selunlock ( sel ) ; 
return true; 
} 
void
runtime·block()
{
#line 631 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"

	runtime·park(nil, nil, "select (no cases)");	// forever
}

#line 635 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
static void* selectgo ( Select** ) ; 
#pragma textflag NOSPLIT

void
runtime·selectgo(Select* sel)
{
#line 642 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"

	runtime·setcallerpc(&sel, selectgo(&sel));
}

#line 646 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
static void* 
selectgo ( Select **selp ) 
{ 
Select *sel; 
uint32 o , i , j , k , done; 
int64 t0; 
Scase *cas , *dfl; 
Hchan *c; 
SudoG *sg; 
G *gp; 
byte *as; 
void *pc; 
#line 659 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
sel = *selp; 
#line 661 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( debug ) 
runtime·printf ( "select: sel=%p\n" , sel ) ; 
#line 664 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
t0 = 0; 
if ( runtime·blockprofilerate > 0 ) { 
t0 = runtime·cputicks ( ) ; 
for ( i=0; i<sel->ncase; i++ ) 
sel->scase[i].sg.releasetime = -1; 
} 
#line 680 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
for ( i=0; i<sel->ncase; i++ ) 
sel->pollorder[i] = i; 
for ( i=1; i<sel->ncase; i++ ) { 
o = sel->pollorder[i]; 
j = runtime·fastrand1 ( ) % ( i+1 ) ; 
sel->pollorder[i] = sel->pollorder[j]; 
sel->pollorder[j] = o; 
} 
#line 691 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
for ( i=0; i<sel->ncase; i++ ) { 
j = i; 
c = sel->scase[j].chan; 
while ( j > 0 && sel->lockorder[k= ( j-1 ) /2] < c ) { 
sel->lockorder[j] = sel->lockorder[k]; 
j = k; 
} 
sel->lockorder[j] = c; 
} 
for ( i=sel->ncase; i-->0; ) { 
c = sel->lockorder[i]; 
sel->lockorder[i] = sel->lockorder[0]; 
j = 0; 
for ( ;; ) { 
k = j*2+1; 
if ( k >= i ) 
break; 
if ( k+1 < i && sel->lockorder[k] < sel->lockorder[k+1] ) 
k++; 
if ( c < sel->lockorder[k] ) { 
sel->lockorder[j] = sel->lockorder[k]; 
j = k; 
continue; 
} 
break; 
} 
sel->lockorder[j] = c; 
} 
#line 726 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
sellock ( sel ) ; 
#line 728 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
loop: 
#line 730 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
dfl = nil; 
for ( i=0; i<sel->ncase; i++ ) { 
o = sel->pollorder[i]; 
cas = &sel->scase[o]; 
c = cas->chan; 
#line 736 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
switch ( cas->kind ) { 
case CaseRecv: 
if ( c->dataqsiz > 0 ) { 
if ( c->qcount > 0 ) 
goto asyncrecv; 
} else { 
sg = dequeue ( &c->sendq ) ; 
if ( sg != nil ) 
goto syncrecv; 
} 
if ( c->closed ) 
goto rclose; 
break; 
#line 750 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
case CaseSend: 
if ( raceenabled ) 
runtime·racereadpc ( c , cas->pc , chansend ) ; 
if ( c->closed ) 
goto sclose; 
if ( c->dataqsiz > 0 ) { 
if ( c->qcount < c->dataqsiz ) 
goto asyncsend; 
} else { 
sg = dequeue ( &c->recvq ) ; 
if ( sg != nil ) 
goto syncsend; 
} 
break; 
#line 765 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
case CaseDefault: 
dfl = cas; 
break; 
} 
} 
#line 771 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( dfl != nil ) { 
selunlock ( sel ) ; 
cas = dfl; 
goto retc; 
} 
#line 779 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
done = 0; 
for ( i=0; i<sel->ncase; i++ ) { 
o = sel->pollorder[i]; 
cas = &sel->scase[o]; 
c = cas->chan; 
sg = &cas->sg; 
sg->g = g; 
sg->selectdone = &done; 
#line 788 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
switch ( cas->kind ) { 
case CaseRecv: 
enqueue ( &c->recvq , sg ) ; 
break; 
#line 793 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
case CaseSend: 
enqueue ( &c->sendq , sg ) ; 
break; 
} 
} 
#line 799 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
g->param = nil; 
runtime·park ( selparkcommit , sel , "select" ) ; 
#line 802 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
sellock ( sel ) ; 
sg = g->param; 
#line 807 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
for ( i=0; i<sel->ncase; i++ ) { 
cas = &sel->scase[i]; 
if ( cas != ( Scase* ) sg ) { 
c = cas->chan; 
if ( cas->kind == CaseSend ) 
dequeueg ( &c->sendq ) ; 
else 
dequeueg ( &c->recvq ) ; 
} 
} 
#line 818 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( sg == nil ) 
goto loop; 
#line 821 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
cas = ( Scase* ) sg; 
c = cas->chan; 
#line 824 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( c->dataqsiz > 0 ) 
runtime·throw ( "selectgo: shouldn't happen" ) ; 
#line 827 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( debug ) 
runtime·printf ( "wait-return: sel=%p c=%p cas=%p kind=%d\n" , 
sel , c , cas , cas->kind ) ; 
#line 831 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( cas->kind == CaseRecv ) { 
if ( cas->receivedp != nil ) 
*cas->receivedp = true; 
} 
#line 836 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( raceenabled ) { 
if ( cas->kind == CaseRecv && cas->sg.elem != nil ) 
runtime·racewriteobjectpc ( cas->sg.elem , c->elemtype , cas->pc , chanrecv ) ; 
else if ( cas->kind == CaseSend ) 
runtime·racereadobjectpc ( cas->sg.elem , c->elemtype , cas->pc , chansend ) ; 
} 
#line 843 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
selunlock ( sel ) ; 
goto retc; 
#line 846 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
asyncrecv: 
#line 848 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( raceenabled ) { 
if ( cas->sg.elem != nil ) 
runtime·racewriteobjectpc ( cas->sg.elem , c->elemtype , cas->pc , chanrecv ) ; 
runtime·raceacquire ( chanbuf ( c , c->recvx ) ) ; 
} 
if ( cas->receivedp != nil ) 
*cas->receivedp = true; 
if ( cas->sg.elem != nil ) 
c->elemtype->alg->copy ( c->elemsize , cas->sg.elem , chanbuf ( c , c->recvx ) ) ; 
c->elemtype->alg->copy ( c->elemsize , chanbuf ( c , c->recvx ) , nil ) ; 
if ( ++c->recvx == c->dataqsiz ) 
c->recvx = 0; 
c->qcount--; 
sg = dequeue ( &c->sendq ) ; 
if ( sg != nil ) { 
gp = sg->g; 
selunlock ( sel ) ; 
if ( sg->releasetime ) 
sg->releasetime = runtime·cputicks ( ) ; 
runtime·ready ( gp ) ; 
} else { 
selunlock ( sel ) ; 
} 
goto retc; 
#line 873 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
asyncsend: 
#line 875 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( raceenabled ) { 
runtime·racerelease ( chanbuf ( c , c->sendx ) ) ; 
runtime·racereadobjectpc ( cas->sg.elem , c->elemtype , cas->pc , chansend ) ; 
} 
c->elemtype->alg->copy ( c->elemsize , chanbuf ( c , c->sendx ) , cas->sg.elem ) ; 
if ( ++c->sendx == c->dataqsiz ) 
c->sendx = 0; 
c->qcount++; 
sg = dequeue ( &c->recvq ) ; 
if ( sg != nil ) { 
gp = sg->g; 
selunlock ( sel ) ; 
if ( sg->releasetime ) 
sg->releasetime = runtime·cputicks ( ) ; 
runtime·ready ( gp ) ; 
} else { 
selunlock ( sel ) ; 
} 
goto retc; 
#line 895 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
syncrecv: 
#line 897 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( raceenabled ) { 
if ( cas->sg.elem != nil ) 
runtime·racewriteobjectpc ( cas->sg.elem , c->elemtype , cas->pc , chanrecv ) ; 
racesync ( c , sg ) ; 
} 
selunlock ( sel ) ; 
if ( debug ) 
runtime·printf ( "syncrecv: sel=%p c=%p o=%d\n" , sel , c , o ) ; 
if ( cas->receivedp != nil ) 
*cas->receivedp = true; 
if ( cas->sg.elem != nil ) 
c->elemtype->alg->copy ( c->elemsize , cas->sg.elem , sg->elem ) ; 
gp = sg->g; 
gp->param = sg; 
if ( sg->releasetime ) 
sg->releasetime = runtime·cputicks ( ) ; 
runtime·ready ( gp ) ; 
goto retc; 
#line 916 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
rclose: 
#line 918 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
selunlock ( sel ) ; 
if ( cas->receivedp != nil ) 
*cas->receivedp = false; 
if ( cas->sg.elem != nil ) 
c->elemtype->alg->copy ( c->elemsize , cas->sg.elem , nil ) ; 
if ( raceenabled ) 
runtime·raceacquire ( c ) ; 
goto retc; 
#line 927 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
syncsend: 
#line 929 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( raceenabled ) { 
runtime·racereadobjectpc ( cas->sg.elem , c->elemtype , cas->pc , chansend ) ; 
racesync ( c , sg ) ; 
} 
selunlock ( sel ) ; 
if ( debug ) 
runtime·printf ( "syncsend: sel=%p c=%p o=%d\n" , sel , c , o ) ; 
if ( sg->elem != nil ) 
c->elemtype->alg->copy ( c->elemsize , sg->elem , cas->sg.elem ) ; 
gp = sg->g; 
gp->param = sg; 
if ( sg->releasetime ) 
sg->releasetime = runtime·cputicks ( ) ; 
runtime·ready ( gp ) ; 
#line 944 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
retc: 
#line 950 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
pc = cas->pc; 
if ( cas->so > 0 ) { 
as = ( byte* ) selp + cas->so; 
*as = true; 
} 
if ( cas->sg.releasetime > 0 ) 
runtime·blockevent ( cas->sg.releasetime - t0 , 2 ) ; 
runtime·free ( sel ) ; 
return pc; 
#line 960 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
sclose: 
#line 962 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
selunlock ( sel ) ; 
runtime·panicstring ( "send on closed channel" ) ; 
return nil; 
} 
#line 968 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
typedef struct runtimeSelect runtimeSelect; 
struct runtimeSelect 
{ 
uintptr dir; 
ChanType *typ; 
Hchan *ch; 
byte *val; 
} ; 
#line 978 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
enum SelectDir { 
SelectSend = 1 , 
SelectRecv , 
SelectDefault , 
} ; 
void
reflect·rselect(Slice cases, intgo chosen, bool recvOK)
{
#line 984 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"

	int32 i;
	Select *sel;
	runtimeSelect* rcase, *rc;

	chosen = -1;
	recvOK = false;

	rcase = (runtimeSelect*)cases.array;

	sel = newselect(cases.len);
	for(i=0; i<cases.len; i++) {
		rc = &rcase[i];
		switch(rc->dir) {
		case SelectDefault:
			selectdefault(sel, (void*)i, 0);
			break;
		case SelectSend:
			if(rc->ch == nil)
				break;
			selectsend(sel, rc->ch, (void*)i, rc->val, 0);
			break;
		case SelectRecv:
			if(rc->ch == nil)
				break;
			selectrecv(sel, rc->ch, (void*)i, rc->val, &recvOK, 0);
			break;
		}
	}

	chosen = (intgo)(uintptr)selectgo(&sel);
	FLUSH(&chosen);
	FLUSH(&recvOK);
}

#line 1017 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
static void closechan ( Hchan *c , void *pc ) ; 
#pragma textflag NOSPLIT

void
runtime·closechan(Hchan* c)
{
#line 1020 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"

	closechan(c, runtime·getcallerpc(&c));
}

#pragma textflag NOSPLIT
void
reflect·chanclose(Hchan* c)
{
#line 1025 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"

	closechan(c, runtime·getcallerpc(&c));
}

#line 1029 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
static void 
closechan ( Hchan *c , void *pc ) 
{ 
SudoG *sg; 
G* gp; 
#line 1035 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( c == nil ) 
runtime·panicstring ( "close of nil channel" ) ; 
#line 1038 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
runtime·lock ( c ) ; 
if ( c->closed ) { 
runtime·unlock ( c ) ; 
runtime·panicstring ( "close of closed channel" ) ; 
} 
#line 1044 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( raceenabled ) { 
runtime·racewritepc ( c , pc , runtime·closechan ) ; 
runtime·racerelease ( c ) ; 
} 
#line 1049 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
c->closed = true; 
#line 1052 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
for ( ;; ) { 
sg = dequeue ( &c->recvq ) ; 
if ( sg == nil ) 
break; 
gp = sg->g; 
gp->param = nil; 
if ( sg->releasetime ) 
sg->releasetime = runtime·cputicks ( ) ; 
runtime·ready ( gp ) ; 
} 
#line 1064 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
for ( ;; ) { 
sg = dequeue ( &c->sendq ) ; 
if ( sg == nil ) 
break; 
gp = sg->g; 
gp->param = nil; 
if ( sg->releasetime ) 
sg->releasetime = runtime·cputicks ( ) ; 
runtime·ready ( gp ) ; 
} 
#line 1075 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
runtime·unlock ( c ) ; 
} 
void
reflect·chanlen(Hchan* c, intgo len)
{
#line 1078 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"

	if(c == nil)
		len = 0;
	else
		len = c->qcount;
	FLUSH(&len);
}
void
reflect·chancap(Hchan* c, intgo cap)
{
#line 1085 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"

	if(c == nil)
		cap = 0;
	else
		cap = c->dataqsiz;
	FLUSH(&cap);
}

#line 1092 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
static SudoG* 
dequeue ( WaitQ *q ) 
{ 
SudoG *sgp; 
#line 1097 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
loop: 
sgp = q->first; 
if ( sgp == nil ) 
return nil; 
q->first = sgp->link; 
#line 1104 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( sgp->selectdone != nil ) { 
#line 1106 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
if ( *sgp->selectdone != 0 || !runtime·cas ( sgp->selectdone , 0 , 1 ) ) 
goto loop; 
} 
#line 1110 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
return sgp; 
} 
#line 1113 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
static void 
dequeueg ( WaitQ *q ) 
{ 
SudoG **l , *sgp , *prevsgp; 
#line 1118 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
prevsgp = nil; 
for ( l=&q->first; ( sgp=*l ) != nil; l=&sgp->link , prevsgp=sgp ) { 
if ( sgp->g == g ) { 
*l = sgp->link; 
if ( q->last == sgp ) 
q->last = prevsgp; 
break; 
} 
} 
} 
#line 1129 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
static void 
enqueue ( WaitQ *q , SudoG *sgp ) 
{ 
sgp->link = nil; 
if ( q->first == nil ) { 
q->first = sgp; 
q->last = sgp; 
return; 
} 
q->last->link = sgp; 
q->last = sgp; 
} 
#line 1142 "/home/pi/go_build/hg/go/src/pkg/runtime/chan.goc"
static void 
racesync ( Hchan *c , SudoG *sg ) 
{ 
runtime·racerelease ( chanbuf ( c , 0 ) ) ; 
runtime·raceacquireg ( sg->g , chanbuf ( c , 0 ) ) ; 
runtime·racereleaseg ( sg->g , chanbuf ( c , 0 ) ) ; 
runtime·raceacquire ( chanbuf ( c , 0 ) ) ; 
} 
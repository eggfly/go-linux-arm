// auto generated by go tool dist
// goos=linux goarch=arm


#include "runtime.h"
#include "../../cmd/ld/textflag.h"
#pragma textflag NOSPLIT
void
runtime·cmpstring(String s1, String s2, intgo v)
{
#line 13 "/home/pi/go_build/hg/go/src/pkg/runtime/noasm_arm.goc"

	uintgo i, l;
	byte c1, c2;
	
	l = s1.len;
        if(s2.len < l)
		l = s2.len;
        for(i=0; i<l; i++) {
		c1 = s1.str[i];
		c2 = s2.str[i];
		if(c1 < c2) {
			v = -1;
			goto done;
		}
		if(c1 > c2) {
			v = +1;
			goto done;
		}
	}
        if(s1.len < s2.len) {
		v = -1;
		goto done;
	}
        if(s1.len > s2.len) {
		v = +1;
		goto done;
	}
        v = 0;
 done:;
	FLUSH(&v);
}

#pragma textflag NOSPLIT
void
bytes·Compare(Slice s1, Slice s2, intgo v)
{
#line 45 "/home/pi/go_build/hg/go/src/pkg/runtime/noasm_arm.goc"

	uintgo i, l;
	byte c1, c2;
	
	l = s1.len;
        if(s2.len < l)
		l = s2.len;
        for(i=0; i<l; i++) {
		c1 = s1.array[i];
		c2 = s2.array[i];
		if(c1 < c2) {
			v = -1;
			goto done;
		}
		if(c1 > c2) {
			v = +1;
			goto done;
		}
	}
        if(s1.len < s2.len) {
		v = -1;
		goto done;
	}
	if(s1.len > s2.len) {
		v = +1;
		goto done;
	}
	v = 0;
 done:;
	FLUSH(&v);
}

;
;   Copyright (C) 1998 by Jorrit Tyberghein
;   Written by Andrew Zabolotny
;   8-bit scanline drawing routines
;
;   This library is free software; you can redistribute it and/or
;   modify it under the terms of the GNU Library General Public
;   License as published by the Free Software Foundation; either
;   version 2 of the License, or (at your option) any later version.
;
;   This library is distributed in the hope that it will be useful,
;   but WITHOUT ANY WARRANTY; without even the implied warranty of
;   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;   Library General Public License for more details.
;
;   You should have received a copy of the GNU Library General Public
;   License along with this library; if not, write to the Free
;   Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
;

;   PLEASE read first the comment at the start of scan.ash

%include "cs.ash"
%include "scan.ash"

;-----======xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx======-----
; Summary:
;   The internal scanloop for scan_map_zfil
;   Draw one horizontal scanline (with lighting)
; Arguments:
;   none
; Example:
;   scanproc 8,scan_map_zfil,SCANPROC_MAP,scanloop_map
;-----======xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx======-----
%define		scanloop_map_args 16
%macro		scanloop_map_init 0
		loc	%$dudvFP,8	; fixed-point value of (dv * tex_w + du)
		tloc	%$duFP		; fixed-point duu
		tloc	%$destend8	; dest+(destend-dest) & ~7
%endmacro
%macro		scanloop_map_body 0
	%ifdef PIC
		push	ebx
	%endif
; dudvInt[1] = ((dvv >> 16) << shifter) + (duu >> 16);
		mov	eax,%$dvv		; eax = dvv		; 0
		mov	edx,%$duu		; edx = duu		; 0
		sar	eax,16			; eax = dvv >> 16	; 1
		mov	esi,bitmap2		; esi = texture ptr	; 1
		sar	edx,16			; edx = duu >> 16	; 2
		mov	ecx,shf_u		; ecx = shifter		; 2
		shl	eax,cl			; eax = (dvv >> 16) << s; 3/4
; dudvInt[0] = dudvInt[1] + (1 << shifter);
		mov	ebx,%$vv		; ebx = vv		; 3
		add	eax,edx			; eax = dudvInt[1]	; 7
		mov	edx,1			; edx = 1		; 7
		shl	edx,cl			; edx = 1 << shifter	; 8/4
		mov	[%$dudvFP_+4],eax	; dudvInt[1] = eax	; 8
		sar	ebx,16			; ebx = vv >> 16	; 12
		add	eax,edx			; eax = dudvInt[0]	; 12
		mov	[%$dudvFP_+0],eax	; dudvInt[0] = eax	; 13
; unsigned char *s = srcTex + ((vv >> 16) << shifter) + (uu >> 16);
		mov	edx,%$uu		; edx = uu >> 16	; 13
		sar	edx,16						; 14
		mov	eax,%$uu					; 14
		shl	ebx,cl						; 15/4
		test	[edi],al		; fetch dest into cache	; 15
		add	esi,edx			; esi = source texture	; 19
		mov	edx,%$destend					; 19
		add	esi,ebx						; 20

; uFrac = uu << 16;
; vFrac = vv << 16;
		mov	ebx,%$vv					; 20
		shl	eax,16			; eax = uu << 16	; 21
		mov	ebp,%$dvv					; 21
		shl	ebx,16			; ebx = vv << 16	; 22
; duFrac = duu << 16;
; dvFrac = dvv << 16;
		mov	ecx,%$duu					; 22
		shl	ebp,16			; ebp = dvv << 16	; 23
		sub	edx,edi						; 23
		shl	ecx,16			; ecx = duu << 16	; 24
		and	edx,~7			; (destend-dest) & ~7	; 24
		mov	%$duFP,ecx					; 25
		jz	near %$sloop1					; 25

		add	edx,edi						; 26
		mov	%$destend8,edx					; 26

; --------------// Register usage //--------------
; EAX	frac(uu)	mem	frac(duu)
; EBX	frac(vv)	EBP	frac(dvv)
; ECX	--//texel//--	EDX	--//scratch//--
; ESI	texture		EDI	dest

; draw texture in segments by 8 pixels
; A.Z. proudly presents: the FASTEST texture-mapping cycle ever written :-)
; A one-pixel cycle occupies 5 (!) clocks on P5/MMX vs 7 clocks in Quake
; There are only two clock lags in V-pipe, all other time CPU is 100% loaded.
; For some strange reason (?) if we put an "inc edi" there (instead of
; "add edi,8" at the end of loop) the loop executes slower
%$sloop8:	add	ebx,ebp			; vv += dvv		; 0
		mov	cl,[esi]		; Get texel		; 0
		sbb	edx,edx			; carry (vv + dvv)	; 1
		mov	[edi],cl		; Put pixel		; 1
		mov	ecx,%$duFP		; ecx = duu		; 2 (v stall)
		add	eax,ecx			; uu += duu		; 3
		mov	ecx,[%$dudvFP_+4+edx*4]	; ecx = delta texture	; 3
		adc	esi,ecx			; update texture ptr	; 4

		add	ebx,ebp			; vv += dvv		; 4
		mov	cl,[esi]		; Get texel		; 5 (v stall)
		sbb	edx,edx			; carry (vv + dvv)	; 6
		mov	[edi+1],cl		; Put pixel		; 6
		mov	ecx,%$duFP		; ecx = duu		; 7 (v stall)
		add	eax,ecx			; uu += duu		; 8
		mov	ecx,[%$dudvFP_+4+edx*4]	; ecx = delta texture	; 8
		adc	esi,ecx			; update texture ptr	; 9

		add	ebx,ebp			; vv += dvv		; 9
		mov	cl,[esi]		; Get texel		; 10 (v stall)
		sbb	edx,edx			; carry (vv + dvv)	; 11
		mov	[edi+2],cl		; Put pixel		; 11
		mov	ecx,%$duFP		; ecx = duu		; 12 (v stall)
		add	eax,ecx			; uu += duu		; 13
		mov	ecx,[%$dudvFP_+4+edx*4]	; ecx = delta texture	; 13
		adc	esi,ecx			; update texture ptr	; 14

		add	ebx,ebp			; vv += dvv		; 14
		mov	cl,[esi]		; Get texel		; 15 (v stall)
		sbb	edx,edx			; carry (vv + dvv)	; 16
		mov	[edi+3],cl		; Put pixel		; 16
		mov	ecx,%$duFP		; ecx = duu		; 17 (v stall)
		add	eax,ecx			; uu += duu		; 18
		mov	ecx,[%$dudvFP_+4+edx*4]	; ecx = delta texture	; 18
		adc	esi,ecx			; update texture ptr	; 19

		add	ebx,ebp			; vv += dvv		; 19
		mov	cl,[esi]		; Get texel		; 20 (v stall)
		sbb	edx,edx			; carry (vv + dvv)	; 21
		mov	[edi+4],cl		; Put pixel		; 21
		mov	ecx,%$duFP		; ecx = duu		; 22 (v stall)
		add	eax,ecx			; uu += duu		; 23
		mov	ecx,[%$dudvFP_+4+edx*4]	; ecx = delta texture	; 23
		adc	esi,ecx			; update texture ptr	; 24

		add	ebx,ebp			; vv += dvv		; 24
		mov	cl,[esi]		; Get texel		; 25 (v stall)
		sbb	edx,edx			; carry (vv + dvv)	; 26
		mov	[edi+5],cl		; Put pixel		; 26
		mov	ecx,%$duFP		; ecx = duu		; 27 (v stall)
		add	eax,ecx			; uu += duu		; 28
		mov	ecx,[%$dudvFP_+4+edx*4]	; ecx = delta texture	; 28
		adc	esi,ecx			; update texture ptr	; 29

		add	ebx,ebp			; vv += dvv		; 29
		mov	cl,[esi]		; Get texel		; 30 (v stall)
		sbb	edx,edx			; carry (vv + dvv)	; 31
		mov	[edi+6],cl		; Put pixel		; 31
		mov	ecx,%$duFP		; ecx = duu		; 32 (v stall)
		add	eax,ecx			; uu += duu		; 33
		mov	ecx,[%$dudvFP_+4+edx*4]	; ecx = delta texture	; 33
		adc	esi,ecx			; update texture ptr	; 34

		add	ebx,ebp			; vv += dvv		; 34
		mov	cl,[esi]		; Get texel		; 35 (v stall)
		sbb	edx,edx			; carry (vv + dvv)	; 36
		mov	[edi+7],cl		; Put pixel		; 36
		mov	ecx,%$duFP		; ecx = duu		; 37 (v stall)
		add	eax,ecx			; uu += duu		; 38
		mov	ecx,[%$dudvFP_+4+edx*4]	; ecx = delta texture	; 38
		adc	esi,ecx			; update texture ptr	; 39

		add	edi,8
		cmp	edi,%$destend8					; 39
		jb	near %$sloop8					; 40
		cmp	edi,%$destend
		jae	%$sexit

%$sloop1:	add	ebx,ebp			; vv += dvv		; 0
		mov	cl,[esi]		; Get texel		; 0
		sbb	edx,edx			; carry (vv + dvv)	; 1
		mov	[edi],cl		; Put pixel		; 1
		mov	ecx,%$duFP		; ecx = duu		; 2
		add	eax,ecx			; uu += duu		; 3
		mov	ecx,[%$dudvFP_+4+edx*4]	; ecx = delta texture	; 3
		adc	esi,ecx			; update texture ptr	; 4

		inc	edi			; increment dest	; 4
		cmp	edi,%$destend
		jb	%$sloop1
%$sexit:
	%ifdef PIC
		pop	ebx
	%endif
%endmacro
%define		scanloop_map_fini zfill

;-----======xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx======-----
; Summary:
;   The internal scanloop for mmx_scan_map_zfil
;   Draw one horizontal scanline (with lighting) using MMX
; Arguments:
;   none
; Example:
;   scanproc 8,mmx_scan_map_zfil,SCANPROC_MAP|SCANPROC_MMX,mmx_scanloop_map
;-----======xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx======-----
%define		mmx_scanloop_map_args scanloop_map_args
%define		mmx_scanloop_map_init scanloop_map_init
%define		mmx_scanloop_map_body scanloop_map_body
%define		mmx_scanloop_map_fini mmx_zfill

;-----======xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx======-----
; Summary:
;   The internal scanloop for scan_map_zuse
;   Draw one horizontal scanline (Z buffer and lighting).
; Arguments:
;   none
; Example:
;   scanproc 8,scan_map_zuse,SCANPROC_MAP,scanloop_map_z
;-----======xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx======-----
%define		scanloop_map_z_args 16
%macro		scanloop_map_z_init 0
		loc	%$dudvFP,8	; fixed-point value of (dv * tex_w + du)
		tloc	%$duFP		; fixed-point duu
		tloc	%$dvFP		; fixed-point dvv
%endmacro
%macro		scanloop_map_z_body 0
	%ifdef PIC
		push	ebx
	%endif
; dudvInt[1] = ((dvv >> 16) << shifter) + (duu >> 16);
		mov	eax,%$dvv		; eax = dvv >> 16	; 0
		mov	edx,%$duu		; edx = duu >> 16	; 0
		sar	eax,16						; 1
		mov	esi,bitmap2					; 1
		sar	edx,16						; 2
		mov	ecx,shf_u					; 2
		shl	eax,cl						; 3/4
		add	eax,edx						; 7
; dudvInt[0] = dudvInt[1] + (1 << shifter);
		mov	edx,1						; 7
		mov	[%$dudvFP_+4],eax				; 8
		mov	ebx,%$vv		; ebx = vv >> 16	; 8
		shl	edx,cl						; 9/4
		sar	ebx,16						; 13
		add	eax,edx						; 13
		mov	[%$dudvFP_+0],eax				; 14
; unsigned char *s = srcTex + ((vv >> 16) << shifter) + (uu >> 16);
		mov	edx,%$uu		; edx = uu >> 16	; 14
		sar	edx,16						; 15
		mov	eax,%$uu					; 15
		shl	ebx,cl						; 16/4
		add	esi,edx			; esi = source texture	; 20
		add	esi,ebx						; 21

; uFrac = uu << 16;
; vFrac = vv << 16;
		mov	ebx,%$vv					; 21
		shl	eax,16			; eax = uu << 16	; 22
		mov	ebp,%$dvv					; 22
		shl	ebx,16			; ebx = vv << 16	; 23
; duFrac = duu << 16;
; dvFrac = dvv << 16;
		mov	edx,%$duu					; 23
		shl	ebp,16			; ebp = dvv << 16	; 24
		mov	cl,[edi]		; fetch into cache	; 24
		shl	edx,16			; ecx = duu << 16	; 25
		mov	%$dvFP,ebp					; 25
		mov	%$duFP,edx					; 26

; --------------// Register usage //--------------
; EAX	frac(uu)	mem	frac(duu)
; EBX	frac(vv)	mem	frac(dvv)
; ECX	1/z		EDX	--//scratch//--
; ESI	texture		EDI	dest
; EBP	Z-buffer

		mov	ebp,%$zbuff
		mov	ecx,%$izz

%$sloop1:	mov	edx,[ebp]		; Get Z buffer value	; 0 (v stall)
		cmp	ecx,edx			; Check	Z-buffer	; 1
		jb	%$invtex		; We're	below surface	; 1
		mov	dl,[esi]		; Get texel		; 2
		mov	[ebp],ecx		; *zbuff = z		; 2
		mov	[edi],dl		; Put texel		; 3

%$invtex:	add	ebx,%$dvFP		; v = v	+ dv		; 3
		sbb	edx,edx						; 4
		add	eax,%$duFP		; u = u	+ du		; 4/2
		adc	esi,[%$dudvFP_+4+edx*4]	; Update texture ptr	; 6/2
		add	ecx,%$dzz		; z = z	+ dz		; 6/2
		inc	edi			; dest++		; 8
		add	ebp,4			; zbuff++		; 8
		cmp	edi,%$destend		; dest < destend?	; 9
		jb	%$sloop1					; 9

		mov	%$zbuff,ebp
		mov	%$izz,ecx

	%ifdef PIC
		pop	ebx
	%endif
%endmacro
%define		scanloop_map_z_fini

;-----======xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx======-----
; Summary:
;   The internal scanloop for scan_map_fixalpha1
;   Draw one horizontal scanline (lighting and alpha transparency).
; Arguments:
;   none
; Example:
;   scanproc 8,scan_map_fixalpha1,SCANPROC_MAP,scanloop_map_a1
;-----======xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx======-----
%define		scanloop_map_a1_args 20
%macro		scanloop_map_a1_init 0
		loc	%$dudvFP,8	; fixed-point value of (dv * tex_w + du)
		tloc	%$dvFP		; fixed-point dvv
		tloc	%$duFP		; fixed-point duu
		tloc	%$destend8
%endmacro
%macro		scanloop_map_a1_body 0
	%ifdef PIC
		push	ebx
	%endif
; dudvInt[1] = ((dvv >> 16) << shifter) + (duu >> 16);
		mov	eax,%$dvv		; eax = dvv		; 0
		mov	edx,%$duu		; edx = duu		; 0
		sar	eax,16			; eax = dvv >> 16	; 1
		mov	esi,bitmap2		; esi = texture ptr	; 1
		sar	edx,16			; edx = duu >> 16	; 2
		mov	ecx,shf_u		; ecx = shifter		; 2
		shl	eax,cl			; eax = (dvv >> 16) << s; 3/4
; dudvInt[0] = dudvInt[1] + (1 << shifter);
		mov	ebx,%$vv		; ebx = vv		; 3
		add	eax,edx			; eax = dudvInt[1]	; 7
		mov	edx,1			; edx = 1		; 7
		shl	edx,cl			; edx = 1 << shifter	; 8/4
		mov	[%$dudvFP_+4],eax	; dudvInt[1] = eax	; 8
		sar	ebx,16			; ebx = vv >> 16	; 12
		add	eax,edx			; eax = dudvInt[0]	; 12
		mov	[%$dudvFP_+0],eax	; dudvInt[0] = eax	; 13
; unsigned char *s = srcTex + ((vv >> 16) << shifter) + (uu >> 16);
		mov	edx,%$uu		; edx = uu >> 16	; 13
		sar	edx,16						; 14
		mov	eax,%$uu					; 14
		shl	ebx,cl						; 15/4
		test	[edi],al		; fetch dest into cache	; 15
		add	esi,edx			; esi = source texture	; 19
		mov	edx,%$destend					; 19
		add	esi,ebx						; 20

; uFrac = uu << 16;
; vFrac = vv << 16;
		mov	ebx,%$vv					; 20
		shl	eax,16			; eax = uu << 16	; 21
		mov	ebp,%$dvv					; 21
		shl	ebx,16			; ebx = vv << 16	; 22
; duFrac = duu << 16;
; dvFrac = dvv << 16;
		mov	ecx,%$duu					; 22
		shl	ebp,16			; ebp = dvv << 16	; 23
		sub	edx,edi						; 23
		shl	ecx,16			; ecx = duu << 16	; 24
		mov	%$dvFP,ebp					; 24
		mov	%$duFP,ecx					; 25
		xor	ecx,ecx						; 25
		mov	ebp,AlphaMap					; 26
		and	edx,~7			; (destend-dest) & ~7	; 26
		jz	near %$sloop1					; 27

		add	edx,edi						; 28
		mov	%$destend8,edx					; 28

; --------------// Register usage //--------------
; EAX	frac(uu)	mem	frac(duu)
; EBX	frac(vv)	mem	frac(dvv)
; ECX	--//texel//--	EDX	--//scratch//--
; ESI	texture		EDI	dest
; EBP	Alpha mapping buffer

; The pixel loop has two stalls: one tick in U-pipe and one tick in V-pipe

%$sloop8:	mov	cl,[esi]		; Get texel		; 0
		mov	edx,%$dvFP		; get dvv		; 0
		mov	ch,[edi]		; Get screen pixel	; 1
		add	ebx,edx			; vv += dvv		; 1
		sbb	edx,edx			; carry flag (vv+dvv)	; 2 (1 tick u stall)
		add	eax,%$duFP		; uu += duu		; 2/2
		mov	cl,[ebp+ecx]		; Get resulting pixel	; 4 (v stall)
		mov	[edi],cl		; Store pixel		; 5
		mov	edx,[%$dudvFP_+4+edx*4]	; get delta texture	; 5
		adc	esi,edx			; update texture ptr	; 6

		mov	cl,[esi]		; Get texel		; 0
		mov	edx,%$dvFP		; get dvv		; 0
		mov	ch,[edi+1]		; Get screen pixel	; 1
		add	ebx,edx			; vv += dvv		; 1
		sbb	edx,edx			; carry flag (vv+dvv)	; 2
		add	eax,%$duFP		; uu += duu		; 2/2
		mov	cl,[ebp+ecx]		; Get resulting pixel	; 4
		mov	[edi+1],cl		; Store pixel		; 5
		mov	edx,[%$dudvFP_+4+edx*4]	; get delta texture	; 5
		adc	esi,edx			; update texture ptr	; 6

		mov	cl,[esi]		; Get texel		; 0
		mov	edx,%$dvFP		; get dvv		; 0
		mov	ch,[edi+2]		; Get screen pixel	; 1
		add	ebx,edx			; vv += dvv		; 1
		sbb	edx,edx			; carry flag (vv+dvv)	; 2
		add	eax,%$duFP		; uu += duu		; 2/2
		mov	cl,[ebp+ecx]		; Get resulting pixel	; 4
		mov	[edi+2],cl		; Store pixel		; 5
		mov	edx,[%$dudvFP_+4+edx*4]	; get delta texture	; 5
		adc	esi,edx			; update texture ptr	; 6

		mov	cl,[esi]		; Get texel		; 0
		mov	edx,%$dvFP		; get dvv		; 0
		mov	ch,[edi+3]		; Get screen pixel	; 1
		add	ebx,edx			; vv += dvv		; 1
		sbb	edx,edx			; carry flag (vv+dvv)	; 2
		add	eax,%$duFP		; uu += duu		; 2/2
		mov	cl,[ebp+ecx]		; Get resulting pixel	; 4
		mov	[edi+3],cl		; Store pixel		; 5
		mov	edx,[%$dudvFP_+4+edx*4]	; get delta texture	; 5
		adc	esi,edx			; update texture ptr	; 6

		mov	cl,[esi]		; Get texel		; 0
		mov	edx,%$dvFP		; get dvv		; 0
		mov	ch,[edi+4]		; Get screen pixel	; 1
		add	ebx,edx			; vv += dvv		; 1
		sbb	edx,edx			; carry flag (vv+dvv)	; 2
		add	eax,%$duFP		; uu += duu		; 2/2
		mov	cl,[ebp+ecx]		; Get resulting pixel	; 4
		mov	[edi+4],cl		; Store pixel		; 5
		mov	edx,[%$dudvFP_+4+edx*4]	; get delta texture	; 5
		adc	esi,edx			; update texture ptr	; 6

		mov	cl,[esi]		; Get texel		; 0
		mov	edx,%$dvFP		; get dvv		; 0
		mov	ch,[edi+5]		; Get screen pixel	; 1
		add	ebx,edx			; vv += dvv		; 1
		sbb	edx,edx			; carry flag (vv+dvv)	; 2
		add	eax,%$duFP		; uu += duu		; 2/2
		mov	cl,[ebp+ecx]		; Get resulting pixel	; 4
		mov	[edi+5],cl		; Store pixel		; 5
		mov	edx,[%$dudvFP_+4+edx*4]	; get delta texture	; 5
		adc	esi,edx			; update texture ptr	; 6

		mov	cl,[esi]		; Get texel		; 0
		mov	edx,%$dvFP		; get dvv		; 0
		mov	ch,[edi+6]		; Get screen pixel	; 1
		add	ebx,edx			; vv += dvv		; 1
		sbb	edx,edx			; carry flag (vv+dvv)	; 2
		add	eax,%$duFP		; uu += duu		; 2/2
		mov	cl,[ebp+ecx]		; Get resulting pixel	; 4
		mov	[edi+6],cl		; Store pixel		; 5
		mov	edx,[%$dudvFP_+4+edx*4]	; get delta texture	; 5
		adc	esi,edx			; update texture ptr	; 6

		mov	cl,[esi]		; Get texel		; 0
		mov	edx,%$dvFP		; get dvv		; 0
		mov	ch,[edi+7]		; Get screen pixel	; 1
		add	ebx,edx			; vv += dvv		; 1
		sbb	edx,edx			; carry flag (vv+dvv)	; 2
		add	eax,%$duFP		; uu += duu		; 2/2
		mov	cl,[ebp+ecx]		; Get resulting pixel	; 4
		mov	[edi+7],cl		; Store pixel		; 5
		mov	edx,[%$dudvFP_+4+edx*4]	; get delta texture	; 5
		adc	esi,edx			; update texture ptr	; 6

		add	edi,8			; advance screen pointer
		cmp	edi,%$destend8
		jb	near %$sloop8
		cmp	edi,%$destend
		jae	%$sexit

%$sloop1:	mov	cl,[esi]		; Get texel		; 0
		mov	edx,%$dvFP		; get dvv		; 0
		mov	ch,[edi]		; Get screen pixel	; 1
		add	ebx,edx			; vv += dvv		; 1
		sbb	edx,edx			; carry flag (vv+dvv)	; 2
		add	eax,%$duFP		; uu += duu		; 2/2
		mov	cl,[ebp+ecx]		; Get resulting pixel	; 4
		mov	[edi],cl		; Store pixel		; 5
		mov	edx,[%$dudvFP_+4+edx*4]	; get delta texture	; 5
		adc	esi,edx			; update texture ptr	; 6
		inc	edi			; advance screen pointer; 6

		cmp	edi,%$destend		; We finished?
		jb	%$sloop1

%$sexit:
	%ifdef PIC
		pop	ebx
	%endif
%endmacro
%define		scanloop_map_a1_fini

;-----======xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx======-----
; Summary:
;   The internal scanloop for scan_map_fixalpha2
;   Draw one horizontal scanline (lighting and alpha transparency).
; Arguments:
;   none
; Example:
;   scanproc 8,scan_map_fixalpha2,SCANPROC_MAP,scanloop_map_a2
;-----======xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx======-----
%define		scanloop_map_a2_args scanloop_map_a1_args
%define		scanloop_map_a2_init scanloop_map_a1_init
%macro		scanloop_map_a2_body 0
	%ifdef PIC
		push	ebx
	%endif
; dudvInt[1] = ((dvv >> 16) << shifter) + (duu >> 16);
		mov	eax,%$dvv		; eax = dvv		; 0
		mov	edx,%$duu		; edx = duu		; 0
		sar	eax,16			; eax = dvv >> 16	; 1
		mov	esi,bitmap2		; esi = texture ptr	; 1
		sar	edx,16			; edx = duu >> 16	; 2
		mov	ecx,shf_u		; ecx = shifter		; 2
		shl	eax,cl			; eax = (dvv >> 16) << s; 3/4
; dudvInt[0] = dudvInt[1] + (1 << shifter);
		mov	ebx,%$vv		; ebx = vv		; 3
		add	eax,edx			; eax = dudvInt[1]	; 7
		mov	edx,1			; edx = 1		; 7
		shl	edx,cl			; edx = 1 << shifter	; 8/4
		mov	[%$dudvFP_+4],eax	; dudvInt[1] = eax	; 8
		sar	ebx,16			; ebx = vv >> 16	; 12
		add	eax,edx			; eax = dudvInt[0]	; 12
		mov	[%$dudvFP_+0],eax	; dudvInt[0] = eax	; 13
; unsigned char *s = srcTex + ((vv >> 16) << shifter) + (uu >> 16);
		mov	edx,%$uu		; edx = uu >> 16	; 13
		sar	edx,16						; 14
		mov	eax,%$uu					; 14
		shl	ebx,cl						; 15/4
		test	[edi],al		; fetch dest into cache	; 15
		add	esi,edx			; esi = source texture	; 19
		mov	edx,%$destend					; 19
		add	esi,ebx						; 20

; uFrac = uu << 16;
; vFrac = vv << 16;
		mov	ebx,%$vv					; 20
		shl	eax,16			; eax = uu << 16	; 21
		mov	ebp,%$dvv					; 21
		shl	ebx,16			; ebx = vv << 16	; 22
; duFrac = duu << 16;
; dvFrac = dvv << 16;
		mov	ecx,%$duu					; 22
		shl	ebp,16			; ebp = dvv << 16	; 23
		sub	edx,edi						; 23
		shl	ecx,16			; ecx = duu << 16	; 24
		mov	%$dvFP,ebp					; 24
		mov	%$duFP,ecx					; 25
		xor	ecx,ecx						; 25
		mov	ebp,AlphaMap					; 26
		and	edx,~7			; (destend-dest) & ~7	; 26
		jz	near %$sloop1					; 27

		add	edx,edi						; 28
		mov	%$destend8,edx					; 28

; --------------// Register usage //--------------
; EAX	frac(uu)	mem	frac(duu)
; EBX	frac(vv)	mem	frac(dvv)
; ECX	--//texel//--	EDX	--//scratch//--
; ESI	texture		EDI	dest
; EBP	Alpha mapping buffer

; The pixel loop has two stalls: one tick in U-pipe and one tick in V-pipe

%$sloop8:	mov	ch,[esi]		; Get texel		; 0
		mov	edx,%$dvFP		; get dvv		; 0
		mov	cl,[edi]		; Get screen pixel	; 1
		add	ebx,edx			; vv += dvv		; 1
		sbb	edx,edx			; carry flag (vv+dvv)	; 2 (1 tick u stall)
		add	eax,%$duFP		; uu += duu		; 2/2
		mov	cl,[ebp+ecx]		; Get resulting pixel	; 4 (v stall)
		mov	[edi],cl		; Store pixel		; 5
		mov	edx,[%$dudvFP_+4+edx*4]	; get delta texture	; 5
		adc	esi,edx			; update texture ptr	; 6

		mov	ch,[esi]		; Get texel		; 0
		mov	edx,%$dvFP		; get dvv		; 0
		mov	cl,[edi+1]		; Get screen pixel	; 1
		add	ebx,edx			; vv += dvv		; 1
		sbb	edx,edx			; carry flag (vv+dvv)	; 2
		add	eax,%$duFP		; uu += duu		; 2/2
		mov	cl,[ebp+ecx]		; Get resulting pixel	; 4
		mov	[edi+1],cl		; Store pixel		; 5
		mov	edx,[%$dudvFP_+4+edx*4]	; get delta texture	; 5
		adc	esi,edx			; update texture ptr	; 6

		mov	ch,[esi]		; Get texel		; 0
		mov	edx,%$dvFP		; get dvv		; 0
		mov	cl,[edi+2]		; Get screen pixel	; 1
		add	ebx,edx			; vv += dvv		; 1
		sbb	edx,edx			; carry flag (vv+dvv)	; 2
		add	eax,%$duFP		; uu += duu		; 2/2
		mov	cl,[ebp+ecx]		; Get resulting pixel	; 4
		mov	[edi+2],cl		; Store pixel		; 5
		mov	edx,[%$dudvFP_+4+edx*4]	; get delta texture	; 5
		adc	esi,edx			; update texture ptr	; 6

		mov	ch,[esi]		; Get texel		; 0
		mov	edx,%$dvFP		; get dvv		; 0
		mov	cl,[edi+3]		; Get screen pixel	; 1
		add	ebx,edx			; vv += dvv		; 1
		sbb	edx,edx			; carry flag (vv+dvv)	; 2
		add	eax,%$duFP		; uu += duu		; 2/2
		mov	cl,[ebp+ecx]		; Get resulting pixel	; 4
		mov	[edi+3],cl		; Store pixel		; 5
		mov	edx,[%$dudvFP_+4+edx*4]	; get delta texture	; 5
		adc	esi,edx			; update texture ptr	; 6

		mov	ch,[esi]		; Get texel		; 0
		mov	edx,%$dvFP		; get dvv		; 0
		mov	cl,[edi+4]		; Get screen pixel	; 1
		add	ebx,edx			; vv += dvv		; 1
		sbb	edx,edx			; carry flag (vv+dvv)	; 2
		add	eax,%$duFP		; uu += duu		; 2/2
		mov	cl,[ebp+ecx]		; Get resulting pixel	; 4
		mov	[edi+4],cl		; Store pixel		; 5
		mov	edx,[%$dudvFP_+4+edx*4]	; get delta texture	; 5
		adc	esi,edx			; update texture ptr	; 6

		mov	ch,[esi]		; Get texel		; 0
		mov	edx,%$dvFP		; get dvv		; 0
		mov	cl,[edi+5]		; Get screen pixel	; 1
		add	ebx,edx			; vv += dvv		; 1
		sbb	edx,edx			; carry flag (vv+dvv)	; 2
		add	eax,%$duFP		; uu += duu		; 2/2
		mov	cl,[ebp+ecx]		; Get resulting pixel	; 4
		mov	[edi+5],cl		; Store pixel		; 5
		mov	edx,[%$dudvFP_+4+edx*4]	; get delta texture	; 5
		adc	esi,edx			; update texture ptr	; 6

		mov	ch,[esi]		; Get texel		; 0
		mov	edx,%$dvFP		; get dvv		; 0
		mov	cl,[edi+6]		; Get screen pixel	; 1
		add	ebx,edx			; vv += dvv		; 1
		sbb	edx,edx			; carry flag (vv+dvv)	; 2
		add	eax,%$duFP		; uu += duu		; 2/2
		mov	cl,[ebp+ecx]		; Get resulting pixel	; 4
		mov	[edi+6],cl		; Store pixel		; 5
		mov	edx,[%$dudvFP_+4+edx*4]	; get delta texture	; 5
		adc	esi,edx			; update texture ptr	; 6

		mov	ch,[esi]		; Get texel		; 0
		mov	edx,%$dvFP		; get dvv		; 0
		mov	cl,[edi+7]		; Get screen pixel	; 1
		add	ebx,edx			; vv += dvv		; 1
		sbb	edx,edx			; carry flag (vv+dvv)	; 2
		add	eax,%$duFP		; uu += duu		; 2/2
		mov	cl,[ebp+ecx]		; Get resulting pixel	; 4
		mov	[edi+7],cl		; Store pixel		; 5
		mov	edx,[%$dudvFP_+4+edx*4]	; get delta texture	; 5
		adc	esi,edx			; update texture ptr	; 6

		add	edi,8			; advance screen pointer
		cmp	edi,%$destend8
		jb	near %$sloop8
		cmp	edi,%$destend
		jae	%$sexit

%$sloop1:	mov	ch,[esi]		; Get texel		; 0
		mov	edx,%$dvFP		; get dvv		; 0
		mov	cl,[edi]		; Get screen pixel	; 1
		add	ebx,edx			; vv += dvv		; 1
		sbb	edx,edx			; carry flag (vv+dvv)	; 2
		add	eax,%$duFP		; uu += duu		; 2/2
		mov	cl,[ebp+ecx]		; Get resulting pixel	; 4
		mov	[edi],cl		; Store pixel		; 5
		mov	edx,[%$dudvFP_+4+edx*4]	; get delta texture	; 5
		adc	esi,edx			; update texture ptr	; 6
		inc	edi			; advance screen pointer; 6

		cmp	edi,%$destend		; We finished?
		jb	%$sloop1

%$sexit:
	%ifdef PIC
		pop	ebx
	%endif
%endmacro
%define		scanloop_map_a2_fini scanloop_map_a1_fini

;-----======xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx======-----
; Summary:
;   The internal scanloop for scan_tex_zfil
;   Draw one horizontal scanline (no lighting)
; Arguments:
;   none
; Example:
;   scanproc 8,scan_tex_zfil,SCANPROC_MAP,scanloop_tex
;-----======xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx======-----
%define		scanloop_tex_args 12
%macro		scanloop_tex_init 0
		tloc	%$and_w		; texture width mask
		tloc	%$and_h		; texture height mask
		tloc	%$paltable	; index->palette conversion table

		mov	eax,and_w
		mov	ecx,and_h
		mov	ebp,PaletteTable
		mov	%$and_w,eax
		mov	%$and_h,ecx
		mov	%$paltable,ebp
%endmacro
%macro		scanloop_tex_body 0
	%ifdef PIC
		push	ebx
	%endif
		mov	esi,bitmap
		mov	ecx,shf_h
		mov	eax,%$uu
		mov	ebx,%$vv

; --------------// Register usage //--------------
; EAX	uu		ESI	source texture
; EBX	vv		EDI	dest video ram
; ECX	shf_h		EBP	--//scratch//--
; EDX	--//scratch//--

;  do									\
;  {									\
;    *_dest++ = srcTex[((uu>>16)&ander_w) + ((vv>>shifter_h)&ander_h)];	\
;    uu += duu;								\
;    vv += dvv;								\
;  }									\
;  while (_dest <= _destend)
%$sloop:	mov	edx,ebx		; edx = vv			; 0
		mov	ebp,eax		; ebp = uu			; 0
		shr	edx,cl		; edx = vv >> shf_h		; 1/4
		add	ebx,%$dvv	; vv += dvv			; 1/2
		shr	ebp,16		; ebp = uu >> 16		; 5
		and	ebp,%$and_w	; ebp = (uu >> 16) & and_w	; 6/2
		and	edx,%$and_h	; edx = (vv >> shf_h) & and_h	; 6/2
		add	ebp,edx						; 8
		xor	edx,edx						; 8
		mov	dl,[esi+ebp]					; 10
		mov	ebp,%$paltable					; 9
		add	eax,%$duu					; 11/2
		mov	dl,[ebp+edx]					; 10
		mov	[edi],dl					; 13
		inc	edi						; 13
		cmp	edi,%$destend					; 14
		jb	%$sloop						; 14

%$sexit:
	%ifdef PIC
		pop	ebx
	%endif
%endmacro
%define		scanloop_tex_fini zfill

;-----======xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx======-----
; Summary:
;   The internal scanloop for mmx_scan_tex_zfil
;   Draw one horizontal scanline (no lighting) using MMX
; Arguments:
;   none
; Example:
;   scanproc 8,mmx_scan_tex_zfil,SCANPROC_MAP,mmx_scanloop
;-----======xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx======-----
%define		mmx_scanloop_tex_args scanloop_tex_args
%define		mmx_scanloop_tex_init scanloop_tex_init
%define		mmx_scanloop_tex_body scanloop_tex_body
%define		mmx_scanloop_tex_fini mmx_zfill

;-----======xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx======-----
; Summary:
;   Draw a perspective-incorrect texture mapped polygon scanline
; Arguments:
;   void *dest, int width, unsigned long *zbuff,
;   long u, long du, long v, long dv, unsigned long z, long dz,
;   unsigned char *bitmap, int bitmap_log2w
;-----======xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx======-----
proc		csScan_8_scan_pi_tex_zuse,24,ebx,esi,edi,ebp
		targ	%$dest		; void *dest
		targ	%$width		; int width
		targ	%$zbuff		; long *zbuff
		targ	%$u		; long u
		targ	%$du		; long du
		targ	%$v		; long v
		targ	%$dv		; long dv
		targ	%$z		; long z
		targ	%$dz		; long dz
		targ	%$bitmap	; unsigned char *bitmap
		targ	%$bitmap_log2w	; int bitmap_log2w

		loc	%$dudvFP,8	; fixed-point value of (dv * tex_w + du)
		tloc	%$duFP		; fixed-point duu
		tloc	%$dvFP		; fixed-point dvv
		tloc	%$destend
		tloc	%$paltable	; index->palette conversion table
; if (len <= 0)
;   return;
		mov	eax,%$width
		mov	edi,%$dest
		test	eax,eax
		exit	le

		add	eax,edi
		mov	ebp,PaletteTable
		mov	%$destend,eax
		mov	%$paltable,ebp

; dudvInt[1] = ((dv >> 16) << bitmap_log2w) + (du >> 16);
		mov	eax,%$dv		; eax = dv		; 0
		mov	edx,%$du		; edx = du		; 0
		sar	eax,16			; eax = dv >> 16	; 1
		mov	esi,%$bitmap		; esi = texture ptr	; 1
		sar	edx,16			; edx = du >> 16	; 2
		mov	ecx,%$bitmap_log2w	; ecx = bitmap_log2w	; 2
		shl	eax,cl			; eax = (dv >> 16) << s	; 3/4
; dudvInt[0] = dudvInt [1] + (1 << bitmap_log2w);
		mov	ebx,%$v			; ebx = v		; 3
		add	eax,edx			; eax = dudvInt[1]	; 7
		mov	edx,1			; edx = 1		; 7
		shl	edx,cl			; edx = 1 << bitmap_l2w	; 8/4
		mov	[%$dudvFP_+4],eax	; dudvInt[1] = eax	; 8
		sar	ebx,16			; ebx = v >> 16		; 12
		add	eax,edx			; eax = dudvInt[0]	; 12
		mov	[%$dudvFP_+0],eax	; dudvInt[0] = eax	; 13
; unsigned char *src = bitmap + ((v >> 16) << bitmap_log2w) + (u >> 16);
		mov	edx,%$u			; edx = u >> 16		; 13
		sar	edx,16						; 14
		mov	eax,%$u						; 14
		shl	ebx,cl						; 15/4
		test	[edi],al		; fetch dest into cache	; 15
		add	esi,edx			; esi = source texture	; 19
		add	esi,ebx						; 20

; uFrac = u << 16;
; vFrac = v << 16;
		mov	ebx,%$v						; 20
		shl	eax,16			; eax = uu << 16	; 21
		mov	ebp,%$dv					; 21
		shl	ebx,16			; ebx = vv << 16	; 22
; duFrac = du << 16;
; dvFrac = dv << 16;
		mov	ecx,%$du					; 22
		shl	ebp,16			; ebp = dvv << 16	; 23
		shl	ecx,16			; ecx = duu << 16	; 24
		mov	%$dvFP,ebp					; 25
		mov	%$duFP,ecx					; 25
		mov	ebp,%$zbuff					; 26
		mov	ecx,%$z						; 26

; --------------// Register usage //--------------
; EAX	u		ESI	source texture
; EBX	v		EDI	dest video ram
; ECX	z		EBP	Z-buffer
; EDX	--//scratch//--

%$sloop:	cmp	ecx,[ebp]		; Check Z-buffer	; 0
		jb	%$zbelow		; We're below surface	; 0
		xor	edx,edx
		mov	[ebp],ecx		; *zbuff = z		; 1
		mov	ecx,%$paltable		; Get 8->16 table	; 2
		mov	dl,[esi]		; Get texel		; 1
		mov	dl,[ecx+edx]		; Convert to pal	; 3/2
		mov	ecx,[ebp]		; z = *zbuff		; 3
		mov	[edi],dl		; Put texel		; 2

%$zbelow:	add	ebx,%$dvFP		; v = v + dv		; 2
		sbb	edx,edx						; 3
		add	eax,%$duFP		; u = u + du		; 3/2
		adc	esi,[%$dudvFP_+4+edx*4]	; Update texture ptr	; 5/2
		add	ecx,%$dz		; z = z + dz		; 5/2
		add	ebp,4			; zbuff++		; 6
		inc	edi			; dest++		; 6
		cmp	edi,%$destend		; dest < destend?	; 7
		jb	%$sloop						; 7
endproc

; Create the scanline routines using above defined macros
scanproc 8,scan_map_zfil,SCANPROC_MAP,scanloop_map
scanproc 8,mmx_scan_map_zfil,SCANPROC_MAP|SCANPROC_MMX,mmx_scanloop_map
scanproc 8,scan_map_zuse,SCANPROC_MAP,scanloop_map_z
scanproc 8,scan_map_fixalpha1,SCANPROC_MAP,scanloop_map_a1
scanproc 8,scan_map_fixalpha2,SCANPROC_MAP,scanloop_map_a2
scanproc 8,scan_tex_zfil,SCANPROC_TEX,scanloop_tex
scanproc 8,mmx_scan_tex_zfil,SCANPROC_TEX|SCANPROC_MMX,mmx_scanloop_tex

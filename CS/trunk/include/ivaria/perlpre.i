/*
    Copyright (C) 2003 Mat Sutcliffe <oktal@gmx.co.uk>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifdef SWIGPERL5

/****************************************************************************
 * Renaming operators is the first stage of wrapping them.
 * We ignore operator [] and () and unary *
 * since these will have to be wrapped manually.
 * We ignore operator &&= and ||=
 * since they have nothing to which to be wrapped
 ****************************************************************************/
%ignore			*::operator[];
%ignore			*::operator();
%ignore			*::operator* ();

%rename(__add__)	*::operator+;
%rename(__subtr__)	*::operator-;
%ignore			*::operator+ ();
%rename(__neg__)	*::operator- ();

%rename(__mult__)	*::operator*;
%rename(__div__)	*::operator/;
%rename(__modulo__)	*::operator%;
%rename(__lshift__)	*::operator<<;
%rename(__rshift__)	*::operator>>;
%rename(__and__)	*::operator&;
%rename(__or__)		*::operator|;
%rename(__xor__)	*::operator^;

%rename(__lt__)		*::operator<;
%rename(__le__)		*::operator<=;
%rename(__gt__)		*::operator>;
%rename(__ge__)		*::operator>=;
%rename(__eq__)		*::operator==;
%rename(__ne__)		*::operator!=;

%rename(__not__)	*::operator!;
%rename(__compl__)	*::operator~;
%rename(__inc__)	*::operator++;
%rename(__dec__)	*::operator--;

%rename(__copy__)	*::operator=;
%rename(__add_ass__)	*::operator+=;
%rename(__subtr_ass__)	*::operator-=;
%rename(__mult_ass__)	*::operator*=;
%rename(__divide_ass__)	*::operator/=;
%rename(__modulo_ass__)	*::operator%=;
%rename(__lshift_ass__)	*::operator<<=;
%rename(__rshift_ass__)	*::operator>>=;
%rename(__and_ass__)	*::operator&=;
%rename(__or_ass__)	*::operator|=;
%rename(__xor_ass__)	*::operator^=;

%rename(__land__)	*::operator&&;
%rename(__lor__)	*::operator||;

%ignore			*::operator&&=;
%ignore			*::operator||=;

/****************************************************************************
 * Applying this Perl code is the second and final stage of wrapping
 * operator overloads. It is commented out since Swig doesn't yet have a
 * %perl5code directive.
 ****************************************************************************/
#if 0
%perl5code %{
  use overload (
    'abs'	=> '__abs__',
    'bool'	=> '__bool__',
    '""'	=> '__string__',
    '0+'	=> '__numer__',

    '+'		=> '__add__',
    '-'		=> '__subtr__',
    '*'		=> '__mult__',
    '/'		=> '__div__',
    '%'		=> '__modulo__',
    '**'	=> '__pow__',

    '<<'	=> '__lshift__',
    '>>'	=> '__rshift__',
    '&'		=> '__and__',
    '|'		=> '__or__',
    '^'		=> '__xor__',

    '+='	=> '__add_ass__',
    '-='	=> '__subtr_ass__',
    '*='	=> '__mult_ass__',
    '/='	=> '__div_ass__',
    '%='	=> '__modulo_ass__',
    '**='	=> '__pow_ass__',
    '<<='	=> '__lshift_ass__',
    '>>='	=> '__rshift_ass__',
    '&='	=> '__and_ass__',
    '|='	=> '__or_ass__',
    '^='	=> '__xor_ass__',

    '<'		=> '__lt__',
    '<='	=> '__le__',
    '>'		=> '__gt__',
    '>='	=> '__ge__',
    '=='	=> '__eq__',
    '!='	=> '__ne__',

    'lt'	=> '__slt__',
    'le'	=> '__sle__',
    'gt'	=> '__sgt__',
    'ge'	=> '__sge__',
    'eq'	=> '__seq__',
    'ne'	=> '__sne__',

    '!'		=> '__not__',
    '~'		=> '__compl__',
    '++'	=> '__inc__',
    '--'	=> '__dec__',

    'x'		=> '__repeat__',
    '.'		=> '__concat__',
    'x='	=> '__repeat_ass__',
    '.='	=> '__concat_ass__',
    '='		=> '__copy__',

    'neg'	=> '__neg__',

    '${}'	=> '__sv__',
    '@{}'	=> '__av__',
    '%{}'	=> '__hv__',
    '*{}'	=> '__gv__',
    '&{}'	=> '__cv__',

    '<>'	=> '__iter__'
  );

  *and = *__land__;
  *or = *__lor__;
%}
#endif // 0

/****************************************************************************
 * Fix wrapping of int8 so that Perl uses an int instead of a length-1 string.
 ****************************************************************************/
%typemap(in) int8
{
  $1 = SvIV ($input);
}
%typemap(out) int8
{
  $result = newSViv ($1);
}

/****************************************************************************
 * Define typemaps to get the pointers out of csRef, csPtr and csWrapPtr.
 ****************************************************************************/
%define TYPEMAP_OUT_csRef_BODY(result, pT, cT)
  if (rf.IsValid ())
  {
    rf->IncRef ();
    SV *rv = newSVsv (& PL_sv_undef);
    sv_setref_iv (rv, pT, (int) (cT *) rf);
    result = rv;
  }
  else
  {
    SvREFCNT_inc (& PL_sv_undef);
    result = & PL_sv_undef;
  }
%enddef

#undef TYPEMAP_OUT_csRef
%define TYPEMAP_OUT_csRef(T)
  %typemap(out) csRef<T>
  {
    csRef<T> rf ($1);
    TYPEMAP_OUT_csRef_BODY ($result, "cspace::" #T, T)
  }
%enddef

#undef TYPEMAP_OUT_csPtr
%define TYPEMAP_OUT_csPtr(T)
  %typemap(out) csPtr<T>
  {
    csRef<T> rf ($1);
    TYPEMAP_OUT_csRef_BODY ($result, "cspace::" #T, T)
  }
%enddef

#undef TYPEMAP_OUT_csWrapPtr
%define TYPEMAP_OUT_csWrapPtr
  %typemap(out) csWrapPtr
  {
    csRef<iBase> rf ($1.Ref);
    TYPEMAP_OUT_csRef_BODY ($result, $1.Type, iBase)
  }
%enddef

/****************************************************************************
 * Typemap to get the pointers out of csRefArray and convert to a Perl array.
 ****************************************************************************/
#undef TYPEMAP_OUT_csRefArray
%define TYPEMAP_OUT_csRefArray(T)
  %typemap(out) csRefArray<T>
  {
    AV *av = newAV ();
    for (int i = 0; i < $1.Length (); i++)
    {
      SV *ae;
      TYPEMAP_OUT_csRef_BODY (ae, "cspace::" #T, T)
      av_push (av, ae);
      SvREFCNT_dec (ae);
    }
    $result = newRV_noinc ((SV *) av);
  }
%enddef

/****************************************************************************
 * Typemaps to convert csArray to a Perl array and vice versa.
 ****************************************************************************/
%define _TYPEMAP_csArray(T, toSV, fromSV)
  %typemap(out) csArray<T>
  {
    AV *av = newAV ();
    for (int i = 0; i < $1.Length (); i++)
    {
      SV *ae = toSV ($1.Get (i));
      av_push (av, ae);
      SvREFCNT_dec (ae);
    }
    $result = newRV_noinc ((SV *) av);
  }
  %typemap(in) csArray<T>
  {
    if (SvTYPE ($input) != SVt_PVAV)
      croak ("%s", "Argument must be an array reference");
    AV *av = (AV *) SvRV ($input);
    if (av_len (av) >= 0)
    {
      for (int i = 0; i <= av_len (av); i++)
      {
        SV *sv = av_fetch (av, i, 0);
        $1.Push (fromSV (sv));
      }
    }
  }
%enddef

_TYPEMAP_csArray(int,			newSViv,	SvIV)
_TYPEMAP_csArray(short,			newSViv,	SvIV)
_TYPEMAP_csArray(long,			newSViv,	SvIV)
_TYPEMAP_csArray(unsigned int,		newSVuv,	SvUV)
_TYPEMAP_csArray(unsigned long,		newSVuv,	SvUV)
_TYPEMAP_csArray(unsigned short,	newSVuv,	SvUV)
_TYPEMAP_csArray(float,			newSVnv,	SvNV)
_TYPEMAP_csArray(double,		newSVnv,	SvNV)

#undef TYPEMAP_csArray
%define TYPEMAP_csArray(T)
  %typemap(out) csArray<T>
  {
    AV *av = newAV ();
    for (int i = 0; i < $1.Length (); i++)
    {
      SV *rv = newSViv (0);
      sv_setref_iv (rv, "cspace::" #T, (int) (void *) & $1.Get (i));
      av_push (av, rv);
      SvREFCNT_dec (ae);
    }
    $result = newRV_noinc ((SV *) av);
  }
  %typemap(in) csArray<T>
  {
    if (SvTYPE ($input) != SVt_PVAV)
      croak ("%s", "Argument must be an array reference");
    AV *av = (AV *) SvRV ($input);
    if (av_len (av) >= 0)
      for (int i = 0; i <= av_len (av); i++)
      {
        SV *sv = av_fetch (av, i, 0);
        if (! sv_isa (sv, "cspace::" #T))
          croak ("All elements of array must be cspace::%s", #T);
        T *v = (T *) SvIV (SvRV (sv));
        $1.Push (* v);
      }
  }
%enddef

/****************************************************************************
 * It seems there's a bug in Swig with this function.
 ****************************************************************************/
%ignore scfInitialize(int argc, const char * const argv[]);

/****************************************************************************
 * Typemaps to convert an argc/argv pair to a Perl array.
 ****************************************************************************/
%typemap(in) (int argc, const char * argv[])
{
  if (SvTYPE ($input) != SVt_PVAV)
    croak ("%s", "Argument must be an array reference");
  AV *av = (AV *) SvRV ($input);
  $1 = av_len (av) + 1;
  $2 = new (char *)[$1];
  for (int i = 0; i < $1; i++)
  {
    SV *sv = av_shift (av);
    $2[i] = SvPV_nolen (sv);
    SvREFCNT_dec (sv);
  }
}

%typemap(freearg) (int argc, const char * argv[])
{
  delete [] $2;
}

%typemap(in) (const char * description, ...)
{
  $1 = "%s";
  $2 = SvPV_nolen ($input);
}

/****************************************************************************
 * Typemaps to handle arrays.
 ****************************************************************************/
#undef TYPEMAP_OUTARG_ARRAY_BODY
%define TYPEMAP_OUTARG_ARRAY_BODY(array_type, base_type, cnt, ptr, to_item)
  AV *av = newAV ();
  for (int i = 0; i < cnt; i++)
  {
    base_type *item = new base_type (to_item ptr[i]);
    SV *o = newSVsv (& PL_sv_undef);
    SvREFCNT_dec (sv_setref_iv (o, #base_type, (int) item));
    av_push (av, o);
    SvREFCNT_dec (o);
  }
  $result = newRV ((SV *) av);
  AvREFCNT_dec (av);
%enddef

#undef TYPEMAP_OUTARG_ARRAY_CNT_PTR
%define TYPEMAP_OUTARG_ARRAY_CNT_PTR(pattern, ptr_init, to_item)
  %typemap(in, numinputs = 0) pattern ($2_type ptr, $1_basetype cnt)
  {
    $1 = & cnt;
    $2 = ($2_type) ptr_init;
  }
  %typemap(outarg) pattern
  {
    TYPEMAP_OUTARG_ARRAY_BODY ($*2_type, $2_basetype, $1, $2, to_item)
  }
  %typemap(freearg) pattern
  {
    delete [] $2;
  }
%enddef

#undef TYPEMAP_OUTARG_ARRAY_PTR_CNT
%define TYPEMAP_OUTARG_ARRAY_PTR_CNT(pattern, ptr_init, to_item)
  %typemap(in, numinputs = 0) pattern ($1_type ptr, $2_basetype cnt)
  {
    $1 = ($1_type) ptr_init;
    $2 = & cnt;
  }
  %typemap(outarg) pattern
  {
    TYPEMAP_OUTARG_ARRAY_BODY ($*1_type, $1_basetype, $2, $1, to_item)
  }
  %typemap(freearg) pattern
  {
    delete [] $1;
  }
%enddef

#undef TYPEMAP_IN_ARRAY_BODY
%define TYPEMAP_IN_ARRAY_BODY(array_type, base_type, cnt, ptr, to_item)
  if (SvTYPE ($input) != SVt_PVAV)
    croak ("%s", "Argument must be an array reference");
  AV *av = (AV *) SvRV ($input);
  cnt = av_len (av) + 1;
  ptr = new array_type [cnt];
  for (int i = 0; i < cnt; i++)
  {
    SV *oref = av_shift (av);
    SV *o = SvRV (oref);
    SvREFCNT_dec (oref);
    if (! sv_isa (o, #base_type))
    {
      croak ("%s", "Array must contain " #base_type "'s");
      delete [] ptr;
      return;
    }
    base_type *p = (base_type *) SvIV (o);
    SvREFCNT_dec (o);
    ptr [i] = to_item p;
  }
%enddef

#undef TYPEMAP_IN_ARRAY_CNT_PTR
%define TYPEMAP_IN_ARRAY_CNT_PTR(pattern, to_item)
  %typemap(in) pattern
  {
    TYPEMAP_IN_ARRAY_BODY ($*2_type, $2_basetype, $1, $2, to_item)
  }
  %typemap(freearg) pattern
  {
    delete [] $2;
  }
%enddef

#undef TYPEMAP_IN_ARRAY_PTR_CNT
%define TYPEMAP_IN_ARRAY_PTR_CNT(pattern, to_item)
  %typemap(in) pattern
  {
    TYPEMAP_IN_ARRAY_BODY ($*1_type, $1_basetype, $2, $1, to_item)
  }
  %typemap(freearg) pattern
  {
    delete [] $1;
  }
%enddef

#endif // SWIGPERL5

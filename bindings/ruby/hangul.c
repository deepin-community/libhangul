/*
 *  Ruby extenstion library for libhangul.
 *
 *  * Author: Gyoung-Yoon Noh <nohmad@sub-port.net>
 *  * License: Same as libhangul.
 */

#include <locale.h>

#include "ruby.h"
#include "hangul.h"

static void
rbhic_free(HangulInputContext *hic)
{
    hangul_ic_delete(hic);
}

static VALUE
rbhic_alloc(VALUE klass)
{
    setlocale(LC_CTYPE, "");
    HangulInputContext *hic = hangul_ic_new("2");
    return Data_Wrap_Struct(klass, 0, rbhic_free, hic);
}

static VALUE
rbhic_initialize(int argc, VALUE *argv, VALUE self)
{
    HangulInputContext *hic;
    Data_Get_Struct(self, HangulInputContext, hic);
    VALUE keyboard;
    rb_scan_args(argc, argv, "01", &keyboard);
    if (argc > 0) {
        Check_Type(keyboard, T_STRING);
        hangul_ic_select_keyboard(hic, RSTRING_PTR(keyboard));
    }
    return self;
}

static VALUE
rbhic_process(VALUE self, VALUE ch)
{
    Check_Type(ch, T_FIXNUM);
    HangulInputContext *hic;
    Data_Get_Struct(self, HangulInputContext, hic);

    bool ret = hangul_ic_process(hic, NUM2CHR(ch));
    return ret ? Qtrue : Qfalse;
}

static VALUE
rbhic_preedit_string(VALUE self)
{
}

static VALUE
rbhic_commit_string(VALUE self)
{
    HangulInputContext *hic;
    Data_Get_Struct(self, HangulInputContext, hic);

    char cbuf[64] = { '\0', };
    wchar_t *wstr = (wchar_t *) hangul_ic_get_commit_string(hic);
    int len = wcstombs(cbuf, wstr, sizeof(cbuf));
    if (strlen(cbuf) > 0)
        return rb_str_new(cbuf, len);
    else
        return Qnil;
}

static VALUE
rbhic_backspace(VALUE self)
{
}

static VALUE
rbhic_reset(VALUE self)
{
    HangulInputContext *hic;
    Data_Get_Struct(self, HangulInputContext, hic);
    hangul_ic_reset(hic);
    return Qnil;
}

static VALUE
rbhic_flush(VALUE self)
{
    HangulInputContext *hic;
    Data_Get_Struct(self, HangulInputContext, hic);
    hangul_ic_flush(hic);
    return Qnil;
}

void
Init_hangul(void)
{
    /* ::Hangul module. */
    VALUE rb_mHangul;
    rb_mHangul = rb_define_module("Hangul");

    /* Hangul::InputContext class. */
    VALUE rb_cInputContext;
    rb_cInputContext = rb_define_class_under(rb_mHangul, "InputContext", rb_cObject);
    rb_define_alloc_func(rb_cInputContext, rbhic_alloc);

    /* Hangul::InputContext methods. */
    rb_define_method(rb_cInputContext, "initialize", rbhic_initialize, -1);
    rb_define_method(rb_cInputContext, "process", rbhic_process, 1);
    rb_define_method(rb_cInputContext, "commit_string", rbhic_commit_string, 0);
    rb_define_method(rb_cInputContext, "preedit_string", rbhic_preedit_string, 0);
    rb_define_method(rb_cInputContext, "backspace", rbhic_backspace, 0);
    rb_define_method(rb_cInputContext, "flush", rbhic_flush, 0);
    rb_define_method(rb_cInputContext, "reset", rbhic_reset, 0);
}


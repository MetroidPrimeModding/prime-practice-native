

int
sscanf (const char *__restrict str,
        const char * fmt, ...)
{
//  int ret;
//  va_list ap;
//  FILE f;
//
//  f._flags = __SRD | __SSTR;
//  f._bf._base = f._p = (unsigned char *) str;
//  f._bf._size = f._r = strlen (str);
//  f._read = __seofread;
//  f._ub._base = NULL;
//  f._lb._base = NULL;
//  f._file = -1;  /* No file. */
//  va_start (ap, fmt);
//  ret = __ssvfscanf_r (_REENT, &f, fmt, ap);
//  va_end (ap);
// TODO: implement
// For now, it's just stubbed out because, luckily, it's not used in what we're using in imgui
  return 0;
}

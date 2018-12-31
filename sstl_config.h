#ifndef SSTL_DEF_H
#define SSTL_DEF_H

#define __SSTL_TRY

#ifdef __SSTL_USE_EXCEPTIONS
    #undef __SSTL_USE_EXCEPTIONS
#endif

#define __SSTL_TEMPLATE_NULL    template <>

#define __SSTL_BEGIN_NAMESPACE   namespace sstl {
#define __SSTL_END_NAMESPACE     }

#endif // SSTL_DEF_H

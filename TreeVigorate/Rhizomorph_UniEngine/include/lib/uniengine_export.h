
#ifndef UNIENGINE_API_H
#define UNIENGINE_API_H

#ifdef UNIENGINE_STATIC_DEFINE
#  define UNIENGINE_API
#  define UNIENGINE_NO_EXPORT
#else
#  ifndef UNIENGINE_API
#    ifdef uniengine_EXPORTS
        /* We are building this library */
#      define UNIENGINE_API __declspec(dllexport)
#    else
        /* We are using this library */
#      define UNIENGINE_API __declspec(dllimport)
#    endif
#  endif

#  ifndef UNIENGINE_NO_EXPORT
#    define UNIENGINE_NO_EXPORT 
#  endif
#endif

#ifndef UNIENGINE_DEPRECATED
#  define UNIENGINE_DEPRECATED __declspec(deprecated)
#endif

#ifndef UNIENGINE_DEPRECATED_EXPORT
#  define UNIENGINE_DEPRECATED_EXPORT UNIENGINE_API UNIENGINE_DEPRECATED
#endif

#ifndef UNIENGINE_DEPRECATED_NO_EXPORT
#  define UNIENGINE_DEPRECATED_NO_EXPORT UNIENGINE_NO_EXPORT UNIENGINE_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef UNIENGINE_NO_DEPRECATED
#    define UNIENGINE_NO_DEPRECATED
#  endif
#endif

#endif /* UNIENGINE_API_H */

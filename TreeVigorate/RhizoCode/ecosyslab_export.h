
#ifndef ECOSYSLAB_API_H
#define ECOSYSLAB_API_H

#ifdef ECOSYSLAB_STATIC_DEFINE
#  define ECOSYSLAB_API
#  define ECOSYSLAB_NO_EXPORT
#else
#  ifndef ECOSYSLAB_API
#    ifdef EcoSysLab_EXPORTS
        /* We are building this library */
#      define ECOSYSLAB_API __declspec(dllexport)
#    else
        /* We are using this library */
#      define ECOSYSLAB_API __declspec(dllimport)
#    endif
#  endif

#  ifndef ECOSYSLAB_NO_EXPORT
#    define ECOSYSLAB_NO_EXPORT 
#  endif
#endif

#ifndef ECOSYSLAB_DEPRECATED
#  define ECOSYSLAB_DEPRECATED __declspec(deprecated)
#endif

#ifndef ECOSYSLAB_DEPRECATED_EXPORT
#  define ECOSYSLAB_DEPRECATED_EXPORT ECOSYSLAB_API ECOSYSLAB_DEPRECATED
#endif

#ifndef ECOSYSLAB_DEPRECATED_NO_EXPORT
#  define ECOSYSLAB_DEPRECATED_NO_EXPORT ECOSYSLAB_NO_EXPORT ECOSYSLAB_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef ECOSYSLAB_NO_DEPRECATED
#    define ECOSYSLAB_NO_DEPRECATED
#  endif
#endif

#endif /* ECOSYSLAB_API_H */

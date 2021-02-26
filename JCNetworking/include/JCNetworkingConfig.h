#pragma once
#ifndef JC_NETWORKING_CONFIG_H
#define JC_NETWORKING_CONFIG_H

#define JCN_NAMESPACE jcn

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

#define JCN_WINSOCK

#elif __linux__

#define JCN_UNIX

#elif __unix__

#define JCN_UNIX

// #elif defined(_POSIX_VERSION)

#else
# error "Unsupported compiler target OS"
#endif

#endif
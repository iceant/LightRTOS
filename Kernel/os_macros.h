#ifndef INCLUDED_OS_MACROS_H
#define INCLUDED_OS_MACROS_H

////////////////////////////////////////////////////////////////////////////////
////

#define CONCAT_(prefix, suffix) prefix##suffix
/// Concatenate `prefix, suffix` into `prefixsuffix`

#define CONCAT(prefix, suffix) CONCAT_(prefix, suffix)

/// Make a unique variable name containing the line number at the end of the
/// name. Ex: `uint64_t MAKE_UNIQUE_VARIABLE_NAME(counter) = 0;` would
/// produce `uint64_t counter_7 = 0` if the call is on line 7!
#define MAKE_UNIQUE_VARIABLE_NAME(prefix) CONCAT(prefix##_, __LINE__)

#endif /*INCLUDED_OS_MACROS_H*/

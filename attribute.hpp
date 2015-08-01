#pragma once

#ifdef __GNUC__
#define Li_WARN_IGNORED_RESULT __attribute__((warn_unused_result))
#else
#define Li_WARN_IGNORED_RESULT
#endif

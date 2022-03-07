/*
 * Copyright (c) 2019 Jolla Ltd.
 *
 * License: Proprietary
 */

#ifndef SAILFISH_MINUI_LOGGING_H
# define SAILFISH_MINUI_LOGGING_H

# include <iostream>

/* Assumed: Logging options are defined in sailfish-minui-common.pri
 *          and it gets included in all sailfish-minui subprojects.
 */
# ifndef LOGGING_ENABLE_PRIVATE
#  warning Build environment does not define LOGGING_ENABLE_PRIVATE
#  define LOGGING_ENABLE_PRIVATE 0
# endif

# ifndef LOGGING_ENABLE_DEBUG
#  warning Build environment does not define LOGGING_ENABLE_DEBUG
#  define LOGGING_ENABLE_DEBUG 0
# endif

# define log_err(ITEMS) do { std::cerr << "E:" << ITEMS << std::endl; } while (0)

# define log_warning(ITEMS) do { std::cerr << "W:" << ITEMS << std::endl; } while (0)

# if LOGGING_ENABLE_DEBUG
#  define log_debug(ITEMS) do { std::cerr << "D:" << ITEMS << std::endl; } while (0)
# else
#  define log_debug(ITEMS) do { } while (0)
# endif

# if LOGGING_ENABLE_PRIVATE
#  define log_private(ITEMS) do { std::cerr << "P:" << ITEMS << std::endl; } while (0)
# else
#  define log_private(ITEMS) do { } while (0)
# endif

#endif /* SAILFISH_MINUI_LOGGING_H */

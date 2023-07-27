#pragma once

// SDL
#include <SDL3/SDL_log.h>

#define CORE_LOG_INFO(Category, Format, ...) \
	SDL_LogMessage(Category, SDL_LOG_PRIORITY_INFO, "%d:%d - Info: %s", __FILE__, __LINE__, ##__VA_ARGS__)

#define CORE_LOG_WARN(Category, Format, ...) \
	SDL_LogMessage(Category, SDL_LOG_PRIORITY_WARN, "%d:%d - Warning:  %s", __FILE__, __LINE__, ##__VA_ARGS__)

#define CORE_LOG_ERROR(Category, Format, ...) \
	SDL_LogMessage(Category, SDL_LOG_PRIORITY_ERROR, "%d:%d - Error: %s", __FILE__, __LINE__, ##__VA_ARGS__)
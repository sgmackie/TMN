#pragma once

#include "Allocator.h"
#include <stdio.h>

namespace Core {
namespace Platform {
	// General
    const wchar_t *CharToWideChar(Allocator *Allocator, const char *string);

	// Memory
	void *VirtualReserve(usize size);
	void VirtualCommit(void *pointer, usize size);
	void *VirtualMap(usize size);
	void VirtualUnmap(void *pointer, usize size);
}
}


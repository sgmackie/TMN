
typedef struct ProfilerEvent {
    u64 id;
    const char *category;
    const char *name;
} ProfilerEvent;

typedef struct Profiler {
    bool isEnabled;

    //USE A HASHMAP
    u32 eventCount;
} Profiler;

static Profiler profilerGlobal = { 0 };

u64 BeginScope(const char *category, const char *name) {
    if (!profilerGlobal.isEnabled)
        return 0;

    ProfilerEvent event = { 0 };
    event.category = category;
    event.name = name;
    //event.id = profilerGlobal.eventCount;
    profilerGlobal.eventCount++;

    return event.id;
}

void EndScope(const u64 eventID) {
}

#define PROFILE_START() u64 localProfilerEventID = BeginScope(__func__, __FILE__);
#define PROFILE_END() EndScope(localProfilerEventID);

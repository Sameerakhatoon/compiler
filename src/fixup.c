#include "compiler.h"
#include <stdlib.h>
#include "helpers/vector.h"

FixupSystem* create_new_fixup_system();

FixupConfig* get_fixup_config(Fixup* fixup);

void free_fixup(Fixup* fixup);

void free_fixups(FixupSystem* system);

void start_iterating_fixups(FixupSystem* system);

Fixup* next_fixup(FixupSystem* system);

void free_fixup_system(FixupSystem* system);

int get_count_of_unresolved_fixups(FixupSystem* system);

Fixup* register_fixup(FixupSystem* system, FixupConfig* config);

bool is_fixup_resolved(Fixup* fixup);

void* return_fixup_private_data(Fixup* fixup);

bool is_fixup_system_resolved(FixupSystem* system);

FixupSystem* create_new_fixup_system(){
    FixupSystem* system = calloc(1, sizeof(FixupSystem));
    system->fixups = create_vector(sizeof(Fixup));
    return system;
}

FixupConfig* get_fixup_config(Fixup* fixup){
    return &fixup->config;
}

void free_fixup(Fixup* fixup){
    fixup->config.end(fixup);
    free(fixup);
}

void free_fixups(FixupSystem* system){
    start_iterating_fixups(system);
    Fixup* fixup = next_fixup(system);
    while(fixup){
        free_fixup(fixup);
        fixup = next_fixup(system);
    }
}

void start_iterating_fixups(FixupSystem* system){
    set_peek_index(system->fixups, 0);
}

Fixup* next_fixup(FixupSystem* system){
    return peek_pointer(system->fixups);
}

void free_fixup_system(FixupSystem* system){
    free_fixups(system);
    destroy_vector(system->fixups);
    free(system);
}

int get_count_of_unresolved_fixups(FixupSystem* system){
    int count = 0;
    start_iterating_fixups(system);
    Fixup* fixup = next_fixup(system);
    while(fixup){
        if(!(fixup->flags & FIXUP_FLAG_RESOLVED)){
            count++;
        }
        fixup = next_fixup(system);
    }
    return count;
}

Fixup* register_fixup(FixupSystem* system, FixupConfig* config){
    Fixup* fixup = calloc(1, sizeof(Fixup));
    memcpy(&fixup->config, config, sizeof(FixupConfig));
    fixup->system = system;
    push_element(system->fixups, fixup);
    return fixup;
}

bool is_fixup_resolved(Fixup* fixup){
    if(get_fixup_config(fixup)->fix(fixup)){
        fixup->flags |= FIXUP_FLAG_RESOLVED;
        return true;
    }
    return false;
}

void* return_fixup_private_data(Fixup* fixup){
    return get_fixup_config(fixup)->private_data;
}

bool is_fixup_system_resolved(FixupSystem* system){
    start_iterating_fixups(system);
    Fixup* fixup = next_fixup(system);
    while(fixup){
        if(fixup->flags & FIXUP_FLAG_RESOLVED){
            continue;
        }
        is_fixup_system_resolved(system);
        fixup = next_fixup(system);
    }
    return get_count_of_unresolved_fixups(system) == 0;
}
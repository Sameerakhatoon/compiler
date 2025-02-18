#include "compiler.h"
#include <memory.h>
#include <stdlib.h>
#include <assert.h>

Scope* allocate_scope();

Scope* create_root_scope(CompileProcess* process);

Scope* deallocate_scope(Scope* scope);

void free_root_scope(CompileProcess* process);

Scope* new_scope(CompileProcess* process, int flags);

void scope_iteration_start(Scope* scope);

void scope_iteration_end(Scope* scope);

void* scope_iterate_back(Scope* scope);

void* get_last_entity_of_scope(Scope* scope);

void* get_last_entity_from_scope_stop_at(Scope* scope, Scope* stop_scope);

void* get_scope_last_entity_stop_at(CompileProcess *process, Scope* stop_scope);

void* get_scope_last_entity(CompileProcess* process);

void push_scope(CompileProcess* process, void* pointer, size_t element_size);

void finish_scope(CompileProcess* process);

Scope* get_current_scope(CompileProcess* process);






Scope* allocate_scope(){
    Scope* scope = calloc(1, sizeof(Scope));
    scope->entities = create_vector(sizeof(void*));
    set_peek_index_to_end(scope->entities);
    set_vector_flag(scope->entities, VECTOR_FLAG_DECREMENT_PEEK);
    return scope;
}

Scope* create_root_scope(CompileProcess* process){ //the global scope
    assert(!process->scope.root);
    assert(!process->scope.current);
    Scope* root_scope = allocate_scope();
    process->scope.root = root_scope;
    process->scope.current = root_scope;
    return root_scope;
}

Scope* deallocate_scope(Scope* scope){

}

void free_root_scope(CompileProcess* process){
    deallocate_scope(process->scope.root);
    process->scope.root = NULL;
    process->scope.current = NULL;
}

Scope* new_scope(CompileProcess* process, int flags){
    assert(process->scope.root);
    assert(process->scope.current);
    Scope* new_scope = allocate_scope();
    new_scope->flags = flags;
    new_scope->parent = process->scope.current;
    process->scope.current = new_scope;
    return new_scope;
}

void scope_iteration_start(Scope* scope){
    set_peek_index(scope->entities, 0);
    if(scope->entities->flag_settings & VECTOR_FLAG_DECREMENT_PEEK){
        set_peek_index_to_end(scope->entities);
    }
}

void scope_iteration_end(Scope* scope){
}

void* scope_iterate_back(Scope* scope){
    if(get_element_count(scope->entities) == 0){
        return NULL;
    }
    return peek_pointer(scope->entities);
}

void* get_last_entity_of_scope(Scope* scope){
    if(get_element_count(scope->entities) == 0){
        return NULL;
    }
    return get_last_element_pointer(scope->entities);
}

void* get_last_entity_from_scope_stop_at(Scope* scope, Scope* stop_scope){
    if(scope == stop_scope){
        return NULL;
    }
    void* last_entity = get_last_entity_of_scope(scope);
    if(last_entity){
        return last_entity;
    }
    Scope* parent_scope = scope->parent;
    if(parent_scope){
        return get_last_entity_from_scope_stop_at(parent_scope, stop_scope);
    }
    return NULL;
}

void* get_scope_last_entity_stop_at(CompileProcess *process, Scope* stop_scope){
    return get_last_entity_from_scope_stop_at(process->scope.current, stop_scope);
}

void* get_scope_last_entity(CompileProcess* process){
    return get_last_entity_from_scope_stop_at(process->scope.current, NULL);
}

void push_scope(CompileProcess* process, void* pointer, size_t element_size){
    push_element(process->scope.current->entities, &pointer);
    process->scope.current->size+=element_size;
}

void finish_scope(CompileProcess* process){
    Scope* new_currnet_scope = process->scope.current->parent;
    deallocate_scope(process->scope.current);
    process->scope.current = new_currnet_scope;
    if(process->scope.root && !process->scope.current){
        process->scope.root = NULL;
    }
}

Scope* get_current_scope(CompileProcess* process){
    return process->scope.current;
}


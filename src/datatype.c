#include "compiler.h"

bool is_datatype_struct_or_union(DataType* datatype){
    return datatype->type == DATA_TYPE_STRUCT || datatype->type == DATA_TYPE_UNION;
}

bool is_datatype_struct_or_union_given_name(const char* name){
    return ARE_STRINGS_EQUAL(name, "struct") || ARE_STRINGS_EQUAL(name, "union");
}

size_t get_datatype_size(DataType* datatype){
    if(datatype->flags & DATATYPE_FLAG_IS_POINTER && datatype->pointer_level > 0){
        return DATA_SIZE_DWORD;
    }
    if(datatype->flags & DATATYPE_FLAG_IS_ARRAY){
        return datatype->array.size;
    }
    return datatype->size;
}

size_t get_datatype_size_no_pointer(DataType* datatype){
    if(datatype->flags & DATATYPE_FLAG_IS_ARRAY){
        return datatype->array.size;
    }
    return datatype->size;
}

size_t get_datatype_size_for_array_access(DataType* datatype){
    if(is_datatype_struct_or_union(datatype) && datatype->flags & DATATYPE_FLAG_IS_POINTER && datatype->pointer_level == 1){
        return datatype->size;
    }
    return get_datatype_size(datatype);
}

size_t get_datatype_element_size(DataType* datatype){
    if(datatype->flags & DATATYPE_FLAG_IS_POINTER){
        return DATA_SIZE_DWORD;
    }
    return datatype->size;
}

bool is_datatype_primitive(DataType* datatype){
    return !is_datatype_struct_or_union(datatype);
}
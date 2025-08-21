#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "Allocator.cpp"
#include "array.hpp"
#include "string.hpp"

#define SPRINTF_TO_REPLACE "sprintf_s"

int main(int argc, char **args) { 
    // if (argc < 2) {
    //     fprintf(stderr, "Not enough args!\n");
    //     return -1;
    // }
    
    init_allocator(&temp_allocator, Megabytes(1));
    
    Array<String> splitted_line = {0};
    String_Builder builder = make_string_builder(16, NULL);
    String_Builder full_file_builder = make_string_builder(Megabytes(1), NULL);
    
    FILE *changed_files = fopen("files_to_change.txt", "r");
    char file_name_buffer[1024] = {0};
    char file_content_buffer[2048] = {0};
    while (fgets(file_name_buffer, sizeof(file_name_buffer), changed_files)) {
        if (str_contains(file_name_buffer, "\n")) {
            file_name_buffer[strlen(file_name_buffer) - 1] = '\0';
        }
        FILE *file = fopen(file_name_buffer, "r");
        if (!file) {
            continue;  
        } 
        
        builder_clear(&full_file_builder);
        
        int line_number = 0;
        while (fgets(file_content_buffer, sizeof(file_content_buffer), file)) {
            line_number += 1;
            String line_string = string(file_content_buffer);
            
            if (str_contains(file_content_buffer, SPRINTF_TO_REPLACE)) {
                Array<char> tabs_and_spaces_in_beginning = {.allocator = &temp_allocator};
                for (i32 i = 0; i < line_string.count; i++) {
                    if (line_string.data[i] == ' ' || line_string.data[i] == '\t') {
                        tabs_and_spaces_in_beginning.append(line_string.data[i]);
                    } else {
                        break;
                    }
                }
            
                split_string(&splitted_line, line_string, string("() ,\n\t"));
                // That could meant that sprintf_s is in comment section later in the line, so no need to replace that.
                if (splitted_line.get_value(0) != tstring(SPRINTF_TO_REPLACE)) {
                    goto append_end;
                    // continue;
                }
                
                builder_clear(&builder);
                
                for (i32 i = 0; i < tabs_and_spaces_in_beginning.count; i++) {
                    builder_append(&builder, tstring("%c", tabs_and_spaces_in_beginning.get_value(i)));
                }
                
                String *snprintf_written_string = splitted_line.get(1);
                
                builder_append(&builder, string("std::snprintf("));
                builder_append(&builder, tstring("%s, ", snprintf_written_string->data));
                builder_append(&builder, tstring("sizeof(%s), ", snprintf_written_string->data));
                
                i32 rest_of_line_start_index = get_index_of_substring_start(file_content_buffer, snprintf_written_string->data) + snprintf_written_string->count + 2; // + 2 for comma and space that we've written earlier.
                
                // String line_string = string(file_content_buffer);
                String rest_of_line = make_substring(line_string, rest_of_line_start_index, line_string.count - 1, NULL);
                
                builder_append(&builder, rest_of_line);
                
                line_string.data = builder.data;
                line_string.count = builder.count;
                
                printf("%s\n", builder.data);
            }
            append_end:
            builder_append(&full_file_builder, line_string);
            clear_allocator(&temp_allocator);
        }
        
        FILE *changed_file = fopen("changed.c", "w");
        fprintf(changed_file, "%s", full_file_builder.data);
    }
    
    
    return 0;
}

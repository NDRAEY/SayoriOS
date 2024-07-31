#include "lib/command_parser.h"
#include "mem/vmm.h"

void command_parser_new(command_parser_t* parser, const char* _s) {
	if(!parser) {
		return;
	}

	parser->original_string = kcalloc(strlen(_s) + 1, 1);
	memcpy(parser->original_string, _s, strlen(_s));

	parser->argc = 0;


	char* end = parser->original_string + strlen(_s);
	char* ptr = parser->original_string;
	// Count arguments
 	while(ptr != end && *ptr == ' ') {
		ptr++;
 	}
	
	while(*ptr++) {
		if(*ptr == ' ' || *ptr == 0) {
			parser->argc++;

			while(*ptr++ == ' ')
				;
			ptr--;
		}
	}

	
	parser->argv = kcalloc(parser->argc, sizeof(char*));
	
	char* prev = parser->original_string;
	ptr = prev;

	size_t cur = 0;

	// Parse
	while(ptr != end && *ptr == ' ') {
		ptr++;
	}
	
	prev = ptr;
	while(*ptr++) {
		if(*ptr == ' ' || *ptr == 0) {
			size_t len = ptr - prev;

			parser->argv[cur] = kcalloc(len + 1, 1);

			memcpy(parser->argv[cur], prev, len);

			cur++;

			while(*ptr++ == ' ')
				;

			prev = --ptr;
		}
	}
}

void command_parser_destroy(command_parser_t* parser) {
	kfree(parser->original_string);

	for(int i = 0; i < parser->argc; i++) {
		kfree(parser->argv[i]);
	}

	kfree(parser->argv);
}


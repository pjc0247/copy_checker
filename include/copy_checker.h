#pragma once

#include <stdio.h>
#include <Windows.h>
#include <DbgHelp.h>

#pragma comment (lib, "dbghelp")

#define ALLOW_COPY copy_checker::allow_copy += 1
#define END_COPY   copy_checker::allow_copy -= 1

class copy_checker {
public:
	copy_checker() = default;
	copy_checker(const copy_checker &o) {
		if (allow_copy == 0) {
			print_stacktrace();
			breakpoint();
		}
	}
	copy_checker(copy_checker &&o) = default;
	copy_checker &operator =(const copy_checker &o) {
		if (allow_copy == 0) {
			print_stacktrace();
			breakpoint();
		}
		return *this;
	}
	copy_checker &operator =(copy_checker &&o) = default;

private:
	void breakpoint() {
		/* triggers a breakpoint.
		   if the program is running on debug mode, 
		   the debugger will catch it. */
		__asm {
			int 3;
		}
	}
	void print_stacktrace() {
		void         * stack[100];
		unsigned short frames;
		SYMBOL_INFO  * symbol;
		HANDLE         process;

		process = GetCurrentProcess();
		SymInitialize(process, NULL, TRUE);
		frames = CaptureStackBackTrace(0, 100, stack, NULL);
		symbol = (SYMBOL_INFO *)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
		symbol->MaxNameLen = 255;
		symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

		printf(
			"An unexpected copy operation occuered.\n"
			"  at\n");
		for (int i = 2; i < frames; i++) {
			SymSetOptions(SYMOPT_LOAD_LINES);
			SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol);

			IMAGEHLP_LINE64 line;
			DWORD  dwDisplacement;

			line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

			if (SymGetLineFromAddr64(process, (DWORD64)(stack[i]), &dwDisplacement, &line)) {
				printf("    %i : %s(line: %d) - 0x%0X\n", frames - i - 1, symbol->Name, line.LineNumber, symbol->Address);
			}
			else {
				/* a case that symbol doesn't have line number information,
				ignore */
				printf("    %i : %s - 0x%0X\n", frames - i - 1, symbol->Name, symbol->Address);
			}
		}

		free(symbol);
	}

	static int allow_copy;
};

int copy_checker::allow_copy = 0;

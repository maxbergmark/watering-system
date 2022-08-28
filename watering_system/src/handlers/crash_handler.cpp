#include "crash_handler.h"

char buf[EEPROM_MAX_ADDR];
char buf2[2000];

String stack;
String reset;

String loadStack(){
	String stack = "";
	eeprom_read_string(0, buf, EEPROM_MAX_ADDR);

	if (buf[0]=='>'){
		stack = String(buf);
	} else {
		buf[200] = '\0';
		stack = String(buf);
	}
	return stack;
}

String loadResetInfo(){
	String reset;

	reset = ESP.getResetInfo();
	return reset;
}

int getStack(uint32_t starter, uint32_t ender, char* buffer){
	char stackline[49];
	int idx = 0;
	for (uint32_t pos = starter; pos < ender; pos += 0x10) {
			uint32_t* values = (uint32_t*)(pos);
			bool looksLikeStackFrame = (values[2] == pos + 0x10);
			sprintf(stackline, "%08x:  %08x %08x %08x %08x %c", 
				pos, values[0], values[1], values[2], values[3], 
				(looksLikeStackFrame)?'<':'\n');
			idx += sprintf(&buffer[idx], "%s", stackline);
			if (idx > 1200) {
				return idx;
			}
	}
	return idx;
}

extern "C" void custom_crash_callback(struct rst_info * rst_info, 
	uint32_t stack, uint32_t stack_end) {	
	
	cont_t g_cont __attribute__ ((aligned (16)));
	uint32_t cont_stack_start = (uint32_t) &(g_cont.stack);
	uint32_t cont_stack_end = (uint32_t) &(g_cont.stack_end);
	uint32_t offset = 0;
	int idx = 0;
	
	idx += sprintf(&buf2[idx], ">>>stack>>>\n");

	if (rst_info->reason == REASON_SOFT_WDT_RST) {
			offset = 0x1b0;
	} else if (rst_info->reason == REASON_EXCEPTION_RST) {
			offset = 0x1a0;
	} else if (rst_info->reason == REASON_WDT_RST) {
			offset = 0x10;
	}

	if (stack > cont_stack_start && stack < cont_stack_end) {
			idx += sprintf(&buf2[idx], "%s", "ctx: cont\n");
	} else {
			idx += sprintf(&buf2[idx], "%s", "ctx: sys\n");
	}

	idx += sprintf(&buf2[idx], "sp: %08x end: %08x offset: %04x\n", 
		stack, stack_end, offset);
	idx += getStack(stack, stack_end, &buf2[idx]);
	idx += sprintf(&buf2[idx], "<<<stack<<<\n");

	eeprom_erase_all();
	eeprom_write_string(0, buf2);
	EEPROM.commit();
} 
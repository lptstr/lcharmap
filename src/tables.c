#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "lcharmap.h"
#include "tables.h"
#include "utf.h"
#include "util.h"

#define CTRL "<control>"

static char *
fmt_bytes(char *bytes)
{
	static char buf[255];
	memset(buf, 0x0, sizeof(buf));

	for (usize i = 0; bytes[i]; ++i)
		strcat(buf, format("\\x%hhX ", bytes[i]));

	return (char *)&buf;
}

void
r_table_print_entry(Rune entry, char *description)
{
	char charbuf[7];
	runetochar((char*) &charbuf, &entry);
	charbuf[sizeof(charbuf)-1] = '\0';

	/* disable character field if entry is a control character */
	bool iscontrol = entry < 32 || (entry > 126 && entry < 160);

	printf("codepoint    %-5d 0x%-5X 0o%-5o\n", entry, entry, entry);
	printf("UTF-8        %s\n", fmt_bytes(charbuf));
	printf("glyph        %s\n", iscontrol ? CTRL : charbuf);
	printf("description  %s\n", description);
	printf("\n");
}

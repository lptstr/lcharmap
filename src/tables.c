#include <stdio.h>
#include <string.h>

#include "fort.h"
#include "lcharmap.h"
#include "tables.h"
#include "utf.h"

static void table_print_line(struct Table *self);
static void table_print_header(struct Table *self);
static void table_print_entry(
	struct Table *self,
	Rune entry,
	char *description,
	ft_table_t *t
);

void
table_print_line(struct Table *self)
{
	/* TODO: allow user to change line character
	 * via cmd options, just in case their terminal/font
	 * doesn't fully support UTF8 */

	/* don't forget the null terminator! */
	char line[((self->ttywidth) * sizeof("─")) + 1];
	strcpy(line, "─");
	for (usize i = 0; i < (usize) self->ttywidth - 1; ++i) {
	        strcat(line, "─");
	}
	line[self->ttywidth * sizeof("─")] = '\n';
	line[(self->ttywidth + 1) * sizeof("─")] = '\0';
	printf(line);
}

void
table_print_header(struct Table *self)
{
	table_print_line(self);
	printf("%-8s%-8s%-8s%-8s%s\n", "DEC", "HEX", "OCT", "CHAR", "DESC");
	table_print_line(self);
}

void
table_print_entry(struct Table *self, Rune entry, char *description, ft_table_t *t)
{
	char dec[snprintf(NULL, 0, "%d", entry)];
	sprintf((char*) &dec, "%d", entry);

	char hex[snprintf(NULL, 0, "0x%X", entry)];
	sprintf((char*) &hex, "0x%X", entry);

	char oct[snprintf(NULL, 0, "0o%o", entry)];
	sprintf((char*) &oct, "0o%o", entry);

	/*
	 * TODO: display "readable" html entities
	 * e.g. &amp; instead of &#38; for '&'
	 */
	char htm[snprintf(NULL, 0, "&#%d;", entry)];
	sprintf((char*) &htm, "&#%d;", entry);

	/* disable character field if entry is
	 * a control character */
	bool iscontrol = entry < 32 || (entry > 126 && entry < 160);

	usize sz;
	if (iscontrol) sz = sizeof("<ctrl>");
	else sz = runelen(entry) + 1;
	char cha[sz];
	if (iscontrol) {
		strcpy((char*) &cha, "<ctrl>");
	} else {
		runetochar((char*) &cha, &entry);
		cha[sz] = '\0';
	}

	const usize max_short_desc_sz = 50;

	char dsc[128];
	if (!self->format_long) {
		/* depending on how long the descriptions is,
		 * we might have to truncate it */
		if (strlen(description) > max_short_desc_sz) {
			strncpy(dsc, description, max_short_desc_sz - 3);
			dsc[max_short_desc_sz - 2] = '\0';
			strcat(dsc, "...");
		} else {
			strcpy(dsc, description);
		}
	} else {
		strcpy(dsc, description);
	}

	ft_write_ln(t, &dec, &hex, &oct, &cha, &dsc);
	return;

	if (self->format_long) {
		printf("%c[1m%20s  %c[m%s\n", 0x1B, "decimal", 0x1B,
			(char*) &dec);
		printf("%c[1m%20s  %c[m%s\n", 0x1B, "hexadecimal", 0x1B,
			(char*) &hex);
		printf("%c[1m%20s  %c[m%s\n", 0x1B, "octal", 0x1B,
			(char*) &oct);
		printf("%c[1m%20s  %c[m%s\n", 0x1B, "HTML entity", 0x1B,
			(char*) &htm);
		printf("%c[1m%20s  %c[m%s\n", 0x1B, "character", 0x1B,
			(char*) &cha);
		printf("%c[1m%20s  %c[m%s\n", 0x1B, "description", 0x1B,
			description);
		printf("\n");
	} else {
		printf("%-8s%-8s%-8s%-8s%s\n", &dec, &hex, &oct,
			&cha, description);
		table_print_line(self);
	}
}

void
table_show(struct Table *self)
{
	/* TODO: use libfort for long format too */
	ft_table_t *t = ft_create_table();

	ft_set_border_style(t, FT_PLAIN_STYLE);

	ft_set_cell_prop(t, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
	ft_write_ln(t, "DEC", "HEX", "OCT", "CHAR", "DESC");


	for (usize i = 0; i < self->entries->length; ++i) {
		table_print_entry(
			self,
			self->entries->data[i],
			self->descrips->data[i],
			t
		);
	}

	printf("%s\n", ft_to_string(t));
	ft_destroy_table(t);
}

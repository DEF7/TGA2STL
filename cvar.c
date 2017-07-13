// cvar.c - handle variables with string names

#include <stdio.h>
#include <string.h>

#include "main.h"
#include "cvar.h"


#define MAX_CVARS		64



//
unsigned int num_cvars = 0;
cvar_t cvars[MAX_CVARS];



//
cvar_t *cvar_find(char *name)
{
	unsigned int x;

	for(x = 0; x < num_cvars; x++)
		if(!strcmp(cvars[x].name, name))
			return &cvars[x];

	return 0;
}
//


//
void cvar_setvalue(char *name, char *value)
{
	cvar_t *c;

	if(!(c = cvar_find(name)))
		return;

	switch(c->type)
	{
		case VAR_STR:
			sprintf(c->value.s, "%s", value);
			break;

		case VAR_INT:
			sscanf(value, "%d", &c->value.i);
			break;

		case VAR_FLT:
			sscanf(value, "%f", &c->value.f);
			break;
	}
}
//


//
cvar_t *cvar_add(char *name, int type)
{
	cvar_t c;
	sprintf(c.name, "%s", name);
	c.type = type;
	cvars[num_cvars] = c;
	return &cvars[num_cvars++];
}
//

//
cvar_t *cvar_newstring(char *name, char *value)
{
	cvar_t *c = cvar_add(name, VAR_STR);
	sprintf(c->value.s, "%s", value);
	return c;
}
//

//
cvar_t *cvar_newint(char *name, int value)
{
	cvar_t *c = cvar_add(name, VAR_INT);
	c->value.i = value;
	return c;
}
//

//
cvar_t *cvar_newfloat(char *name, float value)
{
	cvar_t *c = cvar_add(name, VAR_FLT);
	c->value.f = value;
	return c;
}
//

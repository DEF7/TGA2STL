#ifndef H_CVAR
#define H_CVAR


#define VAR_NONE		0
#define VAR_STR			1
#define VAR_INT			2
#define VAR_FLT			3


//
typedef struct
{
	char name[32];
	int type;

	union
	{
		char s[64];
		int i;
		float f;
	} value;
} cvar_t;
//


//
void cvar_setvalue(char *name, char *value);

cvar_t *cvar_newstring(char *name, char *value);
cvar_t *cvar_newint(char *name, int value);
cvar_t *cvar_newfloat(char *name, float value);


#endif

/*
** Lua shell/interpreter for lk.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define lua_c
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#define LUA_PROGNAME "lua"
#define LUA_PROMPT "> "
#define LUA_PROMPT2 ">> "
#define LUA_MAXINPUT 80

#define lua_stdin_is_tty()	1  /* assume stdin is a tty */
#define lua_readline(L,b,p) \
        ((void)L, fputs(p, stdout), fflush(stdout),  /* show prompt */ \
        fgets(b, LUA_MAXINPUT, stdin) != NULL)  /* get line */
#define lua_saveline(L,idx)	{ (void)L; (void)idx; }
#define lua_freeline(L,b)	{ (void)L; (void)b; }

static lua_State *globalL = NULL;
static const char *progname = LUA_PROGNAME;

static void l_message (const char *pname, const char *msg) {
	if (pname) luai_writestringerror("%s: ", pname);
	luai_writestringerror("%s\n", msg);
}

static int report (lua_State *L, int status) {
	if (status != LUA_OK && !lua_isnil(L, -1)) {
		const char *msg = lua_tostring(L, -1);
		if (msg == NULL)
			msg = "(error object is not a string)";

		l_message(progname, msg);
		lua_pop(L, 1);
		/* force a complete garbage collection in case of errors */
		lua_gc(L, LUA_GCCOLLECT, 0);
	}

	return status;
}

/* the next function is called unprotected, so it must avoid errors */
static void finalreport (lua_State *L, int status) {
	if (status != LUA_OK) {
		const char *msg = (lua_type(L, -1) == LUA_TSTRING) ? lua_tostring(L, -1)
			: NULL;
		if (msg == NULL)
			msg = "(error object is not a string)";

		l_message(progname, msg);
		lua_pop(L, 1);
	}

}

static int traceback (lua_State *L) {
	const char *msg = lua_tostring(L, 1);
	if (msg)
		luaL_traceback(L, L, msg, 1);
	else if (!lua_isnoneornil(L, 1)) {  /* is there an error object? */
		if (!luaL_callmeta(L, 1, "__tostring"))  /* try its 'tostring' metamethod */
			lua_pushliteral(L, "(no error message)");

	}

	return 1;
}

static int docall (lua_State *L, int narg, int nres) {
	int status;
	int base = lua_gettop(L) - narg;  /* function index */
	lua_pushcfunction(L, traceback);  /* push traceback function */
	lua_insert(L, base);  /* put it under chunk and args */
	globalL = L;  /* to be available to 'laction' */
	status = lua_pcall(L, narg, nres, base);
	lua_remove(L, base);  /* remove traceback function */
	return status;
}

static const char *get_prompt (lua_State *L, int firstline) {
	const char *p;
	lua_getglobal(L, firstline ? "_PROMPT" : "_PROMPT2");
	p = lua_tostring(L, -1);
	if (p == NULL)
		p = (firstline ? LUA_PROMPT : LUA_PROMPT2);

	lua_pop(L, 1);  /* remove global */
	return p;
}

/* mark in error messages for incomplete statements */
#define EOFMARK		"<eof>"
#define marklen		(sizeof(EOFMARK)/sizeof(char) - 1)

static int incomplete (lua_State *L, int status) {
	if (status == LUA_ERRSYNTAX) {
		size_t lmsg;
		const char *msg = lua_tolstring(L, -1, &lmsg);
		if (lmsg >= marklen && strcmp(msg + lmsg - marklen, EOFMARK) == 0) {
			lua_pop(L, 1);
			return 1;
		}

	}

	return 0;  /* else... */
}

static int pushline (lua_State *L, int firstline) {
	char buffer[LUA_MAXINPUT];
	char *b = buffer;
	size_t l;
	const char *prmt = get_prompt(L, firstline);
	if (lua_readline(L, b, prmt) == 0)
		return 0;  /* no input */

	l = strlen(b);
	if (l > 0 && b[l-1] == '\n')  /* line ends with newline? */
		b[l-1] = '\0';  /* remove it */

	if (firstline && b[0] == '=')  /* first line starts with `=' ? */
		lua_pushfstring(L, "return %s", b+1);  /* change it to `return' */
	else
		lua_pushstring(L, b);

	lua_freeline(L, b);
	return 1;
}

static int loadline (lua_State *L) {
	int status;
	lua_settop(L, 0);
	if (!pushline(L, 1))
		return -1;  /* no input */

	for (;;) {  /* repeat until gets a complete line */
		size_t l;
		const char *line = lua_tolstring(L, 1, &l);
		status = luaL_loadbuffer(L, line, l, "=stdin");
		if (!incomplete(L, status))
			break;  /* cannot try to add lines? */

		if (!pushline(L, 0))  /* no more input? */
			return -1;

		lua_pushliteral(L, "\n");  /* add a new line... */
		lua_insert(L, -2);  /* ...between the two lines */
		lua_concat(L, 3);  /* join them */
	}

	lua_saveline(L, 1);
	lua_remove(L, 1);  /* remove line */
	return status;
}

static void print_version (void) {
	luai_writestring(LUA_COPYRIGHT, strlen(LUA_COPYRIGHT));
	luai_writeline();
}

static void dotty (lua_State *L) {
	int status;
	const char *oldprogname = progname;
	progname = NULL;
	while ((status = loadline(L)) != -1) {
		if (status == LUA_OK)
			status = docall(L, 0, LUA_MULTRET);

		report(L, status);
		if (status == LUA_OK && lua_gettop(L) > 0) {  /* any result to print? */
			luaL_checkstack(L, LUA_MINSTACK, "too many results to print");
			lua_getglobal(L, "print");
			lua_insert(L, 1);
			if (lua_pcall(L, lua_gettop(L)-1, 0, 0) != LUA_OK)
				l_message(progname,
						  lua_pushfstring(L,
										  "error calling " LUA_QL("print") " (%s)",
										  lua_tostring(L, -1)));

		}

	}

	lua_settop(L, 0);  /* clear stack */
	luai_writeline();
	progname = oldprogname;
}

static int pmain (lua_State *L) {
	/* open standard libraries */
	luaL_checkversion(L);
	lua_gc(L, LUA_GCSTOP, 0);  /* stop collector during initialization */
	luaL_openlibs(L);  /* open libraries */
	lua_gc(L, LUA_GCRESTART, 0);

	print_version();
	dotty(L);

	lua_pushboolean(L, 1);  /* signal no errors */
	return 1;
}

#include <app.h>
#include <debug.h>

static void luash_init(const struct app_descriptor *app)
{
}

#include <malloc.h>

static void luash_entry(const struct app_descriptor *app, void *args)
{
	int status;

	iprintf("Got here 1, BUFSIZ=%d\n", BUFSIZ);

	malloc_stats();

	lua_State *L = luaL_newstate();  /* create state */

	iprintf("Got here 2\n");

	if (L == NULL) {
		l_message(progname, "cannot create state: not enough memory");
		return;
	}

	/* call 'pmain' in protected mode */
	lua_pushcfunction(L, &pmain);
	status = lua_pcall(L, 2, 1, 0);
	lua_toboolean(L, -1);
	finalreport(L, status);
	lua_close(L);
	return;
}

APP_START(luash)
    .init = luash_init,
    .entry = luash_entry,
    .flags = APP_FLAG_CUSTOM_STACK_SIZE,
    .stack_size = 2048,
APP_END

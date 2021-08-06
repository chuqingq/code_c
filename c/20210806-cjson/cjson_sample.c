#include <cjson/cJSON.h>

int main() {
	cJSON *json = cJSON_CreateObject();
	cJSON_AddNumberToObject(json,"remainmile", 1.33f);

	char *buf = cJSON_Print(json);
	printf("%s\n", buf);
	cJSON_Delete(json);
	return 0;
}

/*
 gcc cjson_sample.c -lcjson
*/

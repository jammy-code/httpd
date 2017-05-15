#include <stdlib.h>
#include <stdio.h>

#include <string.h>

#include "json_fmt.h"



int json_add_string(struct json_obj* obj, char *key, char *val)
{
	int iret = -1;
	
	if (obj==NULL || key==NULL || val==NULL)
		return iret;
	
	if (obj->count < obj->size){
		obj->nodes[obj->count].key = strdup(key);
		obj->nodes[obj->count].dtype = json_type_string;
		obj->nodes[obj->count].d.c_string = strdup(val);
		obj->count++;
		iret = 0;
	}

	return iret;
}


int json_add_int(struct json_obj* obj, char *key, int i)
{
	int iret = -1;
	
	if (obj==NULL || key==NULL)
		return iret;
	
	if (obj->count < obj->size){
		obj->nodes[obj->count].key = strdup(key);
		obj->nodes[obj->count].dtype = json_type_int;
		obj->nodes[obj->count].d.c_int= i;
		obj->count++;
		iret = 0;
	}

	return iret;
}

struct json_obj* json_new_obj(int size)
{
	struct json_obj *obj = NULL;
	
	if (size < 1){
		size = JSON_OBJECT_DEF_HASH_NODES;
	}
	
	obj =(struct json_obj*)calloc(sizeof(struct json_obj), 1);
	
	if (!obj)
		return obj;

	obj->size = size;
	obj->nodes = (struct hash_node *)calloc(sizeof(struct hash_node), size);
	if(!obj->nodes) {
		free(obj);
		obj = NULL;
	}
	
	return obj;
}

void json_free(struct json_obj* obj)
{

	if (obj){
		int i=0;
		for (; i<obj->count; i++){
			if (obj->nodes[i].key)
				free(obj->nodes[i].key);
			if (obj->nodes[i].dtype == json_type_string){
				free(obj->nodes[i].d.c_string);
			}
			else if (obj->nodes[i].dtype == json_type_object){
				json_free(obj->nodes[i].d.c_obj);
			}
		}
		free(obj);	
	}
}

int json_count_buffer(struct json_obj* obj)
{
	int cnt = 0;
	
	if (obj){
		int i=0;
		for (; i<obj->count; i++){
			if (obj->nodes[i].dtype == json_type_string){
				cnt += snprintf(NULL, 0, "\"%s\":\"%s\",\n", obj->nodes[i].key, obj->nodes[i].d.c_string);
			}
			else if (obj->nodes[i].dtype == json_type_int){
				cnt += snprintf(NULL, 0, "\"%s\":%d,\n", obj->nodes[i].key, obj->nodes[i].d.c_int);
			}
			else if (obj->nodes[i].dtype == json_type_object){
				cnt += json_count_buffer(obj->nodes[i].d.c_obj);
			}
		}
		cnt += 4;//{\n}\n
		if (i) cnt--; //,
	}
	return cnt;
}

int json_printbuf(char* buff, int len, struct json_obj* obj)
{
	char *testp = buff;
	if (buff && len>4 &&obj){
		int i=0;
		strcpy(buff, "{\n");
		buff+=2;
		len -=2;
		for (; i<obj->count && len>0; i++){
			int cnt;
			if (i){
				strcpy(buff, ",\n");
				buff+=2;
				len -=2;
			}
			if (obj->nodes[i].dtype == json_type_string){
				cnt = snprintf(buff, len, "\"%s\":\"%s\"", obj->nodes[i].key, obj->nodes[i].d.c_string);
				buff += cnt;
				len -= cnt;
			}
			else if (obj->nodes[i].dtype == json_type_int){
				cnt = snprintf(buff, len, "\"%s\":%d", obj->nodes[i].key, obj->nodes[i].d.c_int);
				buff += cnt;
				len -= cnt;
			}
			else if (obj->nodes[i].dtype == json_type_object){
				json_printbuf(buff, len, obj->nodes[i].d.c_obj);
			}
		}
		if (i && len) {
			strcpy(buff, "\n");
			buff++;
			len--;
		}
		strcpy(buff, "}\n");
		buff+=2;
		len -=2;
	}
	printf("%d:%s() buff %s\n",__LINE__, __func__, testp);
	return len;
}
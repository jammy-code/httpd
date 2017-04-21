#ifndef __JSON_FMT_H__
#define __JSON_FMT_H__


#define JSON_OBJECT_DEF_HASH_NODES 16

enum json_type {
  json_type_null,
  json_type_boolean,
  json_type_double,
  json_type_int,
  json_type_object,
  json_type_array,
  json_type_string
};

struct hash_node{
	char* key;
	enum json_type dtype;
	union data {
    int c_int;
    char *c_string;
    struct json_obj *c_obj;
  } d;
};

struct json_obj {
	int count;
	int size;
	struct hash_node *nodes;
};

struct json_obj* json_new_obj(int size);
int json_add_int(struct json_obj* obj, char *key, int i);
int json_add_string(struct json_obj* obj, char *key, char *val);
int json_add_object(struct json_obj* obj, char *key,  struct json_obj *val);

int json_count_buffer(struct json_obj* obj);
int json_printbuf(char* buff, int len, struct json_obj* obj);
void json_free(struct json_obj* obj);

#endif
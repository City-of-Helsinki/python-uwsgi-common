/*
  Based on:
  https://github.com/velebit-ai/uwsgi-json-logging-plugin/blob/4edb5cc59013b18f32658195b328afb2ac21b2d2/escape_json.c

  uWSGI plugin that creates custom JSON-escaped logging variables.

  Build the plugin with: `uwsgi --build-plugin <filename.c>`
  Use it with: `uwsgi --plugin <filename_plugin.so> ...`
*/

#include <uwsgi.h>

/**
 * @brief JSON-escapes a string field in a wsgi_request structure.
 *
 * This function reads a string and its length from specified offsets
 * within the `wsgi_req` object, escapes it for safe JSON usage, and stores
 * the result in the output buffer.
 *
 * @param[in]  offset_data Offset to the string pointer inside the wsgi_request struct.
 * @param[in]  offset_length Offset to the string length inside the wsgi_request struct.
 * @param[in]  wsgi_req Pointer to the wsgi_request structure.
 * @param[out] buf Pointer to a char pointer where the escaped string will be stored.
 *
 * @return Length of the escaped string on success; -1 or non-zero on error.
 */
static ssize_t escape_uwsgi_field(
    long offset_data,
    long offset_length,
    struct wsgi_request *wsgi_req,
    char **buf
) {
    char **data = (char **) (((char *) wsgi_req) + offset_data);
    uint16_t *length = (uint16_t *) (((char *) wsgi_req) + offset_length);

    char *json_str = uwsgi_malloc((*length * 2) + 1);  // Worst-case size
    escape_json(*data, *length, json_str);
    
    *buf = json_str;
    return strlen(*buf);
}

/**
 * @brief Returns the escaped URI from the wsgi_request.
 *
 * @param[in]  wsgi_req Pointer to the wsgi_request structure.
 * @param[out] buf Pointer to a char pointer where the escaped URI will be stored.
 *
 * @return Length of the escaped URI on success; -1 or non-zero on error.
 */
static ssize_t uwsgi_lf_json_uri(struct wsgi_request *wsgi_req, char **buf) {
    long offset_uri = offsetof(struct wsgi_request, uri);
    long offset_uri_len = offsetof(struct wsgi_request, uri_len);
    return escape_uwsgi_field(offset_uri, offset_uri_len, wsgi_req, buf);
}

/**
 * @brief Returns the escaped Host from the wsgi_request.
 *
 * @param[in]  wsgi_req Pointer to the wsgi_request structure.
 * @param[out] buf Pointer to a char pointer where the escaped host will be stored.
 *
 * @return Length of the escaped host on success; -1 or non-zero on error.
 */
static ssize_t uwsgi_lf_json_host(struct wsgi_request *wsgi_req, char **buf) {
    long offset_host = offsetof(struct wsgi_request, host);
    long offset_host_len = offsetof(struct wsgi_request, host_len);
    return escape_uwsgi_field(offset_host, offset_host_len, wsgi_req, buf);
}

/**
 * @brief Returns the escaped Referer from the wsgi_request.
 *
 * @param[in]  wsgi_req Pointer to the wsgi_request structure.
 * @param[out] buf Pointer to a char pointer where the escaped referer will be stored.
 *
 * @return Length of the escaped referer on success; -1 or non-zero on error.
 */
static ssize_t uwsgi_lf_json_referer(struct wsgi_request *wsgi_req, char **buf) {
    long offset_referer = offsetof(struct wsgi_request, referer);
    long offset_referer_len = offsetof(struct wsgi_request, referer_len);
    return escape_uwsgi_field(offset_referer, offset_referer_len, wsgi_req, buf);
}

/**
 * @brief Returns the escaped User-Agent from the wsgi_request.
 *
 * @param[in]  wsgi_req Pointer to the wsgi_request structure.
 * @param[out] buf Pointer to a char pointer where the escaped user-agent will be stored.
 *
 * @return Length of the escaped user-agent on success; -1 or non-zero on error.
 */
static ssize_t uwsgi_lf_json_uagent(struct wsgi_request *wsgi_req, char **buf) {
    long offset_uagent = offsetof(struct wsgi_request, user_agent);
    long offset_uagent_len = offsetof(struct wsgi_request, user_agent_len);
    return escape_uwsgi_field(offset_uagent, offset_uagent_len, wsgi_req, buf);
}

/**
 * @brief Registers custom JSON logchunk functions for uWSGI logging.
 */
static void register_logchunks() {
    uwsgi_register_logchunk("json_uri", uwsgi_lf_json_uri, 1);
    uwsgi_register_logchunk("json_host", uwsgi_lf_json_host, 1);
    uwsgi_register_logchunk("json_referer", uwsgi_lf_json_referer, 1);
    uwsgi_register_logchunk("json_uagent", uwsgi_lf_json_uagent, 1);
}

/**
 * @brief Plugin definition for the uWSGI JSON escape plugin.
 */
struct uwsgi_plugin escape_json_plugin = {
    .name = "escape_json",
    .on_load = register_logchunks,
};
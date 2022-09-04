#include "compute_runtime.h"
#include <stdint.h>

typedef int32_t fastly_status_t;
typedef int32_t response_handle_t;
typedef int32_t body_handle_t;
typedef int32_t body_write_end_t;

#define BODY_WRITE_END_BACK 0
#define BODY_WRITE_END_FRONT 1

__attribute__((__import_module__("fastly_http_body"),
               __import_name__("new")))
fastly_status_t fastly_http_body_new(body_handle_t *body_handle);

__attribute__((__import_module__("fastly_http_body"),
               __import_name__("write")))
fastly_status_t fastly_http_body_write(body_handle_t body_handle, const uint8_t *buf, size_t buf_len, body_write_end_t end, size_t *nwritten_out);


__attribute__((__import_module__("fastly_http_resp"),
               __import_name__("new")))
fastly_status_t fastly_http_resp_new(response_handle_t *resp_handle);

__attribute__((__import_module__("fastly_http_resp"),
               __import_name__("send_downstream")))
fastly_status_t fastly_http_resp_send_downstream(response_handle_t resp_handle, body_handle_t body_handle, int32_t streaming);

static VALUE _fastly_http_body_new(VALUE obj)
{
  body_handle_t body_handle;
  fastly_status_t status = fastly_http_body_new(&body_handle);
  if (status != 0) {
    rb_raise(rb_eRuntimeError, "fastly_http_body_new failed");
  }
  return INT2NUM(body_handle);
}

static VALUE _fastly_http_body_write(VALUE obj, VALUE body_handle, VALUE buf, VALUE end)
{
  body_handle_t handle = NUM2INT(body_handle);
  const uint8_t *buf_ptr = (const uint8_t *)RSTRING_PTR(buf);
  size_t buf_len = RSTRING_LEN(buf);
  body_write_end_t write_end = NUM2INT(end);
  size_t nwritten;
  fastly_status_t status = fastly_http_body_write(handle, buf_ptr, buf_len, write_end, &nwritten);
  if (status != 0) {
    rb_raise(rb_eRuntimeError, "fastly_http_body_write failed");
  }
  return INT2NUM(nwritten);
}

static VALUE _fastly_http_resp_new(VALUE obj) {
  response_handle_t resp_handle;
  fastly_status_t status = fastly_http_resp_new(&resp_handle);
  if (status != 0) {
    rb_raise(rb_eRuntimeError, "fastly_http_resp_new failed");
  }
  return INT2NUM(resp_handle);
}

static VALUE _fastly_http_resp_send_downstream(VALUE obj, VALUE resp_handle, VALUE body_handle, VALUE streaming) {
  response_handle_t handle = NUM2INT(resp_handle);
  body_handle_t body_handle_handle = NUM2INT(body_handle);
  int32_t is_streaming = NUM2INT(streaming);
  fastly_status_t status = fastly_http_resp_send_downstream(handle, body_handle_handle, is_streaming);
  if (status != 0) {
    rb_raise(rb_eRuntimeError, "fastly_http_resp_send_downstream failed");
  }
  return Qnil;
}

VALUE rb_mComputeRuntime;

void
Init_compute_runtime(void)
{
  rb_mComputeRuntime = rb_define_module("ComputeRuntime");
  VALUE rb_mABI = rb_define_module_under(rb_mComputeRuntime, "ABI");

  rb_define_module_function(rb_mABI, "fastly_http_resp_new", _fastly_http_resp_new, 0);
  rb_define_module_function(rb_mABI, "fastly_http_resp_send_downstream", _fastly_http_resp_send_downstream, 3);
  rb_define_module_function(rb_mABI, "fastly_http_body_new", _fastly_http_body_new, 0);
  rb_define_module_function(rb_mABI, "fastly_http_body_write", _fastly_http_body_write, 3);
}

project('Generic Reference Counting Header-file Library', 'c',
  version: '1.0',
  license: 'BSD',
  default_options: [
    'werror=true', 'c_std=gnu99', 'b_sanitize=address,undefined',
    'refcnt_trace=true', 'refcnt_check=true'])


if get_option('refcnt_trace')
  add_project_arguments('-DREFCNT_TRACE', language : 'c')
endif
if get_option('refcnt_check')
  add_project_arguments('-DREFCNT_CHECK', language : 'c')
endif


inc_dirs = include_directories('./')

install_headers(files('./refcnt.h'))

test = executable(
  'test_refcnt',
  files('./test.c')
)

test('test_refcnt', test)




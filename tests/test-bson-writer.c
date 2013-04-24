/*
 * Copyright 2013 10gen Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include <assert.h>
#include <time.h>
#include <unistd.h>

#include "bson-tests.h"


static void
test_bson_writer_shared_buffer (void)
{
   bson_writer_t *writer;
   bson_uint8_t *buf = bson_malloc0(32);
   bson_bool_t rolled_back = FALSE;
   size_t buflen = 32;
   size_t n_bytes;
   bson_t *b;
   char key[5];
   int i = 0;
   int j = 0;
   int n_docs = 10000;

   writer = bson_writer_new(&buf, &buflen, 0, bson_realloc);

   for (i = 0; i < n_docs; i++) {
      bson_writer_begin(writer, &b);

      for (j = 0; j < 1000; j++) {
         snprintf(key, sizeof key, "%u", j);
         bson_append_int64(b, key, -1, j);
      }

      if (bson_writer_get_length(writer) > (48 * 1024 * 1024)) {
         rolled_back = TRUE;
         bson_writer_rollback(writer);
         break;
      } else {
         bson_writer_end(writer);
      }
   }

   n_bytes = bson_writer_get_length(writer);

   bson_writer_destroy(writer);

   assert_cmpint(n_bytes, <, (48 * 1024 * 1024));
   assert_cmpint(rolled_back, ==, TRUE);

   bson_free(buf);
}


int
main (int   argc,
      char *argv[])
{
   run_test("/bson/writer/shared_buffer", test_bson_writer_shared_buffer);

   return 0;
}
#ifndef WRITER_BIN_H
#define WRITER_BIN_H

#include "src/data_structs.h"

/*
Binary file format

 63    56|55    48|47    40|39    32|31    24|23    16|15     8|7      0
 
+-----------------------------------------------------------------------+
|                               Film count                              |
+-----------------------------------+-----------------------------------+
|         (1) Rating count          |            (1) Film ID            |
+-----------------------------------+-----------------------------------+
|                          (1) Ratings offset                           |--+
+-----------------------------------+-----------------------------------+  |
|         (2) Rating count          |            (2) Film ID            |  |
+-----------------------------------+-----------------------------------+  |
|                          (2) Ratings offset                           |--|--+
+-----------------------------------+-----------------------------------+  |  |
|         (3) Rating count          |            (3) Film ID            |  |  |
+-----------------------------------+-----------------------------------+  |  |
|                          (3) Ratings offset                           |--|--|--+
+-----------------------------------+-----------------------------------+  |  |  |
|                                                                       |  |  |  |
|                                  ...                                  |  |  |  |
|                                                                       |  |  |  |
+-----------------------------------+--------+--------+--------+--------+<-+  |  |
|          (1) user_id_a1           |(1) note| (1) dd | (1) mm | (1) yy |     |  |
+-----------------------------------+--------+--------+--------+--------+     |  |
|          (1) user_id_a2           |(1) note| (1) dd | (1) mm | (1) yy |     |  |
+-----------------------------------+--------+--------+--------+--------+     |  |
|          (1) user_id_a3           |(1) note| (1) dd | (1) mm | (1) yy |     |  |
+-----------------------------------+--------+--------+--------+--------+<----+  |
|          (2) user_id_b1           |(2) note| (2) dd | (2) mm | (2) yy |        |
+-----------------------------------+--------+--------+--------+--------+        |
|          (2) user_id_b2           |(2) note| (2) dd | (2) mm | (2) yy |        |
+-----------------------------------+--------+--------+--------+--------+        |   (user list sorted by user_id)
|          (2) user_id_b3           |(2) note| (2) dd | (2) mm | (2) yy |        |
+-----------------------------------+--------+--------+--------+--------+        |
|          (2) user_id_b4           |(2) note| (2) dd | (2) mm | (2) yy |        |
+-----------------------------------+--------+--------+--------+--------+        |
|          (2) user_id_b5           |(2) note| (2) dd | (2) mm | (2) yy |        |
+-----------------------------------+--------+--------+--------+--------+<-------+
|          (3) user_id_c1           |(3) note| (3) dd | (3) mm | (3) yy |
+-----------------------------------+--------+--------+--------+--------+
|          (3) user_id_c2           |(3) note| (3) dd | (3) mm | (3) yy |
+-----------------------------------+--------+--------+--------+--------+
|                                                                       |
|                                  ...                                  |
|                                                                       |
+-----------------------------------------------------------------------+
|                              user_count                               |
+-----------------------------------+-----------------+-----------------+
|             user_id1              |   rating_count  |    avg_note     |
+-----------------------------------+-----------------+-----------------+
|             user_id2              |   rating_count  |    avg_note     |   (user list sorted by user_id)
+-----------------------------------+-----------------+-----------------+
|             user_id3              |   rating_count  |    avg_note     |
+-----------------------------------+-----------------+-----------------+
|                                                                       |
|                                  ...                                  |
|                                                                       |
+-----------------------------------------------------------------------+
*/

int write_films(const char* out_filename, SA_DynamicArray* films);

#endif
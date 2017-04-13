# simple_compiler

## Hashmap structure:

`struct hashmap`
-    `size`: how many buckets
-    `used`: how many items
-    `load_factor`: maximum used/size ratio before resizing
-    `value_size`: size of the value stored in items
-    `buckets`: array of pointers to items


```
+---------------+
| buckets       |             +--------+            +--------+
+---------------+   zoom      |  item  |            |  item  |
| item_address +----------->  +--------+     +--->  +--------+
| NULL          |             |  ...   |     |      |  ...   |
| item_address  |             |        |     |      |        |
| item_address  |             |        |     |      |        |
| NULL          |             |  next +------+      |  NULL  |
| ...           |             |        |            |        |
+---------------+             +--------+            +--------+
```

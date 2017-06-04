# simple_compiler

## Files and their constants

### astree.h

- `AST_*`: the AST node type.

### symtab.h

- `SYMBOL_*`: the symbol type in the symbol table (identifier, integer literal, real literal, character literal, string literal)
- `ID_*`: the "nature" of the identifier (variable, vector, function)
- `TP_*`: the data type of the identifier (byte, short, long, float, double)

## Hashmap structure

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

First version:
- [x] Tokenize
- [x] Build AST
- [x] Solve only + and -
- [x] Solve + - and * / by precedence
- [ ] Solve with parenthesis
- [ ] Tokenizer should accept: 
-    [ ] Scientific notation
-    [ ] .xyz float notation(shorthand for 0.xyz)
-    [ ] Variables
- [ ] AST should handle variables in equations like: 1 + 2 + x - y
- [ ] Solver should interpolate variables
- [ ] Introduce optimisation in AST builder. `1 + 2 + 3 + x` should be left only as `6 + x`

Second version:
- [ ] Tokenizer should accept single and double quoted strings
- [ ] Tokenizer should accept boolean operators also `true` and `false`
- [ ] Solver should accept strings are valid values
-     [ ] If strings are added to numbers or vice-versa, strings numbers should be converted to strings
-     [ ] If strings are subtracted, multiplied, divided - result should be marked as "NaN"(each node should have this)
-     [ ] Make boolean operations available
-     [ ] Introduce current type in the node as node may be a number or may be a float or maybe a boolean   

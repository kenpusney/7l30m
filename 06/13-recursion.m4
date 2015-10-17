define(`len',`ifelse($1,,0,`eval(1+len(substr($1,1)))')')dnl
len(hello world!)

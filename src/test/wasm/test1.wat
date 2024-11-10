(module
    (global $g i32 (i32.const 25))
    (func $main  (result i32)    
        i32.const 10
        i32.const -10  
        global.get 0
        return
    )
    (export "main" (func $main))
)
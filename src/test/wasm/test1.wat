(module
    (global $g i32 (i32.const 25))
    (func $main (param $value_1 i32) (result i32) 
        (local $sum i32)   
        i32.const 12
        i32.const -10 
        i32.add
        local.set $sum
        global.get $g
        local.get $value_1
        i32.add
        i32.add
        return
    )
    (export "main" (func $main))
)
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
        local.get $sum
        i32.add
        return
    )
    (export "main" (func $main))
)

(; Estructura wasm
    index 0x0A type section content
    index 0x12 Function section content
    index 0x16 Global section content
    index 0x1E Export section content
    index 0x28 Code section content
        inndex 0x2D body for function 1
 
 ;)
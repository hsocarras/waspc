(module
    (func $add (param $p1 i32) (param $p2 i32) (result i32) (local $temp i32)
        local.get $p1
        local.get $p2
        i32.add
        local.set $temp
        i32.const 2
        local.get  $temp
        i32.sub
    )
    (func $main (result i32)
        i32.const 10
        i32.const 31
        call $add
    )
    (export "main" (func $main))
)
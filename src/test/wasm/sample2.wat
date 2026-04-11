(module
  ;; Definir tamaño de memoria (1 página = 64KB)
  (memory (export "memory") 1)

  ;; Cada Point ocupa 16 bytes (2 x f64 = 8 bytes cada uno)
  (global $point_size (mut i32) (i32.const 16))

  ;; Base del arreglo (offset 0 en memoria)
  (global $array_base (mut i32) (i32.const 0))

  ;; Función para calcular la dirección de un índice
  (func $addr_from_index (param $i i32) (result i32)
    (global.get $array_base)
    (local.get $i)
    (global.get $point_size)
    (i32.mul)
    (i32.add)
  )

  ;; set(index, x, y)
  (func (export "set") (param $i i32) (param $x f64) (param $y f64)
    (local $addr i32)

    ;; calcular dirección base
    (local.set $addr (call $addr_from_index (local.get $i)))

    ;; guardar x en offset 0
    (local.get $addr)
    (local.get $x)
    (f64.store)

    ;; guardar y en offset 8
    (local.get $addr)
    (i32.const 8)
    (i32.add)
    (local.get $y)
    (f64.store)
  )

  ;; get(index) -> (x, y)
  (func (export "get") (param $i i32) (result f64 f64)
    (local $addr i32)

    ;; calcular dirección
    (local.set $addr (call $addr_from_index (local.get $i)))

    ;; cargar x
    (local.get $addr)
    (f64.load)

    ;; cargar y
    (local.get $addr)
    (i32.const 8)
    (i32.add)
    (f64.load)
  )
)
(module
  ;; Importar memoria compartida
  (import "points" "memory" (memory 1))

  ;; Importar funciones del módulo anterior
  (import "points" "set" (func $set (param i32 f64 f64)))
  (import "points" "get" (func $get (param i32) (result f64 f64)))  

  ;; Función distancia entre dos puntos
  (func $distance (param $i i32) (param $j i32) (result f64)
    (local $x1 f64) (local $y1 f64)
    (local $x2 f64) (local $y2 f64)
    (local $dx f64) (local $dy f64)

    ;; get(i)
    (call $get (local.get $i))
    (local.set $y1)
    (local.set $x1)

    ;; get(j)
    (call $get (local.get $j))
    (local.set $y2)
    (local.set $x2)

    ;; dx = x2 - x1
    (local.get $x2)
    (local.get $x1)
    (f64.sub)
    (local.set $dx)

    ;; dy = y2 - y1
    (local.get $y2)
    (local.get $y1)
    (f64.sub)
    (local.set $dy)

    ;;dx square
    (local.get $dx)
    (local.get $dx)
    (f64.mul)
    (local.set $dx)

    ;;dy square
    (local.get $dy)
    (local.get $dy)
    (f64.mul)
    (local.set $dy)

    ;;dx + dy
    (local.get $dx)
    (local.get $dy)
    (f64.add)
    (f64.sqrt)
  )

  ;; main: crea triángulo y calcula longitudes
  (func (export "main") (result f64 f64 f64)
    (local $d01 f64)
    (local $d12 f64)
    (local $d20 f64)

    ;; Definir 3 puntos (triángulo)
    ;; A = (0,0)
    (call $set (i32.const 0) (f64.const 0) (f64.const 0))

    ;; B = (3,0)
    (call $set (i32.const 1) (f64.const 3) (f64.const 0))

    ;; C = (0,4)
    (call $set (i32.const 2) (f64.const 0) (f64.const 4))

    ;; Calcular lados
    ;; AB
    (call $distance (i32.const 0) (i32.const 1))
    (local.set $d01)

    ;; BC
    (call $distance (i32.const 1) (i32.const 2))
    (local.set $d12)

    ;; CA
    (call $distance (i32.const 2) (i32.const 0))
    (local.set $d20)

    ;; devolver longitudes
    (local.get $d01)
    (local.get $d12)
    (local.get $d20)
  )
)

(; Estructura wasm
    index 0x0A type section content
    index 0x25 import section content
    index 0x53 Function section content
    index 0x58 Export section content
    index 0x63 Code section content
        inndex 0x68 body for function 1
        inndex 0x9F body for function 2
 
 ;)
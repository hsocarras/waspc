(module
  ;; Importa funciones algebraicas del módulo "math"
  (import "math" "mul" (func $mul (param f64 f64) (result f64)))
  (import "math" "div" (func $div (param f64 f64) (result f64)))
  ;; Importa la constante global pi del módulo "math"
  (import "math" "pi" (global $pi f64))

  ;; Exporta la función main
  (func $main (param $x f64) (param $shape i32) (result f64)
    ;; shape: 0 = cuadrado, 1 = círculo
    (if (result f64)
      (i32.eq (local.get $shape) (i32.const 0))
      ;; Área del cuadrado: lado * lado
      (then
        (call $mul (local.get $x) (local.get $x))
      )
      ;; Área del círculo: pi * (d/2)^2
      (else
        (call $mul
          (global.get $pi)
          (call $mul
            (call $div (local.get $x) (f64.const 2))
            (call $div (local.get $x) (f64.const 2))
          )
        )
      )
    )
  )
  (export "main" (func $main))
)
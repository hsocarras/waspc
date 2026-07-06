(module
  ;; Tipo: f64 -> f64
  (type $F64_TO_F64 (func (param f64) (result f64)))

  ;; Importa host-std.SinF64
  (import "host-std" "SinF64"
    (func $SinF64 (type $F64_TO_F64))
  )

  ;; calc_hyp(angle, opposite) -> hypotenuse
  (func $calc_hyp
    (param $angle f64)
    (param $opposite f64)
    (result f64)

    local.get $opposite
    local.get $angle
    call $SinF64
    f64.div
  )

  (export "calc_hyp" (func $calc_hyp))
)
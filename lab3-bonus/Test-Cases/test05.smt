(declare-var get_int_out_0 Int)
(define-fun f ((a Int)) Int (+ a a))
(define-fun g ((a Int)) Int (* a 2))
(assert (let ((d get_int_out_0)) (not (= (f d) (g d)))))
(check-sat)
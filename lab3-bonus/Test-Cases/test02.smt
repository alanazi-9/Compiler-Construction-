(declare-var get_int_out_0 Int)
(declare-var get_int_out_1 Int)
(declare-var get_int_out_2 Int)
(define-fun f1 ((a1 Int) (a2 Int) (a3 Int)) Bool (< a1 a2))
(define-fun f2 ((a1 Int)) Int (* 2 2))
(assert (f1 get_int_out_0 get_int_out_1 (f2 get_int_out_2)))
(check-sat)

(define-fun double ((a Int)) Int (* a 2))
(assert (< (double 2) 8))
(check-sat)

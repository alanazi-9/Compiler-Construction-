(declare-var a1 Int)
(declare-var v2 Int)
(declare-var a2 Bool)
(declare-var v3 Bool)
(declare-var v5 Bool)
(declare-var v6 Int)
(declare-var v7 Int)
(declare-var v9 Int)
(declare-var v10 Int)
(declare-var v12 Int)
(declare-var v13 Int)
(declare-var v16 Int)
(declare-var v17 Int)
(declare-var v21 Int)
(declare-var v21 Int)
(declare-var rv 
(declare-var v25 Bool)
(declare-var v5 Bool)
(declare-var v8 Bool)
(declare-var v18 Bool)
(assert (=> true (and
  (= v2 a1)
  (= v3 a2)
  (= v5 v3)
(assert (=> (and v5) (and
  (= v6 v2)
  (= v7 10)
  (= v8 (< v6 v7))
(assert (=> (and v5 v8) (and
  (= v9 v2)
  (= v10 4)
  (= v15 (+ v9 v10))
(assert (=> (and v5 v8) (and
  (= v12 v2)
  (= v13 8)
  (= v15 (- v12 v13))
(assert (=> (and v5 v8) (and
(assert (=> true (and
  (= v16 v2)
  (= v17 5)
  (= v18 (> v16 v17))
(assert (=> (and (not v5) v18) (and
  (= v21 4)
(assert (=> (and (not v5) v18 v0) (and
  (= v21 5)
(assert (=> (and (not v5) v18 v0) (and
(assert (=> true (and
  (= rv v22)
)))
(check-sat)
(get-model)

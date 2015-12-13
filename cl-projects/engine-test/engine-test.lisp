(ql:quickload "desktop-engine")
;(asdf:load-system :engine)

(use-package :frp)
(use-package :engine)

(defmethod test2 (x)
  (print "HEY!!!!!!!trololo")
  (print x))

(defun thing (n x y ang)
  (make-scene-node 20 100 nil
		   (affine2:init-transform! (affine2:make-idt)
					    :orig-x 10
					    :orig-y 90
					    :trans-x x
					    :trans-y y
					    :scl-x 0.85
					    :scl-y 0.85
					    :ang (float ang 0.0))
		   (if (> n 0) (list (thing (1- n) 10 5 ang)) nil)))

(defmethod gen-scene (time)
  (thing 100 300.0 500.0 (sin time)))

(defun app ()
  (map-sig #'test2 *life-cycle*)
  (map-sig #'print *touch-events*)
  (map-sig #'gen-scene *before-render*))

(defun go! ()
  (start-app #'app :title "ZOMG!"))


(defpackage #:testproj
  (:use #:cl))

(in-package #:testproj)

(defun baz (x) x)



;(defun bar ()
;  (macrolet ((mac ()
;	       (baz 100)))
;    (mac)))

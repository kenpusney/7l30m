(define-syntax for
  (syntax-rules (in as from to)
    ((_ element in list body ...)
     (for-each (λ (element)
            body ...)
          list))
    
    ((_ list as element body ...)
     (for element in list body ...))
    
    ((_ element from initial to last body ...)
     (for element in (range initial last)
       body ...))))

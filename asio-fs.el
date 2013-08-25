(require 'my-c-mode)

(c-add-style "infinit"
             '("epita"
               (c-offsets-alist . ((inclass              . +)))))
(setq c-default-style '((c++-mode . "infinit")))

(c++-project
 '("asio-fs/src")
 "_build/linux64"
 "./drake -j 4 //check"
 "tests/tests"
 '()
)

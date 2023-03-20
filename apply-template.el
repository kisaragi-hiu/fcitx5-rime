;;; -*- lexical-binding: t -*-

(require 'subr-x)

(let ((data nil))
  (defun t:get (&optional key)
    "Get template value of KEY.
If KEY is nil, return the whole data instead."
    (unless data
      (setq data (with-temp-buffer
                   (insert-file-contents "template.json")
                   (json-parse-buffer
                    :object-type 'hash-table))))
    (if key
        (gethash key data)
      data)))

(defun t:prepare-directory ()
  "Create the directory."
  (when (file-directory-p "rebranded")
    (shell-command-to-string "rm -rf rebranded"))
  (make-directory "rebranded" t))

(defun t:prepare-files ()
  "Copy the files over and apply the placeholders."
  (dolist (path (directory-files-recursively "template" "."))
    (let ((newpath
           (thread-last
             path
             (replace-regexp-in-string "\\`template" "rebranded")
             (replace-regexp-in-string
              "{{{\\([^{}]+\\)}}}"
              (lambda (str)
                (or (t:get (match-string 1 str))
                    (error "%s: %s is not a valid key" path (match-string 1))))))))
      (make-directory (file-name-directory newpath)
                      t)
      (copy-file path newpath t)
      (with-temp-file newpath
        (insert-file-contents newpath)
        (goto-char (point-min))
        (while (re-search-forward "{{{\\([^{}]+\\)}}}" nil t)
          (replace-match
           (or (t:get (match-string 1))
               (error "%s: %s is not a valid key" path (match-string 1)))))))))

(t:prepare-directory)
(t:prepare-files)

;; Local Variables:
;; mode: lisp-interaction
;; End:

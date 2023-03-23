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

(defun t:dir-to-target (dir)
  "Return output directory for DIR."
  (format "rebranded-%s" dir))

(defun t:prepare-directory (dir)
  "Create the target directory for DIR."
  (let ((target (t:dir-to-target dir)))
    (when (file-directory-p target)
      (shell-command-to-string (format "rm -rf %s" target)))
    (make-directory target t)))

(defun t:prepare-files (dir)
  "Copy the files over from DIR and apply the placeholders.
DIR must have no spaces in it and should just be a directory name."
  (let ((target (t:dir-to-target dir)))
    (dolist (path (seq-remove
                   (lambda (x) (string-match-p "/\\." x))
                   (directory-files-recursively dir ".")))
      (let ((newpath
             (thread-last
               path
               (replace-regexp-in-string (format "\\`%s" dir) target)
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
                 (error "%s: %s is not a valid key" path (match-string 1))))))))))

(let ((dir (or (car argv)
               "template")))
  (t:prepare-directory dir)
  (t:prepare-files dir))

;; Local Variables:
;; mode: lisp-interaction
;; End:

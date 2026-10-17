---
---
# Editor setup

Violet does not include an LSP, so autocompletions are out of scope for this page. This page does describe what can be set up for at least syntax highlighting, which is currently manual because the templating library violet uses is not widely used.

Note that for setting up editors not in this list, the usually only step necessary is to associate `.inja` with a syntax renderer for jinja templates. inja templates are similar enough in implementation to jinja templates that it'll mostly be correct.

## Emacs

`web-mode` supports jinja syntax, which inja is based enough on for it to be functionally identical. Adding the `.inja` extension to web-mode with the `jinja` engine allows templates to be rendered:

```elisp
(use-package web-mode
  :ensure t
  :config
  (add-to-list 'auto-mode-alist '("\\.inja\\'" . web-mode))
  (setq web-mode-engines-alist
        '(
          ("jinja"    . "\\.inja\\'")
        )
  )
  (setq web-mode-enable-front-matter-block t)
)

```
